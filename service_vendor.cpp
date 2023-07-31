//g++ -I/usr/local/include/botan-2 -L/usr/local/lib -Wl,-rpath=/usr/local/lib -Wall -o sv service_vendor.cpp -lbotan-2
//g++ -I/usr/include/botan-2 -L/usr/lib -Wl,-rpath=/usr/lib -Wall -o sv service_vendor.cpp -lbotan-2
//./sv

#include "headers.h"
#include "vendor.h"

#define SERVER_PORT 8008

using namespace std;

map<string, pair<string, int>> edge_servers;

int get_edge_server_data()
{
	FILE *fp;
	char buffer[LINE_SIZE];
	int num_of_edge_servers = 0;
	int col_count = 0;
	string edge_name = "";
	string edge_ip = "";
	int edge_port = 0; 


	//Opening the file
    fp = fopen("sv_list.txt", "r");
	if (!fp) {
        printf("Error, Can't Open file\n");
        return -1;
    }


    while (fgets(buffer, LINE_SIZE, fp)) {


    	num_of_edge_servers++;
    	col_count++;


    	char* token = strtok(buffer, "/");

    	while (token != NULL) {

    		if ( col_count == 1 ) {
    			edge_name  = token;
    		} else if ( col_count  == 2 ) {
    			edge_ip = token;
    		} else {
    			edge_port = atoi(token);
    		}

    		col_count++;
        	token = strtok(NULL, "/");
    	}

    	// cout << edge_name <<" "<<edge_ip<<" "<<edge_port<<"\n";

    	edge_servers[edge_name] = {edge_ip, edge_port};

    	col_count = 0;

    }

    fclose(fp);

    return num_of_edge_servers;

}


int start_server() 
{

    int sockfd;
    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("socket creation failed.");
            exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));


    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;


    if (bind(sockfd, (const struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
            perror("bind failed.");
            exit(EXIT_FAILURE);
    }

    return sockfd;

}


void sendFileToClient(int sockfd,  const std::string& filename, struct sockaddr_in	servaddr) {
    FILE* ptr;
    char ch;


    ptr = fopen(filename.c_str(), "r");
    if (NULL == ptr) {
        std::cout << "File can't be opened" << std::endl;
    }
    else {
        std::cout << "File is being sent by the server........" << std::endl;
        do {
            char buf[100];
            memset(buf, '\0', 100);
            ch = fgetc(ptr);
            if (ch != EOF) {
                buf[0] = ch;
                sendto(sockfd, buf, 100, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
            }
        } while (ch != EOF);
        fclose(ptr);
    }

    char buf[100];
    strcpy(buf, "file_complete_");
    sendto(sockfd, buf, 100, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));

}


void send_file(int sockfd) {  // will see it later

	string filename;

	cout << "\tEnter the file name to be checked from:\n\t";
	cin >> filename;

	for ( auto x : edge_servers ) {

		cout <<"y";

		struct file f;
		memset(&f, '\0', sizeof(f));
		strcpy(f.name, filename.c_str());

		struct message m;
		m.type = 1;
		m.size = sizeof(f);
		m.f = f;

		struct sockaddr_in	 servaddr;

        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(x.second.second);
        char* ip = const_cast<char*>(x.second.first.c_str());
        servaddr.sin_addr.s_addr = inet_addr(ip);

        sendto(sockfd, &m, sizeof(m), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));

        sendFileToClient(sockfd, filename, servaddr);

	}

	return;
}

vector<pair<Botan::BigInt, Botan::BigInt>> key_generation (int k) { 

	Botan::AutoSeeded_RNG rngA;
	bool flag;
	Botan::BigInt p;
	Botan::BigInt q;
	Botan::BigInt n;
	Botan::BigInt g;
	vector<pair<Botan::BigInt, Botan::BigInt>> keys;


	Botan::RSA_PrivateKey private_key(rngA, k);


	p = private_key.get_p();
	q = private_key.get_q();
	n = private_key.get_n();

	flag = 1;
	while ( flag ) {
		g = Botan::BigInt::random_integer(rngA, 2, n - 1);
		if (( gcd ( g.operator+=(1), n ) == 1 ) && ( gcd ( g.operator-=(1), n ) == 1 )) {
			g.operator*=(g);
			flag = 0;
		}
	}


	keys.push_back({n, g});
	keys.push_back({p, q});

	return keys;
}

vector<Botan::BigInt> calculate_challenges ( int k  , int num_of_edge_servers) {

	Botan::AutoSeeded_RNG rngA;
	vector<Botan::BigInt> challenges;
	Botan::BigInt exp(k);
	Botan::BigInt two("2");
	Botan::BigInt max_limit  = (binpow(two, exp));
	max_limit.operator--();



	for ( int i = 0; i < num_of_edge_servers ; i++ ) {
		challenges.push_back(Botan::BigInt::random_integer(rngA, 2, max_limit));
	}

	return challenges;

}

//send challenges and recv pois
vector<Botan::BigInt> send_challenges (int sockfd, string filename, vector<Botan::BigInt> challenges, pair<Botan::BigInt, Botan::BigInt> keys) { // we will see later
	
	vector<Botan::BigInt> received_pois;
	int counter = 0;

	for ( auto x : edge_servers ) {

		struct challenge c;
		memset(&c, '\0', sizeof(c));
		strcpy(c.name, filename.c_str());
		strcpy(c.challenge, challenges[counter].to_dec_string().c_str());
		strcpy(c.n, keys.first.to_dec_string().c_str());
		strcpy(c.g, keys.second.to_dec_string().c_str());

		struct message m;
		m.type = 2;
		m.size = sizeof(c);
		m.c = c;

		struct sockaddr_in	 servaddr;

        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(x.second.second);
        char* ip = const_cast<char*>(x.second.first.c_str());
        servaddr.sin_addr.s_addr = inet_addr(ip);

        sendto(sockfd, &m, sizeof(m), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));


        struct message res;
        socklen_t len;
        struct sockaddr_in edge_server_addr;

        len = sizeof(edge_server_addr);

        memset(&edge_server_addr, 0, sizeof(edge_server_addr));

        recvfrom( sockfd, &res, sizeof(struct message), MSG_WAITALL, ( struct sockaddr *) &edge_server_addr, &len );
        // cout << res.type <<"\n";
        // cout << res.p.poi <<"\n";

        received_pois.push_back(Botan::BigInt (res.p.poi));

		counter++;
	}


	return received_pois;
}

void check_integrity_utility(int sockfd) {

	int k = 1024;
	int num_of_edge_servers;
	string filename;
	vector<pair<Botan::BigInt, Botan::BigInt>> keys; //0 is public key 1 is private key
	vector<Botan::BigInt> file_data; // contains data fo the required file in numeric form of 1KB blocks.
	vector<Botan::BigInt> tags; // contains tags for each data block
	vector<Botan::BigInt> challenges; // contains challenges for the edge servers
	vector<Botan::BigInt> received_pois; // contains poi from edges servers for their respective challenges
	vector<Botan::BigInt> calculated_pois; // contains pois for each challenge as calculted by service vendor
	vector<bool> result;
	bool flag = false;
 
	num_of_edge_servers = get_edge_server_data();
	result.resize(num_of_edge_servers, true);


	cout << "\tEnter the file name to be checked:\n\t";
	cin >> filename;


	// Generate keys for the checking
	keys = key_generation(k);
	cout <<"\n\tKeys Generated...\n";


	// Convert file to numeric data of 1KB block size
	file_data = convert_file_to_numarray(filename);
	cout <<"\tFile data converted to numeric blocks...\n\n";


	//Calculate Tags for each block
	tags = calculate_tags(keys[0].second, file_data, keys[0].first);
	cout <<"\tHomomorphic tags calulated for the file....\n\n";


	//Generating challenges for the edge servers
	challenges = calculate_challenges(k, num_of_edge_servers);
	cout <<"\tGenerated challenges for the edge servers....\n\n";


	//Send challenges to edges servers with public key
	received_pois = send_challenges(sockfd, filename, challenges, keys[0]);
	cout <<"\tReceived poi's from all edge servers....\n\n";


	//For each challenge get proof of integrity
	for ( int i = 0; i < num_of_edge_servers ; i++ ) {
		calculated_pois.push_back(calculate_poi(challenges[i], tags, keys[0].first));
	}
	cout <<"\tCalculated correct poi's for each edge server....\n\n";


	// Find and localize corrupted edge data replicas
	binary_search(received_pois, calculated_pois, 0, calculated_pois.size() - 1, keys[0].first,  result);
	cout <<"\tCompared received results with calulated pois....\n\n";


	for ( int i = 0; i < num_of_edge_servers ; i++ ) {
		if ( result[i] == true )
			flag = true;
	}


	if ( flag == false ) {
		cout <<"\tAll data replicas of the given file are correct and up to date.\n\n";
	} else {
		cout <<"\tName and ip addresses of edge servers having corrupted data replicas are:\n\n";
		int temp = 0;
		for ( auto x : edge_servers ) {
			if ( result[temp] == true ) {
				cout <<"\t"<< x.first <<" "<< x.second.first<<"::"<<x.second.second<<"\n\n";
			}

			temp++;
		}
	}



	return;
}

int main() {

	int sockfd;
	int option;


	sockfd = start_server();


	while ( 1 ) {
		cout << "Choose the action you want to perform....."<<"\n";
		cout << "1 :: Check integrity of a file in edge servers."<<"\n";
		cout << "2 :: EXIT"<<"\n";
		cin >> option;

		if ( option != 1 )
			break;
		else {
			switch ( option ) {
				case 1 : check_integrity_utility(sockfd);
					break;
				default : break;
			}
		}

	}

	return 0;
}
