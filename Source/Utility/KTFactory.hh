/*
 * KTFactory.hh
 *
 *  Created on: Jul 31, 2012
 *      Author: nsoblath
 */

#ifndef KTFACTORY_HH_
#define KTFACTORY_HH_

#include "KTSingleton.hh"
#include "KTLogger.hh"

#include <map>
#include <string>

namespace Katydid
{
    KTLOGGER(utillog_factory, "katydid.utility");

    template< class XBaseType >
    class KTFactory;

    template< class XBaseType >
    class KTRegistrar
    {
        public:
            KTRegistrar() {}
            virtual ~KTRegistrar() {}

        public:
            friend class KTFactory< XBaseType >;

        protected:
            virtual XBaseType* Create() const = 0;

    };

    template< class XBaseType, class XDerivedType >
    class KTDerivedRegistrar : public KTRegistrar< XBaseType >
    {
        public:
            KTDerivedRegistrar(const std::string& className);
            virtual ~KTDerivedRegistrar();

        protected:
            void Register(const std::string& className) const;

            XBaseType* Create() const;

    };


    template< class XBaseType >
    class KTFactory : public KTSingleton< KTFactory< XBaseType > >
    {
        protected:
            typedef std::map< std::string, const KTRegistrar< XBaseType >* > FactoryMap;
            typedef typename FactoryMap::value_type FactoryEntry;
            typedef typename FactoryMap::iterator FactoryIt;
            typedef typename FactoryMap::const_iterator FactoryCIt;

        public:
            XBaseType* Create(const std::string& className);
            void Register(const std::string& className, const KTRegistrar< XBaseType >* registrar);

        protected:
            FactoryMap* fMap;


        protected:
            friend class KTSingleton< KTFactory >;
            friend class KTDestroyer< KTFactory >;
            KTFactory();
            ~KTFactory();
    };

    template< class XBaseType >
    XBaseType* KTFactory< XBaseType >::Create(const std::string& className)
    {
        FactoryCIt it = fMap->find(className);
        if (it == fMap->end())
        {
            KTERROR(utillog_factory, "Did not find factory for <" << className << ">.");
            return NULL;
        }

        return it->second->Create();
    }

    template< class XBaseType >
    void KTFactory< XBaseType >::Register(const std::string& className, const KTRegistrar< XBaseType >* registrar)
    {
        FactoryCIt it = fMap->find(className);
        if (it != fMap->end())
        {
            KTERROR(utillog_factory, "Already have factory registered for <" << className << ">.");
            return;
        }
        fMap->insert(std::pair< std::string, const KTRegistrar< XBaseType >* >(className, registrar));
    }

    template< class XBaseType >
    KTFactory< XBaseType >::KTFactory() :
        fMap(new FactoryMap())
    {}

    template< class XBaseType >
    KTFactory< XBaseType >::~KTFactory()
    {
        delete fMap;
    }




    template< class XBaseType, class XDerivedType >
    KTDerivedRegistrar< XBaseType, XDerivedType >::KTDerivedRegistrar(const std::string& className) :
            KTRegistrar< XBaseType >()
    {
        Register(className);
    }

    template< class XBaseType, class XDerivedType >
    KTDerivedRegistrar< XBaseType, XDerivedType >::~KTDerivedRegistrar()
    {}

    template< class XBaseType, class XDerivedType >
    void KTDerivedRegistrar< XBaseType, XDerivedType >::Register(const std::string& className) const
    {
        KTFactory< XBaseType >::GetInstance()->Register(className, this);
        return;
    }

    template< class XBaseType, class XDerivedType >
    XBaseType* KTDerivedRegistrar< XBaseType, XDerivedType >::Create() const
    {
        return dynamic_cast< XBaseType* >(new XDerivedType());
    }


} /* namespace Katydid */
#endif /* KTFACTORY_HH_ */