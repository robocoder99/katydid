/*
 * KTROOTTreeTypeWriterAnalysis.cc
 *
 *  Created on: May 21, 2013
 *      Author: nsoblath
 */

#include "KTROOTTreeTypeWriterAnalysis.hh"

#include "KT2ROOT.hh"
#include "KTAmplitudeDistribution.hh"
#include "KTDiscriminatedPoints1DData.hh"
#include "KTKDTreeData.hh"
#include "KTHoughData.hh"
#include "KTLogger.hh"
#include "KTSliceHeader.hh"
#include "KTTIFactory.hh"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"

#include <sstream>



using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "KTROOTTreeTypeWriterAnalysis");

    static KTTIRegistrar< KTROOTTreeTypeWriter, KTROOTTreeTypeWriterAnalysis > sRTTWCRegistrar;

    KTROOTTreeTypeWriterAnalysis::KTROOTTreeTypeWriterAnalysis() :
            KTROOTTreeTypeWriter(),
            //KTTypeWriterAnalysis()
            fDiscPoints1DTree(NULL),
            fKDTreeTree(NULL),
            fAmpDistTree(NULL),
            fHoughTree(NULL),
            fDiscPoints1DData(),
            fKDTreePointData(),
            fAmpDistData(),
            fHoughData()
    {
    }

    KTROOTTreeTypeWriterAnalysis::~KTROOTTreeTypeWriterAnalysis()
    {
    }


    void KTROOTTreeTypeWriterAnalysis::RegisterSlots()
    {
        fWriter->RegisterSlot("disc-1d", this, &KTROOTTreeTypeWriterAnalysis::WriteDiscriminatedPoints1D);
        fWriter->RegisterSlot("kd-tree", this, &KTROOTTreeTypeWriterAnalysis::WriteKDTree);
        fWriter->RegisterSlot("amp-dist", this, &KTROOTTreeTypeWriterAnalysis::WriteAmplitudeDistributions);
        fWriter->RegisterSlot("hough", this, &KTROOTTreeTypeWriterAnalysis::WriteHoughData);
        return;
    }


    //*********************
    // Discriminated Points
    //*********************

    void KTROOTTreeTypeWriterAnalysis::WriteDiscriminatedPoints1D(KTDataPtr data)
    {
        KTDiscriminatedPoints1DData& fcData = data->Of< KTDiscriminatedPoints1DData >();
        KTSliceHeader& header = data->Of< KTSliceHeader >();

        if (! fWriter->OpenAndVerifyFile()) return;

        if (fDiscPoints1DTree == NULL)
        {
            if (! SetupDiscriminatedPoints1DTree())
            {
                KTERROR(publog, "Something went wrong while setting up the discriminated points 1D tree! Nothing was written.");
                return;
            }
        }

        fDiscPoints1DData.fSlice = header.GetSliceNumber();
        fDiscPoints1DData.fTimeInRunC = header.GetTimeInRun() + 0.5 * header.GetSliceLength();

        for (fDiscPoints1DData.fComponent = 0; fDiscPoints1DData.fComponent < fcData.GetNComponents(); fDiscPoints1DData.fComponent++)
        {
            const KTDiscriminatedPoints1DData::SetOfPoints& points = fcData.GetSetOfPoints(fDiscPoints1DData.fComponent);
            for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator it = points.begin(); it != points.end(); ++it)
            {
                fDiscPoints1DData.fBin = it->first;
                fDiscPoints1DData.fAbscissa = it->second.fAbscissa;
                fDiscPoints1DData.fOrdinate = it->second.fOrdinate;
                fDiscPoints1DData.fThreshold = it->second.fThreshold;

                fDiscPoints1DTree->Fill();
           }
        }

        return;
    }

    bool KTROOTTreeTypeWriterAnalysis::SetupDiscriminatedPoints1DTree()
    {
        fDiscPoints1DTree = new TTree("discPoints1D", "Discriminated Points 1D");
        if (fDiscPoints1DTree == NULL)
        {
            KTERROR(publog, "Tree was not created!");
            return false;
        }
        fWriter->AddTree(fDiscPoints1DTree);

        //fDiscPoints1DData = new TDiscriminatedPoints1DData();

        fDiscPoints1DTree->Branch("Slice", &fDiscPoints1DData.fSlice, "fSlice/i");
        fDiscPoints1DTree->Branch("TimeInRunC", &fDiscPoints1DData.fTimeInRunC, "fTimeInRunC/d");
        fDiscPoints1DTree->Branch("Component", &fDiscPoints1DData.fComponent, "fComponent/i");
        fDiscPoints1DTree->Branch("Bin", &fDiscPoints1DData.fBin, "fBin/i");
        fDiscPoints1DTree->Branch("Abscissa", &fDiscPoints1DData.fAbscissa, "fAbscissa/d");
        fDiscPoints1DTree->Branch("Ordinate", &fDiscPoints1DData.fOrdinate, "fOrdinate/d");
        fDiscPoints1DTree->Branch("Threshold", &fDiscPoints1DData.fThreshold, "fThreshold/d");
        //fDiscPoints1DTree->Branch("freqAnalysis", &fDiscPoints1DData.fComponent, "fComponent/s:fSlice/l:fTimeInRun/d:fThreshold/d:fFirstBin/i:fLastBin/i:fMeanFrequency/d:fPeakAmplitude/d");

        return true;
    }

    //*********************
    // K-D Tree
    //*********************

    void KTROOTTreeTypeWriterAnalysis::WriteKDTree(KTDataPtr data)
    {
        static Long64_t lastSlice = -1;

        KTKDTreeData& kdtData = data->Of< KTKDTreeData >();

        if (! fWriter->OpenAndVerifyFile()) return;

        if (fKDTreeTree == NULL)
        {
            if (! SetupKDTreeTree())
            {
                KTERROR(publog, "Something went wrong while setting up the k-d tree tree! Nothing was written.");
                return;
            }
        }

        Long64_t lastSliceThisData = lastSlice;
        for (fKDTreePointData.fComponent = 0; fKDTreePointData.fComponent < kdtData.GetNComponents(); fKDTreePointData.fComponent++)
        {
            const KTKDTreeData::SetOfPoints& points = kdtData.GetSetOfPoints(fKDTreePointData.fComponent);
            const KTKDTreeData::TreeIndex* index = kdtData.GetTreeIndex(fKDTreePointData.fComponent);
            unsigned pid = 0;
            for (KTKDTreeData::SetOfPoints::const_iterator it = points.begin(); it != points.end(); ++it)
            {
                if ((int64_t)it->fSliceNumber > lastSlice)
                {
                    if ((int64_t)it->fSliceNumber > lastSliceThisData) lastSliceThisData = (int64_t)it->fSliceNumber;
                    fKDTreePointData.fSlice = it->fSliceNumber;
                    fKDTreePointData.fTimeInRunC = it->fCoords[0];
                    fKDTreePointData.fFrequency = it->fCoords[1];
                    fKDTreePointData.fAmplitude = it->fAmplitude;
                    fKDTreePointData.fNoiseFlag = it->fNoiseFlag;
                    KTKDTreeData::TreeIndex::Neighbors neighbors = index->knnSearch(pid, 2);
                    fKDTreePointData.fNNDistance = neighbors.dist(1);

                    fKDTreeTree->Fill();
                    ++pid;
                }
           }
        }
        lastSlice = lastSliceThisData;

        return;
    }

    bool KTROOTTreeTypeWriterAnalysis::SetupKDTreeTree()
    {
        fKDTreeTree = new TTree("kdTree", "K-D Tree");
        if (fKDTreeTree == NULL)
        {
            KTERROR(publog, "Tree was not created!");
            return false;
        }
        fWriter->AddTree(fKDTreeTree);

        //fDiscPoints1DData = new TDiscriminatedPoints1DData();

        fKDTreeTree->Branch("Slice", &fKDTreePointData.fSlice, "fSlice/l");
        fKDTreeTree->Branch("TimeInRunC", &fKDTreePointData.fTimeInRunC, "fTimeInRunC/d");
        fKDTreeTree->Branch("Frequency", &fKDTreePointData.fFrequency, "fFrequency/d");
        fKDTreeTree->Branch("Amplitude", &fKDTreePointData.fAmplitude, "fAmplitude/d");
        fKDTreeTree->Branch("NoiseFlag", &fKDTreePointData.fNoiseFlag, "fNoiseFlag/d");
        fKDTreeTree->Branch("NNDistance", &fKDTreePointData.fNNDistance, "fNNDistance/d");

        return true;
    }

    //**************************
    // Amplitude Distribution
    //**************************

    void KTROOTTreeTypeWriterAnalysis::WriteAmplitudeDistributions(KTDataPtr data)
    {
        KTAmplitudeDistribution& adData = data->Of< KTAmplitudeDistribution >();
        //KTSliceHeader& header = data->Of< KTSliceHeader >();

        if (! fWriter->OpenAndVerifyFile()) return;

        if (fAmpDistTree == NULL)
        {
            if (! SetupAmplitudeDistributionTree())
            {
                KTERROR(publog, "Something went wrong while setting up the amplitude distribution tree! Nothing was written.");
                return;
            }
        }

        for (fAmpDistData.fComponent = 0; fAmpDistData.fComponent < adData.GetNComponents(); fAmpDistData.fComponent++)
        {
            for (fAmpDistData.fFreqBin = 0; fAmpDistData.fFreqBin < adData.GetNFreqBins(); fAmpDistData.fFreqBin++)
            {
                stringstream name;
                name << "histAmpDist_" << fAmpDistData.fComponent << "_" << fAmpDistData.fFreqBin;
                const KTAmplitudeDistribution::Distribution& dist = adData.GetDistribution(fAmpDistData.fFreqBin, fAmpDistData.fComponent);
                unsigned nBins = dist.size();
                fAmpDistData.fDistribution = new TH1D(name.str().c_str(), "Amplitude Distribution", (int)nBins, dist.GetRangeMin(), dist.GetRangeMax());
                for (unsigned iBin=0; iBin<nBins; iBin++)
                {
                    fAmpDistData.fDistribution->SetBinContent((int)iBin+1, dist(iBin));
                }
                fAmpDistData.fDistribution->SetXTitle("Amplitude");
                fAmpDistData.fDistribution->SetYTitle("Slices");
                fAmpDistData.fDistribution->SetDirectory(NULL);

                fAmpDistTree->Fill();
           }
        }

        return;
    }

    bool KTROOTTreeTypeWriterAnalysis::SetupAmplitudeDistributionTree()
    {
        fAmpDistTree = new TTree("freqCand", "Frequency Analysis");
        if (fAmpDistTree == NULL)
        {
            KTERROR(publog, "Tree was not created!");
            return false;
        }
        fWriter->AddTree(fAmpDistTree);

        fAmpDistTree->Branch("Component", &fAmpDistData.fComponent, "fComponent/i");
        fAmpDistTree->Branch("FreqBin", &fAmpDistData.fFreqBin, "fFreqBin/i");
        fAmpDistTree->Branch("Distribution", &fAmpDistData.fDistribution, 32000, 0);

        return true;
    }


    //*************************
    // Hough Transform Data
    //*************************

    void KTROOTTreeTypeWriterAnalysis::WriteHoughData(KTDataPtr data)
    {
        KTDEBUG(publog, "Attempting to write to hough data root tree");
        KTHoughData& htData = data->Of< KTHoughData >();
        //KTSliceHeader& header = data->Of< KTSliceHeader >();

        if (! fWriter->OpenAndVerifyFile()) return;

        if (fHoughTree == NULL)
        {
            if (! SetupHoughTree())
            {
                KTERROR(publog, "Something went wrong while setting up the Hough tree! Nothing was written.");
                return;
            }
        }

        for (fHoughData.fComponent = 0; fHoughData.fComponent < htData.GetNComponents(); fHoughData.fComponent++)
        {
            fHoughData.fTransform = KT2ROOT::CreateHistogram(htData.GetTransform(fHoughData.fComponent));
            fHoughData.fTransform->SetDirectory(NULL);
            fHoughData.fTransform->SetTitle("Hough Space");
            fHoughData.fTransform->SetXTitle("Angle");
            fHoughData.fTransform->SetYTitle("Radius");
            KTINFO(publog, "Angle axis: " << fHoughData.fTransform->GetNbinsX() << " bins; range: " << fHoughData.fTransform->GetXaxis()->GetXmin() << " - " << fHoughData.fTransform->GetXaxis()->GetXmax());
            KTINFO(publog, "Radius axis: " << fHoughData.fTransform->GetNbinsY() << " bins; range: " << fHoughData.fTransform->GetYaxis()->GetXmin() << " - " << fHoughData.fTransform->GetYaxis()->GetXmax());

            fHoughData.fXOffset = htData.GetXOffset(fHoughData.fComponent);
            fHoughData.fXScale = htData.GetXScale(fHoughData.fComponent);
            fHoughData.fYOffset = htData.GetYOffset(fHoughData.fComponent);
            fHoughData.fYScale = htData.GetYScale(fHoughData.fComponent);
        }

        fHoughTree->Fill();

        return;
    }

    bool KTROOTTreeTypeWriterAnalysis::SetupHoughTree()
    {
        fHoughTree = new TTree("hough", "Hough Transform");
        if (fHoughTree == NULL)
        {
            KTERROR(publog, "Tree was not created!");
            return false;
        }
        fWriter->AddTree(fHoughTree);

        fHoughTree->Branch("Component", &fHoughData.fComponent, "fComponent/i");
        fHoughTree->Branch("Transform", &fHoughData.fTransform, 32000, 0);
        fHoughTree->Branch("XOffset", &fHoughData.fXOffset, "fXOffset/d");
        fHoughTree->Branch("XScale", &fHoughData.fXScale, "fXScale/d");
        fHoughTree->Branch("YOffset", &fHoughData.fYOffset, "fYOffset/d");
        fHoughTree->Branch("YScale", &fHoughData.fYScale, "fYScale/d");

        return true;
    }



} /* namespace Katydid */



