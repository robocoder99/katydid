/*
 * KTDataQueueProcessorTemplate.hh
 *
 *  Created on: Jan 10, 2013
 *      Author: nsoblath
 */

#ifndef KTDATAQUEUEPROCESSOR_HH_
#define KTDATAQUEUEPROCESSOR_HH_

#include "KTPrimaryProcessor.hh"

#include "KTConcurrentQueue.hh"
#include "KTData.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
#include "KTSlot.hh"

#include <boost/shared_ptr.hpp>

namespace Katydid
{
    KTLOGGER(eqplog, "katydid.core");

    //***********************************
    // Data Queue Processor Template
    //***********************************

    /*!
     @class KTDataQueueProcessorTemplate
     @author N. S. Oblath

     @brief Template class for creating data queueing processors

     @details

     Available configuration values:
     \li \c "timeout": UInt_t -- maximum time to wait for new data (integer number of milliseconds)

     Slots:

     Signals:
     \li \c "queue-done": void () -- Emitted when queue is emptied
    */
    template< class XProcessorType >
    class KTDataQueueProcessorTemplate : public KTPrimaryProcessor
    {
        public:
            struct DataAndFunc
            {
                boost::shared_ptr< KTData > fData;
                void (XProcessorType::*fFuncPtr)(boost::shared_ptr<KTData>);
            };

            typedef KTConcurrentQueue< DataAndFunc > Queue;

            enum Status
            {
                kStopped,
                kRunning
            };

        public:
            KTDataQueueProcessorTemplate(const std::string& name = "default-data-queue-proc-template-name");
            virtual ~KTDataQueueProcessorTemplate();

            Bool_t Configure(const KTPStoreNode* node);
            virtual Bool_t ConfigureSubClass(const KTPStoreNode* node) = 0;

            Status GetStatus() const;
            void SetStatus(KTDataQueueProcessorTemplate< XProcessorType >::Status);

        protected:
            Status fStatus;

            //**************************************
            // Derived Processor function pointer
            //**************************************
        public:
            void SetFuncPtr(void (XProcessorType::*ptr)(boost::shared_ptr<KTData>));

        protected:
            void (XProcessorType::*fFuncPtr)(boost::shared_ptr<KTData>);


            //*********
            // Queue
            //*********
        public:
            /// Begins processing of queue (switches status from kStopped to kRunning)
            Bool_t Run();

            /// Stops processing of queue (switches status to kStopped)
            void Stop();

            /// Begins processing of queue if status is already kRunning; otherwise does nothing.
            Bool_t ProcessQueue();

            void ClearQueue();

        protected:
            Queue fQueue;

            //*********
            // Queueing functions for slots
            //*********
        protected:
            /// Queue an data object
            /// Assumes ownership of the data; original shared pointer will be nullified
            void DoQueueData(boost::shared_ptr<KTData>& data, void (XProcessorType::*func)(boost::shared_ptr<KTData>));

            /// Queue a list of data objects
            /// Assumes ownership of all data objects and the list; original shared pointers will be nullified
            //void DoQueueDataList(std::list< boost::shared_ptr<KTData>& >* dataList, void (XProcessorType::*fFuncPtr)(boost::shared_ptr<KTData>));

            //*********
            // Signals
            //*********
        protected:
            KTSignalOneArg< void > fQueueDoneSignal;

    };


    //**************************
    // Data Queue Processor
    //**************************

    /*!
     @class KTDataQueueProcessor
     @author N. S. Oblath

     @brief Generic data queue for asynchronous processing

     @details

     Available configuration values:

     Slots:
     \li \c "data": void (shared_ptr< KTData >) -- Queue a data object for asynchronous processing; use signal "data"

     Signals:
     \li \c "data": void (shared_ptr< KTData >) -- Emitted for each data object in the queue
     \li \c "queue-done": void () -- Emitted when queue is emptied (inherited from KTDataQueueProcessorTemplate)
    */
    class KTDataQueueProcessor : public KTDataQueueProcessorTemplate< KTDataQueueProcessor >
    {
        public:
            KTDataQueueProcessor(const std::string& name = "data-queue");
            virtual ~KTDataQueueProcessor();

            Bool_t ConfigureSubClass(const KTPStoreNode* node);

        public:
            void EmitDataSignal(boost::shared_ptr<KTData> data);

            //***************
            // Signals
            //***************

        private:
            KTSignalData fDataSignal;

            //*********
            // Slots
            //*********
        public:
            /// Queue an data object; will emit data signal
            /// Assumes ownership of the data; original shared pointer will be nullified
            void QueueData(boost::shared_ptr<KTData>& data);

            /// Queue a list of data objects; will emit data signal
            /// Assumes ownership of all data objects and the list; original shared pointers will be nullified
            //void QueueDataList(std::list< boost::shared_ptr<KTData> >* dataList);

    };


    //**************************************************
    // Data Queue Processor Template Implementation
    //**************************************************


    template< class XProcessorType >
    KTDataQueueProcessorTemplate< XProcessorType >::KTDataQueueProcessorTemplate(const std::string& name) :
            KTPrimaryProcessor(name),
            fStatus(kStopped),
            fFuncPtr(NULL),
            fQueue(),
            fQueueDoneSignal("queue-done", this)
    {
    }

    template< class XProcessorType >
    KTDataQueueProcessorTemplate< XProcessorType >::~KTDataQueueProcessorTemplate()
    {
        ClearQueue();
    }

    template< class XProcessorType >
    Bool_t KTDataQueueProcessorTemplate< XProcessorType >::Configure(const KTPStoreNode* node)
    {
        fQueue.set_timeout(node->GetData< UInt_t >("timeout", fQueue.get_timeout()));

        if (! ConfigureSubClass(node)) return false;
        return true;
    }

    template< class XProcessorType >
    Bool_t KTDataQueueProcessorTemplate< XProcessorType >::Run()
    {
        fStatus = kRunning;
        KTINFO(eqplog, "Queue started");
        return ProcessQueue();
    }

    template< class XProcessorType >
    void KTDataQueueProcessorTemplate< XProcessorType >::Stop()
    {
        fStatus = kStopped;
        fQueue.interrupt();
        KTINFO(eqplog, "Queue stopped");
        return;
    }

    template< class XProcessorType >
    void KTDataQueueProcessorTemplate< XProcessorType >::SetFuncPtr(void (XProcessorType::*ptr)(boost::shared_ptr<KTData>))
    {
        fFuncPtr = ptr;
        return;
    }


    template< class XProcessorType >
    Bool_t KTDataQueueProcessorTemplate< XProcessorType >::ProcessQueue()
    {
        KTINFO(eqplog, "Beginning to process queue");
        while (fStatus != kStopped)
        {
            KTDEBUG(eqplog, "processing . . .");
            DataAndFunc daf;
            if (fQueue.timed_wait_and_pop(daf))
            {
                KTDEBUG(eqplog, "Data acquired for processing");
                (static_cast<XProcessorType*>(this)->*(daf.fFuncPtr))(daf.fData);
                if (daf.fData->fLastData) fStatus = kStopped;
            }
            else
            {
                fStatus = kStopped;
            }
        }
        fQueueDoneSignal();
        KTINFO(eqplog, "Queue processing has ended");
        return true;
    }

    template< class XProcessorType >
    void KTDataQueueProcessorTemplate< XProcessorType >::ClearQueue()
    {
        while (! fQueue.empty())
        {
            DataAndFunc daf;
            fQueue.try_pop(daf);
        }
        KTINFO(eqplog, "Queue cleared");
        return;
    }


    template< class XProcessorType >
    void KTDataQueueProcessorTemplate< XProcessorType >::DoQueueData(boost::shared_ptr<KTData>& data, void (XProcessorType::*func)(boost::shared_ptr<KTData>))
    {
        KTDEBUG(eqplog, "Queueing data");
        DataAndFunc daf;
        daf.fData = data; // I'd like to use move semantics here (operator=(shared_ptr&&)), but they didn't work, so I bootstrapped with copy and reset.
        data.reset();
        daf.fFuncPtr = func;
        fQueue.push(daf);
        return;
    }
/*
    template< class XProcessorType >
    void KTDataQueueProcessorTemplate< XProcessorType >::DoQueueDataList(std::list< boost::shared_ptr<KTData>& >* dataList, void (XProcessorType::*func)(boost::shared_ptr<KTData>))
    {
        typedef std::list< boost::shared_ptr<KTData> > DataList;

        KTDEBUG(eqplog, "Queueing data objects");
        DataAndFunc daf;
        while (! dataList->empty())
        {
            daf.fData = &(dataList->front()); // using move semantics
            daf.fFuncPtr = func;
            dataList->pop_front();
            fQueue.push(daf);
        }
        delete dataList;
        return;
    }
*/

} /* namespace Katydid */
#endif /* KTDATAQUEUEPROCESSOR_HH_ */