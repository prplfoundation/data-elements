//
//  WCCFProcMultiAP.cpp
//  WCCFProcMultiAP
//
//  Created by jeff on 11/2/18.
//  Copyright © 2018 jeff. All rights reserved.
//

#include "WCCFProcMultiAP.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <rapidjson/istreamwrapper.h>
#include "rapidjson/prettywriter.h"
#include <algorithm>
#include <fstream>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <set>

WCCFProcMultiAP::WCCFProcMultiAP(string input_path, string output_path) {
    inputPath = input_path;
    outputPath = output_path;
}

int WCCFProcMultiAP::testForInputFiles() {
    DIR           *dirp;
    struct dirent *directory;
    int filesFound = 0;

    dirp = opendir(inputPath.c_str());
    if (dirp) {
        while ((directory = readdir(dirp)) != NULL) {
            if (directory->d_type != DT_REG)
                continue;
            filesFound++;
        }
    }
    closedir(dirp);
    return filesFound;
}

void WCCFProcMultiAP::processDataFiles() {
    DIR           *dirp;
    struct dirent *directory;
    
    while ( 5 >= testForInputFiles() ) sleep(2);

    dirp = opendir(inputPath.c_str());
    if (dirp) {
        while ((directory = readdir(dirp)) != NULL) {
            if (directory->d_type != DT_REG)
                continue;
            reportFiles.push_back(string(directory->d_name));
        }
        closedir(dirp);
        extractUniqueReportElements();
    }
}

string stripExtension(const string& str) {
    size_t lastindex = str.find_last_of(".");
    return str.substr(0, lastindex);
}

int convertToRCPI(int signalStrength) {
    //
    // IEEE Std 802.11TM-2016 table 9-154 (RCPI)
    //
    if ((signalStrength < 0) && (signalStrength > -109)) {
        signalStrength = 2 * (signalStrength + 110);
    } else if (signalStrength > 0) {
        signalStrength = 220;
    } else if (signalStrength <= -109) {
        signalStrength = 0;
    }
    return signalStrength;
}

int normalizeToUnsignedByte(int input) {
    if (input < 0)
        return 127 + abs(input);
    return input;
}

vector<string> split(const string& str, const string& delim) {
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do {
        pos = str.find(delim, prev);
        if (pos == string::npos)
            pos = str.length();
        string token = str.substr(prev, pos-prev);
        if (!token.empty())
            tokens.push_back(token);
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
}

void WCCFProcMultiAP::extractUniqueReportElements() {
    string underscore = string("_");
    string dash = string("-");
    
    for (ReportFiles::iterator i=reportFiles.begin(); i != reportFiles.end(); ++i) {
        string reportName = stripExtension(*i);
        vector<string> nameParts = split(reportName, underscore);
        reportAddrs.push_back(nameParts[0]);
        reportTimes.push_back(nameParts[2]);
        vector<string> nameRadio = split(nameParts[1], dash);
        reportNames.push_back(nameRadio[0]);
        if (2 == nameRadio.size())
            reportRadios.push_back(nameRadio[1]);
    }
    
    sort(reportAddrs.begin(), reportAddrs.end());
    reportAddrs.resize( distance(reportAddrs.begin(), unique(reportAddrs.begin(), reportAddrs.end())));
    
    sort(reportTimes.begin(), reportTimes.end());
    reportTimes.resize( distance(reportTimes.begin(), unique(reportTimes.begin(), reportTimes.end())));
    
    sort(reportNames.begin(), reportNames.end());
    reportNames.resize( distance(reportNames.begin(), unique(reportNames.begin(), reportNames.end())));
    
    sort(reportRadios.begin(), reportRadios.end());
    reportRadios.resize( distance(reportRadios.begin(), unique(reportRadios.begin(), reportRadios.end())));
    
    return;
}

int WCCFProcMultiAP::getInterfaceReport(string id, string &testTime, Document& result) {
    for (ReportNames::iterator i=reportNames.begin(); i != reportNames.end(); ++i) {
        if ((*i).compare("interface") == 0) {
            string file_name = id + "_" + *i + "_";
            for (ReportTimes::iterator j=reportTimes.begin(); j != reportTimes.end(); ++j) {
                string test_name = file_name + *j + ".json";
                for (ReportFiles::iterator k=reportFiles.begin(); k != reportFiles.end();++k) {
                    if ((*k).compare(test_name) == 0) {
                        string full_path = inputPath + "/" + (*k);
                        ifstream ifs(full_path);
                        IStreamWrapper isw(ifs);
                        result.ParseStream(isw);
                        if(result.HasParseError())
                            return 1;
                        testTime = (*j);
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}

// Note:
// Normally scans execute 60 seconds before other reports because of the effect
// other reports have on the scan results. SO we need to subtract time from the
// normal report time string to look for the related scan data files. 10/30/18
// 11:45 data will have a scan timestamp of 10/30/18 11:44.
//
string parseTimeAndOffset(string inputTime, unsigned offset) {
    time_t result = 0;
    string offsetTime;
    struct tm *atime = NULL;
    int year, month, day, hour, minute, second;
    
    year    = stoi(inputTime.substr(0, 4));
    month   = stoi(inputTime.substr(4, 2));
    day     = stoi(inputTime.substr(6, 2));
    hour    = stoi(inputTime.substr(8, 2));
    minute  = stoi(inputTime.substr(10, 2));
    second  = stoi(inputTime.substr(12, 2));
    
    time(&result);
    
    atime = gmtime( &result );
    atime->tm_year = year - 1900;
    atime->tm_mon = month - 1;
    atime->tm_mday = day;
    atime->tm_hour = hour;
    atime->tm_min = minute;
    atime->tm_sec = second;
    
    putenv((char *)"TZ=UTC"); // Hack to force us to stay in UTC
    result = mktime(atime);
    result -= offset;         // subtract offset seconds from time
    
    atime = localtime( &result );
    char offsetCStr[14];
    strftime(offsetCStr, 14, "%Y%m%d%H%M", atime);
    
    offsetTime = string(offsetCStr);
    
    return offsetTime;
}

int WCCFProcMultiAP::getScanReport(string id, string testTime, string name, Document& result, unsigned offset) {
    string offsetTime = parseTimeAndOffset(testTime, offset);
    
    for (ReportFiles::iterator i=reportFiles.begin(); i != reportFiles.end();++i) {
        string test_name = id + "_scan-" + name + "_" + offsetTime;
        string reportFileTruncated = (*i).substr(0, test_name.length());
        if (reportFileTruncated.compare(test_name) == 0) {
            string full_path = inputPath + "/" +  (*i);
            ifstream ifs(full_path);
            IStreamWrapper isw(ifs);
            result.ParseStream(isw);
            if(result.HasParseError())
                return 1;
            return 0;
        }
    }
    return 1;
    
}

int WCCFProcMultiAP::getChannelReport(string id, string testTime, string name, Document& result) {
    for (ReportFiles::iterator i=reportFiles.begin(); i != reportFiles.end();++i) {
        string test_name = id + "_channel-" + name + "_" + testTime + ".json";
        if ((*i).compare(test_name) == 0) {
            string full_path = inputPath + "/" +  test_name;
            ifstream ifs(full_path);
            IStreamWrapper isw(ifs);
            result.ParseStream(isw);
            if(result.HasParseError())
                return 1;
            return 0;
        }
    }
    return 1;
}

int WCCFProcMultiAP::getStationReport(string id, string testTime, string name, Document& result) {
    for (ReportFiles::iterator i=reportFiles.begin(); i != reportFiles.end();++i) {
        string test_name = id + "_station-" + name + "_" + testTime + ".json";
        if ((*i).compare(test_name) == 0) {
            string full_path = inputPath + "/" +  test_name;
            ifstream ifs(full_path);
            IStreamWrapper isw(ifs);
            result.ParseStream(isw);
            if(result.HasParseError())
                return 1;
            return 0;
        }
    }
    return 1;
}

int WCCFProcMultiAP::getPhyCapaReport(string id, string testTime, string name, Document& result) {
    for (ReportFiles::iterator i=reportFiles.begin(); i != reportFiles.end();++i) {
        string test_name = id + "_phycapa-" + name + "_" + testTime + ".json";
        if ((*i).compare(test_name) == 0) {
            string full_path = inputPath + "/" +  test_name;
            ifstream ifs(full_path);
            IStreamWrapper isw(ifs);
            result.ParseStream(isw);
            if(result.HasParseError())
                return 1;
            return 0;
        }
    }
    return 1;
}

void WCCFProcMultiAP::generateNetworkReport(unsigned collectionInterval, unsigned scanOffset) {
    
    for (ReportAddrs::iterator i = reportAddrs.begin(); i != reportAddrs.end(); ++i) {
        networkDoc.SetObject();
        Document::AllocatorType& alloc = networkDoc.GetAllocator();
        Value object(rapidjson::kObjectType);
        string testTime;
        
        if (getInterfaceReport(string((*i).c_str()), testTime, interfaceDoc[interfaceIndex])) {
            cout << "error parsing interface document for " << *i << endl;
            continue;
        }

        const Value &dev = interfaceDoc[interfaceIndex++]["Device"];
        assert(dev.IsObject());
        const Value &wif = dev["Wifi"];
        assert(wif.IsObject());
        const Value &ifc = wif["Interface"];
        assert(ifc.IsArray());
        string stamp = ifc[0]["TimeStamp"].GetString();
        string addrStr = *i;

        Value addr(addrStr.c_str(), alloc);
        object.AddMember("ID", addr, alloc);
        Value tstamp(stamp.c_str(), alloc);
        object.AddMember("TimeStamp", tstamp, alloc);
        object.AddMember("ControllerID", addr, alloc); // TODO JSP What should this be?
        object.AddMember("NumberOfDevices", 1, alloc);
        
        Value deviceList(kArrayType);
        {
        Value device(kObjectType);
            {
            Value addr((*i).c_str(), alloc);
            device.AddMember("ID", addr, alloc);
            Value tstamp(stamp.c_str(), alloc);
            device.AddMember("TimeStamp", tstamp, alloc);
            device.AddMember("MultiAPCapabilities", "", alloc);  // TODO JSP What should this be?
            device.AddMember("CollectionInterval", collectionInterval, alloc);
            unsigned ifcCount = 0;
            Value radioList(kArrayType);
                {
                for (auto& i : ifc.GetArray()) {
                    if (i.HasMember("ChannelWidth")) {
                        Value radio(kObjectType);
                        {
                        string idStr = i["APIfIndex"].GetString();
                        Value idVal(idStr.c_str(), alloc);
                        radio.AddMember("ID", idVal, alloc);
                        string apName = i["APName"].GetString();

                        if (getChannelReport(addrStr, testTime, apName, channelDoc[channelIndex])) {
                            continue;
                        }
                        const Value &cdev = channelDoc[channelIndex++]["Device"];
                        assert(cdev.IsObject());
                        const Value &cwif = cdev["Wifi"];
                        assert(cwif.IsObject());
                        const Value &chan = cwif["Channel"];
                        assert(chan.IsArray());
                        for (auto& j : chan.GetArray()) {
                            const int enabled = j["Enabled"].GetInt();
                            if (enabled != 0) {
                                radio.AddMember("Enabled", Value(true), alloc);
                                Value curOpCl(kArrayType);
                                radio.AddMember("CurrentOperatingClasses", curOpCl, alloc); //TODO JSP add results?
                                radio.AddMember("NumberOfCurrOpClass", 0, alloc);
                                int noise = convertToRCPI(j["Noise"].GetInt());
                                radio.AddMember("Noise", noise, alloc);
                                int utilization = j["Utilization"].GetInt();
                                radio.AddMember("Utilization", utilization, alloc);
                                int transmit = j["Transmit"].GetInt();
                                radio.AddMember("Transmit", transmit, alloc);
                                radio.AddMember("ReceiveSelf", 0, alloc); // TODO JSP What should this be?
                                radio.AddMember("ReceiveOther", 0, alloc); // TODO JSP What should this be?

                                if (getPhyCapaReport(addrStr, testTime, apName, phyCapaDoc[phyCapaIndex])) {
                                    continue;
                                }
                                
                                Value capabilities(kObjectType);
                                {
                                const Value &pdev = phyCapaDoc[phyCapaIndex++]["Device"];
                                assert(cdev.IsObject());
                                const Value &pwif = pdev["Wifi"];
                                assert(cwif.IsObject());
                                const Value &capa = pwif["PhyCapa"];
                                for (auto& k : capa.GetArray()) {
                                    if (k.HasMember("HTCapabilities")) {
                                        const string htcapa = k["HTCapabilities"].GetString();
                                        Value htval(htcapa.c_str(), alloc);
                                        capabilities.AddMember("HTCapabilities", htval, alloc);
                                    }
                                    if (k.HasMember("VHTCapabilities")) {
                                        const string vhtcapa = k["VHTCapabilities"].GetString();
                                        Value vhtval(vhtcapa.c_str(), alloc);
                                        capabilities.AddMember("VHTCapabilities", vhtval, alloc);
                                    }
                                    const string hecapa = ""; // TODO JSP how to get this value?
                                    Value heval(hecapa.c_str(), alloc);
                                    capabilities.AddMember("HECapabilities", heval, alloc);
                                    
                                    Value operatingClasses(kArrayType);// TODO JSP how to get this value?
                                    capabilities.AddMember("OperatingClasses", operatingClasses, alloc);
                                    const int numberOfOpClasses = 0;
                                    capabilities.AddMember("NumberOfOpClasses", numberOfOpClasses, alloc);
                                }
                                }
                                radio.AddMember("Capabilities", capabilities, alloc);
                                
                                Value backhaulSta(kObjectType);// TODO JSP how to get this value?
                                radio.AddMember("BackhaulSta", backhaulSta, alloc);
                                
                                Value unassociatedStaList(kArrayType);// TODO JSP how to get this value?
                                radio.AddMember("UnassociatedStaList", unassociatedStaList, alloc);

                                const int numberOfUnassocSta = 0;
                                radio.AddMember("NumberOfUnassocSta", numberOfUnassocSta, alloc);
                                
                                if (getScanReport(addrStr, testTime, apName, scanDoc[scanIndex], scanOffset)) {
                                    continue;
                                }
                                
                                const Value &sdev = scanDoc[scanIndex++]["Device"];
                                assert(sdev.IsObject());
                                const Value &swif = sdev["Wifi"];
                                assert(swif.IsObject());
                                const Value &scan = swif["Scan"];
                                
                                Value scanResultList(kObjectType);
                                {
                                Value opClassScanList(kArrayType); // TODO JSP where to get these data?
                                    {
                                    Value opClassItem(kObjectType);
                                        {
                                        const int numberOfOpClassScans = 1;
                                        scanResultList.AddMember("NumberOfOpClassScans", numberOfOpClassScans, alloc);
                                        Value opstamp(stamp.c_str(), alloc);
                                        scanResultList.AddMember("TimeStamp", opstamp, alloc);
                                        Value channelScanList(kArrayType);
                                            {
                                            //
                                            // first get the set of unique channels
                                            //
                                            set<int> uniqueChannels;
                                            for (auto& l : scan.GetArray()) {
                                                if (l.HasMember("ChannelNumber")) {
                                                    uniqueChannels.insert(l["ChannelNumber"].GetInt());
                                                }
                                            }
                                            //
                                            // now report on the unique channels and their neighbors
                                            //
                                            for (set<int>::iterator m = uniqueChannels.begin(); m != uniqueChannels.end(); ++m) {
                                                Value channel(kObjectType);
                                                int chan = *m;
                                                {
                                                channel.AddMember("Channel", chan, alloc);
                                                Value opstamp(stamp.c_str(), alloc);
                                                channel.AddMember("TimeStamp", opstamp, alloc);
                                                channel.AddMember("Noise", 0, alloc); // TODO JSP where to get this number?
                                                channel.AddMember("Utilization", 0, alloc); // TODO JSP where to get this number?
                                                }
                                                Value neighborList(kArrayType);
                                                {
                                                int numberOfNeighbors = 0;
                                                for (auto& n : scan.GetArray()) {
                                                    int channelNumber = n["ChannelNumber"].GetInt();
                                                    if (channelNumber == chan) {
                                                        Value neighbor(kObjectType);
                                                        {
                                                        numberOfNeighbors++;
                                                        string bssid = n["RemoteMACAddress"].GetString();
                                                        Value bssidVal(bssid.c_str(), alloc);
                                                        neighbor.AddMember("BSSID", bssidVal, alloc);
                                                        string ssid = n["SSID"].GetString();
                                                        Value ssidVal(ssid.c_str(), alloc);
                                                        neighbor.AddMember("SSID", ssidVal, alloc);
                                                        neighbor.AddMember("StationCount", 1, alloc); // TODO JSP where to get this number?
                                                        
                                                        //
                                                        // IEEE Std 802.11TM-2016 table 9-154 (RCPI)
                                                        //
                                                        int signalStrength = convertToRCPI(n["Signal"].GetInt());
                                                        
                                                        neighbor.AddMember("SignalStrength", signalStrength, alloc);
                                                        neighbor.AddMember("ChannelBandwidth", 0, alloc); // TODO JSP where to get this number?
                                                        neighbor.AddMember("ChannelUtilization", 0, alloc); // TODO JSP where to get this number?
                                                        }
                                                        neighborList.PushBack(neighbor, alloc);
                                                    }
                                                }
                                                channel.AddMember("NumberOfNeighbors", numberOfNeighbors, alloc);
                                                channel.AddMember("NeighborList", neighborList, alloc);
                                                }
                                                
                                                channelScanList.PushBack(channel, alloc);
                                            }
                                            }
                                        opClassItem.AddMember("ChannelScanList",channelScanList, alloc);
                                        }
                                    opClassScanList.PushBack(opClassItem, alloc);
                                    }
                                scanResultList.AddMember("OpClassScanList", opClassScanList, alloc);
                                }
                                radio.AddMember("ScanResultList", scanResultList, alloc);
                                
                                int numberOfBSS = 0;
                                Value bSSList(kArrayType);
                                {
                                if (getStationReport(addrStr, testTime, apName, stationDoc[stationIndex])) {
                                    continue;
                                }
                                
                                const Value &stdev = stationDoc[stationIndex++]["Device"];
                                assert(stdev.IsObject());
                                const Value &stwif = stdev["Wifi"];
                                assert(stwif.IsObject());
                                const Value &station = stwif["Station"];
                                
                                Value bSSItem(kObjectType);
                                Value sTAList(kArrayType);
                                int numberOfSta = 0;
                                for (auto& o : station.GetArray()) {
                                    string bssts;
                                    {
                                    if (numberOfBSS == 0) {
                                        numberOfBSS++;
                                        string bssid = o["ID"].GetString();
                                        Value bssidVal(bssid.c_str(), alloc);
                                        bSSItem.AddMember("BSSID", bssidVal, alloc);
                                        string ssid = ""; // TODO JSP get this from scan?
                                        Value ssidVal(ssid.c_str(), alloc);
                                        bSSItem.AddMember("SSID", ssidVal, alloc);
                                        bool enabled = true;
                                        bSSItem.AddMember("Enabled", enabled, alloc);
                                        //int lastChange = o["LastChange"].GetInt(); // TODO JSP where to get this number?
                                        //bSSItem.AddMember("LastChange", lastChange, alloc);
                                        bssts = o["TimeStamp"].GetString();
                                        Value bsstsVal(bssts.c_str(), alloc);
                                        bSSItem.AddMember("TimeStamp", bsstsVal, alloc);
                                        bSSItem.AddMember("UnicastBytesSent", o["UnicastBytesSent"].GetInt(), alloc);
                                        bSSItem.AddMember("UnicastBytesReceived", o["UnicastBytesReceived"].GetInt(), alloc);
                                        bSSItem.AddMember("MulticastBytesSent", 0, alloc); // TODO JSP where to get this number?
                                        bSSItem.AddMember("MulticastBytesReceived", 0, alloc); // TODO JSP where to get this number?
                                        bSSItem.AddMember("BroadcastBytesSent", 0, alloc); // TODO JSP where to get this number?
                                        bSSItem.AddMember("BroadcastBytesReceived", 0, alloc); // TODO JSP where to get this number?
                                    }
                                    Value sTAItem(kObjectType);
                                        {
                                        numberOfSta++;

                                        string macaddr = o["MACAddress"].GetString();
                                        Value macaddrVal(macaddr.c_str(), alloc);
                                        sTAItem.AddMember("MACAddress", macaddrVal, alloc);

                                        string sts = o["TimeStamp"].GetString();
                                        Value statsVal(sts.c_str(), alloc);
                                        sTAItem.AddMember("TimeStamp", statsVal, alloc);

                                        string vhtCapa = "";
                                        Value vhtCapaVal(vhtCapa.c_str(), alloc);
                                        sTAItem.AddMember("VHTCapabilities", vhtCapaVal, alloc);
                                        
                                        string htCapa = "";
                                        Value htCapaVal(htCapa.c_str(), alloc);
                                        sTAItem.AddMember("HTCapabilities", htCapaVal, alloc);

                                        int signalStrength = convertToRCPI(o["SignalStrength"].GetInt());
                                        sTAItem.AddMember("SignalStrength", signalStrength, alloc);

                                        string lastCon = "";
                                        Value lastConVal(lastCon.c_str(), alloc);
                                        sTAItem.AddMember("LastConnectionTime", lastConVal, alloc);

                                        float lastDataDownlinkRate = o["LastDataDownlinkRate"].GetFloat();
                                        int iLastDataDownlinkRate = lastDataDownlinkRate;
                                        sTAItem.AddMember("LastDataDownlinkRate", iLastDataDownlinkRate, alloc);

                                        float lastDataUplinkRate = o["LastDataUplinkRate"].GetFloat();
                                        int iLastDataUplinkRate = lastDataUplinkRate;
                                        sTAItem.AddMember("LastDataUplinkRate", iLastDataUplinkRate, alloc);

                                        sTAItem.AddMember("EstMACDataRateDownlink", 0, alloc);
                                        sTAItem.AddMember("EstMACDataRateUplink", 0, alloc);

                                        sTAItem.AddMember("UtilizationReceive", 0, alloc);
                                        sTAItem.AddMember("UtilizationTransmit", 0, alloc);

                                        sTAItem.AddMember("BytesSent", 0, alloc);
                                        sTAItem.AddMember("BytesReceived", 0, alloc);

                                        sTAItem.AddMember("PacketsSent", o["PacketsSent"].GetInt(), alloc);
                                        sTAItem.AddMember("PacketsReceived", o["PacketsReceived"].GetInt(), alloc);

                                        sTAItem.AddMember("ErrorsSent", o["ErrorsSent"].GetInt(), alloc);
                                        sTAItem.AddMember("ErrorsReceived", 0, alloc); // TODO JSP where to get this number?
                                        sTAItem.AddMember("RetransCount", o["RetransCount"].GetInt(), alloc);

                                        string iPV6Address = "";
                                        Value iPV6AddressVal(iPV6Address.c_str(), alloc);
                                        sTAItem.AddMember("IPV6Address", iPV6AddressVal, alloc);

                                        string iPV4Address = "";
                                        Value iPV4AddressVal(iPV4Address.c_str(), alloc);
                                        sTAItem.AddMember("IPV4Address", iPV4AddressVal, alloc);

                                        string hostname = "";
                                        Value hostnameVal(hostname.c_str(), alloc);
                                        sTAItem.AddMember("Hostname", hostnameVal, alloc);

                                        sTAItem.AddMember("NumberOfMeasurementReports", 0, alloc);
                                        Value measReport(kArrayType);
                                        sTAItem.AddMember("MeasurementReport", measReport, alloc);
                                        }
                                    
                                    sTAList.PushBack(sTAItem, alloc);
                                    }
                                }
                                bSSItem.AddMember("NumberOfSTA", numberOfSta, alloc);
                                bSSItem.AddMember("STAList", sTAList, alloc);

                                string estServiceParametersBE = "";
                                Value estServiceParametersBEVal(estServiceParametersBE.c_str(), alloc);
                                bSSItem.AddMember("EstServiceParametersBE", estServiceParametersBEVal, alloc);

                                string estServiceParametersBK = "";
                                Value estServiceParametersBKVal(estServiceParametersBK.c_str(), alloc);
                                bSSItem.AddMember("EstServiceParametersBK", estServiceParametersBKVal, alloc);

                                string estServiceParametersVI = "";
                                Value estServiceParametersVIVal(estServiceParametersVI.c_str(), alloc);
                                bSSItem.AddMember("EstServiceParametersVI", estServiceParametersVIVal, alloc);

                                string estServiceParametersVO = "";
                                Value estServiceParametersVOVal(estServiceParametersVO.c_str(), alloc);
                                bSSItem.AddMember("EstServiceParametersVO", estServiceParametersVOVal, alloc);

                                bSSList.PushBack(bSSItem, alloc);
                                }
                                radio.AddMember("NumberOfBSS", numberOfBSS, alloc);
                                radio.AddMember("BSSList", bSSList, alloc);
                            }
                        }
                        }
                        radioList.PushBack(radio, alloc);
                        ifcCount++;
                    }
                }
                device.AddMember("NumberOfRadios", ifcCount, alloc);
                device.AddMember("RadioList", radioList, alloc);
                }
            deviceList.PushBack(device, alloc);
            }
        }
        
        object.AddMember("DeviceList", deviceList, alloc);
        
        networkDoc.AddMember("wfa-dataelements:Network", object, alloc);
        
        StringBuffer strbuf;
        PrettyWriter<StringBuffer> writer(strbuf);
        networkDoc.Accept(writer);
        string outputFileName = outputPath + "/" + string((*i)) + "_multiap_" + testTime + ".json";
        ofstream output;
        output.open(outputFileName, ios::out);
        output << strbuf.GetString() << std::endl;
        output.close();
    }
    
    return;
}

void WCCFProcMultiAP::purgeDataFiles() {
    for (ReportFiles::const_iterator i = reportFiles.begin(); i != reportFiles.end(); ++i) {
        string fname = inputPath + "/" + *i;
        remove(fname.c_str());
    }
}
