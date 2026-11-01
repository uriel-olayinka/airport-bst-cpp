#ifndef AIRPORTBST_H
#define AIRPORTBST_H

#include <string>
#include <vector>
#include "Airport.h"

class AirportBST {
public:
    AirportBST();
    ~AirportBST();
    AirportBST(const AirportBST&) = delete;
    AirportBST& operator=(const AirportBST&) = delete;

    bool insert(const Airport& a);
    void clear();
    int size() const;
    int height() const;
    double averageDepth() const;
    std::vector<std::string> inorderCodes() const;

    bool find(const std::string& code, Airport& result, std::vector<std::string>& path) const;
    bool minAirport(Airport& result) const;
    bool maxAirport(Airport& result) const;
    bool remove(const std::string& code, std::string& info);
    bool validate() const;
    std::vector<std::vector<std::string> > levelOrder(int levels) const;
        std::vector<Airport> inorderAirports() const;
    std::vector<std::string> preorderCodes() const;
    std::vector<std::string> levelOrderCodes() const;
    int range(const std::string& from, const std::string& to, std::vector<Airport>& result) const;
    bool contains(const std::string& code) const;
    void buildBalanced(const std::vector<Airport>& sortedAirports);
    void rebalance();

private:
    struct Node {
        Airport data;
        Node* left;
        Node* right;
        Node(const Airport& a) : data(a), left(nullptr), right(nullptr) {}
    };

    Node* buildRange(const std::vector<Airport>& a, int lo, int hi);
    Node* root;
    int count;
};

#endif