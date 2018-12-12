//
//  WCCFProcMultiAP.h
//  WCCFProcMultiAP
//
//  Created by jeff on 11/2/18.
//  Copyright © 2018 jeff. All rights reserved.
//

#ifndef WCCFProcMultiAP_h
#define WCCFProcMultiAP_h

#include <string>
#include <vector>
#include <dirent.h>
#include "rapidjson/document.h"

using namespace std;
using namespace rapidjson;

#define MAX_INTERFACE_REPORTS 10
#define MAX_CHANNEL_REPORTS   10
#define MAX_PHYCAPA_REPORTS   10
#define MAX_SCAN_REPORTS      10
#define MAX_STATION_REPORTS   10

typedef vector<string> ReportFiles;
typedef vector<string> ReportTimes;
typedef vector<string> ReportNames;
typedef vector<string> ReportAddrs;
typedef vector<string> ReportRadios;

typedef enum { Interface, Channel, Station, Scan, PhyCapa } ReportType;

class WCCFProcMultiAP {
public:
    WCCFProcMultiAP(string input_path, string output_path);
    void processDataFiles();
    void generateNetworkReport(unsigned interval, unsigned scanOffset);
    void purgeDataFiles();
private:
    int testForInputFiles();
    void extractUniqueReportElements();
    int getInterfaceReport(string id, string &testTime, Document& result);
    int getChannelReport(string id, string testTime, string name, Document& result);
    int getPhyCapaReport(string id, string testTime, string name, Document& result);
    int getScanReport(string id, string testTime, string name, Document& result, unsigned offset);
    int getStationReport(string id, string testTime, string name, Document& result);
    
    Document networkDoc;
    Document interfaceDoc[MAX_INTERFACE_REPORTS];
    Document channelDoc[MAX_CHANNEL_REPORTS];
    Document phyCapaDoc[MAX_PHYCAPA_REPORTS];
    Document scanDoc[MAX_SCAN_REPORTS];
    Document stationDoc[MAX_STATION_REPORTS];
    
    unsigned interfaceIndex = 0;
    unsigned channelIndex = 0;
    unsigned phyCapaIndex = 0;
    unsigned scanIndex = 0;
    unsigned stationIndex = 0;

    string          inputPath;
    string          outputPath;
    ReportFiles     reportFiles;
    ReportTimes     reportTimes;
    ReportNames     reportNames;
    ReportAddrs     reportAddrs;
    ReportRadios    reportRadios;
};

#endif /* WCCFProcMultiAP_h */
