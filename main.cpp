#include <algorithm>
#include <iostream>
#include <iomanip>
#include <memory>
#include <sstream>
#include <unistd.h>
#include <vector>
#include "stdlib.h" 
#include "EthLayer.h"
#include "IPv4Layer.h"
#include "Packet.h"
#include "PcapLiveDeviceList.h"
#include "SystemUtils.h"

void printUsage();
std::string incrementMac(std::string MACAddr, int incr);

int main(int argc, char** argv) {
    // TODO: fix this
    /**
    // Process command line args
    if (argc == 1) {
        printUsage();
        return 0;
    }

    int opt;
    std::string interfaceAddr;
    while ((opt = getopt(argc, argv, "am")) != -1) {
        switch(opt) {
            case 'a':
                interfaceAddr = optarg;
                std::cout << interfaceAddr;
                break;
            case 'm':
                interfaceAddr = optarg;
                break;
            default:
                printUsage();
                return 0;
        }
    }

   */ 

    // start

    // Put your current IP address here
    std::string interfaceAddr = "";

    if (interfaceAddr.empty()) {
        std::cout 
            << "You need to input an IP address for this to work. (see line 43)"
            << std::endl;
        return 0;
    }

    auto* device =
        pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(interfaceAddr);

    if (device == nullptr) {
        std::cerr << "Cannot find interface with address [" << interfaceAddr
            << "]" << std::endl;
        return 1;
    }

    // Open the device
    if (!device->open()) {
        std::cerr << "Cannot open device: " <<  device->getName() << std::endl;
    }

    std::cout << std::endl << "Starting capture..." << std::endl;

    // create empty packet vector
    pcpp::RawPacketVector rawPacketVec;

    // Asynchronous capture, TODO change to synchronous
    device->startCapture(rawPacketVec);

    // Sleep for x secs while the packets are captured
    pcpp::multiPlatformSleep(2);

    // stop capture
    device->stopCapture();

    // Variable used to increment the mac in below loop
    int inc = 0;

    for (const auto& packet : rawPacketVec) {
        pcpp::Packet parsedPacket(packet);
        auto* ethernetLayer = parsedPacket.getLayerOfType<pcpp::EthLayer>();
        auto* ipLayer = parsedPacket.getLayerOfType<pcpp::IPv4Layer>();

        if (ethernetLayer == nullptr) {
            std::cerr << "Error, couldn't find Ethernet layer" << std::endl;
            return 1;
        }

        if (ipLayer == nullptr) {
            std::cerr << "Error, couldn't find IP layer" << std::endl;
            return 1;
        }

        std::cout 
            << "Original Packet ---------------------"          << std::endl
            << "Source MAC:\t" << ethernetLayer->getSourceMac() << std::endl
            << "Dest MAC:\t" << ethernetLayer->getDestMac()     << std::endl
            << "Source IP:\t" << ipLayer->getSrcIPAddress()     << std::endl
            << "Dest IP:\t" << ipLayer->getDstIPAddress() << "\n\n";


        // Change the src address
        ethernetLayer->setSourceMac(
                pcpp::MacAddress(
                    incrementMac(
                        ethernetLayer->getSourceMac().toString(), 
                        ++inc)
                    )
                );

        std::cout 
            << "Edited Packet -----------------------"          << std::endl
            << "Source MAC:\t" << ethernetLayer->getSourceMac() << std::endl
            << "*Dest MAC:\t" << ethernetLayer->getDestMac()    << std::endl
            << "Source IP:\t" << ipLayer->getSrcIPAddress()     << std::endl
            << "Dest IP:\t" << ipLayer->getDstIPAddress() << "\n\n";

        // DEBUG: returning after the first attempt, just for testing
        // Comment out the return if you want to modify every packet coming through
        //  or uncomment if you want to run a bunch of packets through. 
        //  Warning, it's a lot
        // return 0;
    }
    return 0;
}

void printUsage() {
    std::cout << "Usage: ./PacketAnalyzer [Options]" << std::endl
        << "\t-a [IPv4 Address]" << std::endl
        << "\t-m [MAC Address]" << std::endl;
}

std::string incrementMac(std::string mac, int incr) {
    // remove the colons
    mac.erase(std::remove(mac.begin(), mac.end(), ':'), mac.end());

    // store the stringy hex value into a decimal int and increment
    long tempDecVal = std::stol(mac, nullptr, 16);
    tempDecVal += incr;

    // Convert back to hex, pad with zeros if too small
    std::stringstream ss; 
    ss << std::hex << std::setw(12) << std::setfill('0') << tempDecVal;
    mac = ss.str();

    // re add the colons
    for (int i = 2; i < mac.length(); i += 3) {
        mac.insert(i, ":");
    }

    return mac;
}
