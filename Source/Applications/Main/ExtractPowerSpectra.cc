/**
 @file ExtractPowerSpectra.cc
 @brief Executable to produce 1-D power spectra
 @details Produces 1-D power spectra from Egg events; optionally can average multiple events together
 @author: N. S. Oblath
 @date: Jan 5, 2012
 */

#include "KTApplication.hh"
#include "KTBasicROOTFileWriter.hh"
#include "KTEggProcessor.hh"
#include "KTLogger.hh"
#include "KTSimpleFFT.hh"

#include <string>

using namespace std;
using namespace Katydid;

KTLOGGER(extpslog, "katydid.extractps");


int main(int argc, char** argv)
{
    KTApplication* app = new KTApplication(argc, argv);
    app->ReadConfigFile();

    string appConfigName("extract-power-spectra");

    // Setup the processors and their signal/slot connections
    KTEggProcessor procEgg;
    KTSimpleFFT procFFT;
    KTBasicROOTFileWriter procPub;

    // Configure the processors
    app->Configure(&procEgg, appConfigName);
    app->Configure(&procFFT, appConfigName);
    app->Configure(&procPub, appConfigName);

    try
    {
        // this will ensure that every time procEgg hatches an event, procFFT.ProcessEvent will be called
        procEgg.ConnectASlot("event", &procFFT, "event");

        // this will ensure that when procEgg parses the header, the info is passed to PrepareFFT
        procEgg.ConnectASlot("header", &procFFT, "header");

        // this will get the output histogram when an FFT is complete
        //procFFT.ConnectASlot("fft", &powerSpectra, "get_ps");
        procFFT.ConnectASlot("fft", &procPub, "fs-data");
    }
    catch (std::exception& e)
    {
        KTERROR(extpslog, "An error occurred while connecting signals and slots:\n"
                << '\t' << e.what());
        return -1;
    }

    // Process the egg file
    Bool_t success = procEgg.ProcessEgg();

    if (! success) return -1;
    return 0;
}
