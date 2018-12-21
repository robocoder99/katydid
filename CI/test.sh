#!/bin/sh

# Miscalleneous 
./build/bin/TestRandom

# Spectrum Analysis
./build/bin/TestTrackProcessing

# Event Analysis
./build/bin/TestDBScanTrackClustering
./build/bin/TestSequentialTrackFinder
./build/bin/TestSpectrumDiscriminator