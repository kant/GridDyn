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

#ifndef GRIDAREA_H_
#define GRIDAREA_H_

// headers
#include "gridObjects.h"
#include "primary/listMaintainer.h"

// forward classes
class gridDynSimulation;
class gridRelay;
class gridLink;
class gridBus;
class gridDynGenerator;
class gridCoreList;

/** @brief class implmenting a power system area
 the area class acts as a container for other primary objects including areas
it also acts as focal point for wide area controls such as AGC and can compute other functions and statistics across a wide area
*/
class gridArea : public gridPrimary
{
  friend class listMaintainer;
public:
  /** @brief flags for area operations and control*/
  enum area_flags
  {
    reverse_converge = object_flag1,           //!< flag indicating that the area should do a convergence/algebraic loop in reverse
    direction_oscillate = object_flag2,           //!< flag indicating that the direction of iteration for convergence functions should flip every time the function is called
  };
  static count_t areaCount;  //!< basic counter for the areas to compute an id

protected:
  std::vector<gridBus *> m_Buses;              //!< list of buses contained in a the area
  std::vector<gridLink *> m_Links;          //!< links completely inside the area
  std::vector<gridLink *> m_externalLinks;    //!< links going to other areas
  std::vector<gridArea *> m_Areas;            //!< list of the areas contained within the parent area
  std::vector<gridRelay *> m_Relays;         //!< list of relay objects

  std::vector<gridPrimary *> primaryObjects; //!< list of all the primary objects in the area
  //this is done to break apart the headers
  std::unique_ptr<gridCoreList> obList;      //a search index for object names

  std::vector<gridPrimary *> rootObjects;//!< list of objects with roots
  std::vector<gridPrimary *> pFlowAdjustObjects;  //!< list of objects with Pflow checks
  std::vector<std::shared_ptr<gridPrimary>> objectHolder;  //!< storage location for shared ptrs to an object
  listMaintainer opObjectLists;
  int masterBus = -1;                   //!< the master bus for frequency calculations purposes
  int zone = 1;                                 //!< the zone of the area
  double fTarget=1.0;                 //!<[puHz] a target frequency
public:
  /** @brief the default constructor*/
  gridArea (const std::string &objName = "area_$");
  /** @brief the default destructor*/
  virtual ~gridArea ();

  virtual gridCoreObject * clone (gridCoreObject *obj = nullptr) const override;
  // add components
  virtual int add (gridCoreObject *obj) override;
  /** @brief add a bus to the area
  @param[in] bus  the bus to add
  @return success indicator  OBJECT_ADD_SUCCESS(0) const on success
  */
  virtual int add (gridBus *bus);
  /** @brief add a link to the area
  @param[in] lnk  the link to add
  @return success indicator  OBJECT_ADD_SUCCESS(0) on success
  */
  virtual int add (gridLink *lnk);
  /** @brief add an area to the area
  @param[in] area  the area to add
  @return success indicator  OBJECT_ADD_SUCCESS(0) on success
  */
  virtual int add (gridArea *area);
  /** @brief add a relay to the area
  @param[in] relay  the relay to add
  @return success indicator  OBJECT_ADD_SUCCESS(0) on success
  */
  virtual int add (gridRelay *relay);
  // remove components
  virtual int remove (gridCoreObject *obj) override;
  /** @brief remove a bus from the area
  @param[in] bus  the bus to remove
  @return success indicator  OBJECT_REMOVE_SUCCESS(0) const on success
  */
  virtual int remove (gridBus *bus);
  /** @brief remove a link from the area
  @param[in] lnk  the link to remove
  @return success indicator  OBJECT_REMOVE_SUCCESS(0) on success
  */
  virtual int remove (gridLink *lnk);
  /** @brief remove an area from the area
  @param[in] area  the area to remove
  @return success indicator  OBJECT_REMOVE_SUCCESS(0) on success
  */
  virtual int remove (gridArea *area);
  /** @brief remove a relay from the area
  @param[in] relay  the relay to remove
  @return success indicator  OBJECT_REMOVE_SUCCESS(0) on success
  */
  virtual int remove (gridRelay *relay);

  //get component models
  virtual gridBus * getBus (index_t x) const override;
  virtual gridLink * getLink (index_t x) const override;
  virtual gridArea * getArea (index_t x) const  override;
  virtual gridRelay * getRelay (index_t x) const override;
  /** @brief get a generator by index number
   this is kind of an ugly function but needed for some applications to search through all buses
  @param[in] x  the index of the generator to search for
  @return a point to the generator or nullptr
  */
  virtual gridDynGenerator * getGen (index_t x);     //
  // initializeB

  virtual void setOffsets (const solverOffsets &newOffsets, const solverMode &sMode) override;
  virtual void setOffset (index_t offset, const solverMode &sMode) override;

  virtual void loadSizes (const solverMode &sMode, bool dynOnly) override;
  virtual void setRootOffset (index_t Roffset, const solverMode &sMode) override;

protected:
  virtual void pFlowObjectInitializeA (double time0, unsigned long flags) override;
  virtual void pFlowObjectInitializeB () override;

  //initializeB dynamics
  virtual void dynObjectInitializeA (double time0, unsigned long flags) override;
  virtual void dynObjectInitializeB (IOdata &outputSet) override;

public:
  virtual void setTime (double time) override;

  virtual double timestep (double ttime, const solverMode &sMode) override;

  //TODO:: Pt make this do something
  /** @brief update the angles may be deprecated
  @param[in] time the time to update to
  */
  virtual void updateTheta (double /*time*/);

  // parameter set functions
  virtual int setFlag (const std::string &flag, bool val) override;
  virtual int set (const std::string &param,  const std::string &val) override;
  virtual int set (const std::string &param, double val, gridUnits::units_t unitType = gridUnits::defUnit) override;
  virtual void getParameterStrings (stringVec &pstr, paramStringType pstype = paramStringType::all) const override;
  void setAll (const std::string &type, std::string param, double val, gridUnits::units_t unitType = gridUnits::defUnit) override;

  virtual double get (const std::string &param, gridUnits::units_t unitType = gridUnits::defUnit) const override;
  /** @brief determine if an object is already a member of the area
  @param[in] obj  the object to check
  @return true if the object is a member false if not
  */
  virtual bool isMember (gridCoreObject *obj) const;
  // find components
  virtual gridCoreObject * find (const std::string &objname) const override;
  virtual gridCoreObject * getSubObject (const std::string &typeName, index_t num) const override;
  virtual gridCoreObject * findByUserID (const std::string &typeName, index_t searchID) const override;
  // solver functions

  virtual void alert (gridCoreObject *obj, int code) override;

  virtual void getStateName (stringVec &stNames, const solverMode &sMode, const std::string &prefix = "") const override;
  virtual void preEx (const stateData *sD, const solverMode &sMode) override;
  virtual void jacobianElements (const stateData *sD, arrayData<double> *ad, const solverMode &sMode) override;
  virtual void residual (const stateData *sD, double resid[], const solverMode &sMode) override;
  virtual void derivative (const stateData *sD, double deriv[], const solverMode &sMode) override;
  virtual void algebraicUpdate (const stateData *sD, double update[], const solverMode &sMode, double alpha) override;

  virtual void delayedResidual (const stateData *sD, double resid[], const solverMode &sMode) override;
  virtual void delayedDerivative (const stateData *sD, double deriv[], const solverMode &sMode) override;
  virtual void delayedJacobian (const stateData *sD, arrayData<double> *ad, const solverMode &sMode) override;
  virtual void delayedAlgebraicUpdate (const stateData *sD, double update[], const solverMode &sMode, double alpha) override;



  virtual change_code powerFlowAdjust (unsigned long flags, check_level_t level) override;
  virtual void pFlowCheck (std::vector<violation> &Violation_vector) override;
  virtual void setState (double ttime, const double state[], const double dstate_dt[], const solverMode &sMode) override;
  //for identifying which variables are algebraic vs differential
  virtual void getVariableType (double sdata[], const solverMode &sMode) override;
  virtual void getTols (double tols[], const solverMode &sMode) override;
  // dynamic simulation
  virtual void guess (double ttime, double state[], double dstate_dt[], const solverMode &sMode) override;

  /** @brief try to do a local converge on the solution
   to be replaced by the algebraic update function soon
  @param[in] ttime the time
  @param[in/out]  the system state
  @param[in/out]  the system state derivative
  @param[in] sMode  the solverMode corresponding ot the state
  @param[in] tol  the tolerance to converge to
  @param[in]  mode the mode to do the convergence
  */
  virtual void converge (double ttime, double state[], double dstate_dt[], const solverMode &sMode, converge_mode mode, double tol) override;
  virtual void updateLocalCache () override;

  virtual void updateLocalCache (const stateData *sD, const solverMode &sMode) override;

  virtual void reset (reset_levels level) override;
  //root finding functions
  virtual void rootTest (const stateData *sD, double roots[], const solverMode &sMode) override;
  virtual void rootTrigger (double ttime, const std::vector<int> &rootMask, const solverMode &sMode) override;
  virtual change_code rootCheck (const stateData *sD, const solverMode &sMode,  check_level_t level) override;
  //grab information
  /** @brief get a vector of voltage from the attached buses
  @param[out] V the vector to put the bus  voltages
  @param[in] start  the index into the vector V to start the voltage states from this area
  @return an index where the last value was placed
  */
  count_t getVoltage (std::vector<double> &V, index_t start = 0) const;
  /** @brief get a vector of voltage from the attached buses
  @param[out] V the vector to put the bus  voltages
  @param[in] state  the system state
  @parma[in] sMode the solverMode corresponding to the states
  @param[in] start  the index into the vector V to start the voltage states from this area
  @return an index where the last value was placed
  */
  count_t getVoltage (std::vector<double> &V, const double state[], const solverMode &sMode, index_t start = 0) const;
  /** @brief get a vector of angles from the attached buses
  @param[out] V the vector to put the bus  angles
  @param[in] start  the index into the vector V to start the angle states from this area
  @return an index where the last value was placed
  */
  count_t getAngle (std::vector<double> &V, index_t start = 0) const;
  /** @brief get a vector of angles from the attached buses
  @param[out] V the vector to put the bus  angles
  @param[in] state  the system state
  @parma[in] sMode the solverMode corresponding to the states
  @param[in] start  the index into the vector V to start the angle states from this area
  @return an index where the last value was placed
  */
  count_t getAngle (std::vector<double> &V, const double state[], const solverMode &sMode, index_t start = 0) const;
  /** @brief get a vector of real power from the attached links
  @param[out] A the vector to put the link real powers
  @param[in] start  the index into the vector V to start the real power values
  @param[in] busNum the bus index of the link to pick off the powers
  @return an index where the last value was placed
  */
  count_t getLinkRealPower (std::vector<double> &A, index_t start = 0, int busNum = 1) const;
  /** @brief get a vector of reactive power from the attached links
  @param[out] A the vector to put the link reactive powers
  @param[in] start  the index into the vector V to start the reactive power values
  @param[in]  busNum the bus index of the link to pick off the powers
  @return an index where the last value was placed
  */
  count_t getLinkReactivePower (std::vector <double> &A, index_t start = 0, int busNum = 1) const;
  /** @brief get a vector of losses of the attached links
  @param[out] L the vector to put the losses
  @param[in] start  the index into the vector V to start the angle states from this area
  @return an index where the last value was placed
  */
  count_t getLinkLoss (std::vector<double> &L, index_t start = 0) const;
  /** @brief get a vector of generation power from the attached buses
  @param[out] A the vector to put the bus real power from generators
  @param[in] start  the index into the vector A to start the generation power values from this area
  @return an index where the last value was placed
  */
  count_t getBusGenerationReal (std::vector<double> &A, index_t start = 0) const;
  /** @brief get a vector of generation reactive power from the attached buses
  @param[out] A the vector to put the bus reactive power from generators
  @param[in] start  the index into the vector A to start the generation power values from this area
  @return an index where the last value was placed
  */
  count_t getBusGenerationReactive (std::vector <double> &A, index_t start = 0) const;
  /** @brief get a vector of bus load power from the attached buses
  @param[out] A the vector to put the bus load real power from bus loads
  @param[in] start  the index into the vector A to start the load power values from this area
  @return an index where the last value was placed
  */
  count_t getBusLoadReal (std::vector<double> &A, index_t start = 0) const;
  /** @brief get a vector of bus load reactive power from the attached buses
  @param[out] A the vector to put the bus load reactive power from bus loads
  @param[in] start  the index into the vector A to start the load reactive power values from this area
  @return an index where the last value was placed
  */
  count_t getBusLoadReactive (std::vector <double> &A, index_t start = 0) const;
  /** @brief get a vector of bus names
  @param[out] nm the vector to put the bus names
  @param[in] start  the index into the vector nm to start the area bus names
  @return an index where the last value was placed
  */
  count_t getBusName (stringVec &nm, index_t start = 0) const;
  /** @brief get a vector of link names
  @param[out] nm the vector to put the link names
  @param[in] start  the index into the vector nm to start the area link names
  @return an index where the last value was placed
  */
  count_t getLinkName (stringVec &nm, index_t start = 0) const;
  /** @brief get a vector of buses attached to the area links
  @param[out] nm the vector to put the bus names
  @param[in] start  the index into the vector nm to start the area link names
  @param[in] busNum  the side of the link to get the bus names for
  @return an index where the last value was placed
  */
  count_t getLinkBus (stringVec &nm, index_t start = 0,int busNum = 0) const;

  /** @brief get the total adjustable CapacityUp for the area within a certain time frame
  @param[in] time  the time within which to make the adjustment
  @return athe total adjustable capacity Up
  */
  double getAdjustableCapacityUp (double time = kBigNum) const;
  /** @brief get the total adjustable Capacity Down for the area within a certain time frame
  @param[in] time  the time within which to make the adjustment
  @return athe total adjustable capacity Down
  */
  double getAdjustableCapacityDown (double time = kBigNum) const;
  /** @brief get the total loss for contained links
  @return the total area loss
  */
  double getLoss () const;
  /** @brief get the total area real generation
  @return the total area real generation
  */
  double getGenerationReal () const;
  /** @brief get the total area reactive generation
  @return the total area lreactive Generation
  */
  double getGenerationReactive () const;
  /** @brief get the total area real load power
  @return the real Load power for the area
  */
  double getLoadReal () const;
  /** @brief get the total area reactive load power
  @return the reactive Load power for the area
  */
  double getLoadReactive () const;
  /** @brief get the average angle for the area
  @return the average angle
  */
  double getAvgAngle () const;
  /** @brief get the average angle for the area
  @param[in] the state data
  @param[in] the solverMode corresponding to the state data
  @return the average angle
  */
  double getAvgAngle (const stateData *sD, const solverMode &sMode) const;
  /** @brief get the total tie line flows into/out of the area
  @return the total tie line flows
  */
  double getTieFlowReal () const;
  /** flag all the voltage states
  * get a vector with an indicator of voltage states
  *@param[out] vStates a vector with a value of 1.0 for all voltage states and 0 otherwise
  *
  */
  void getVoltageStates (double vStates[], const solverMode &sMode) const;
  void getAngleStates (double aStates[],const solverMode &sMode) const;
  double getMasterAngle (const stateData *sD, const solverMode &sMode) const;
  virtual void updateFlags (bool dynOnly = false) override;
  /** @brief  get a vector of buses of the area
  @param[out] busList  a vector of buses
  @param[in] start  the index to start placing the bus pointers
  @return the the total number of buses placed start+busCount
  */
  count_t getBusVector (std::vector<gridBus *> &busList, index_t start = 0) const;
private:
  template<class X>
  friend int addObject (gridArea *area, X* obj, std::vector<X *> &objVector);

  template<class X>
  friend int removeObject (gridArea *area, X* obj, std::vector<X *> &objVector);


};


/** @brief find the matching area in a different tree
  searches a cloned object tree to find the corresponding bus
@param[in] area  the area to search for
@param[in] src  the existing parent object
@param[in] sec  the desired parent object tree
@return a pointer to an area on the second tree that matches the area based on name and location
*/
gridArea * getMatchingArea (gridArea *area, gridPrimary *src, gridPrimary *sec);
#endif
