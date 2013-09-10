/*
 * KTDataQueueProcessor.cc
 *
 *  Created on: Feb 5, 2013
 *      Author: nsoblath
 */

#include "KTDataQueueProcessor.hh"

#include "KTNOFactory.hh"
#include "KTPStoreNode.hh"

using boost::shared_ptr;

//using std::list;

namespace Katydid
{
    static KTDerivedNORegistrar< KTProcessor, KTDataQueueProcessor > sDQProcRegistrar("data-queue");

    KTDataQueueProcessor::KTDataQueueProcessor(const std::string& name) :
        KTDataQueueProcessorTemplate< KTDataQueueProcessor >(name),
        fDataSignal("data", this)
    {
        RegisterSlot("data", this, &KTDataQueueProcessor::QueueData);
        //RegisterSlot("data-list", this, &KTDataQueueProcessor::QueueDataList);
    }

    KTDataQueueProcessor::~KTDataQueueProcessor()
    {

    }

    Bool_t KTDataQueueProcessor::ConfigureSubClass(const KTPStoreNode* node)
    {
        return true;
    }

    void KTDataQueueProcessor::EmitDataSignal(boost::shared_ptr<KTData> data)
    {
        fDataSignal(data);
        return;
    }

    void KTDataQueueProcessor::QueueData(shared_ptr< KTData>& data)
    {
        return DoQueueData(data, &KTDataQueueProcessor::EmitDataSignal);
    }
/*
    void KTDataQueueProcessor::QueueDataList(list< shared_ptr< KTData > >* dataList)
    {
        return DoQueueDataList(dataList, &KTDataQueueProcessor::EmitDataSignal);
    }
*/
}
