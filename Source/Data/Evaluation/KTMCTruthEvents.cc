/*
 * KTMCTruthEvents.cc
 *
 *  Created on: Apr 11, 2013
 *      Author: nsoblath
 */

#include "KTMCTruthEvents.hh"

namespace Katydid
{

    KTMCTruthEvents::KTMCTruthEvents() :
        KTExtensibleData< KTMCTruthEvents >(),
        fEvents(),
        fRecordSize(1)
    {
    }

    KTMCTruthEvents::~KTMCTruthEvents()
    {
    }

} /* namespace Katydid */