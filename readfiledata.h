#ifndef READFILEDATA_H
#define READFILEDATA_H

using namespace std;

// Function declaration
string read_file_data (string filename) { // newlines are ignored

	fstream myfile;
    
    myfile.open(filename);
    vector<string> data;
    string con_data = "";
 
    if (myfile.is_open()) {

        string str;

        while (getline(myfile, str)) {
            data.push_back(str);
        }
        
        myfile.close();
    }

    cout << endl;
    for ( long unsigned int  i = 0 ; i < data.size(); i++ ) {

    	for ( long unsigned int j = 0 ; j < data[i].size() - 1; j++ ){
    		con_data += data[i][j];
    	}
    }

    // cout << con_data <<" "<<con_data.size()<<"\n";
    return con_data;

}

vector<Botan::BigInt> convert_file_to_numarray(string filename) {

	vector<Botan::BigInt> file_data;
	string data;
	long unsigned int i = 0;
	string temp = "";

	data = read_file_data(filename);


	while ( i < data.size() ) {
		temp = temp + data[i];
		if (temp.size() == 128 || i == data.size() - 1 ) {
			string bit_string = convert_to_bitstring1024(temp);
			// cout << bit_string <<"\n\n";
			file_data.push_back(convert_datablock_to_number1024(bit_string));
			temp  = "";
		}
		i++;
	}

	return file_data;
}


vector<Botan::BigInt> calculate_tags (Botan::BigInt g, vector<Botan::BigInt> file_data , Botan::BigInt n) {

	vector<Botan::BigInt> homomorphic_tags;

	for ( long unsigned int  i = 0 ; i < file_data.size(); i++ ) {
		// cout << numeric_data[i] <<"\n";

		homomorphic_tags.push_back(binpowmod(g, file_data[i], n ));
	}

	return homomorphic_tags;

}

#endif
