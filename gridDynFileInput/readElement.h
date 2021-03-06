/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil;  eval: (c-set-offset 'innamespace 0); -*- */
/*
 * LLNS Copyright Start
 * Copyright (c) 2014, Lawrence Livermore National Security
 * This work was performed under the auspices of the U.S. Department
 * of Energy by Lawrence Livermore National Laboratory in part under
 * Contract W-7405-Eng-48 and in part under Contract DE-AC52-07NA27344.
 * Produced at the Lawrence Livermore National Laboratory.
 * All rights reserved.
 * For details, see the LICENSE file.
 * LLNS Copyright End
*/

#ifndef GRIDDYNREADELEMENT_H_
#define GRIDDYNREADELEMENT_H_

// headers
#include "basicDefs.h"
#include "gridDynFileInput.h"
#include "readerHelper.h"
#include <string>
#include <unordered_set>
#include <memory>
class gridParameter;

//struct for holding and passing the information in Element reader files
class readerInfo;
class gridCoreObject;
class gridLoad;
class gridDynGenerator;
class gridArea;
class gridLink;
class gridEventInfo;
class gridSimulation;
class gridDynSimulation;
class gridRelay;
class gridBus;
class gridPrimary;
class gridSecondary;
class gridSubModel;

//forward declarations
class readerElement;

gridBus * readBusElement (std::shared_ptr<readerElement> &element, readerInfo *ri, gridCoreObject *searchObject = nullptr);
gridRelay * readRelayElement (std::shared_ptr<readerElement> &element, readerInfo *ri, gridCoreObject *searchObject = nullptr);

//gridLoad * readLoadElement (std::shared_ptr<readerElement> &element, readerInfo *ri, gridCoreObject *searchObject = nullptr);
//gridDynGenerator * readGeneratorElement (std::shared_ptr<readerElement> &element, readerInfo *ri, gridCoreObject *searchObject = nullptr);
gridLink * readLinkElement (std::shared_ptr<readerElement> &element, readerInfo *ri, gridCoreObject *searchObject = nullptr, bool warnlink = true);
gridArea * readAreaElement (std::shared_ptr<readerElement> &element, readerInfo *ri, gridCoreObject *searchObject = nullptr);
gridSimulation * readSimulationElement (std::shared_ptr<readerElement> &element, readerInfo *ri, gridCoreObject *searchObject = nullptr, gridSimulation *gs = nullptr);

gridCoreObject * readEconElement (std::shared_ptr<readerElement> &element, readerInfo *ri, gridCoreObject *searchObject = nullptr);
void readArrayElement (std::shared_ptr<readerElement> &element, readerInfo *ri, gridCoreObject *parentObject);
void loadConditionElement (std::shared_ptr<readerElement> &element, readerInfo *ri, gridCoreObject *parentObject);
void loadSubObjects (std::shared_ptr<readerElement> &element, readerInfo *ri, gridCoreObject *parentObject);

void readImports (std::shared_ptr<readerElement> &element, readerInfo *ri, gridCoreObject *parentObject, bool finalFlag);

void loadDefines (std::shared_ptr<readerElement> &element, readerInfo *ri); //NOTE: defined in readLibraryElement.cpp
void loadDirectories (std::shared_ptr<readerElement> &element, readerInfo *ri); //NOTE: defined in readLibraryElement.cpp
void loadTranslations (std::shared_ptr<readerElement> &element, readerInfo *ri); //NOTE: defined in readLibraryElement.cpp
void loadCustomSections (std::shared_ptr<readerElement> &element, readerInfo *ri); //NOTE: defined in readLibraryElement.cpp

void loadSolverElement (std::shared_ptr<readerElement> &element, readerInfo *ri, gridDynSimulation *parentObject);
void readLibraryElement (std::shared_ptr<readerElement> &element,readerInfo *ri);

typedef std::unordered_set<std::string> IgnoreListType;

void loadElementInformation (gridCoreObject *obj, std::shared_ptr<readerElement> &element, const std::string &objectName, readerInfo *ri, const IgnoreListType &ignoreList);

void objSetAttributes (gridCoreObject *obj, std::shared_ptr<readerElement> &element, const std::string &typeName, readerInfo *ri, const IgnoreListType &ignoreList);

void paramLoopElement (gridCoreObject *obj, std::shared_ptr<readerElement> &element, const std::string &typeName, readerInfo *ri, const IgnoreListType &ignoreList);

int loadEventElement (std::shared_ptr<readerElement> &element, gridCoreObject *obj, readerInfo *ri);
int loadRecorderElement (std::shared_ptr<readerElement> &element, gridCoreObject *obj, readerInfo *ri);

gridParameter * getElementParam (const std::shared_ptr<readerElement> &element,gridParameter *param = nullptr);

std::string findElementName (std::shared_ptr<readerElement> &el, const std::string &ename, readerConfig::match_type matching = readerConfig::match_type::strict_case_match);

std::string getElementField (std::shared_ptr<readerElement> &element, const std::string &ename, readerConfig::match_type matching = readerConfig::match_type::strict_case_match);
std::string getElementAttribute (std::shared_ptr<readerElement> &element, const std::string &ename, readerConfig::match_type matching = readerConfig::match_type::strict_case_match);
std::string getElementFieldOptions (std::shared_ptr<readerElement> &element, const stringVec &names, readerConfig::match_type matching = readerConfig::match_type::strict_case_match);
stringVec getElementFieldMultiple (std::shared_ptr<readerElement> &element, std::string ename, readerConfig::match_type matching = readerConfig::match_type::strict_case_match);

void setIndex (std::shared_ptr<readerElement> &element, gridCoreObject *mobj, readerInfo *ri);
std::string getObjectName (std::shared_ptr<readerElement> &element, readerInfo *ri);

gridCoreObject* getParent (std::shared_ptr<readerElement> &element, readerInfo *ri, gridCoreObject *parentObject, const std::string &alternateName = "");

const std::string emptyString = "";
const std::string areaTypeString = "area";
const std::string busTypeString = "bus";


inline const std::string &parentSearchComponent (gridCoreObject *)
{
  return emptyString;
}


inline const std::string &parentSearchComponent (gridPrimary *)
{
  return areaTypeString;
}


inline const std::string &parentSearchComponent (gridSecondary *)
{
  return busTypeString;
}

#endif
