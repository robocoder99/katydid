/**
 @file KTSimpleFFT.hh
 @brief Contains KTSimpleFFT
 @details Calculates a 1-dimensional FFT on a set of real data.
 @author: N. S. Oblath
 @date: Sep 12, 2011
 */

#ifndef KTSIMPLEFFT_HH_
#define KTSIMPLEFFT_HH_

#include "KTFFT.hh"
#include "KTProcessor.hh"

#include "KTLogger.hh"

#include "TFFTRealComplex.h"

#include <string>
#include <vector>

class TH1D;

namespace Katydid
{
    KTLOGGER(fftlog_simp, "katydid.fft");

    class KTComplexVector;
    class KTEggHeader;
    class KTPowerSpectrum;
    class KTPStoreNode;
    class KTEvent;

    template< size_t NDims, typename XDataType >
    class KTPhysicalArray;

    /*!
     @class KTSimpleFFT
     @author N. S. Oblath

     @brief A one-dimensional real-to-complex FFT class.

     @details
     KTSimpleFFT performs a real-to-complex FFT on a one-dimensional array of doubles.

     The FFT is currently performed by ROOT's adaptation of FFTW. Specifically the TFFTRealComplex class.

     Available configuration values:
     \li \c transform_flag -- flag that determines how much planning is done prior to any transforms

     Slots:
     \li \c void ProcessHeader(const KTEggHeader* header)
     \li \c void ProcessEvent(UInt_t iEvent, const KTEvent* event)
     Signals:
     \li \c void (UInt_t, const KTSimpleFFT*) emitted upon performance of a transform.
    */

    class KTSimpleFFT : public KTFFT, public KTProcessor
    {
        public:
            typedef KTSignal< void (UInt_t, const KTSimpleFFT*) >::signal FFTSignal;

        public:
            KTSimpleFFT();
            KTSimpleFFT(UInt_t timeSize);
            virtual ~KTSimpleFFT();

            Bool_t Configure(const KTPStoreNode* node);

            void ClearTransformResults();

            virtual void InitializeFFT();

            virtual Bool_t TransformEvent(const KTEvent* event);

            template< typename XDataType >
            KTComplexVector* Transform(const std::vector< XDataType >& data);

            void AddTransformResult(KTComplexVector* result);

            virtual TH1D* CreatePowerSpectrumHistogram(const std::string& name,  UInt_t channelNum = 0) const;
            virtual TH1D* CreatePowerSpectrumHistogram(UInt_t channelNum = 0) const;

            virtual KTPhysicalArray< 1, Double_t >* CreatePowerSpectrumPhysArr(UInt_t channelNum = 0) const;

            virtual KTPowerSpectrum* CreatePowerSpectrum(UInt_t channelNum = 0) const;
            virtual UInt_t GetTimeSize() const;
            virtual UInt_t GetFrequencySize() const;

            /// note: SetTimeSize creates a new fTransform.
            ///       It also sets fIsInitialized to kFALSE.
            void SetTimeSize(UInt_t nBins);

            const TFFTRealComplex* GetFFT() const;
            const KTComplexVector* GetTransformResult(UInt_t channelNum = 0) const;
            const std::string& GetTransformFlag() const;
            Bool_t GetIsInitialized() const;
            Double_t GetFreqBinWidth() const;

            /// note: SetTransoformFlag sets fIsInitialized and fIsDataReady to kFALSE.
            void SetTransformFlag(const std::string& flag);
            void SetFreqBinWidth(Double_t bw);

        protected:
            KTComplexVector* ExtractTransformResult();

            TFFTRealComplex* fTransform;
            std::vector< KTComplexVector* > fTransformResults;

            std::string fTransformFlag;

            Bool_t fIsInitialized;

            Double_t fFreqBinWidth;

            //***************
            // Signals
            //***************

        private:
            FFTSignal fFFTSignal;

            //***************
            // Slots
            //***************

        public:
            void ProcessHeader(const KTEggHeader* header);
            void ProcessEvent(UInt_t iEvent, const KTEvent* event);

    };


    template< typename XDataType >
    KTComplexVector* KTSimpleFFT::Transform(const std::vector< XDataType >& data)
    {
        unsigned int nBins = (unsigned int)data.size();
        if (nBins != (unsigned int)fTransform->GetSize())
        {
            KTWARN(fftlog_simp, "Number of bins in the data provided does not match the number of bins set for this transform\n"
                    << "   Bin expected: " << fTransform->GetSize() << ";   Bins in data: " << nBins);
            return NULL;
        }

        for (unsigned int iPoint=0; iPoint<nBins; iPoint++)
        {
            fTransform->SetPoint(iPoint, Double_t(data[iPoint]));
        }

        fTransform->Transform();

        return ExtractTransformResult();
    }


    inline UInt_t KTSimpleFFT::GetTimeSize() const
    {
        return fTransform->GetSize();
    }

    inline UInt_t KTSimpleFFT::GetFrequencySize() const
    {
        return fTransform->GetSize() / 2 + 1;
    }

    inline void KTSimpleFFT::SetTimeSize(UInt_t nBins)
    {
        delete fTransform;
        fTransform = new TFFTRealComplex((Int_t)nBins, kFALSE);
        fIsInitialized = kFALSE;
        return;
    }

    inline const TFFTRealComplex* KTSimpleFFT::GetFFT() const
    {
        return fTransform;
    }

    inline const KTComplexVector* KTSimpleFFT::GetTransformResult(UInt_t channelNum) const
    {
        return fTransformResults[channelNum];
    }

    inline const std::string& KTSimpleFFT::GetTransformFlag() const
    {
        return fTransformFlag;
    }

    inline Bool_t KTSimpleFFT::GetIsInitialized() const
    {
        return fIsInitialized;
    }

    inline Double_t KTSimpleFFT::GetFreqBinWidth() const
    {
        return fFreqBinWidth;
    }

    inline void KTSimpleFFT::SetTransformFlag(const std::string& flag)
    {
        fTransformFlag = flag;
        fIsInitialized = kFALSE;
        return;
    }

    inline void KTSimpleFFT::SetFreqBinWidth(Double_t bw)
    {
        fFreqBinWidth = bw;
        return;
    }

} /* namespace Katydid */
#endif /* KTSIMPLEFFT_HH_ */
