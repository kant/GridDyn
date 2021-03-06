/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil;  eval: (c-set-offset 'innamespace 0); -*- */
/*
   * LLNS Copyright Start
 * Copyright (c) 2016, Lawrence Livermore National Security
 * This work was performed under the auspices of the U.S. Department
 * of Energy by Lawrence Livermore National Laboratory in part under
 * Contract W-7405-Eng-48 and in part under Contract DE-AC52-07NA27344.
 * Produced at the Lawrence Livermore National Laboratory.
 * All rights reserved.
 * For details, see the LICENSE file.
 * LLNS Copyright End
*/

#include "objectFactory.h"

objectFactory::objectFactory (const std::string & /*componentName*/, const std::string & typeName) : name (typeName)
{
}

objectFactory::objectFactory (const std::string & /*componentName*/, const stringVec & typeNames) : name (typeNames[0])
{
}


void objectFactory::prepObjects (count_t /*count*/, gridCoreObject * /*obj*/)
{
}

count_t objectFactory::remainingPrepped () const
{
  return 0;
}

objectFactory::~objectFactory ()
{
}

componentFactory::componentFactory (const std::string typeName) : name (typeName)
{
  m_defaultType = "";
}

componentFactory::~componentFactory ()
{

}

void componentFactory::registerFactory (std::string typeName, objectFactory *oFac)
{
  auto ret = m_factoryMap.insert (std::pair<std::string, objectFactory *> (typeName, oFac));
  if (ret.second == false)
    {
      ret.first->second = oFac;
    }
}

void componentFactory::registerFactory (objectFactory *oFac)
{
  registerFactory (oFac->name, oFac);
}

stringVec componentFactory::getTypeNames ()
{
  stringVec tnames;
  tnames.reserve (m_factoryMap.size ());
  for (auto tname : m_factoryMap)
    {
      tnames.push_back (tname.first);
    }
  return tnames;
}

gridCoreObject *componentFactory::makeObject ()
{
  if (!m_defaultType.empty ())
    {
      gridCoreObject *obj = m_factoryMap[m_defaultType]->makeObject ();
      return obj;
    }
  else
    {
      return nullptr;
    }
}

bool componentFactory::isValidType (const std::string &typeName) const
{
  auto mfind = m_factoryMap.find (typeName);
  return (mfind != m_factoryMap.end ());
}

gridCoreObject *componentFactory::makeObject (const std::string &type)
{
  auto mfind = m_factoryMap.find (type);
  if (mfind != m_factoryMap.end ())
    {
      gridCoreObject *obj = m_factoryMap[type]->makeObject ();
      return obj;
    }
  else
    {
      if (!m_defaultType.empty ())
        {
          gridCoreObject *obj = m_factoryMap[m_defaultType]->makeObject ();
          return obj;
        }
      else
        {
          return nullptr;
        }

    }
}

gridCoreObject *componentFactory::makeObject (const std::string &type, const std::string &objName)
{

  auto mfind = m_factoryMap.find (type);
  if (mfind != m_factoryMap.end ())
    {
      gridCoreObject *obj = m_factoryMap[type]->makeObject (objName);
      return obj;
    }
  else
    {
      if (!m_defaultType.empty ())
        {
          gridCoreObject *obj = m_factoryMap[m_defaultType]->makeObject (objName);
          return obj;
        }
      else
        {
          return nullptr;
        }

    }
}

void componentFactory::setDefault (const std::string &type)
{
  if (type.empty ())
    {
      return;
    }
  cMap::iterator mfind = m_factoryMap.find (type);
  if (mfind != m_factoryMap.end ())
    {
      m_defaultType = type;
    }
}


objectFactory *componentFactory::getFactory (const std::string &typeName)
{
  if (typeName.empty ())
    {
      return m_factoryMap[m_defaultType];
    }
  else
    {
      auto mfind = m_factoryMap.find (typeName);
      if (mfind != m_factoryMap.end ())
        {
          return m_factoryMap[typeName];
        }
      else
        {
          return nullptr;
        }
    }
}

//create a high level object factory for the coreObject class




std::shared_ptr<coreObjectFactory> coreObjectFactory::instance ()
{
  static std::shared_ptr<coreObjectFactory> factory = std::shared_ptr<coreObjectFactory> (new coreObjectFactory ());
  return factory;
}

void coreObjectFactory::registerFactory (const std::string  name, std::shared_ptr<componentFactory >tf)
{
  auto ret = m_factoryMap.insert (std::pair < std::string, std::shared_ptr < componentFactory >> ( name, tf));
  if (ret.second == false)
    {
      ret.first->second = tf;
    }
}

void coreObjectFactory::registerFactory (std::shared_ptr<componentFactory> tf)
{
  auto ret = m_factoryMap.insert (std::pair < std::string, std::shared_ptr < componentFactory >> (tf->name, tf));
  if (ret.second == false)
    {
      ret.first->second = tf;
    }
}

stringVec coreObjectFactory::getFactoryNames ()
{
  stringVec fnames;
  fnames.reserve (m_factoryMap.size ());
  for (auto fname : m_factoryMap)
    {
      fnames.push_back (fname.first);
    }
  return fnames;
}

stringVec coreObjectFactory::getTypeNames (const std::string &componentName)
{
  auto mfind = m_factoryMap.find (componentName);
  if (mfind != m_factoryMap.end ())
    {
      return m_factoryMap[componentName]->getTypeNames ();
    }
  else
    {
      /* *INDENT-OFF* */
       return {};
      /* *INDENT-ON* */
    }
}

gridCoreObject *coreObjectFactory::createObject (const std::string &obType, const std::string &typeName)
{
  auto mfind = m_factoryMap.find (obType);
  if (mfind != m_factoryMap.end ())
    {
      gridCoreObject *obj = m_factoryMap[obType]->makeObject (typeName);
      return obj;
    }
  else
    {
      return nullptr;
    }
}

gridCoreObject *coreObjectFactory::createObject (const std::string &obType, const std::string &typeName, const std::string &objName)
{
  auto mfind = m_factoryMap.find (obType);
  if (mfind != m_factoryMap.end ())
    {
      gridCoreObject *obj = m_factoryMap[obType]->makeObject (typeName, objName);
      return obj;
    }
  else
    {
      return nullptr;
    }
}

std::shared_ptr<componentFactory> coreObjectFactory::getFactory (const std::string &componentName)
{
  auto mfind = m_factoryMap.find (componentName);
  if (mfind != m_factoryMap.end ())
    {
      return (m_factoryMap[componentName]);
    }
  else       //make a new factory
    {
      auto tf = std::make_shared<componentFactory> ();
      tf->name = componentName;
      m_factoryMap.insert (std::pair < std::string, std::shared_ptr < componentFactory >> (componentName, tf));
      return tf;
    }

}

bool coreObjectFactory::isValidObject (const std::string &componentName)
{
  auto mfind = m_factoryMap.find (componentName);
  return (mfind != m_factoryMap.end ());
}

bool coreObjectFactory::isValidType (const std::string &componentName, const std::string &typeName)
{
  auto mfind = m_factoryMap.find (componentName);
  if (mfind != m_factoryMap.end ())
    {
      return mfind->second->isValidType (typeName);
    }
  else
    {
      return false;
    }
}


void coreObjectFactory::prepObjects (const std::string &componentName, const std::string &typeName, count_t numObjects, gridCoreObject *obj)
{
  auto mfind = m_factoryMap.find (componentName);
  if (mfind != m_factoryMap.end ())
    {
      auto obfact = m_factoryMap[componentName]->getFactory (typeName);
      if (obfact)
        {
          obfact->prepObjects (numObjects, obj);
        }
    }
}

void coreObjectFactory::prepObjects (const std::string &componentName, count_t numObjects, gridCoreObject *obj)
{
  auto mfind = m_factoryMap.find (componentName);
  if (mfind != m_factoryMap.end ())
    {
      auto obfact = m_factoryMap[componentName]->getFactory ("");
      obfact->prepObjects (numObjects, obj);
    }
}

coreObjectFactory::coreObjectFactory ()
{

}

coreObjectFactory::~coreObjectFactory ()
{

}
