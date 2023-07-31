#ifndef CONVERSION_H
#define CONVERSION_H

using namespace std;


// Function declaration
Botan::BigInt binpow(Botan::BigInt a, Botan::BigInt b) {
	Botan::BigInt res("1");
	
	if (a.is_zero()) return Botan::BigInt("0");
	
	while (Botan::BigInt("0").is_less_than(b)) {
		if (b.is_odd()){
			res.operator*=(a);
		}
			
		a.operator*=(a);
		b.operator>>=(1);
	}
	
	return res;
}

Botan::BigInt binpowmod(Botan::BigInt a, Botan::BigInt b, Botan::BigInt p) {
	Botan::BigInt res("1");
	
	a.operator%=(p);
	
	
	if (a.is_zero()) return Botan::BigInt("0");
	
	while (Botan::BigInt("0").is_less_than(b)) {
		if (b.is_odd()){
			res.operator*=(a);
			res.operator%=(p);
		}
			
		a.operator*=(a);
		a.operator%=(p);
		b.operator>>=(1);
	}
	
	return res;
}

string convert_to_bitstring1024(string str) 
{
	while (str.size() != 128) {
		str = str + '0';
	}

	string bit_string = "";

	for (long unsigned int i = 0; i < str.length(); ++i) {
        bitset<8> bs4(str[i]);

        for ( long int j = 7; j >= 0 ; --j ) {
        	if (bs4[j] == 1) {
        		bit_string  += '1';
        	} else {
        		bit_string  += '0';
        	}
        }
    }

    return bit_string;
}


string convert_to_4bitstring(string str) 
{

	string bit_string = "";

	for (long unsigned int i = 0; i < str.length(); ++i) {
        bitset<4> bs4(str[i]);

        for ( long int j = 3; j >= 0 ; j-- ) {
        	if (bs4[j] == 1) {
        		bit_string  += '1';
        	} else {
        		bit_string  += '0';
        	}
        }
    }

    return bit_string;
}

Botan::BigInt convert_datablock_to_number1024( string bit_string ) {
	
	Botan::BigInt two("2");
	Botan::BigInt pow("0");
	Botan::BigInt ans("0");
	for ( int i = bit_string.size() - 1; i >= 0; i--   ) {
		if ( bit_string[i] == '1' ) {
			ans.operator+=(binpow(two, pow));
		}

		pow.operator+=(1);
	}

	return ans;
}

#endif
