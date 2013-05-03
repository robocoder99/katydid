/*
 * KTSincWindow.cc
 *
 *  Created on: Apr 18, 2013
 *      Author: nsoblath
 */

#include "KTSincWindow.hh"

#include "KTFactory.hh"
#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTPStoreNode.hh"

#include <cmath>

using std::string;

namespace Katydid
{
    KTLOGGER(windowlog, "katydid.fft");

    static KTDerivedRegistrar< KTWindowFunction, KTSincWindow > sWFSincRegistrar("sinc");

    KTSincWindow::KTSincWindow(const string& name) :
            KTWindowFunction(name)
    {
    }

    KTSincWindow::~KTSincWindow()
    {
    }

    Bool_t KTSincWindow::ConfigureWFSubclass(const KTPStoreNode* node)
    {
        KTDEBUG(windowlog, "Sinc WF configured");
        return true;
    }

    Double_t KTSincWindow::GetWeight(Double_t time) const
    {
        return GetWeight(KTMath::Nint(time / fBinWidth));
    }

    void KTSincWindow::RebuildWindowFunction()
    {
        fWindowFunction.resize(fSize);
        Double_t xVal = 0.;
        Int_t halfSize = Int_t(fSize / 2);
        Double_t twoPiOverNBinsMinus1 = KTMath::TwoPi() / (Double_t)(halfSize - 1);
        for (Int_t iBin=0; iBin < Int_t(fSize); iBin++)
        {
            xVal = twoPiOverNBinsMinus1 * Double_t(iBin-halfSize);
            fWindowFunction[iBin] = xVal == 0. ? 1. : sin(xVal) / xVal;
        }
        return;
    }


} /* namespace Katydid */