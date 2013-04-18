/*
 * KTBlackmanHarrisWindow.cc
 *
 *  Created on: Apr 18, 2013
 *      Author: nsoblath
 */

#include "KTBlackmanHarrisWindow.hh"

#include "KTFactory.hh"
#include "KTMath.hh"
#include "KTPStoreNode.hh"

#include <cmath>

using std::string;

namespace Katydid
{
    static KTDerivedRegistrar< KTWindowFunction, KTBlackmanHarrisWindow > sEWFHammRegistrar("blackman-harris-window");

    KTBlackmanHarrisWindow::KTBlackmanHarrisWindow(const string& name) :
            KTWindowFunction(name)
    {
    }

    KTBlackmanHarrisWindow::~KTBlackmanHarrisWindow()
    {
    }

    Bool_t KTBlackmanHarrisWindow::ConfigureWFSubclass(const KTPStoreNode* node)
    {
        return true;
    }

    Double_t KTBlackmanHarrisWindow::GetWeight(Double_t time) const
    {
        return GetWeight(KTMath::Nint(time / fBinWidth));
    }

    void KTBlackmanHarrisWindow::RebuildWindowFunction()
    {
        fWindowFunction.resize(fSize);
        Double_t twoPiOverNBinsMinus1 = KTMath::TwoPi() / (Double_t)(fSize - 1);
        for (Int_t iBin=0; iBin<fSize; iBin++)
        {
            fWindowFunction[iBin] = 0.35875 - 0.48829 * cos(Double_t(iBin) * twoPiOverNBinsMinus1) +
                    0.14128 * cos(Double_t(2 * iBin) * twoPiOverNBinsMinus1) - 0.01168 * cos(Double_t(3 * iBin) * twoPiOverNBinsMinus1);
        }
        return;
    }

} /* namespace Katydid */