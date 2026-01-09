#include "analyzer.h"

void TripAnalyzer::ingestFile(const std::string& csvPath) {
    std::ifstream data(csvPath);
    if(!data.is_open())
        return;
    if (std::filesystem::is_empty(csvPath)) return;
    map.clear();
    
    std::string line;
    std::string zone;
    
    line.reserve(256);
    zone.reserve(15);
    const char* commas[5];

    std::getline(data, line);
    while (std::getline(data, line)) {
        if (line.empty()) continue;
        const char* p = &line[0];
        int found = 0;
        while (*p && found < 5) {
            if (*p == ',')
                commas[found++] = p;
            p++;
        }
        if (found < 5) continue;
        const char* zoneStart = commas[0] + 1;
        size_t zoneLen = commas[1] - zoneStart;
        if (zoneLen == 0) continue;
        
        const char* timeStart = commas[2] + 1;
        size_t timeLen = commas[3] - timeStart;
        if (timeLen < 13) continue;
        
        const char* spacePos = timeStart;
        while (spacePos < commas[3] && *spacePos != ' ') spacePos++;
        if (spacePos == commas[3]) continue;
        
        const char* hourPtr = spacePos + 1;
        if (hourPtr >= commas[3]) continue;

        int hour = 0;
        if (*hourPtr >= '0' && *hourPtr <= '9') {
            hour = *hourPtr - '0';
            hourPtr++;
            if (hourPtr < commas[3] && *hourPtr >= '0' && *hourPtr <= '9') {
                hour = hour * 10 + (*hourPtr - '0');
            }
        }
        else {
            continue;
        }
        if (hour > 23) continue;
        
        zone.assign(zoneStart, zoneLen);
        ZoneStats& stats = map[zone];
        stats.totalCount++;
        stats.hourCount[hour]++;
    }
}

std::vector<ZoneCount> TripAnalyzer::topZones(int k) const {
    std::vector<ZoneCount> zone_vector;
    zone_vector.reserve(map.size());

    for (const auto& element : map) {
        zone_vector.push_back({ element.first, element.second.totalCount });
    }
    
    auto sorter = [](const ZoneCount& a, const ZoneCount& b){
        if ( a.count != b.count )return a.count > b.count;
        return a.zone < b.zone; };
    if (k >= (int)zone_vector.size()) {
        std::sort(zone_vector.begin(), zone_vector.end(), sorter);
        return zone_vector;
    }

    std::partial_sort(zone_vector.begin(), zone_vector.begin() + k, zone_vector.end(), sorter);

    zone_vector.resize(k);
    return zone_vector;
}

std::vector<SlotCount> TripAnalyzer::topBusySlots(int k) const {
    std::vector<SlotCount> slot_vector;
    slot_vector.reserve(map.size());
    for (const auto& [key, count] : map) {
        for (int h = 0; h < 24; ++h) {
            if (count.hourCount[h] > 0) {
                slot_vector.push_back({ key, h, count.hourCount[h] });
            }
        }
    }
    
    auto sorter = [](const SlotCount& a, const SlotCount& b){
        if(a.count != b.count) return a.count > b.count;
        if(a.zone != b.zone) return a.zone < b.zone;
        return a.hour < b.hour; };
    
    if (k >= (int)slot_vector.size()) { 
        std::sort(slot_vector.begin(), slot_vector.end(), sorter);
        return slot_vector;
    }

    std::partial_sort(
        slot_vector.begin(),
        slot_vector.begin() + k,
        slot_vector.end(),
        sorter);

    slot_vector.resize(k);
    return slot_vector;
}
