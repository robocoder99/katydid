/*
 * KTDataAccumulator.cc
 *
 *  Created on: Oct 21, 2013
 *      Author: nsoblath
 */

#include "KTDataAccumulator.hh"

#include "KTConvolvedSpectrumData.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"



using std::map;
using std::string;

namespace Katydid
{
    KTLOGGER(avlog, "KTDataAccumulator");

    KT_REGISTER_PROCESSOR(KTDataAccumulator, "data-accumulator");

    KTDataAccumulator::KTDataAccumulator(const std::string& name) :
            KTProcessor(name),
            fAccumulatorSize(10),
            fAveragingFrac(0.1),
            fSignalInterval(1),
            fDataMap(),
            fLastAccumulatorPtr(),

            fTSSignal("ts", this),
            fTSDistSignal("ts-dist", this),
            fFSPolarSignal("fs-polar", this),
            fFSFFTWSignal("fs-fftw", this),
            fConvPSSignal("conv-ps", this),
            fPSSignal("ps", this),

            fTSVarianceSignal("ts-variance", this),
            fTSDistVarianceSignal("ts-dist-variance", this),
            fFSPolarVarianceSignal("fs-polar-variance", this),
            fFSFFTWVarianceSignal("fs-fftw-variance", this),
            fConvPSVarianceSignal("conv-ps-variance", this),
            fPSVarianceSignal("ps-variance", this),

            fTSFinishedSignal("ts-finished", this),
            fTSDistFinishedSignal("ts-dist-finished", this),
            fFSPolarFinishedSignal("fs-polar-finished", this),
            fFSFFTWFinishedSignal("fs-fftw-finished", this),
            fConvPSFinishedSignal("conv-ps-finished", this),
            fPSFinishedSignal("ps-finished", this),

            fTSVarianceFinishedSignal("ts-variance-finished", this),
            fTSDistVarianceFinishedSignal("ts-dist-variance-finished", this),
            fFSPolarVarianceFinishedSignal("fs-polar-variance-finished", this),
            fFSFFTWVarianceFinishedSignal("fs-fftw-variance-finished", this),
            fConvPSVarianceFinishedSignal("conv-ps-variance-finished", this),
            fPSVarianceFinishedSignal("ps-variance-finished", this),

            fSignalMap()
    {
        RegisterSlot("ts", this, &KTDataAccumulator::SlotFunction< KTTimeSeriesData >);
        fSignalMap.insert(SignalMapValue(&typeid(KTTimeSeriesData), SignalSet(&fTSSignal, &fTSVarianceSignal, &fTSFinishedSignal, &fTSVarianceFinishedSignal)));

        RegisterSlot("ts-dist", this, &KTDataAccumulator::SlotFunction< KTTimeSeriesDistData >);
        fSignalMap.insert(SignalMapValue(&typeid(KTTimeSeriesDistData), SignalSet(&fTSDistSignal, &fTSDistVarianceSignal, &fTSDistFinishedSignal, &fTSDistVarianceFinishedSignal)));

        RegisterSlot("fs-polar", this, &KTDataAccumulator::SlotFunction< KTFrequencySpectrumDataPolar >);
        fSignalMap.insert(SignalMapValue(&typeid(KTFrequencySpectrumDataPolar), SignalSet(&fFSPolarSignal, &fFSPolarVarianceSignal, &fFSPolarFinishedSignal, &fFSPolarVarianceFinishedSignal)));

        RegisterSlot("fs-fftw", this, &KTDataAccumulator::SlotFunction< KTFrequencySpectrumDataFFTW >);
        fSignalMap.insert(SignalMapValue(&typeid(KTFrequencySpectrumDataFFTW), SignalSet(&fFSFFTWSignal, &fFSFFTWVarianceSignal, &fFSFFTWFinishedSignal, &fFSFFTWVarianceFinishedSignal)));

        RegisterSlot("conv-ps", this, &KTDataAccumulator::SlotFunction< KTConvolvedPowerSpectrumData >);
        fSignalMap.insert(SignalMapValue(&typeid(KTConvolvedPowerSpectrumData), SignalSet(&fConvPSSignal, &fConvPSVarianceSignal, &fConvPSFinishedSignal, &fConvPSVarianceFinishedSignal)));

        RegisterSlot("ps", this, &KTDataAccumulator::SlotFunction< KTPowerSpectrumData >);
        fSignalMap.insert(SignalMapValue(&typeid(KTPowerSpectrumData), SignalSet(&fPSSignal, &fPSVarianceSignal, &fPSFinishedSignal, &fPSVarianceFinishedSignal)));
    }

    KTDataAccumulator::~KTDataAccumulator()
    {
    }

    bool KTDataAccumulator::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetAccumulatorSize(node->get_value<unsigned>("number-to-average", fAccumulatorSize));
        SetSignalInterval(node->get_value<unsigned>("signal-interval", fSignalInterval));

        return true;
    }

    bool KTDataAccumulator::AddData(KTTimeSeriesData& data)
    {
        Accumulator& accDataStruct = GetOrCreateAccumulator< KTTimeSeriesData >();
        KTTimeSeriesData& accData = accDataStruct.fData->Of<KTTimeSeriesData>();
        KTTimeSeriesData& devData = accDataStruct.fVarianceData->Of<KTTimeSeriesData>();
        if (dynamic_cast< KTTimeSeriesReal* >(data.GetTimeSeries(0)) != NULL)
        {
            return CoreAddTSDataReal(data, accDataStruct, accData, devData);
        }
        else
        {
            return CoreAddTSDataFFTW(data, accDataStruct, accData, devData);
        }
    }

    bool KTDataAccumulator::AddData(KTTimeSeriesDistData& data)
    {
        Accumulator& accDataStruct = GetOrCreateAccumulator< KTTimeSeriesDistData >();
        KTTimeSeriesDistData& accData = accDataStruct.fData->Of<KTTimeSeriesDistData>();
        KTTimeSeriesDistData& devData = accDataStruct.fVarianceData->Of<KTTimeSeriesDistData>();
        return CoreAddData(data, accDataStruct, accData, devData);
    }

    bool KTDataAccumulator::AddData(KTFrequencySpectrumDataPolar& data)
    {
        Accumulator& accDataStruct = GetOrCreateAccumulator< KTFrequencySpectrumDataPolar >();
        KTFrequencySpectrumDataPolar& accData = accDataStruct.fData->Of<KTFrequencySpectrumDataPolar>();
        KTFrequencySpectrumDataPolar& devData = accDataStruct.fVarianceData->Of<KTFrequencySpectrumDataPolar>();
        return CoreAddData(data, accDataStruct, accData, devData);
    }

    bool KTDataAccumulator::AddData(KTFrequencySpectrumDataFFTW& data)
    {
        Accumulator& accDataStruct = GetOrCreateAccumulator< KTFrequencySpectrumDataFFTW >();
        KTFrequencySpectrumDataFFTW& accData = accDataStruct.fData->Of<KTFrequencySpectrumDataFFTW>();
        KTFrequencySpectrumDataFFTW& devData = accDataStruct.fVarianceData->Of<KTFrequencySpectrumDataFFTW>();
        return CoreAddData(data, accDataStruct, accData, devData);
    }

    bool KTDataAccumulator::AddData(KTConvolvedPowerSpectrumData& data)
    {
        Accumulator& accDataStruct = GetOrCreateAccumulator< KTConvolvedPowerSpectrumData >();
        KTConvolvedPowerSpectrumData& accData = accDataStruct.fData->Of<KTConvolvedPowerSpectrumData>();
        KTConvolvedPowerSpectrumData& devData = accDataStruct.fVarianceData->Of<KTConvolvedPowerSpectrumData>();
        return CoreAddData(data, accDataStruct, accData, devData);
    }

    bool KTDataAccumulator::AddData(KTPowerSpectrumData& data)
    {
        Accumulator& accDataStruct = GetOrCreateAccumulator< KTPowerSpectrumData >();
        KTPowerSpectrumData& accData = accDataStruct.fData->Of<KTPowerSpectrumData>();
        KTPowerSpectrumData& devData = accDataStruct.fVarianceData->Of<KTPowerSpectrumData>();
        return CoreAddData(data, accDataStruct, accData, devData);
    }

    bool KTDataAccumulator::CoreAddTSDataReal(KTTimeSeriesData& data, Accumulator& accDataStruct, KTTimeSeriesData& accData, KTTimeSeriesData& devData)
    {
        double remainingFrac = 1.;
        if (fAccumulatorSize != 0 && accDataStruct.GetSliceNumber() >= fAccumulatorSize)
            remainingFrac -= fAveragingFrac;
        //KTDEBUG(avlog, "averaging frac: " << fAveragingFrac << "    remaining frac: " << remainingFrac);

        unsigned nComponents = data.GetNComponents();

        if (accDataStruct.GetSliceNumber() == 0)
        {
            accData.SetNComponents(nComponents);
            devData.SetNComponents(nComponents);
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                KTTimeSeriesReal* dataTS = static_cast< KTTimeSeriesReal* >(data.GetTimeSeries(iComponent));

                KTTimeSeriesReal* newTS = new KTTimeSeriesReal(dataTS->GetNTimeBins(), dataTS->GetRangeMin(), dataTS->GetRangeMax());
                (*newTS) *= 0.;
                KTTimeSeriesReal* newVarTS = new KTTimeSeriesReal(dataTS->GetNTimeBins(), dataTS->GetRangeMin(), dataTS->GetRangeMax());
                (*newVarTS) *= 0.;

                accData.SetTimeSeries(newTS, iComponent);
                devData.SetTimeSeries(newVarTS, iComponent);
            }
        }

        accDataStruct.BumpSliceNumber();

        if (nComponents != accData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the average and in the new data do not match");
            return false;
        }
        if (nComponents != devData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the variance and in the new data do not match");
            return false;
        }

        unsigned arraySize = data.GetTimeSeries(0)->GetNTimeBins();
        if (arraySize != accData.GetTimeSeries(0)->GetNTimeBins())
        {
            KTERROR(avlog, "Sizes of arrays in the average and in the new data do not match");
            return false;
        }
        if (arraySize != devData.GetTimeSeries(0)->GetNTimeBins())
        {
            KTERROR(avlog, "Sizes of arrays in the variance and in the new data do not match");
            return false;
        }

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTTimeSeriesReal* newTS = static_cast< KTTimeSeriesReal* >(data.GetTimeSeries(iComponent));
            KTTimeSeriesReal* avTS = static_cast< KTTimeSeriesReal* >(accData.GetTimeSeries(iComponent));
            KTTimeSeriesReal* varTS = static_cast< KTTimeSeriesReal* >(devData.GetTimeSeries(iComponent));
            for (unsigned iBin = 0; iBin < arraySize; ++iBin)
            {
                //KTDEBUG(avlog, (*avTS)(iBin) << "  " << (*newTS)(iBin) << "  " << remainingFrac << "  " << fAveragingFrac);
                (*avTS)(iBin) = (*avTS)(iBin) * remainingFrac + (*newTS)(iBin) * fAveragingFrac;
                (*varTS)(iBin) = (*varTS)(iBin) * remainingFrac + (*newTS)(iBin) * (*newTS)(iBin) * fAveragingFrac;
                //KTDEBUG(avlog, (*avTS)(iBin));
            }
        }

        return true;
    }

    bool KTDataAccumulator::CoreAddTSDataFFTW(KTTimeSeriesData& data, Accumulator& accDataStruct, KTTimeSeriesData& accData, KTTimeSeriesData& devData)
    {
        double remainingFrac = 1.;
        if (fAccumulatorSize != 0 && accDataStruct.GetSliceNumber() >= fAccumulatorSize)
            remainingFrac -= fAveragingFrac;

        unsigned nComponents = data.GetNComponents();

        if (accDataStruct.GetSliceNumber() == 0)
        {
            accData.SetNComponents(nComponents);
            devData.SetNComponents(nComponents);
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                KTTimeSeriesFFTW* dataTS = static_cast< KTTimeSeriesFFTW* >(data.GetTimeSeries(iComponent));

                KTTimeSeriesFFTW* newTS = new KTTimeSeriesFFTW(dataTS->GetNTimeBins(), dataTS->GetRangeMin(), dataTS->GetRangeMax());
                KTTimeSeriesFFTW* newVarTS = new KTTimeSeriesFFTW(dataTS->GetNTimeBins(), dataTS->GetRangeMin(), dataTS->GetRangeMax());

                (*newTS) *= 0.;
                (*newVarTS) *= 0.;

                accData.SetTimeSeries(newTS, iComponent);
                devData.SetTimeSeries(newVarTS, iComponent);
            }
        }

        accDataStruct.BumpSliceNumber();

        if (nComponents != accData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the average and in the new data do not match");
            return false;
        }
        if (nComponents != devData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the variance and in the new data do not match");
            return false;
        }

        unsigned arraySize = data.GetTimeSeries(0)->GetNTimeBins();
        if (arraySize != accData.GetTimeSeries(0)->GetNTimeBins())
        {
            KTERROR(avlog, "Sizes of arrays in the average and in the new data do not match");
            return false;
        }
        if (arraySize != devData.GetTimeSeries(0)->GetNTimeBins())
        {
            KTERROR(avlog, "Sizes of arrays in the variance and in the new data do not match");
            return false;
        }

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTTimeSeriesFFTW* newTS = static_cast< KTTimeSeriesFFTW* >(data.GetTimeSeries(iComponent));
            KTTimeSeriesFFTW* avTS = static_cast< KTTimeSeriesFFTW* >(accData.GetTimeSeries(iComponent));
            KTTimeSeriesFFTW* varTS = static_cast< KTTimeSeriesFFTW* >(devData.GetTimeSeries(iComponent));
            for (unsigned iBin = 0; iBin < arraySize; ++iBin)
            {
                (*avTS)(iBin)[0] = (*avTS)(iBin)[0] * remainingFrac + (*newTS)(iBin)[0] * fAveragingFrac;
                (*avTS)(iBin)[1] = (*avTS)(iBin)[1] * remainingFrac + (*newTS)(iBin)[1] * fAveragingFrac;

                (*varTS)(iBin)[0] = (*varTS)(iBin)[0] * remainingFrac + ((*newTS)(iBin)[0] * (*newTS)(iBin)[0] + (*newTS)(iBin)[1] * (*newTS)(iBin)[1]) * fAveragingFrac;
                (*varTS)(iBin)[1] = 0.;
            }
        }

        return true;
    }

    bool KTDataAccumulator::CoreAddData(KTTimeSeriesDistData& data, Accumulator& accDataStruct, KTTimeSeriesDistData& accData, KTTimeSeriesDistData& devData)
    {
        double remainingFrac = 1.;
        if (fAccumulatorSize != 0 && accDataStruct.GetSliceNumber() >= fAccumulatorSize)
            remainingFrac -= fAveragingFrac;

        unsigned nComponents = data.GetNComponents();

        if (accDataStruct.GetSliceNumber() == 0)
        {
            accData.SetNComponents(nComponents);
            devData.SetNComponents(nComponents);
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                KTTimeSeriesDist* dataFS = data.GetTimeSeriesDist(iComponent);

                KTTimeSeriesDist* newFS = new KTTimeSeriesDist(dataFS->size(), dataFS->GetRangeMin(), dataFS->GetRangeMax());
                KTTimeSeriesDist* newVarFS = new KTTimeSeriesDist(dataFS->size(), dataFS->GetRangeMin(), dataFS->GetRangeMax());

                newFS->operator*=(double(0.));
                newVarFS->operator*=(double(0.));

                accData.SetTimeSeriesDist(newFS, iComponent);
                devData.SetTimeSeriesDist(newVarFS, iComponent);
            }
        }

        accDataStruct.BumpSliceNumber();

        if (nComponents != accData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the average and in the new data do not match");
            return false;
        }
        if (nComponents != devData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the variance and in the new data do not match");
            return false;
        }

        unsigned arraySize = data.GetTimeSeriesDist(0)->size();
        if (arraySize != accData.GetTimeSeriesDist(0)->size())
        {
            KTERROR(avlog, "Sizes of arrays in the average and in the new data do not match");
            return false;
        }
        if (arraySize != devData.GetTimeSeriesDist(0)->size())
        {
            KTERROR(avlog, "Sizes of arrays in the variance and in the new data do not match");
            return false;
        }

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTTimeSeriesDist* newSpect = data.GetTimeSeriesDist(iComponent);
            KTTimeSeriesDist* avSpect = accData.GetTimeSeriesDist(iComponent);
            KTTimeSeriesDist* varSpect = devData.GetTimeSeriesDist(iComponent);
            for (unsigned iBin = 0; iBin < arraySize; ++iBin)
            {
                (*avSpect)(iBin) = (*avSpect)(iBin) * remainingFrac + (*newSpect)(iBin) * fAveragingFrac;
                (*varSpect)(iBin) = (*varSpect)(iBin) * remainingFrac + (*newSpect)(iBin) * (*newSpect)(iBin) * fAveragingFrac;
            }
        }

        return true;
    }

    bool KTDataAccumulator::CoreAddData(KTFrequencySpectrumDataPolarCore& data, Accumulator& accDataStruct, KTFrequencySpectrumDataPolarCore& accData, KTFrequencySpectrumDataPolarCore& devData)
    {
        double remainingFrac = 1.;
        if (fAccumulatorSize != 0 && accDataStruct.GetSliceNumber() >= fAccumulatorSize)
            remainingFrac -= fAveragingFrac;

        unsigned nComponents = data.GetNComponents();

        if (accDataStruct.GetSliceNumber() == 0)
        {
            accData.SetNComponents(nComponents);
            devData.SetNComponents(nComponents);
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                KTFrequencySpectrumPolar* dataFS = data.GetSpectrumPolar(iComponent);

                KTFrequencySpectrumPolar* newFS = new KTFrequencySpectrumPolar(dataFS->size(), dataFS->GetRangeMin(), dataFS->GetRangeMax());
                KTFrequencySpectrumPolar* newVarFS = new KTFrequencySpectrumPolar(dataFS->size(), dataFS->GetRangeMin(), dataFS->GetRangeMax());
                
                newFS->SetNTimeBins(dataFS->GetNTimeBins());
                newFS->operator*=(double(0.));
                newVarFS->SetNTimeBins(dataFS->GetNTimeBins());
                newVarFS->operator*=(double(0.));

                accData.SetSpectrum(newFS, iComponent);
                devData.SetSpectrum(newVarFS, iComponent);
            }
        }

        accDataStruct.BumpSliceNumber();

        if (nComponents != accData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the average and in the new data do not match");
            return false;
        }
        if (nComponents != devData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the variance and in the new data do not match");
            return false;
        }

        unsigned arraySize = data.GetSpectrumPolar(0)->size();
        if (arraySize != accData.GetSpectrumPolar(0)->size())
        {
            KTERROR(avlog, "Sizes of arrays in the average and in the new data do not match");
            return false;
        }
        if (arraySize != devData.GetSpectrumPolar(0)->size())
        {
            KTERROR(avlog, "Sizes of arrays in the variance and in the new data do not match");
            return false;
        }

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTFrequencySpectrumPolar* newSpect = data.GetSpectrumPolar(iComponent);
            KTFrequencySpectrumPolar* avSpect = accData.GetSpectrumPolar(iComponent);
            KTFrequencySpectrumPolar* varSpect = devData.GetSpectrumPolar(iComponent);
            for (unsigned iBin = 0; iBin < arraySize; ++iBin)
            {
                (*avSpect)(iBin) = (*avSpect)(iBin) * remainingFrac + (*newSpect)(iBin) * fAveragingFrac;
                (*varSpect)(iBin) = (*varSpect)(iBin) * remainingFrac + (*newSpect)(iBin) * (*newSpect)(iBin) * fAveragingFrac;
            }
        }

        return true;
    }

    bool KTDataAccumulator::CoreAddData(KTFrequencySpectrumDataFFTWCore& data, Accumulator& accDataStruct, KTFrequencySpectrumDataFFTWCore& accData, KTFrequencySpectrumDataFFTWCore& devData)
    {
        double remainingFrac = 1.;
        if (fAccumulatorSize != 0 && accDataStruct.GetSliceNumber() >= fAccumulatorSize)
            remainingFrac -= fAveragingFrac;

        unsigned nComponents = data.GetNComponents();

        if (accDataStruct.GetSliceNumber() == 0)
        {
            accData.SetNComponents(nComponents);
            devData.SetNComponents(nComponents);
            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                KTFrequencySpectrumFFTW* dataFS = data.GetSpectrumFFTW(iComponent);
                
                KTFrequencySpectrumFFTW* newFS = new KTFrequencySpectrumFFTW(dataFS->size(), dataFS->GetRangeMin(), dataFS->GetRangeMax());
                KTFrequencySpectrumFFTW* newVarFS = new KTFrequencySpectrumFFTW(dataFS->size(), dataFS->GetRangeMin(), dataFS->GetRangeMax());
                
                newFS->SetNTimeBins(dataFS->GetNTimeBins());
                newFS->operator*=(double(0.));
                newVarFS->SetNTimeBins(dataFS->GetNTimeBins());
                newVarFS->operator*=(double(0.));

                accData.SetSpectrum(newFS, iComponent);
                devData.SetSpectrum(newVarFS, iComponent);
            }
        }

        accDataStruct.BumpSliceNumber();

        if (nComponents != accData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the average and in the new data do not match");
            return false;
        }
        if (nComponents != devData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the variance and in the new data do not match");
            return false;
        }

        unsigned arraySize = data.GetSpectrumFFTW(0)->size();
        if (arraySize != accData.GetSpectrumFFTW(0)->size())
        {
            KTERROR(avlog, "Sizes of arrays in the average and in the new data do not match");
            return false;
        }
        if (arraySize != devData.GetSpectrumFFTW(0)->size())
        {
            KTERROR(avlog, "Sizes of arrays in the variance and in the new data do not match");
            return false;
        }

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTFrequencySpectrumFFTW* newSpect = data.GetSpectrumFFTW(iComponent);
            KTFrequencySpectrumFFTW* avSpect = accData.GetSpectrumFFTW(iComponent);
            KTFrequencySpectrumFFTW* varSpect = devData.GetSpectrumFFTW(iComponent);
            for (unsigned iBin = 0; iBin < arraySize; ++iBin)
            {
                (*avSpect)(iBin)[0] = (*avSpect)(iBin)[0] * remainingFrac + (*newSpect)(iBin)[0] * fAveragingFrac;
                (*avSpect)(iBin)[1] = (*avSpect)(iBin)[1] * remainingFrac + (*newSpect)(iBin)[1] * fAveragingFrac;

                (*varSpect)(iBin)[0] = (*varSpect)(iBin)[0] * remainingFrac + ((*newSpect)(iBin)[0] * (*newSpect)(iBin)[0] + (*newSpect)(iBin)[1] * (*newSpect)(iBin)[1]) * fAveragingFrac;
                (*varSpect)(iBin)[1] = 0.;
            }
        }

        return true;
    }

    bool KTDataAccumulator::CoreAddData(KTPowerSpectrumDataCore& data, Accumulator& accDataStruct, KTPowerSpectrumDataCore& accData, KTPowerSpectrumDataCore& devData)
    {
        double remainingFrac = 1.;
        if (fAccumulatorSize != 0 && accDataStruct.GetSliceNumber() >= fAccumulatorSize)
            remainingFrac -= fAveragingFrac;
        KTDEBUG(avlog, "Accumulating a power spectrum; remainingFrac = " << remainingFrac << "   fAveragingFrac = " << fAveragingFrac);

        unsigned nComponents = data.GetNComponents();

        if (accDataStruct.GetSliceNumber() == 0)
        {
            accData.SetNComponents(nComponents);
            devData.SetNComponents(nComponents);

            for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
            {
                KTPowerSpectrum* dataPS = data.GetSpectrum(iComponent);

                KTPowerSpectrum* newPS = new KTPowerSpectrum(dataPS->size(), dataPS->GetRangeMin(), dataPS->GetRangeMax());
                KTPowerSpectrum* newVarPS = new KTPowerSpectrum(dataPS->size(), dataPS->GetRangeMin(), dataPS->GetRangeMax());

                newPS->operator*=(double(0.));
                newVarPS->operator*=(double(0.));

                accData.SetSpectrum(newPS, iComponent);
                devData.SetSpectrum(newVarPS, iComponent);
            }
        }

        accDataStruct.BumpSliceNumber();

        if (nComponents != accData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the average and in the new data do not match");
            return false;
        }
        if (nComponents != devData.GetNComponents())
        {
            KTERROR(avlog, "Numbers of components in the variance and in the new data do not match");
            return false;
        }

        unsigned arraySize = data.GetSpectrum(0)->size();
        if (arraySize != accData.GetSpectrum(0)->size())
        {
            KTERROR(avlog, "Sizes of arrays in the average and in the new data do not match");
            return false;
        }
        if (arraySize != devData.GetSpectrum(0)->size())
        {
            KTERROR(avlog, "Sizes of arrays in the variance and in the new data do not match");
            return false;
        }

        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTPowerSpectrum* newSpect = data.GetSpectrum(iComponent);
            KTPowerSpectrum* avSpect = accData.GetSpectrum(iComponent);
            KTPowerSpectrum* varSpect = devData.GetSpectrum(iComponent);
            avSpect->SetMode(newSpect->GetMode());
            varSpect->SetMode(newSpect->GetMode());
            for (unsigned iBin = 0; iBin < arraySize; ++iBin)
            {
                (*avSpect)(iBin) = (*avSpect)(iBin) * remainingFrac + (*newSpect)(iBin) * fAveragingFrac;
                (*varSpect)(iBin) = (*varSpect)(iBin) * remainingFrac + (*newSpect)(iBin) * (*newSpect)(iBin) * fAveragingFrac;
            }
        }

        return true;
    }

    bool KTDataAccumulator::Scale(KTTimeSeriesData& data, KTSliceHeader& header)
    {
        double scale = 1. / (double)(header.GetSliceNumber());
        KTDEBUG(avlog, "Scaling time series by " << scale);
        unsigned nComponents = data.GetNComponents();
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTTimeSeries* avSpect = data.GetTimeSeries(iComponent);
            avSpect->Scale(scale);
        }
        return true;
    }

    bool KTDataAccumulator::Scale(KTTimeSeriesDistData& data, KTSliceHeader& header)
    {
        double scale = 1. / (double)(header.GetSliceNumber());
        KTDEBUG(avlog, "Scaling time series dist by " << scale);
        unsigned nComponents = data.GetNComponents();
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTTimeSeriesDist* avSpect = data.GetTimeSeriesDist(iComponent);
            avSpect->Scale(scale);
        }
        return true;
    }

    bool KTDataAccumulator::Scale(KTFrequencySpectrumDataPolar& data, KTSliceHeader& header)
    {
        double scale = 1. / (double)(header.GetSliceNumber());
        KTDEBUG(avlog, "Scaling frequency-spectrum polar by " << scale);
        unsigned nComponents = data.GetNComponents();
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTFrequencySpectrum* avSpect = data.GetSpectrum(iComponent);
            avSpect->Scale(scale);
        }
        return true;
    }

    bool KTDataAccumulator::Scale(KTFrequencySpectrumDataFFTW& data, KTSliceHeader& header)
    {
        double scale = 1. / (double)(header.GetSliceNumber());
        KTDEBUG(avlog, "Scaling frequency spectrum fftw by " << scale);
        unsigned nComponents = data.GetNComponents();
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTFrequencySpectrum* avSpect = data.GetSpectrum(iComponent);
            avSpect->Scale(scale);
        }
        return true;
    }

    bool KTDataAccumulator::Scale(KTConvolvedPowerSpectrumData& data, KTSliceHeader& header)
    {
        double scale = 1. / (double)(header.GetSliceNumber());
        KTDEBUG(avlog, "Scaling power spectrum by " << scale);
        unsigned nComponents = data.GetNComponents();
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTPowerSpectrum* avSpect = data.GetSpectrum(iComponent);
            avSpect->Scale(scale);
        }
        return true;
    }

    bool KTDataAccumulator::Scale(KTPowerSpectrumData& data, KTSliceHeader& header)
    {
        double scale = 1. / (double)(header.GetSliceNumber());
        KTDEBUG(avlog, "Scaling power spectrum by " << scale);
        unsigned nComponents = data.GetNComponents();
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            KTPowerSpectrum* avSpect = data.GetSpectrum(iComponent);
            avSpect->Scale(scale);
        }
        return true;
    }

} /* namespace Katydid */
