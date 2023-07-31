#ifndef CALCULATE_H
#define CALCULATE_H

using namespace std;


set<int> calculate_indices (Botan::BigInt challenge, int len) {

	int counter = 0;
	string message;
	set<int> indices;
	string temp = "";

	while ( int(indices.size()) < (len/10) + 1  ) {
		counter++;
		message = to_string(counter);
		temp = challenge.to_dec_string();
		Botan::OctetString key(temp.substr(0,32));
		auto hmac = Botan::MessageAuthenticationCode::create_or_throw("HMAC(SHA-256)");
	  	hmac->set_key(key);
	  	hmac->update(message);
	  	string mac_digest = Botan::hex_encode(hmac->final());
	  	mac_digest = convert_to_4bitstring(mac_digest);
	  	indices.insert(convert_datablock_to_number1024(mac_digest).operator%=(Botan::BigInt(len)).to_u32bit() );
	}

	return indices;

}


Botan::BigInt calculate_poi( Botan::BigInt challenge, vector<Botan::BigInt> tags, Botan::BigInt n ) {

	Botan::BigInt poi("1");

	set<int> indices = calculate_indices(challenge, tags.size());


	for ( auto x : indices ) {
		poi.operator*=(tags[x]);
		poi.operator%=(n);
	}

	return poi;

}

#endif
