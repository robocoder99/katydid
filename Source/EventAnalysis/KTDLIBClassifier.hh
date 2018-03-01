/*
 * KTDLIBClassifier.hh
 *
 * Created on: Feb 18, 2018
 *     Author: ezayas, L. Saldana 
 */

#ifndef KTDLIBCLASSIFIER_HH_
#define KTDLIBCLASSIFIER_HH_

#include "KTProcessor.hh"
#include "KTData.hh"

#include "KTSlot.hh"

#include "KTClassifierResultsData.hh"

namespace Katydid
{
    
    KTLOGGER(avlog_hh, "KTDLIBClassifier.hh");


    /*
     @class KTDLIBClassifier
     @author E. Zayas, L. Saldana
     @brief Conatins DLIB classifier
     @details
     Reads in a decision function from a trained ML algorithm and uses this to assign a classifier value to a track. The file to read is generated by the dlib c++ library 
     and exists at runtime.   
     Available configuration values:
     - "df-file": std::string -- location of the dat file produced by dlib containing decision function to read
     Slots:
     - "power-fit": void (Nymph::KTDataPtr) -- Performs SVM classification with all classifier features (power, slope, time length, and rotate-project parameters); Requires KTProcessedTrackData and KTPowerFitData; Adds KTClassifierResultsData
     Signals:
     - "classify": void (Nymph::KTDataPtr) -- Emitted upon successful classification; Guarantees KTProcessedTrackData, KTPowerFitData and KTClassifierResultsData
    */

    class KTDLIBClassifier : public Nymph::KTProcessor
    {
        public:
            KTDLIBClassifier(const std::string& name = "dlib-classifier");
            virtual ~KTDLIBClassifier();

            bool Configure(const scarab::param_node* node);

            std::string GetDFFile() const;
            void SetDFFile(std::string fileName);

        private:
            std::string fDFFile;

            // Dlib stuff
            float fPower;
            float fSlope;
            float fTimeLength;
            float fAverage;
            float fRMS;
            float fSkewness;
            float fKurtosis;
            float fNormCentral;
            float fMeanCentral;
            float fSigmaCentral;
            float fMaxCentral;
            float fNPeaks;
            float fCentralPowerRatio;
            float fRMSAwayFromCentral;

        public:
            bool Initialize();
            bool ClassifyTrack(KTClassifierResultsData& resultData, double label);

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fClassifySignal;

            //***************
            // Slots
            //***************

        private:
            void SlotFunctionPowerFitData( Nymph::KTDataPtr data );

    };

    inline std::string KTDLIBClassifier::GetDFFile() const
    {
        return fDFFile;
    }

    inline void KTDLIBClassifier::SetDFFile(std::string fileName)
    {
        fDFFile = fileName;
        return;
    }

}

#endif /* KTDLIBCLASSIFIER_HH_ */
