#ifndef AIRPORT_H
#define AIRPORT_H

#include <string>

// One airport record. It only holds data, so every field is public
class Airport {
public:
    std::string code;           // the BST key
    std::string name;
    std::string city;
    std::string country;
    std::string icao;
    double latitude;
    double longitude;

    Airport() : latitude(0.0), longitude(0.0) {}
};

#endif