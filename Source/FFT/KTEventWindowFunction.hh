/*
 * KTEventWindowFunction.hh
 *
 *  Created on: Sep 18, 2011
 *      Author: nsoblath
 */

#ifndef KTEVENTWINDOWFUNCTION_HH_
#define KTEVENTWINDOWFUNCTION_HH_

#include "KTWindowFunction.hh"

#include <vector>
using std::vector;

namespace Katydid
{
    class KTEvent;

    class KTEventWindowFunction : public KTWindowFunction
    {
        public:
            KTEventWindowFunction();
            KTEventWindowFunction(const KTEvent* event);
            virtual ~KTEventWindowFunction();

            /// Sets fBinWidth to event->GetBinWidth(), and adapts the length to be the integral number of bins closest to the current fLength.
            /// Returns the adapted length.
            /// If you also need to set the length, it is recommended that you use AdaptTo(const TEvent*, Double_t) instead of separately setting the length.
            virtual Double_t AdaptTo(const KTEvent* event);
            /// Sets fBinWidth to event->GetBinWidth(), and adapts the length to be the integer multiple of the bin width closest to the given length.
            /// Returns the adapted length.
            virtual Double_t AdaptTo(const KTEvent* event, Double_t length);

            virtual Double_t GetWeight(Double_t time) const = 0;
            virtual Double_t GetWeight(UInt_t bin) const = 0;

            virtual TH1D* CreateHistogram(const std::string& name) const;
            virtual TH1D* CreateHistogram() const;
            virtual TH1D* CreateFrequencyResponseHistogram(const std::string& name) const;
            virtual TH1D* CreateFrequencyResponseHistogram() const;

            virtual Double_t GetLength() const;
            virtual unsigned int GetSize() const;

            virtual Double_t GetBinWidth() const;

            /// Sets fLength to length, and adapts the bin width to be an integral number of bins closest to the current bw.
            /// Returns the adapted bin width.
            /// NOTE: this changes the size of the window!
            virtual Double_t SetLength(Double_t length);
            /// Sets fBinWidth to bw, and adapts the length to be the integral number of bins closest to the current fLength.
            /// Returns the adapted length.
            /// NOTE: this changes the size of the window!
            virtual Double_t SetBinWidth(Double_t bw);
            ///Adapts the given length (length) to the be an integer multiple of the given bin width (bw).
            /// Returns the adapted length.
            /// NOTE: this changes the size of the window!
            Double_t SetBinWidthAndLength(Double_t bw, Double_t length);
            ///Adapts the given bin width to be an integer divisor of the given length (length)
            /// Returns the adapted bin width.
            /// NOTE: this changes the size of the window!
            Double_t SetLengthAndBinWidth(Double_t length, Double_t bw);
            ///Sets the number of bins; leaves fBinWidth as is, and sets fLength accordingly.
            ///Returns the adapted length.
            ///NOTE: this changes the size of the window! (duh)
            virtual Double_t SetSize(UInt_t size);

        protected:
            virtual void RebuildWindowFunction() = 0;

            vector< Double_t > fWindowFunction;

            Double_t fLength;
            Double_t fBinWidth;

            unsigned int fSize;

            ClassDef(KTEventWindowFunction, 2);
    };

} /* namespace Katydid */
#endif /* KTEVENTWINDOWFUNCTION_HH_ */