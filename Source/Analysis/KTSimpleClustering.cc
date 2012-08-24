/*
 * KTSimpleCusteringProcessor.cc
 *
 *  Created on: Jan 24, 2012
 *      Author: nsoblath
 */

#include "KTSimpleClustering.hh"

#include "KTMaskedArray.hh"
#include "KTPhysicalArray.hh"
#include "KTPowerSpectrum.hh"
#include "KTPStoreNode.hh"
#include "KTSlidingWindowFFT.hh"

#include "TCanvas.h"
#include "TH1.h"
#include "TMath.h"

#include <iostream>
#include <set>
#include <utility>

using std::list;
using std::multimap;
using std::set;
using std::pair;

namespace Katydid
{

    KTSimpleClustering::KTSimpleClustering() :
            KTProcessor(),
            KTConfigurable(),
            fEventPeakBins(NULL),
            fThresholdMult(8.),
            fBinCuts(NULL),
            fMinimumGroupSize(2),
            fGroupBinsMarginLow(1),
            fGroupBinsMarginHigh(3),
            fGroupBinsMarginSameTime(1),
            fFirstBinToUse(0),
            fDrawFlag(kFALSE)
    {
        fConfigName = "sliding-window-fft";

        RegisterSlot("power_spect", this, &KTSimpleClustering::ProcessPowerSpectrum);
    }

    KTSimpleClustering::~KTSimpleClustering()
    {
        delete fBinCuts;
    }

    Bool_t KTSimpleClustering::Configure(const KTPStoreNode* node)
    {
        if (node != NULL)
        {
            fThresholdMult = node->GetData< Double_t >("threshold-multiplier", 10.0);
            fGroupBinsMarginHigh = node->GetData< Int_t >("group-bins-margin-high", 1);
            fGroupBinsMarginLow = node->GetData< Int_t >("group-bins-margin-low", 3);
            fGroupBinsMarginSameTime = node->GetData< Int_t >("group-bins-margin-same-time", 1);
            fFirstBinToUse = node->GetData< UInt_t >("first-bin-to-use", 1);
            fDrawFlag = node->GetData< Bool_t >("draw-flag", false);
        }

        // No CL options

        return true;
    }


    void KTSimpleClustering::ProcessSlidingWindowFFT(KTSlidingWindowFFT* fft)
    {
        UInt_t nPowerSpectra = fft->GetNPowerSpectra();
        for (UInt_t iPS=0; iPS<nPowerSpectra; iPS++)
        {
            ProcessPowerSpectrum(iPS, fft->GetPowerSpectrum(iPS));
        }

        return;
    }

    void KTSimpleClustering::ProcessPowerSpectrum(UInt_t psNum, KTPowerSpectrum* powerSpectrum)
    {
        // Look for the highest-peaked bins in this power spectrum

        // this will hold the bin numbers that are above the threshold
        set< Int_t > peakBins;

        Double_t* dataArray = powerSpectrum->GetMagnitude().GetMatrixArray();
        UInt_t nBins = (UInt_t)powerSpectrum->GetSize();

        KTMaskedArray< Double_t*, Double_t >* localBinCuts = fBinCuts;
        if (nBins != fBinCuts->GetArraySize())
        {
            std::cout << "Warning from KTSimpleClustering::ProcessPowerSpectrum: size from power spectrum does not match bin cut array size" << std::endl;
            localBinCuts = new KTMaskedArray< Double_t*, Double_t >(dataArray, nBins);
        }
        else
        {
            fBinCuts->ChangeArray(dataArray);
        }
        UInt_t nCutBins = localBinCuts->size();

        Double_t mean = TMath::Mean(nBins-fFirstBinToUse, dataArray+fFirstBinToUse);
        //cout << "   Mean: " << mean << endl;

        Double_t threshold = fThresholdMult * mean;

        /*
        std::cout << "mean: " << mean << "  threshold: " << threshold << std::endl;

        for (unsigned int iBin=0; iBin<fFirstBinToUse; iBin++)
        {
            std::cout << "  " << powerSpectrum->GetMagnitudeAt((Int_t)iBin) << "  " << dataArray[iBin] << std::endl;
        }
        std::cout << std::endl;
        for (unsigned int iBin=fFirstBinToUse; iBin<100; iBin++)
        {
            std::cout << "  " << powerSpectrum->GetMagnitudeAt((Int_t)iBin) << "  " << dataArray[iBin] << std::endl;
        }
        */

        // search for bins above the threshold
        unsigned int firstCutBinToUse = localBinCuts->FindCutPositionOrNext(fFirstBinToUse);
        for (unsigned int iCutBin=firstCutBinToUse; iCutBin<nCutBins; iCutBin++)
        {
            /*
            if (dataArray[iBin] > threshold)
            {
                peakBins.insert(iBin);
            }
            */
            if ((*localBinCuts)[iCutBin] > threshold)
            {
                peakBins.insert(localBinCuts->GetArrayPosition(iCutBin));
            }
        }
        //std::cout << "FFT " << psNum << " -- Peak bins: " << peakBins.size() << std::endl;

        /**/// DEBUG
        if (fDrawFlag && psNum < 5)
        //if (psNum < 5)
        {
            TCanvas *cSCP = new TCanvas("cSCP", "cSCP");
            TH1D* histPS = powerSpectrum->CreateMagnitudeHistogram("ps_scp");
            cSCP->SetLogy(1);
            char projnum[30];
            sprintf(projnum, "%s%i", "fft #", psNum);
            histPS->SetTitle(projnum);
            histPS->Draw();
            cSCP->WaitPrimitive();
            //cSCP->Print(outputFileNamePS.c_str());
            delete cSCP;
        }
        /**/

        // Look for groups
        for (set< Int_t >::iterator iPB=peakBins.begin(); iPB!=peakBins.end(); iPB++)
        {
            Int_t pbVal = *iPB;
            Bool_t foundGroup = kFALSE;
            for (list< multimap< Int_t, Int_t >* >::iterator iEPB=fEventPeakBins->begin(); iEPB!=fEventPeakBins->end(); iEPB++)
            {
                multimap< Int_t, Int_t >* groupMap = *iEPB;
                multimap< Int_t, Int_t >::iterator lastGroup = groupMap->end();
                lastGroup--;
                UInt_t lastFFT = (UInt_t)lastGroup->first;
                // check if we've passed this group, and if so, if the group is too small, remove it
                if (lastFFT < psNum - 1 && (UInt_t)groupMap->size() <= fMinimumGroupSize)
                {
                    delete groupMap;
                    iEPB = fEventPeakBins->erase(iEPB);
                    iEPB--; // move the iterator back one so we don't skip anything when the for loop advances the iterator
                    continue;
                }
                pair< multimap< Int_t, Int_t >::iterator, multimap< Int_t, Int_t >::iterator > lastFFTRange =
                        groupMap->equal_range(lastFFT);
                multimap< Int_t, Int_t >::iterator firstGroupInRange = lastFFTRange.first;
                Int_t firstGroupFreqBin = firstGroupInRange->second;
                Int_t lastGroupFreqBin = firstGroupFreqBin;
                firstGroupInRange++;
                for (multimap< Int_t, Int_t >::iterator grIt=firstGroupInRange; grIt!=lastFFTRange.second; grIt++)
                {
                    if (grIt->second > lastGroupFreqBin) lastGroupFreqBin = grIt->second;
                    else if (grIt->second < firstGroupFreqBin) firstGroupFreqBin = grIt->second;
                }

                if ((UInt_t)lastFFT == psNum)
                {
                    if (pbVal >= firstGroupFreqBin - fGroupBinsMarginSameTime && pbVal <= lastGroupFreqBin + fGroupBinsMarginSameTime)
                    {
                        groupMap->insert( pair< Int_t, Int_t >((Int_t)psNum, pbVal) );
                        foundGroup = kTRUE;
                        break;
                    }
                }
                else if ((UInt_t)lastFFT == psNum - 1)
                {
                    if (pbVal >= firstGroupFreqBin - fGroupBinsMarginLow && pbVal <= lastGroupFreqBin + fGroupBinsMarginHigh)
                    {
                        groupMap->insert( pair< Int_t, Int_t >((Int_t)psNum, pbVal) );
                        foundGroup = kTRUE;
                        break;
                    }
                }
            }
            if (foundGroup) continue;

            // no match to existing groups, so add a new one
            multimap< Int_t, Int_t >* newGroupMap = new multimap< Int_t, Int_t >();
            newGroupMap->insert( pair< Int_t, Int_t >((Int_t)psNum, pbVal) );
            fEventPeakBins->push_back(newGroupMap);
        }

        return;
    }

} /* namespace Katydid */