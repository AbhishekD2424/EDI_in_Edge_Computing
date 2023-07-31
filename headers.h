#ifndef HEADERS_H
#define HEADERS_H

#include <bits/stdc++.h>

#include <botan/auto_rng.h>
#include <botan/rsa.h>
#include <botan/bigint.h>
#include <botan/hex.h>
#include <botan/hmac.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "conversion.h"
#include "calculate.h"
#include "readfiledata.h"

using namespace std;


#define LINE_SIZE 3000


struct file {
	char name[LINE_SIZE];
};

struct challenge {
	char name[LINE_SIZE];
	char challenge[LINE_SIZE];
	char n[LINE_SIZE];
	char g[LINE_SIZE];
};

struct poi {
	char poi[LINE_SIZE];
};

struct message {
	int type;
	int size;
	file f;
	challenge c;
	poi p;
};


#endif
