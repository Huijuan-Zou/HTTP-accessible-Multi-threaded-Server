//
//  HttpRequestParser.cpp
//  Lab2
//
//  Created by Huijuan Zou on 11/3/16.
//  Copyright © 2016 Huijuan Zou. All rights reserved.
//

#include "HttpRequestParser.hpp"

HttpRequestParser :: HttpRequestParser() {
};

/**
 * function to parse http request to information we need.
 * @return parsed information.
 **/
requestInfo HttpRequestParser :: requestParser(string request) {
    istringstream stream(request);
    string method;
    string userName;
    string source;
    string line;
    string contentLength;
    requestInfo info;
    
    if (!getline(stream, line)) {
        cout << "ERROR: parsing request\n";
    }
    istringstream lineStream(line);
    if (!getline(getline(getline(lineStream, method, ' '), userName, ' '), source, ' ')) {
        cout << "ERROR: parsing first line\n";
    }
    //get key
    info.key = userName;
    if (method == "GET") {
        info.type = GET;
    } else if (method == "POST") {
        info.type = POST;
    } else if (method == "DELETE") {
        info.type = DELETE;
    } else {
        cout << "ERROR: request type\n";
    }
    size_t found = string ::npos;
    while (found == string ::npos) {
        if (!getline(stream, line)) {
            return info;
        }
        found = line.find("Content-Length: ");
    }
    //get content length
    contentLength = line.substr(found + 15);
    //trim spaces, \t, \r
    contentLength.erase(remove(contentLength.begin(), contentLength.end(), ' '), contentLength.end());
    contentLength.erase(remove(contentLength.begin(), contentLength.end(), '\r'), contentLength.end());
    contentLength.erase(remove(contentLength.begin(), contentLength.end(), '\t'), contentLength.end());
    if (contentLength == "0") {
        return info;
    }
    while (!line.empty()) {
        if (!getline(stream, line)) {
            break;
        }
        line.erase(remove(line.begin(), line.end(), ' '), line.end());
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());
        line.erase(remove(line.begin(), line.end(), '\t'), line.end());
    }
    
    if (!getline(stream, line)) {
        cout << "ERROR: parsing request\n";
    }
    string value = line.substr(0, stoi(contentLength));
    value.erase(remove(value.begin(), value.end(), ' '), value.end());
    value.erase(remove(value.begin(), value.end(), '\r'), value.end());
    value.erase(remove(value.begin(), value.end(), '\t'), value.end());
    info.value = value;
    return info;
}