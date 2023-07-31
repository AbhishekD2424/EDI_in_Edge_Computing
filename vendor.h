#ifndef VENDOR_H
#define VENDOR_H

using namespace std;

Botan::BigInt gcd(Botan::BigInt a, Botan::BigInt b)
{
    if (a.is_zero())
        return b;
    return gcd(b.operator%=(a), a);
}


void binary_search ( vector<Botan::BigInt> received_pois, vector<Botan::BigInt> calculated_pois, int l, int r, Botan::BigInt n, vector<bool> &result ) {

	Botan::BigInt R ("1");
	Botan::BigInt R_ ("1");
	// cout << l << " " << r << "\n";


	if ( l < r  ) {
		for ( int i = l ; i <= r; i++ ) {
			R.operator*=(received_pois[i]);
			R.operator%=(n);
			R_.operator*=(calculated_pois[i]);
			R_.operator%=(n);
		}

		if ( R.is_equal(R_) ) {
			for ( int i = l ; i <= r; i++ ) {
				result[i] = false;
			}
		} else {
			int mid = ( l + r ) >> 1;
			binary_search(received_pois, calculated_pois, l, mid, n,  result);
			binary_search(received_pois, calculated_pois, mid + 1, r, n,  result);

		}		
	} else if ( l == r ){
		if ( received_pois[l].is_equal(calculated_pois[l]) ) {
			result[l] = false;
		}
	} else {
		return;
	}


}

#endif
