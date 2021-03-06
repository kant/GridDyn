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

#ifndef GRIDBUSOPT_H_
#define GRIDBUSOPT_H_

// headers
#include "basicDefs.h"
#include "gridOptObjects.h"
// forward classes

class gridLinkOpt;
class gridLoadOpt;
class gridGenOpt;
class gridBus;

class gridBusOpt : public gridOptObject
{

public:
  enum bus_flags
  {

  };

protected:
  std::vector<gridLoadOpt *> loadList;
  std::vector<gridLinkOpt *> linkList;
  std::vector<gridGenOpt *> genList;

  gridBus *bus = nullptr;
public:
  gridBusOpt (const std::string &objName = "");
  gridBusOpt (gridCoreObject *obj, const std::string &objName = "");
  ~gridBusOpt ();

  gridCoreObject * clone (gridCoreObject *obj = nullptr) const override;
  // add components
  int add (gridCoreObject *obj) override;
  int add (gridLoadOpt *pl);
  int add (gridGenOpt *gen);
  int add (gridLinkOpt *lnk);

  // remove components
  int remove (gridCoreObject *obj)  override;
  int remove (gridLoadOpt *pl);
  int remove (gridGenOpt *gen);
  int remove (gridLinkOpt *lnk);

  virtual void objectInitializeA (unsigned long flags) override;
  virtual void loadSizes (const optimMode &oMode) override;

  virtual void setValues (const optimData *oD, const optimMode &oMode) override;
  //for saving the state
  virtual void guess (double ttime, double val[], const optimMode &oMode) override;
  virtual void getTols (double tols[], const optimMode &oMode) override;
  virtual void getVariableType (double sdata[], const optimMode &oMode) override;

  virtual void valueBounds (double ttime, double upLimit[], double lowerLimit[], const optimMode &oMode) override;

  virtual void linearObj (const optimData *oD, vectData *linObj, const optimMode &oMode) override;
  virtual void quadraticObj (const optimData *oD, vectData *linObj, vectData *quadObj, const optimMode &oMode) override;

  virtual double objValue (const optimData *oD, const optimMode &oMode) override;
  virtual void derivative (const optimData *oD, double deriv[], const optimMode &oMode) override;
  virtual void jacobianElements (const optimData *oD, arrayData<double> *ad, const optimMode &oMode) override;
  virtual void getConstraints (const optimData *oD, arrayData<double> *cons, double upperLimit[], double lowerLimit[], const optimMode &oMode) override;
  virtual void constraintValue (const optimData *oD, double cVals[], const optimMode &oMode) override;
  virtual void constraintJacobianElements (const optimData *oD, arrayData<double> *ad, const optimMode &oMode) override;
  virtual void getObjName (stringVec &objNames, const optimMode &oMode, const std::string &prefix = "") override;


  void disable () override;
  // parameter set functions
  virtual void setOffsets (const optimOffsets &newOffsets, const optimMode &oMode) override;
  virtual void setOffset (index_t offset, index_t constraintOffset, const optimMode &oMode) override;

  void setAll (const std::string &type, std::string param, double val, gridUnits::units_t unitType = gridUnits::defUnit);
  int set (const std::string &param,  const std::string &val) override;
  int set (const std::string &param, double val, gridUnits::units_t unitType = gridUnits::defUnit) override;
  // parameter get functions
  virtual double get (const std::string &param, gridUnits::units_t unitType = gridUnits::defUnit) const override;


  //void alert (gridCoreObject *object, int code);

  // find components
  gridLinkOpt * findLink (gridBus *bs) const;
  gridCoreObject * find (const std::string &objname) const  override;
  gridCoreObject * getSubObject (const std::string &typeName, index_t num) const  override;
  gridCoreObject * findByUserID (const std::string &typeName, index_t searchID) const  override;

  gridOptObject * getLink (index_t x) const  override;
  gridOptObject * getLoad (index_t x = 0) const;
  gridOptObject * getGen (index_t x = 0) const;


  gridOptObject * getBus (index_t /*index*/) const  override
  {
    return const_cast<gridBusOpt *> (this);
  }
  gridOptObject * getArea (index_t /*index*/) const  override
  {
    return static_cast<gridOptObject *> (parent);
  }
protected:
};

//bool compareBus (gridBus *bus1, gridBus *bus2, bool cmpLink = false,bool printDiff = false);

gridBusOpt * getMatchingBusOpt (gridBusOpt *bus, const gridOptObject *src, gridOptObject *sec);

#endif
