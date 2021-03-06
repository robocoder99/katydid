/*
 * KTFrequencySpectrumDataPolar.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTFrequencySpectrumDataPolar.hh"

namespace Katydid
{
    KTFrequencySpectrumDataPolarCore::KTFrequencySpectrumDataPolarCore() :
            KTFrequencySpectrumData(),
            fSpectra(1)
    {
        fSpectra[0] = NULL;
    }

    KTFrequencySpectrumDataPolarCore::~KTFrequencySpectrumDataPolarCore()
    {
        while (! fSpectra.empty())
        {
            delete fSpectra.back();
            fSpectra.pop_back();
        }
    }


    const std::string KTFrequencySpectrumDataPolar::sName("frequency-spectrum-polar");

    KTFrequencySpectrumDataPolar::KTFrequencySpectrumDataPolar() :
            KTFrequencySpectrumDataPolarCore(),
            KTExtensibleData< KTFrequencySpectrumDataPolar >()
    {
    }

    KTFrequencySpectrumDataPolar::~KTFrequencySpectrumDataPolar()
    {
    }

    KTFrequencySpectrumDataPolar& KTFrequencySpectrumDataPolar::SetNComponents(unsigned components)
    {
        unsigned oldSize = fSpectra.size();
        // if components < oldSize
        for (unsigned iComponent = components; iComponent < oldSize; ++iComponent)
        {
            delete fSpectra[iComponent];
        }
        fSpectra.resize(components);
        // if components > oldSize
        for (unsigned iComponent = oldSize; iComponent < components; ++iComponent)
        {
            fSpectra[iComponent] = NULL;
        }
        return *this;
    }


    const std::string KTFrequencySpectrumVarianceDataPolar::sName("frequency-spectrum-variance-polar");

    KTFrequencySpectrumVarianceDataPolar::KTFrequencySpectrumVarianceDataPolar() :
            KTFrequencySpectrumVarianceDataCore(),
            KTExtensibleData()
    {
    }

    KTFrequencySpectrumVarianceDataPolar::~KTFrequencySpectrumVarianceDataPolar()
    {
    }

    KTFrequencySpectrumVarianceDataPolar& KTFrequencySpectrumVarianceDataPolar::SetNComponents(unsigned num)
    {
        unsigned oldSize = fSpectra.size();
        // if num < oldSize
        for (unsigned iComponent = num; iComponent < oldSize; ++iComponent)
        {
            delete fSpectra[iComponent];
        }
        fSpectra.resize(num);
        // if num > oldSize
        for (unsigned iComponent = oldSize; iComponent < num; ++iComponent)
        {
            fSpectra[iComponent] = NULL;
        }
        return *this;
    }


} /* namespace Katydid */

