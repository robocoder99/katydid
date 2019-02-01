/*
 * KTROOTTreeTypeWriterSpectrumAnalysis.hh
 *
 *  Created on: May 21, 2013
 *      Author: nsoblath
 *
 *  KDTreeData can be written either with "kd-tree" or "kd-tree-scaled".
 *  The latter scales the time and frequency values back to their original units.
 *  Both functions share the same TTree, so only one should be used in a given writer instance.
 *  To write to both, please use two RTW instances.
 */

#ifndef KTROOTTREETYPEWRITERSPECTRUMANALYSIS_HH_
#define KTROOTTREETYPEWRITERSPECTRUMANALYSIS_HH_

#include "KTROOTTreeWriter.hh"

#include "KTData.hh"

#include "Rtypes.h"

class TH1D;
class TH2D;
class TTree;
class KTPowerSpectrum;

namespace Katydid
{
    class KTKDTreeData;
    
    struct TDiscriminatedPoints1DData
    {
        UInt_t fSlice;
        Double_t fTimeInRunC; // bin center on the time axis
        UInt_t fComponent;
        UInt_t fBin;
        Double_t fAbscissa;
        Double_t fOrdinate;
        Double_t fThreshold;
        Double_t fMean; //<-
        Double_t fVariance; //<-
        Double_t fNeighborhoodAmplitude; //<-
    };

    struct TKDTreePointData
    {
        UInt_t fComponent;
        ULong64_t fSlice;
        Double_t fTimeInRunC;
        Double_t fFrequency;
        Double_t fAmplitude;
        Double_t fMean;
        Double_t fVariance;
        Double_t fNeighborhoodAmplitude;
        Bool_t fNoiseFlag;
        UInt_t fBinInSlice;
        ULong64_t fSliceNumber;
        Double_t fNNDistance;
        UInt_t fKNNWithin1;
        UInt_t fKNNWithin2;
        UInt_t fKNNWithin3;
        UInt_t fKNNWithin4;
        UInt_t fKNNWithin5;
        UInt_t fKNNWithin6;
        UInt_t fKNNWithin7;
        UInt_t fKNNWithin8;
    };

    struct TAmplitudeDistributionData
    {
        UInt_t fComponent;
        UInt_t fFreqBin;
        TH1D* fDistribution;
    };

    struct THoughData
    {
        UInt_t fComponent;
        TH2D* fTransform;
        Double_t fXOffset;
        Double_t fXScale;
        Double_t fYOffset;
        Double_t fYScale;
    };

    struct TFlattenedSpectrumData
    {
        UInt_t fComponent;
        Double_t fPSDValue;
        Double_t fLabel;
    };


    class KTROOTTreeTypeWriterSpectrumAnalysis : public KTROOTTreeTypeWriter//, public KTTypeWriterSpectrumAnalysis
    {
        public:
            KTROOTTreeTypeWriterSpectrumAnalysis();
            virtual ~KTROOTTreeTypeWriterSpectrumAnalysis();

            void RegisterSlots();

        public:
            void WriteDiscriminatedPoints1D(Nymph::KTDataPtr data);
            void WriteKDTree(Nymph::KTDataPtr data);
            void WriteKDTreeScaled(Nymph::KTDataPtr data);
            void WriteAmplitudeDistributions(Nymph::KTDataPtr data);
            void WriteHoughData(Nymph::KTDataPtr data);
            void WriteFlattenedPSDData(Nymph::KTDataPtr data);

        public:
            TTree* GetDiscriminatedPoints1DTree() const;
            TTree* GetKDTreeTree() const;
            TTree* GetAmplitudeDistributionTree() const;
            TTree* GetHoughTree() const;
            TTree* GetFlattenedPSDTree() const;

        private:
            bool SetupDiscriminatedPoints1DTree();
            bool SetupKDTreeTree();
            bool SetupAmplitudeDistributionTree();
            bool SetupHoughTree();
            bool SetupFlattenedPSDTree();

            void DoWriteKDTree(KTKDTreeData& kdtData, double xScaling, double yScaling);

            std::vector< KTPowerSpectrum > fPowerCache;

            TTree* fDiscPoints1DTree;
            TTree* fKDTreeTree;
            TTree* fAmpDistTree;
            TTree* fHoughTree;
            TTree* fFlattenedPSDTree;

            TDiscriminatedPoints1DData fDiscPoints1DData;
            TKDTreePointData fKDTreePointData;
            TAmplitudeDistributionData fAmpDistData;
            THoughData fHoughData;
            TFlattenedSpectrumData fFlattenedPSDData;

    };

    inline TTree* KTROOTTreeTypeWriterSpectrumAnalysis::GetDiscriminatedPoints1DTree() const
    {
        return fDiscPoints1DTree;
    }

    inline TTree* KTROOTTreeTypeWriterSpectrumAnalysis::GetKDTreeTree() const
    {
        return fKDTreeTree;
    }

    inline TTree* KTROOTTreeTypeWriterSpectrumAnalysis::GetAmplitudeDistributionTree() const
    {
        return fAmpDistTree;
    }

    inline TTree* KTROOTTreeTypeWriterSpectrumAnalysis::GetHoughTree() const
    {
        return fHoughTree;
    }


} /* namespace Katydid */


#endif /* KTROOTTREETYPEWRITERSPECTRUMANALYSIS_HH_ */
