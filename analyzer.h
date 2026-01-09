// ===================== YOUR LIBRARY IMPORTS =====================
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <filesystem>
#include <fstream>
// ===================== analyzer.h (inlined) =====================

struct ZoneStats {
    long long totalCount = 0;
    long long hourCount[24] = {0};
};
struct ZoneCount {
    std::string zone;
    long long count;
};

struct SlotCount {
    std::string zone;
    int hour;
    long long count;
};

struct SlotKey {
    std::string zone;
    int hour;
    bool operator==(const SlotKey& other) const {
        return hour == other.hour && zone == other.zone;
    }
};

struct SlotKeyHash {
    size_t operator()(const SlotKey& k) const {
        size_t h1 = std::hash<std::string>{}(k.zone);
        size_t h2 = std::hash<int>{}(k.hour);
        return h1 ^ (h2 << 1);
    }
};

class TripAnalyzer {
public:
    // Parse Trips.csv, skip dirty rows, never crash
    void ingestFile(const std::string& csvPath);
    
    // Top K zones: count desc, zone asc
    std::vector<ZoneCount> topZones(int k = 10) const;

    // Top K slots: count desc, zone asc, hour asc
    std::vector<SlotCount> topBusySlots(int k = 10) const;
private:
    //std::unordered_map<std::string, long long> zone_map;
    //std::unordered_map<SlotKey, long long, SlotKeyHash> slot_map;
    std::unordered_map<std::string, ZoneStats> map;
    
};

