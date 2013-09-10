/*
 * KTGainVariationData.cc
 *
 *  Created on: Dec 10, 2012
 *      Author: nsoblath
 */

#include "KTGainVariationData.hh"


namespace Katydid
{
    KTGainVariationData::KTGainVariationData() :
            KTExtensibleData< KTGainVariationData >(),
            fComponentData(1)
    {
        fComponentData[0].fSpline = NULL;
    }

    KTGainVariationData::~KTGainVariationData()
    {
        while (! fComponentData.empty())
        {
            //delete fChannelData.back().fGainVar;
            delete fComponentData.back().fSpline;
            fComponentData.pop_back();
        }
    }

    KTGainVariationData& KTGainVariationData::SetNComponents(UInt_t components)
    {
        UInt_t oldSize = fComponentData.size();
        // if components < oldSize
        for (UInt_t iComponent = components; iComponent < oldSize; iComponent++)
        {
            delete fComponentData[iComponent].fSpline;
        }
        fComponentData.resize(components);
        // if components > oldSize
        for (UInt_t iComponent = oldSize; iComponent < components; iComponent++)
        {
            fComponentData[iComponent].fSpline = NULL;
        }
        return *this;
    }

#ifdef ROOT_FOUND
    TH1D* KTGainVariationData::CreateGainVariationHistogram(UInt_t nBins, UInt_t component, const std::string& name) const
    {
        KTSpline* spline = fComponentData[component].fSpline;
        TH1D* hist = new TH1D(name.c_str(), "Frequency Spectrum: Magnitude", nBins, spline->GetXMin(), spline->GetXMax());
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
            hist->SetBinContent((Int_t)iBin+1, spline->Evaluate(hist->GetBinCenter(iBin+1)));
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Gain Variation");
        return hist;
    }
#endif


} /* namespace Katydid */

