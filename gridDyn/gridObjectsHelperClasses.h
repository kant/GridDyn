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

#ifndef GRIDOBJECTHELPERCLASSES_H_
#define GRIDOBJECTHELPERCLASSES_H_

#include "gridDynTypes.h"
#include <bitset>

const static std::uint64_t flagMask = 0x3FE;  //!< general flag mask for convenience to mask out flags that typically cascade to parents

#define FLAG_NOT_FOUND (1)
#define FLAG_NOT_EDITABLE (2)
#define FLAG_FOUND (0)

/** @brief  an enumeration of flags in the opFlags bitset for gridObject
*/
enum operation_flags
{
  //indicator flags 0 -15 are general indicators flags which default false

  has_updates = 0,        //!< flag indicating if an object uses updates

  //typically cascading flags
  has_constraints = 1,        //!<flag indicating if an object uses constraints
  has_roots = 2,        //!<flag indicating if an object uses root finding
  has_alg_roots = 3,         //!< flag indicated the object has states to solve for
  has_powerflow_adjustments = 4,        //!<flag indicating if an object has voltage adjustements for power flow
  preEx_requested = 5,        //!<flag indicating if an object requests preexecution
  uses_bus_frequency = 6,        //!<flag indicating if an object uses bus frequency calculation
  has_pflow_states = 7,           //!< indicator if the object has pflow states if in question
  has_dyn_states = 8,             //!< indicator if the object has dynamic states if in question
  has_differential_states = 9,       //!< indicator if the object has differential states
  extra_cascading_flag = 10,
  // end of typically cascading flags

  //for handling remote voltage control capabilities
  remote_voltage_control = 11,   //!< indicator that the object controls a remote bus voltage at a specific level
  local_voltage_control = 12,     //!<indicator that the object controls the local bus voltage at a specific level
  indirect_voltage_control = 13,  //!< flag indicating the object must use indirect means to control the voltage
  //for handling remote power control capabilities
  remote_power_control = 14,   //!< indicator that the object controls a remote bus voltage at a specific level
  local_power_control = 15,     //!<indicator that the object controls the local bus voltage at a specific level
  indirect_power_control = 16,  //!< flag indicating the object must use indirect means to control the voltage

  //some indicator flags for local objects
  pFlow_initialized = 17,       //!< indicator that pflow initialize has been completed
  dyn_initialized = 18,      //!<  indicator that dyn Initialize has been completed
  object_armed_flag = 19,        //!<basically an extra object flag if the obect has a trigger mechanism of some sort
  late_b_initialize = 20,         //!< flag indicating the object would like to initialized after most other objects only acknowledged by areas and then only within the area
  adjustable_P = 21,        //!< flag indicating that the object has adjustable power setting and can be used by slk bus for control of angle
  adjustable_Q = 22,        //!< flag indicating that an object has controllable reactive power for power flow
  error_flag = 23,

  //flags  24- 31 indicating some sort of condition change
  state_change_flag = 24,   //!< flag indicating that the state size or nature has changed
  object_change_flag = 25,  //!< flag indicating that an object has changed activity state
  constraint_change_flag = 26,  //!< flag indicating an change in constrachange_code values
  root_change_flag = 27,        //!< flag indicating a change in the root finding functions
  jacobian_count_change_flag = 28,  //!< flag indicating a change in the Jacobian count
  slack_bus_change = 29,   //!< flag indicating a change in the slack bus
  voltage_control_change = 30,  //!< flag indicating a change in voltage control on a bus
  connectivity_change_flag = 31,  //!< flag indicating a change in bus connectivity possibly indicating islanding or isolated buses

  /*flags 32-44 are intended for local object usage*/
  object_flag1 = 32,
  object_flag2 = 33,
  object_flag3 = 34,
  object_flag4 = 35,
  object_flag5 = 36,
  object_flag6 = 37,
  object_flag7 = 38,
  object_flag8 = 39,
  object_flag9 = 40,
  object_flag10 = 41,
  object_flag11 = 42,
  object_flag12 = 43,

  //flags 43 - 45 state control
  no_pflow_states = 44,   //!< flag indicating there is not nor will ever there be power flow states
  no_dyn_states = 45,//!< flag indicating there is not nor will ever there be dynamic states

  disable_flag_updates = 46, //flag to temporarilly disable flag updates from the alert function
  flag_update_required = 47, //!< flag indicated that a flag update is required
  pflow_constant_initialization = 48, //!< flag indicating that an object is using pflow initialization for dynamic elements

  //Various informative flags that can be used in some situations
  disconnected = 49, //!< flag indicating that the object is disconnected
  differential_output = 50, //!< flag that the model has a differential state variable that is the primary output
  no_gridobject_set = 51,  //!< flag indicating skipping of the gridCoreObject set function for parent setting
  being_deleted = 52,  //!<  flag indicating the object is in the process of being deleted NOTE::useful for some large objects with components allocated in larger fashion so we skip over some steps in object removal

  /*flags 53-63 are intended for object capabilities*/

  multipart_calculation_capable = 53,
  extra_capability_flag1 = 54,
  extra_capability_flag2 = 55,
  extra_capability_flag3 = 56,
  dc_only = 57,                       //!<flag indicating the object must be attached to a dc bus
  dc_capable = 58,                    //!<flag indicating the object can be attached to a dc bus
  dc_terminal2 = 59,                      //!<flag indicating the terminal 2 must be a dc bus
  three_phase_only = 61,              //!<flag indicating the object must be attached to a 3 phase bus
  three_phase_capable = 62,           //!<flag indicating the object can be attached to a 3 phase bus
  three_phase_terminal2 = 63,           //!<flag indicating the terminal 2 must be attached to a 3 phase bus


};

/** @brief enumeration of possible convergence modes
*/
enum class converge_mode
{
  single_iteration,        //!< a single iteration loop
  voltage_only,         //!< only iterate on the voltage
  high_error_only,        //!< only iterate on high error states
  local_iteration,         //!< do a simple iteration to tolerance
  block_iteration,                      //!< do an interation loop at higher level
  strong_iteration,        //!< do a stronger iteration to tolerance
};

//for the controlFlags bitset used for initialization and powerFlowAdjustments

/** @brief control flag locations for initialization functions */
enum init_control_flags
{
  constraints_disabled = 0,  //!< disable all constraints
  roots_disabled = 1,  //!< disable all roots
  unused_control_flag1 = 2, //!< currently unused
  no_exciter_limits = 3,  //!< ignore exciter limits
  no_governor_limits = 4,  //!< ignore governor limits
  no_limits = 5,  //!< ignore all limits
  ignore_bus_limits = 6,  //!< ignore bus limits
  no_link_adjustments = 7,  //!< disable all link adjustments
  no_load_adjustments = 8,  //!< disable all load adjustments
  auto_bus_disconnect = 9,  //!< disable automatic bus disconnection in exceptional circumstances
  no_auto_autogen = 10,  //!< disable automatic autogeneration for slk/afix/pv buses
  all_loads_to_constant_impedence = 11, //!< convert all loads to constant impedance
  force_constant_pflow_initialization = 12, //!< for some objects that initialize through power flow calculations force it to be constant
  ignore_saturation = 13, //!< ignore saturation effects
  low_voltage_checking = 15,  //!< enable low voltage checking on buses
};

#define CHECK_CONTROLFLAG(flag,fname) (((flag) & (1 << fname)) != 0)
#define SET_CONTROLFLAG(flag,fname) ((flag) |= (1 << fname))
#define CLEAR_CONTROLFLAG(flag,fname) ((flag) &= (~(1 << fname)))

/** @brief get the lower 32 bits of a flag variable*
@param[in] flags the 64 bit flags to get the lower half from
*/
inline unsigned long lower_flags (unsigned long long flags)
{
  return static_cast<unsigned long> (flags & static_cast<unsigned long long>(0xFFFFFFFF));
}

/** @ brief get the lower 32 bits of 64 bit bitset*
@param[in] flags the 64 bit bitset flags to get the lower half from
*/
inline unsigned long lower_flags (std::bitset<64> flags)
{
  return static_cast<unsigned long> (flags.to_ullong () & static_cast<unsigned long long>(0xFFFFFFFF));
}

#define RESET_CHANGE_FLAG_MASK (0xFFFFFFFF00FFFFFF)  //macro to change flag masks

inline bool anyChangeFlags (std::bitset<64> flags)
{
  return ((flags.to_ullong () & (0x00FF000000)) > 0);
}

#define MIN_CHANGE_ALERT (500)  //!< the minimum change alert flag
#define MAX_CHANGE_ALERT (900)  //!< the maximum change alert flag

#define INVALID_STATE_ALERT (585)  //!< invalid state alert
#define INTIALIZATION_FAILURE (587)   //!< indication an object failed to initialize
#define FLAG_CHANGE (605)                       //!< flag change alert

#define CONNECTIVITY_CHANGE (607)  //!< connectivity change alert

#define ROOT_COUNT_CHANGE (590)     //!< change in the number of roots
#define ROOT_COUNT_INCREASE (591)       //!< increase in the number of roots
#define ROOT_COUNT_DECREASE (592)               //!< decrease in the number of roots

#define STATE_COUNT_CHANGE (600)                //!< change in the number of states
#define STATE_COUNT_INCREASE (601)              //!< state count increase
#define STATE_COUNT_DECREASE (602)              //!< state count decrease
#define STATE_IDENTITY_CHANGE (604)  //!< states change from algebraic to differential or vice versa

#define OBJECT_COUNT_CHANGE (615)               //!< change in the number of active objects
#define OBJECT_COUNT_INCREASE (616)             //!< increase in the number of active objects
#define OBJECT_COUNT_DECREASE (617)             //!< decrease in the number of active objects

#define JAC_COUNT_CHANGE (630)                  //!< change in the number non-zero Jacobian entries
#define JAC_COUNT_INCREASE (631)                //!< increase in the number non-zero Jacobian entries
#define JAC_COUNT_DECREASE (632)                //!< decrease in the number non-zero Jacobian entries

#define SLACK_BUS_CHANGE (655)                  //!< change in the slack bus

#define CONSTRAINT_COUNT_CHANGE (670)           //!< change in the contraint count
#define CONSTRAINT_COUNT_INCREASE (671)         //!< increase in the number of constraints
#define CONSTRAINT_COUNT_DECREASE (672)         //!< decrease in the number of constraints

#define POTENTIAL_FAULT_CHANGE (690)            //!< change in a fault condition
#define VERY_LOW_VOLTAGE_ALERT (700)            //!< low voltage alert

#define VOLTAGE_CONTROL_CHANGE (710)            //!< change in voltage control (used by buses)

#define VOLTAGE_CONTROL_UPDATE (401)            //!< update to voltage control parameters (used by buses)
#define POWER_CONTROL_UDPATE (403)                      //!< update to power control parameters (used by buses)
#define PV_CONTROL_UDPATE (406)                         //!< update to PV control parameters (used by buses)

#define SINGLE_STEP_REQUIRED 2001                       //!< indicator that an object requires single step updates
#define SINGLE_STEP_NOT_REQUIRED 2002           //!< indicator that an object no longer requires single step updates
#define UPDATE_REQUIRED 2010                            //!< indicator that an object is using an update function
#define UPDATE_NOT_REQUIRED 2011                        //!< indicator that an object is no longer using an update function

/** @brief define the reset levels
*/
enum class reset_levels
{
  //Normal reset levels
  minimal = 0,      //!< a minimal reset
  voltage = 1,      //!< reset the voltage levels
  angle = 2,        //!< reset the angles
  voltage_angle = 3,      //!< reset the voltage and the angle
  full = 4,      //!< do a full reset
  //Low voltage reset levels
  low_voltage_pflow = -2,        //!< reset low voltage levels on power flow
  low_voltage_dyn1 = -1,         //!< reset low voltage levels on dynamic simulation
  low_voltage_dyn2 = -10,        //!< reset low voltage levels on dynamic simulation mode 2
  low_voltage_dyn0 = -12,        //!< reset low voltage levels on dynamic simulation mode 0
};

enum class check_level_t
{
  reversable_only = 0,
  full_check = 1,
  low_voltage_check = 3,
  complete_state_check = 4,
};

#include <vector>
#include "solverMode.h"
/** @brief helper struct for containing sizes to group the data*/
class stateSizes
{
public:
  //state sizes
  count_t diffSize = 0;                                  //!< number of differential variables
  count_t algSize = 0;                                   //!< number of algebraic variables
  count_t vSize = 0;                                  //!< number of voltage variables
  count_t aSize = 0;                                   //!< number of angle variables
  //root sizes
  count_t diffRoots = 0;                               //!< number of roots on purely differential states
  count_t algRoots = 0;                                   //!< number of roots based algebraic components

  count_t jacSize = 0;                                 //!<upper bound on number of Jacobian entries

  void reset ();
  void stateReset ();
  void rootAndJacobianReset ();

  void add (const stateSizes &arg);
  void addStateSizes (const stateSizes &arg);
  void addRootAndJacobianSizes (const stateSizes &arg);
};

/**
*@brief Helper class encapsulating the offsets for the solver evaluation functions
 acts as a container for solver offsets and object indices into the state vectors
**/
class solverOffsets
{

public:
  const solverMode *sMode = &cLocalSolverMode;         //!<pointer to the reference sMode
  index_t aOffset = kNullLocation;     //!< Location for the voltage offset
  index_t vOffset = kNullLocation;     //!< Location for the Angle offset
  index_t algOffset = kNullLocation;      //!< location for generic offsets
  index_t diffOffset = kNullLocation;     //!< location for the differential offsets
  index_t rootOffset = kNullLocation;     //!< location for the root offsets


  //local objectSizes
  stateSizes local;        //!< container for local state sizes

  stateSizes total;       //!< container for total state sizes;

  //total object sizes

  bool stateLoaded = false;       //!<flag indicating the state sizes have been loaded
  bool rjLoaded = false;          //!< flag indicated that roots and jac size is loaded
  bool offetLoaded = false;       //!<flag indicating that offsets have been loaded
public:
  /** @brief  default constructor*/
  solverOffsets ()
  {
  }
  /** @brief  copy constructor from pointer*/
  solverOffsets (const solverOffsets *);
  /** @brief  copy constructor*/
  solverOffsets (const solverOffsets &nOffsets);

  /** @brief reset the solverOffset
  */
  void reset ();

  /** @brief reset the solverOffset root and Jacobian component
  */
  void rootAndJacobianCountReset ();

  /** @brief reset the solverOffset state components
  */
  void stateReset ();

  /** @brief increment the offsets using the contained sizes to generate the expected next offset
  */
  void increment ();

  /** @brief increment the offsets using the contained sizes in another solverOffset Object
  /param offsets the solverOffset object to use as the sizes
  */
  void increment (const solverOffsets *offsets);

  /** @brief increment the offsets using the contained local sizes in another solverOffset Object
  /param offsets the solverOffset object to use as the sizes
  */
  void localIncrement (const solverOffsets *offsets);

  /** @brief merge the sizes of two solverOffsets
  /param offsets the solverOffset object to use as the sizes
  */
  void addSizes (const solverOffsets *offsets);

  /** @brief merge the sizes of two solverOffsets state Sizes
    / param offsets the solverOffset object to use as the sizes
    */
  void addStateSizes (const solverOffsets *offsets);

  /** @brief load the Dynamic parameters to the sizes
  */
  void addRootAndJacobianSizes (const solverOffsets *offsets);

  /** @brief load the local variables to the sizes
  */
  void localLoad (bool finishedLoading = false);

  /** @brief set the offsets from another solverOffset object
  /param newOffsets the solverOffset object to use as the sizes
  */
  void setOffsets (const solverOffsets &newOffsets);

  /** @brief set the offsets from a single index
  /param newOffset the index of the new offset
  */
  void setOffset (index_t newOffset);
};



/**@brief local state pointers
*/
class Lp
{
public:
  double time = 0;                                              //!< time
  count_t algOffset = kNullLocation;    //!< data offset for algebraic components
  count_t diffOffset = kNullLocation;   //!< data offset for differential components
  const double *algStateLoc = nullptr;  //!< location of algebraic state variables
  const double *diffStateLoc = nullptr; //!< location of differential state values
  const double *dstateLoc = nullptr;    //!< location of derivatives of differential components
  double *destLoc = nullptr;                    //!< location to place calculations for algebraic component
  double *destDiffLoc = nullptr;                //!< location to place calculations for differential component
  count_t algSize = 0;                                  //!< size of algebraic component
  count_t diffSize = 0;                                 //!< size of differential component
};

/**@brief class for containing state data information
*/
class stateData
{
public:
  double time = 0.0;                    //!< time corresponding to the state data
  const double *state = nullptr;        //!< the current state guess
  const double *dstate_dt = nullptr;    //!< the state time derivative array
  const double *fullState = nullptr;    //!< the full state data (for cases where state contains only differential or algebraic components)
  const double *diffState = nullptr;    //!< the differential state data (for cases where state contains only algebraic components)
  const double *algState = nullptr;     //!< the algebraic state data (for cases where state contains only differential components)
  double cj = 1.0;                      //!< a number used in Jacobian calculations if there is a derivative used in the calculations
  count_t seqID = 0;                    //!< a sequence id to differentiate between subsequent state data objects
  index_t pairIndex = kNullLocation;                                //!< the index of the mode the paired data comes from
  stateData (double sTime = 0.0, const double *sstate = nullptr, const double *ndstate_dt = nullptr, count_t cseq = 0) : time (sTime), state (sstate), dstate_dt (ndstate_dt), seqID (cseq)
  {
  }
};

class gridObject;


/**
* @brief Helper class encapsulating offsets for the various solution solverMode types
**/
class offsetTable
{
private:
  std::vector<solverOffsets> offsetContainer;       //!< a vector of containers for offsets corresponding to the different solver modes
  index_t paramOffset = kNullLocation; //!<offset for storing parameters in an array
  count_t cSize;                    //!< the current size of the offsetContainer
public:
  solverOffsets *local;  //!< a pointer to local state information for code simplification usually points to the first element in offsetContainer
  /** @brief constructor
  */
  offsetTable ();

  /** @copy constructor
  */
  offsetTable (const offsetTable &oTable);

  offsetTable &operator= (const offsetTable &oTable);

  /** @brief check whether an offset set has been fully loaded
  *@param[in] sMode the solverMode we are interested in
  *@return a flag (true) if loaded (false) if not
  */
  bool isLoaded (const solverMode &sMode) const;
  /** @brief check whether the state information is loaded
  *@param[in] sMode the solverMode we are interested in
  *@return a flag (true) if loaded (false) if not
  */
  bool isStateLoaded (const solverMode &sMode) const;
  /** @brief check whether the root and Jacobian information is loaded
  *@param[in] sMode the solverMode we are interested in
  *@return a flag (true) if loaded (false) if not
  */
  bool isrjLoaded (const solverMode &sMode) const;
  /** @brief set the offsets for a solverMode
  *@param[in] sMode the solverMode we are interested in
  *@return a pointer to the
  */
  void setOffsets (const solverOffsets &newOffsets, const solverMode &sMode);

  /** @brief set the base offset
  *@param[in] newOffset the location to set the offset to
  *@param[in] sMode the solverMode we are interested in
  */
  void setOffset (index_t newOffset, const solverMode &sMode);

  /** @brief get the offsets for a solverMode
  *@param[in] sMode the solverMode we are interested in
  *@return a pointer to a solverOffsets object
  */
  solverOffsets * getOffsets (const solverMode &sMode);

  /** @brief get the offsets for a solverMode
  *@param[in] sMode the solverMode we are interested in
  *@return a const pointer to a solverOffsets object
  */
  const solverOffsets * getOffsets (const solverMode &sMode) const;
  /** @brief set the base offset of algebraic variables
  *@param[in] newOffset the location to set the offset to
  *@param[in] sMode the solverMode we are interested in
  */
  void setAlgOffset (index_t newOffset, const solverMode &sMode);
  /** @brief set the root offset
  *@param[in] newOffset the location to set the offset to
  *@param[in] sMode the solverMode we are interested in
  */
  void setRootOffset (index_t newOffset, const solverMode &sMode);
  /** @brief set the differential offset
  *@param[in] newOffset the location to set the offset to
  *@param[in] sMode the solverMode we are interested in
  */
  void setDiffOffset (index_t newOffset, const solverMode &sMode);
  /** @brief set the voltage offset
  *@param[in] newOffset the location to set the offset to
  *@param[in] sMode the solverMode we are interested in
  */
  void setVOffset (index_t newOffset,const solverMode &sMode);
  /** @brief set the angle offset
  *@param[in] newOffset the location to set the offset to
  *@param[in] sMode the solverMode we are interested in
  */
  void setAOffset (index_t newOffset,const solverMode &sMode);
  /** @brief get the base offset
  *@param[in] sMode the solverMode we are interested in
  *@return the base offset
  */
  index_t getAlgOffset (const solverMode &sMode) const
  {
    return offsetContainer[sMode.offsetIndex].algOffset;
  }
  /** @brief get the diff offset
  *@param[in] sMode the solverMode we are interested in
  *@return the differential offset
  */
  index_t getDiffOffset (const solverMode &sMode) const
  {
    return offsetContainer[sMode.offsetIndex].diffOffset;
  }
  /**@brief get the root offset
  *@param[in] sMode the solverMode we are interested in
  *@return the root offset
  */
  index_t getRootOffset (const solverMode &sMode) const
  {
    //assert (offsetContainer[static_cast<int> (sMode)].rootOffset != kNullLocation);
    return offsetContainer[sMode.offsetIndex].rootOffset;
  }
  /**@brief get the voltage offset
  *@param[in] sMode the solverMode we are interested in
  *@return the voltage offset
  */
  index_t getVOffset (const solverMode &sMode) const
  {
    return offsetContainer[sMode.offsetIndex].vOffset;
  }
  /**@brief get the angle offset
  *@param[in] sMode the solverMode we are interested in
  *@return the angle offset
  */
  index_t getAOffset (const solverMode &sMode) const
  {
    return offsetContainer[sMode.offsetIndex].aOffset;
  }
  /** @brief get the maximum used index
  *@param[in] sMode the solverMode we are interested in
  *@return the the maximum used index
  */
  index_t maxIndex (const solverMode &sMode) const;

  /** @brief get the locations for the data from a stateData pointer and output array
  *@param[in] sMode the solverMode we are interested in
  *@param[in] sD the stateData object to fill the Lp from
  *@param[in] d the destination location for the calculations
  @param[in] obj  the object to use if local information is required
  @return Lp the Location pointer object to fill
  */
  Lp getLocations (const stateData *sD, double d[], const solverMode &sMode, const gridObject *obj) const;

  /** @brief get the locations for the data from a stateData pointer
  *@param[in] sMode the solverMode we are interested in
  *@param[in] sD the stateData object to fill the Lp from
  @param[in] obj  the object to use if local information is required
  @return Lp the Location pointer object to fill
  */
  Lp getLocations (const stateData *sD, const solverMode &sMode, const gridObject *obj) const;

  /** @brief get the locations offsets for the data
  *@param[in] sMode the solverMode we are interested in
  @param[in] Loc the location pointer to store the data
  *@return the angle offset
  */
  void getLocations (const solverMode &sMode, Lp *Loc) const;
  /** @brief unload all the solverOffset objects
  *@param[in] dynamic_only only unload the dynamic solverObjects
  */
  void unload (bool dynamic_only = false);
  /** @brief unload state information for the solverOffsets
  *@param[in] dynamic_only only unload the dynamic solverObjects
  */
  void stateUnload (bool dynamic_only = false);
  /** @brief unload the root and Jacobian information for the solverOffsets
   *TODO Separate the root and Jacobian information from each other
  *@param[in] dynamic_only only unload the dynamic solverObjects
  */
  void rjUnload (bool dynamic_only = false);
  /** @brief update all solverOffsets with the local information
  *@param[in] dynamic_only only unload the dynamic solverObjects
  */
  void localUpdateAll (bool dynamic_only = false);
  /** @brief get the size of the solverOffsets
  *@return the size
  */
  count_t size () const
  {
    return cSize;
  }
  /** @brief get the solverMode corresponding to an index
  *@return a solverMode object
  */
  const solverMode &getSolverMode (index_t index) const;
  /** @brief find a solverMode matching another Mode in everything but index
  *@return a solverMode object
  */
  const solverMode &find (const solverMode &tMode) const;
  /**@brief get the parameter offset
  *@return the parameter offset
  */
  index_t getParamOffset () const
  {
    return paramOffset;
  }
  /** @brief set the parameter offset
  *@param[in] newPoffset the location to set the offset to
  */
  void setParamOffset (index_t newPoffset)
  {
    paramOffset = newPoffset;
  }
};

#define ALGEBRAIC_VARIABLE (0.0)
#define DIFFERENTIAL_VARIABLE (1.0)

#define POSITIVITY_CONSTRAINT (1.0)
#define NEGATIVITY_CONSTRAINT (-1.0)
#define NONNEGATIVE_CONSTRAINT (2.0)
#define NONPOSITIVE_CONSTRAINT (-2.0)

#endif
