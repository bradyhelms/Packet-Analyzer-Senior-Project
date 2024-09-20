# Packet Analyzer for Group 15 Senior Project

This repo contains a work in progress example of what our FPGA-based inline network
tap will be able to do. 

Right now, it can only accept an IP address and will output the source and destination
IP and MAC addresses. 

## Requirements
CMake
PCapPlusPlus

## To run on your system (must be linux for now):
Clone the repo:

`gh repo clone bradyhelms/Packet-Analyzer-Senior-Project`

Run CMake:

`cmake -S . -B build`

`cmake --build build`

If you compiled succesfully, run the program using:

`sudo ./PacketAnalyzer`
