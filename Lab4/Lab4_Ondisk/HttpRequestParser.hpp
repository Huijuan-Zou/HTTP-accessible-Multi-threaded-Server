//
//  HttpRequestParser.hpp
//  Lab2
//
//  Created by Huijuan Zou on 11/3/16.
//  Copyright © 2016 Huijuan Zou. All rights reserved.
//

#ifndef HttpRequestParser_hpp
#define HttpRequestParser_hpp

#include <stdio.h>
#include <string>
#include <iostream>
#include <istream>
#include <sstream>
#include <mutex>
#include <algorithm>

using namespace std;

enum requestType {
    GET,
    POST,
    DELETE
};

struct requestInfo {
    requestType type;
    string key;
    string value;
    string contentLength;
};

class HttpRequestParser{
    public :
    requestInfo requestParser(string str);
    HttpRequestParser();
};
#endif /* HttpRequestParser_hpp */

