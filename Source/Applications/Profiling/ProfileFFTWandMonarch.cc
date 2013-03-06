/*
 * ProfileFFTWandMonarch.cc
 *
 *  Created on: Dec 21, 2012
 *      Author: nsoblath
 */

#include "KTEggHeader.hh"
#include "KTLogger.hh"
#include "KTThroughputProfiler.hh"

#include "Monarch.hpp"

#include <fftw3.h>

#include <boost/shared_ptr.hpp>

#include <cstdlib>

using namespace std;
using namespace Katydid;

KTLOGGER(proflog, "katydid.applications.profiling");

int main(const int argc, const char** argv)
{
    if (argc < 2)
    {
        KTWARN(proflog, "Usage:\n" <<
                "\tProfileFFTWandMonarch <input egg file> <# of slices to read>");
        return -1;
    }

    unsigned nSlices = atoi(argv[2]);

    const Monarch* tReadTest = Monarch::OpenForReading(argv[1]);
    if (tReadTest->ReadHeader() == false)
    {
        KTERROR(proflog, "could not read header!");
        return -1;
    }

    const MonarchHeader* tReadHeader = tReadTest->GetHeader();
    KTEggHeader tEggHeader;
    tEggHeader.SetFilename(tReadHeader->GetFilename());
    tEggHeader.SetAcquisitionMode(tReadHeader->GetAcqMode());
    tEggHeader.SetNChannels(2);
    tEggHeader.SetRecordSize(tReadHeader->GetRecordSize());
    tEggHeader.SetSliceSize(tReadHeader->GetRecordSize());
    tEggHeader.SetAcquisitionTime(tReadHeader->GetAcqTime());
    tEggHeader.SetAcquisitionRate(tReadHeader->GetAcqRate() * 1.e6);

    KTDEBUG(proflog, "Parsed header:\n"
         << "\tFilename: " << tEggHeader.GetFilename() << '\n'
         << "\tAcuisition Mode: " << tEggHeader.GetAcquisitionMode() << '\n'
         << "\tNumber of Channels: " << tEggHeader.GetNChannels() << '\n'
         << "\tRecord Size: " << tEggHeader.GetSliceSize() << '\n'
         << "\tRecord Size: " << tEggHeader.GetRecordSize() << '\n'
         << "\tAcquisition Time: " << tEggHeader.GetAcquisitionTime() << " s" << '\n'
         << "\tAcquisition Rate: " << tEggHeader.GetAcquisitionRate() << " Hz ");

    unsigned tSize = tEggHeader.GetRecordSize();

    KTINFO(proflog, "File opened and header extracted successfully (" << tSize << ")");

    // Dummy data pointer
    boost::shared_ptr<KTData> dataPtr(new KTData());

    // Create FFT
    KTINFO(proflog, "Setting up the FFT");
    fftw_complex* tInputArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * tSize);
    fftw_complex* tOutputArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * tSize);
    fftw_plan tPlan = fftw_plan_dft_1d(tSize, tInputArray, tOutputArray, FFTW_FORWARD, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);

    KTINFO(proflog, "FFT setup complete");

    // Create the profiler
    KTThroughputProfiler profiler;

    // Start the timer!
    KTINFO(proflog, "Starting profiling");
    profiler.ProcessHeader(&tEggHeader);

    const MonarchRecord* tRecord1 = tReadTest->GetRecordOne();
    const MonarchRecord* tRecord2 = tReadTest->GetRecordTwo();
    for (unsigned iSlice=0; iSlice < nSlices; iSlice++)
    {
        KTINFO(proflog, "Slice " << iSlice);
        if (tReadTest->ReadRecord() == false)
        {
            KTERROR(proflog, "Problem reading records at slice " << iSlice);
            break;
        }

        // first record
        // copy the data
        for (unsigned index=0; index < tReadHeader->GetRecordSize(); index++)
        {
            tInputArray[index][0] = double((unsigned char)(tRecord1->fDataPtr[index]));
        }
        // perform the fft
        fftw_execute_dft(tPlan, tInputArray, tOutputArray);

        // second record
        // copy the data
        for (unsigned index=0; index < tReadHeader->GetRecordSize(); index++)
        {
            tInputArray[index][0] = double((unsigned char)(tRecord2->fDataPtr[index]));
        }
        // perform the fft
        fftw_execute_dft(tPlan, tInputArray, tOutputArray);

        profiler.ProcessData(dataPtr);
    }

    // Stop the timer and print info
    profiler.Finish();

    // clean up
    fftw_destroy_plan(tPlan);
    fftw_free(tInputArray);
    fftw_free(tOutputArray);

    tReadTest->Close();
    delete tReadTest;

    return 0;
}




