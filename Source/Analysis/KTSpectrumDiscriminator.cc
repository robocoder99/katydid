/*
 * KTSpectrumDiscriminator.cc
 *
 *  Created on: Dec 12, 2012
 *      Author: nsoblath
 */

#include "KTSpectrumDiscriminator.hh"

#include "KTDiscriminatedPoints1DData.hh"
#include "KTDiscriminatedPoints2DData.hh"
#include "KTEvent.hh"
#include "KTFactory.hh"
#include "KTFrequencySpectrum.hh"
#include "KTFrequencySpectrumData.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
#include "KTSlidingWindowFSData.hh"
#include "KTSlidingWindowFSDataFFTW.hh"

#include <cmath>
#include <vector>

using std::string;
using std::vector;
using boost::shared_ptr;

namespace Katydid
{
    KTLOGGER(sdlog, "katydid.analysis");

    static KTDerivedRegistrar< KTProcessor, KTSpectrumDiscriminator > sSimpleFFTRegistrar("spectrum-discriminator");

    KTSpectrumDiscriminator::KTSpectrumDiscriminator() :
            KTProcessor(),
            fSNRThreshold(10.),
            fSigmaThreshold(5.),
            fThresholdMode(eSigma),
            fMinFrequency(0.),
            fMaxFrequency(1.),
            fMinBin(0),
            fMaxBin(1),
            fCalculateMinBin(true),
            fCalculateMaxBin(true),
            fInputDataName("frequency-spectrum"),
            fOutputDataName("peak-list")
    {
        fConfigName = "spectrum-discriminator";

        RegisterSignal("disc-1d", &fDiscrim1DSignal, "void (const KTDiscriminatedPoints1DData*)");
        RegisterSignal("disc-2d", &fDiscrim2DSignal, "void (const KTDiscriminatedPoints2DData*)");

        RegisterSlot("event", this, &KTSpectrumDiscriminator::ProcessEvent, "void (shared_ptr<KTEvent>)");
        RegisterSlot("fsdata", this, &KTSpectrumDiscriminator::ProcessFrequencySpectrumData, "void (const KTFrequencySpectrumData*)");
        RegisterSlot("fsdata-fftw", this, &KTSpectrumDiscriminator::ProcessFrequencySpectrumDataFFTW, "void (const KTFrequencySpectrumDataFFTW*)");
        RegisterSlot("swfsdata", this, &KTSpectrumDiscriminator::ProcessSlidingWindowFSData, "void (const KTSlidingWindowFSData*)");
        RegisterSlot("swfsdata-fftw", this, &KTSpectrumDiscriminator::ProcessSlidingWindowFSDataFFTW, "void (const KTSlidingWindowFSDataFFTW*)");
    }

    KTSpectrumDiscriminator::~KTSpectrumDiscriminator()
    {
    }

    Bool_t KTSpectrumDiscriminator::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        if (node->HasData("snr-threshold"))
        {
            SetSNRThreshold(node->GetData< Double_t >("snr-threshold"));
        }
        if (node->HasData("sigma-threshold"))
        {
            SetSigmaThreshold(node->GetData< Double_t >("sigma-threshold"));
        }

        SetInputDataName(node->GetData<string>("input-data-name", fInputDataName));
        SetOutputDataName(node->GetData<string>("output-data-name", fOutputDataName));

        return true;
    }

    KTDiscriminatedPoints1DData* KTSpectrumDiscriminator::Discriminate(const KTFrequencySpectrumData* data)
    {
        if (fCalculateMinBin) SetMinBin(data->GetSpectrum(0)->FindBin(fMinFrequency));
        if (fCalculateMaxBin) SetMaxBin(data->GetSpectrum(0)->FindBin(fMaxFrequency));

        UInt_t nChannels = data->GetNChannels();

        KTDiscriminatedPoints1DData* newData = new KTDiscriminatedPoints1DData(nChannels);

        newData->SetNBins(data->GetSpectrum(0)->size());
        newData->SetBinWidth(data->GetSpectrum(0)->GetBinWidth());

        // Interval: [fMinBin, fMaxBin)
        UInt_t nBins = fMaxBin - fMinBin + 1;
        Double_t sigmaNorm = 1. / Double_t(nBins - 1);

        for (UInt_t iChannel=0; iChannel<nChannels; iChannel++)
        {

            const KTFrequencySpectrum* spectrum = data->GetSpectrum(iChannel);

            Double_t mean = 0.;
            for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
            {
                mean += (*spectrum)(iBin).abs();
            }
            mean /= (Double_t)nBins;

            Double_t threshold = 0.;
            if (fThresholdMode == eSNR)
            {
                // SNR = P_signal / P_noise = (A_signal / A_noise)^2
                // In this case (i.e. KTFrequencySpectrum), A_noise = mean
                threshold = sqrt(fSNRThreshold) * mean;
                KTDEBUG(sdlog, "Discriminator threshold set at <" << threshold << "> (SNR mode)");
            }
            else if (fThresholdMode == eSigma)
            {
                Double_t sigma = 0., diff;
                for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
                {
                    diff = (*spectrum)(iBin).abs() - mean;
                    sigma += diff * diff;
                }
                sigma = sqrt(sigma * sigmaNorm);

                threshold = mean + fSigmaThreshold * sigma;
                KTDEBUG(sdlog, "Discriminator threshold set at <" << threshold << "> (Sigma mode)");
            }

            newData->SetThreshold(threshold, iChannel);

            // loop over bins, checking against the threshold
            Double_t value;
            for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
            {
                value = (*spectrum)(iBin).abs();
                if (value >= threshold) newData->AddPoint(iBin, value, iChannel);
            }

        }

        newData->SetName(fOutputDataName);
        newData->SetEvent(data->GetEvent());

        fDiscrim1DSignal(newData);

        return newData;
    }

    KTDiscriminatedPoints1DData* KTSpectrumDiscriminator::Discriminate(const KTFrequencySpectrumDataFFTW* data)
    {
        if (fCalculateMinBin) SetMinBin(data->GetSpectrum(0)->FindBin(fMinFrequency));
        if (fCalculateMaxBin) SetMaxBin(data->GetSpectrum(0)->FindBin(fMaxFrequency));

        UInt_t nChannels = data->GetNChannels();

        KTDiscriminatedPoints1DData* newData = new KTDiscriminatedPoints1DData(nChannels);

        newData->SetNBins(data->GetSpectrum(0)->size());
        newData->SetBinWidth(data->GetSpectrum(0)->GetBinWidth());

        // Interval: [fMinBin, fMaxBin)
        UInt_t nBins = fMaxBin - fMinBin + 1;
        Double_t sigmaNorm = 1. / Double_t(nBins - 1);

        // Temporary storage for magnitude values
        vector< Double_t > magnitude(data->GetSpectrum(0)->size());

        for (UInt_t iChannel=0; iChannel<nChannels; iChannel++)
        {
            const KTFrequencySpectrumFFTW* spectrum = data->GetSpectrum(iChannel);
            if (spectrum->size() != magnitude.size())
            {
                magnitude.resize(spectrum->size());
            }

            Double_t mean = 0.;
            for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
            {
                magnitude[iBin] = sqrt((*spectrum)(iBin)[0] * (*spectrum)(iBin)[0] + (*spectrum)(iBin)[1] * (*spectrum)(iBin)[1]);
                mean += magnitude[iBin];
            }
            mean /= (Double_t)nBins;

            Double_t threshold = 0.;
            if (fThresholdMode == eSNR)
            {
                // SNR = P_signal / P_noise = (A_signal / A_noise)^2
                // In this case (i.e. KTFrequencySpectrum), A_noise = mean
                threshold = sqrt(fSNRThreshold) * mean;
                KTDEBUG(sdlog, "Discriminator threshold set at <" << threshold << "> (SNR mode)");
            }
            else if (fThresholdMode == eSigma)
            {
                Double_t sigma = 0., diff;
                for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
                {
                    diff = magnitude[iBin] - mean;
                    sigma += diff * diff;
                }
                sigma = sqrt(sigma * sigmaNorm);

                threshold = mean + fSigmaThreshold * sigma;
                KTDEBUG(sdlog, "Discriminator threshold set at <" << threshold << "> (Sigma mode)");
            }

            newData->SetThreshold(threshold, iChannel);

            // loop over bins, checking against the threshold
            Double_t value;
            for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
            {
                value = magnitude[iBin];
                if (value >= threshold) newData->AddPoint(iBin, value, iChannel);
            }

        }

        newData->SetName(fOutputDataName);
        newData->SetEvent(data->GetEvent());

        fDiscrim1DSignal(newData);

        return newData;
    }

    KTDiscriminatedPoints2DData* KTSpectrumDiscriminator::Discriminate(const KTSlidingWindowFSData* data)
    {
        if (fCalculateMinBin) SetMinBin((*(data->GetSpectra(0)))(0)->FindBin(fMinFrequency));
        if (fCalculateMaxBin) SetMaxBin((*(data->GetSpectra(0)))(0)->FindBin(fMaxFrequency));

        UInt_t nChannels = data->GetNChannels();

        KTDiscriminatedPoints2DData* newData = new KTDiscriminatedPoints2DData(nChannels);

        newData->SetNBinsX(data->GetSpectra(0)->size());
        newData->SetNBinsY((*(data->GetSpectra(0)))(0)->size());
        newData->SetBinWidthX(data->GetSpectra(0)->GetBinWidth());
        newData->SetBinWidthY((*(data->GetSpectra(0)))(0)->GetBinWidth());

        // Interval: [fMinBin, fMaxBin)
        UInt_t nBins = fMaxBin - fMinBin + 1;

        for (UInt_t iChannel=0; iChannel<nChannels; iChannel++)
        {

            const KTPhysicalArray< 1, KTFrequencySpectrum* >* spectra = data->GetSpectra(iChannel);

            Double_t sigmaNorm = 1. / Double_t((nBins * spectra->size()) - 1);

            Double_t mean = 0.;
            for (UInt_t iSpectrum=0; iSpectrum<spectra->size(); iSpectrum++)
            {
                KTFrequencySpectrum* spectrum = (*spectra)(iSpectrum);

                for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
                {
                    mean += (*spectrum)(iBin).abs();
                }
            }
            mean /= (Double_t)(nBins * spectra->size());

            Double_t threshold = 0.;
            if (fThresholdMode == eSNR)
            {
                // SNR = P_signal / P_noise = (A_signal / A_noise)^2
                // In this case (i.e. KTFrequencySpectrum), A_noise = mean
                threshold = sqrt(fSNRThreshold) * mean;
                KTDEBUG(sdlog, "Discriminator threshold set at <" << threshold << "> (SNR mode)");
            }
            else if (fThresholdMode == eSigma)
            {
                Double_t sigma = 0., diff;
                for (UInt_t iSpectrum=0; iSpectrum<spectra->size(); iSpectrum++)
                {
                    KTFrequencySpectrum* spectrum = (*spectra)(iSpectrum);
                    for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
                    {
                        diff = (*spectrum)(iBin).abs() - mean;
                        sigma += diff * diff;
                    }
                }
                sigma = sqrt(sigma * sigmaNorm);

                threshold = mean + fSigmaThreshold * sigma;
                KTDEBUG(sdlog, "Discriminator threshold set at <" << threshold << "> (Sigma mode)");
            }

            newData->SetThreshold(threshold, iChannel);

            // loop over bins, checking against the threshold
            Double_t value;
            for (UInt_t iSpectrum=0; iSpectrum<spectra->size(); iSpectrum++)
            {
                KTFrequencySpectrum* spectrum = (*spectra)(iSpectrum);
                for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
                {
                    value = (*spectrum)(iBin).abs();
                    if (value >= threshold) newData->AddPoint(iSpectrum, iBin, value, iChannel);
                }
            }

        }

        newData->SetName(fOutputDataName);
        newData->SetEvent(data->GetEvent());

        fDiscrim2DSignal(newData);

        return newData;
    }

    KTDiscriminatedPoints2DData* KTSpectrumDiscriminator::Discriminate(const KTSlidingWindowFSDataFFTW* data)
    {
        if (fCalculateMinBin) SetMinBin((*(data->GetSpectra(0)))(0)->FindBin(fMinFrequency));
        if (fCalculateMaxBin) SetMaxBin((*(data->GetSpectra(0)))(0)->FindBin(fMaxFrequency));

        UInt_t nChannels = data->GetNChannels();

        KTDiscriminatedPoints2DData* newData = new KTDiscriminatedPoints2DData(nChannels);

        newData->SetNBinsX(data->GetSpectra(0)->size());
        newData->SetNBinsY((*(data->GetSpectra(0)))(0)->size());
        newData->SetBinWidthX(data->GetSpectra(0)->GetBinWidth());
        newData->SetBinWidthY((*(data->GetSpectra(0)))(0)->GetBinWidth());

        // Interval: [fMinBin, fMaxBin)
        UInt_t nBins = fMaxBin - fMinBin + 1;

        // Temporary storage for magnitude values
        vector< vector< Double_t > > magnitude((*(data->GetSpectra(0)))(0)->size());

        for (UInt_t iChannel=0; iChannel<nChannels; iChannel++)
        {
            const KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* spectra = data->GetSpectra(iChannel);

            Double_t sigmaNorm = 1. / Double_t((nBins * spectra->size()) - 1);

            if (spectra->size() != magnitude.size())
            {
                magnitude.resize(spectra->size());
            }

            Double_t mean = 0.;
            for (UInt_t iSpectrum=0; iSpectrum<spectra->size(); iSpectrum++)
            {
                KTFrequencySpectrumFFTW* spectrum = (*spectra)(iSpectrum);
                if (magnitude[iSpectrum].size() != spectrum->size()) magnitude[iSpectrum].resize(spectrum->size());
                for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
                {
                    magnitude[iSpectrum][iBin] = sqrt((*spectrum)(iBin)[0] * (*spectrum)(iBin)[0] + (*spectrum)(iBin)[1] * (*spectrum)(iBin)[1]);
                    mean += magnitude[iSpectrum][iBin];
                }
            }
            mean /= (Double_t)(nBins * spectra->size());

            Double_t threshold = 0.;
            if (fThresholdMode == eSNR)
            {
                // SNR = P_signal / P_noise = (A_signal / A_noise)^2
                // In this case (i.e. KTFrequencySpectrum), A_noise = mean
                threshold = sqrt(fSNRThreshold) * mean;
                KTDEBUG(sdlog, "Discriminator threshold set at <" << threshold << "> (SNR mode)");
            }
            else if (fThresholdMode == eSigma)
            {
                Double_t sigma = 0., diff;
                for (UInt_t iSpectrum=0; iSpectrum<spectra->size(); iSpectrum++)
                {
                    KTFrequencySpectrumFFTW* spectrum = (*spectra)(iSpectrum);
                    for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
                    {
                        diff = magnitude[iSpectrum][iBin] - mean;
                        sigma += diff * diff;
                    }
                }
                sigma = sqrt(sigma * sigmaNorm);

                threshold = mean + fSigmaThreshold * sigma;
                KTDEBUG(sdlog, "Discriminator threshold set at <" << threshold << "> (Sigma mode)");
            }

            newData->SetThreshold(threshold, iChannel);

            // loop over bins, checking against the threshold
            Double_t value;
            for (UInt_t iSpectrum=0; iSpectrum<spectra->size(); iSpectrum++)
            {
                KTFrequencySpectrumFFTW* spectrum = (*spectra)(iSpectrum);
                for (UInt_t iBin=fMinBin; iBin<fMaxBin; iBin++)
                {
                    value = magnitude[iSpectrum][iBin];
                    if (value >= threshold) newData->AddPoint(iSpectrum, iBin, value, iChannel);
                }
            }
        }

        newData->SetName(fOutputDataName);
        newData->SetEvent(data->GetEvent());

        fDiscrim2DSignal(newData);

        return newData;
    }

    void KTSpectrumDiscriminator::ProcessEvent(shared_ptr<KTEvent> event)
    {
        const KTSlidingWindowFSData* swfsData = dynamic_cast< KTSlidingWindowFSData* >(event->GetData(fInputDataName));
        if (swfsData != NULL)
        {
            KTDiscriminatedPoints2DData* newData = Discriminate(swfsData);
            event->AddData(newData);
            return;
        }

        const KTSlidingWindowFSDataFFTW* swfsDataFFTW = dynamic_cast< KTSlidingWindowFSDataFFTW* >(event->GetData(fInputDataName));
        if (swfsDataFFTW != NULL)
        {
            KTDiscriminatedPoints2DData* newData = Discriminate(swfsDataFFTW);
            event->AddData(newData);
            return;
        }

        const KTFrequencySpectrumData* fsData = dynamic_cast< KTFrequencySpectrumData* >(event->GetData(fInputDataName));
        if (fsData != NULL)
        {
            KTDiscriminatedPoints1DData* newData = Discriminate(fsData);
            event->AddData(newData);
            return;
        }

        const KTFrequencySpectrumDataFFTW* fsDataFFTW = dynamic_cast< KTFrequencySpectrumDataFFTW* >(event->GetData(fInputDataName));
        if (fsDataFFTW != NULL)
        {
            KTDiscriminatedPoints1DData* newData = Discriminate(fsDataFFTW);
            event->AddData(newData);
            return;
        }

        KTWARN(sdlog, "No time series data named <" << fInputDataName << "> was available in the event");
        return;
    }

    void KTSpectrumDiscriminator::ProcessFrequencySpectrumData(const KTFrequencySpectrumData* data)
    {
        KTDiscriminatedPoints1DData* newData = Discriminate(data);
        if (data->GetEvent() != NULL)
            data->GetEvent()->AddData(newData);
        return;
    }

    void KTSpectrumDiscriminator::ProcessFrequencySpectrumDataFFTW(const KTFrequencySpectrumDataFFTW* data)
    {
        KTDiscriminatedPoints1DData* newData = Discriminate(data);
        if (data->GetEvent() != NULL)
            data->GetEvent()->AddData(newData);
        return;
    }

    void KTSpectrumDiscriminator::ProcessSlidingWindowFSData(const KTSlidingWindowFSData* data)
    {
        KTDiscriminatedPoints2DData* newData = Discriminate(data);
        if (data->GetEvent() != NULL)
            data->GetEvent()->AddData(newData);
        return;
    }

    void KTSpectrumDiscriminator::ProcessSlidingWindowFSDataFFTW(const KTSlidingWindowFSDataFFTW* data)
    {
        KTDiscriminatedPoints2DData* newData = Discriminate(data);
        if (data->GetEvent() != NULL)
            data->GetEvent()->AddData(newData);
        return;
    }

} /* namespace Katydid */