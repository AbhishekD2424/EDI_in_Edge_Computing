//g++ -I/usr/local/include/botan-2 -L/usr/local/lib -Wl,-rpath=/usr/local/lib -Wall -o es1 edge_server_1.cpp -lbotan-2
//g++ -I/usr/include/botan-2 -L/usr/lib -Wl,-rpath=/usr/lib -Wall -o es1 edge_server_1.cpp -lbotan-2
//./es1

#include "headers.h"

#define SERVER_PORT 6001

void check_integrity (struct message &m, int sockfd, struct sockaddr_in service_vendor_addr) {
	
	string filename;
	cout << "\tEnter the file name to be checked from:\n\t";
	cin >> filename;
	//string filename = m.c.name;
	Botan::BigInt challenge(m.c.challenge);
	Botan::BigInt n(m.c.n);
	Botan::BigInt g(m.c.g);


	vector<Botan::BigInt> file_data; // contains data fo the required file in numeric form of 1KB blocks.
	vector<Botan::BigInt> tags; // contains tags for each data block
	Botan::BigInt poi; // POI for the given challenge

	cout <<"Checking integrity for file : "<<filename<<"....\n\n";

	// Convert file to numeric data of 1KB block size
	file_data = convert_file_to_numarray(filename);
	cout <<"\tFile data converted to numeric blocks...\n\n";

	//Calculate Tags for each block
	tags = calculate_tags(g, file_data, n);
	cout <<"\tHomomorphic tags calulated for the file....\n\n";


	//Calculating poi for the given challenge
	poi = calculate_poi(challenge, tags, n);
	cout <<"\tCalculated poi for the given challnege....\n\n";
	// cout << poi << "\n\n";


	struct poi p;
	memset(&p, '\0', sizeof(p));
	strcpy(p.poi, poi.to_dec_string().c_str());

	message mes;
	mes.type = 3;
	mes.size = sizeof(p);
	mes.p = p;


	sendto(sockfd, &mes, sizeof(mes), MSG_CONFIRM, (const struct sockaddr *) &service_vendor_addr, sizeof(service_vendor_addr));

	cout <<"\tSent response to the challenge....\n\n";

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
    } else {
    	cout << "Server listening at port " << SERVER_PORT <<"...\n\n";
    }

    return sockfd;

}


void listen1(int sockfd) {

    while(1) {

        struct message m;
        socklen_t len;
        struct sockaddr_in service_vendor_addr;

        len = sizeof(service_vendor_addr);

        memset(&service_vendor_addr, 0, sizeof(service_vendor_addr));

        recvfrom( sockfd, &m, sizeof(struct message), MSG_WAITALL, ( struct sockaddr *) &service_vendor_addr, &len );

        if ( m.type == 2) {
        	check_integrity(m, sockfd, service_vendor_addr);
        } else {
        	cout << "\tInvalid message received...\n\n";
        }
    }

}

int main() {

	int sockfd;


	sockfd = start_server();

	listen1(sockfd);

	return 0;
}
