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

#include "gridDyn.h"
#include "gridDynSimulationFileOps.h"
#include "gridBus.h"
#include "linkModels/acLine.h"
#include "solvers/solverInterface.h"
#include "vectorOps.hpp"
#include "stringOps.h"
#include "ticpp.h"
#include "arrayDataSparse.h"
#include "units.h"
#include <boost/filesystem.hpp>

#include <fstream>
#include <cstdio>
#include <cmath>

using namespace gridUnits;

void savePowerFlow (gridDynSimulation *gds, const std::string &fname)
{
  boost::filesystem::path filePath (fname);
  if (fname.empty ())
    {
      auto powerFlowFile = gds->getString ("powerflowfile");
      if (powerFlowFile.empty ())
        {
          auto sourceFile = gds->getString ("sourcefile");
          filePath = boost::filesystem::path (sourceFile);
          filePath = boost::filesystem::path (filePath.filename ().string () + ".csv");
        }
      else
        {
          filePath = boost::filesystem::path (powerFlowFile);
        }
    }


  std::string ext = convertToLowerCase (filePath.extension ().string ());

  //get rid of the . on the extension if it has one
  if (ext == ".xml")
    {
      savePowerFlowXML (gds,fname);
    }
  else if (ext == ".csv")
    {
      savePowerFlowCSV (gds,fname);
    }
  else if (ext == ".cdf")
    {
      savePowerFlowCdf (gds,fname);
    }
  else if ((ext == ".txt")||(ext == ".out"))
    {
      savePowerFlowTXT (gds, fname);
    }
  else
    {
      savePowerFlowBinary (gds,fname);
    }
}

void savePowerFlowCSV (gridDynSimulation *gds, const std::string &fname)
{
  FILE *fp = fopen (fname.c_str (), "w");
  if (fp == nullptr)
    {
      return;
    }
  double basePower = gds->get ("basepower");
  fprintf (fp, "basepower=%f\n", basePower);
  fprintf (fp, "\"Area #\",\"Bus #\",\"Bus name\",\"voltage(pu)\",\"angle(deg)\",\"Pgen(MW)\",\"Qgen(MW)\",\"Pload(MW)\",\"Qload(MW)\",\"Plink(MW)\",\"Qlink(MW)\"\n");
  gridArea *Area = gds->getArea (0);
  index_t mm = 0;
  while (Area != nullptr)
    {
      index_t nn = 0;
      gridBus *bus = Area->getBus (nn);
      while (bus != nullptr)
        {
          fprintf (fp, "%d, %d,\"%s\", %7.6f, %+8.4f, %7.5f, %7.5f, %7.5f, %7.5f, %7.5f, %7.5f\n",
                   Area->getUserID (), bus->getUserID (), bus->getName ().c_str (), bus->getVoltage (), unitConversionAngle (bus->getAngle (), rad, deg),
                   bus->getGenerationReal () * basePower, bus->getGenerationReactive () * basePower, bus->getLoadReal () * basePower,
                   bus->getLoadReactive () * basePower, bus->getLinkReal () * basePower, bus->getLinkReactive () * basePower);

          ++nn;
          bus = Area->getBus (nn);
        }
      ++mm;
      Area = gds->getArea (mm);
    }

  index_t nn = 0;
  gridBus *bus = gds->getBus (nn);
  while (bus != nullptr)
    {
      fprintf (fp, "%d, %d, \"%s\", %7.6f, %+8.4f, %7.5f, %7.5f, %7.5f, %7.5f, %7.5f, %7.5f\n",
               1, bus->locIndex + 1, bus->getName ().c_str (), bus->getVoltage (), unitConversionAngle (bus->getAngle (), rad, deg),
               bus->getGenerationReal () * basePower, bus->getGenerationReactive () * basePower, bus->getLoadReal () * basePower,
               bus->getLoadReactive () * basePower, bus->getLinkReal () * basePower, bus->getLinkReactive () * basePower);

      ++nn;
      bus = gds->getBus (nn);
    }
  gds->log (gds, GD_NORMAL_PRINT, "saving csv powerflow to " + fname);
  fclose (fp);
}


void savePowerFlowTXT (gridDynSimulation *gds, const std::string &fname)
{
  FILE *fp = fopen (fname.c_str (), "w");
  if (fp == nullptr)
    {
      return;
    }
  double basePower = gds->get ("basepower");
  fprintf (fp, "%s basepower=%f\n", gds->getName ().c_str (), basePower);
  fprintf (fp, "Simulation %d buses %d lines\n", gds->getInt ("totalbuscount"), gds->getInt ("totallinkcount"));

  fprintf (fp, "===============BUS INFORMATION=====================\n");
  fprintf (fp, "Area#\tBus#\tBus name\t\t\t\tvoltage(pu)\tangle(deg)\tPgen(MW)\tQgen(MW)\tPload(MW)\tQload(MW)\tPlink(MW)\tQlink(MW)\n");
  gridArea *Area = gds->getArea (0);
  gridBus *bus;
  index_t nn = 0;
  index_t mm = 0;
  while (Area != nullptr)
    {
      nn = 0;
      bus = Area->getBus (nn);
      while (bus != nullptr)
        {
          fprintf (fp, "%d\t\t %d\t\t\"%-20s\"\t\t %7.6f\t %+8.4f\t %7.5f\t %7.5f\t %7.5f\t %7.5f\t %7.5f\t %7.5f\n",
                   Area->getUserID (), bus->getUserID (), bus->getName ().c_str (), bus->getVoltage (), unitConversionAngle (bus->getAngle (), rad, deg),
                   bus->getGenerationReal () * basePower, bus->getGenerationReactive () * basePower, bus->getLoadReal () * basePower,
                   bus->getLoadReactive () * basePower, bus->getLinkReal () * basePower, bus->getLinkReactive () * basePower);

          ++nn;
          bus = Area->getBus (nn);
        }
      ++mm;
      Area = gds->getArea (mm);
    }

  nn = 0;
  bus = gds->getBus (nn);
  while (bus != nullptr)
    {
      fprintf (fp, "%d\t\t %d\t\t \"%-20s\"\t %7.6f\t %8.4f\t %7.5f\t %7.5f\t %7.5f\t %7.5f\t %7.5f\t %7.5f\n",
               1, bus->getUserID (), bus->getName ().c_str (), bus->getVoltage (), unitConversionAngle (bus->getAngle (), rad, deg),
               bus->getGenerationReal () * basePower, bus->getGenerationReactive () * basePower, bus->getLoadReal () * basePower,
               bus->getLoadReactive () * basePower, bus->getLinkReal () * basePower, bus->getLinkReactive () * basePower);

      ++nn;
      bus = gds->getBus (nn);
    }
  fprintf (fp, "===============LINE INFORMATION=====================\n");
  fprintf (fp, "Area#\tLine #\tLine Name\t\t\t\t\tfrom\tto\t\tP1_2\t\tQ1_2\t\tP2_1\t\tQ2_1\t\tLoss\n");
  gridLink *lnk;
  Area = gds->getArea (0);
  while (Area != nullptr)
    {
      nn = 0;
      lnk = Area->getLink (nn);
      while (bus != nullptr)
        {
          fprintf (fp, "%d\t\t%d\t\t\"%-20s\"\t %5d\t%5d\t %7.5f\t %7.5f\t %7.5f\t %7.5f\t %7.5f\n",
                   Area->getUserID (), lnk->getUserID (), lnk->getName ().c_str (), lnk->getBus (1)->getUserID (), lnk->getBus (2)->getUserID (),
                   lnk->getRealPower (1) * basePower, lnk->getReactivePower (1) * basePower,
                   lnk->getRealPower (2) * basePower, lnk->getReactivePower (2) * basePower,
                   lnk->getLoss () * basePower);

          ++nn;
          lnk = Area->getLink (nn);
        }
      ++mm;
      Area = gds->getArea (mm);
    }

  nn = 0;
  lnk = gds->getLink (nn);
  while (lnk != nullptr)
    {
      fprintf (fp, "%d\t\t %d\t\t\"%-20s\"\t %5d, %5d\t %7.5f\t %7.5f\t %7.5f\t %7.5f\t %7.5f\n",
               1, lnk->getUserID (), lnk->getName ().c_str (), lnk->getBus (1)->getUserID (), lnk->getBus (2)->getUserID (),
               lnk->getRealPower (1) * basePower, lnk->getReactivePower (1) * basePower,
               lnk->getRealPower (2) * basePower, lnk->getReactivePower (2) * basePower,
               lnk->getLoss () * basePower);

      ++nn;
      lnk = gds->getLink (nn);
    }

  fprintf (fp, "===============AREA INFORMATION=====================\n");
  fprintf (fp, "Area#\tArea Name\t\t\t\tGen Real\t Gen Reactive\t Load Real\t Load Reactive\t Loss\t Export\n");

  Area = gds->getArea (0);
  while (Area != nullptr)
    {
      fprintf (fp, "%d\t\t\"%-20s\"\t %7.2f\t %7.2f\t %7.2f\t %7.2f\t %7.2f\t %7.2f\n",
               Area->getUserID (), Area->getName ().c_str (), Area->getGenerationReal () * basePower, Area->getGenerationReactive () * basePower,
               Area->getLoadReal () * basePower, Area->getLoadReactive () * basePower,Area->getLoss () * basePower,-99999.0);

      ++mm;
      Area = gds->getArea (mm);
    }

  fprintf (fp, "%d\t\t\"%-20s\"\t %7.2f\t %7.2f\t %7.2f\t %7.2f\t %7.2f\t %7.2f\n",
           1, gds->getName ().c_str (), gds->getGenerationReal () * basePower, gds->getGenerationReactive () * basePower,
           gds->getLoadReal () * basePower, gds->getLoadReactive () * basePower, gds->getLoss () * basePower, -99999.0);
  gds->log (gds, GD_NORMAL_PRINT, "saving txt powerflow to " + fname);
  fclose (fp);
}

static const double deflim1 = 99999.999;
//static const double deflim2 = -99999.99;

/*
Columns  1- 4   Bus number (I) *
Columns  6-17   Name (A) (left justify) *
Columns 19-20   Load flow area number (I) Don't use zero! *
Columns 21-23   Loss zone number (I)
Columns 25-26   Type (I) *
0 - Unregulated (load, PQ)
1 - Hold MVAR generation within voltage limits, (PQ)
2 - Hold voltage within VAR limits (gen, PV)
3 - Hold voltage and angle (swing, V-Theta) (must always
have one)
Columns 28-33   Final voltage, pu (F) *
Columns 34-40   Final angle, degrees (F) *
Columns 41-49   Load MW (F) *
Columns 50-58   Load MVAR (F) *
Columns 59-67   Generation MW (F) *
Columns 68-75   Generation MVAR (F) *
Columns 77-83   Base KV (F)
Columns 85-90   Desired volts (pu) (F) (This is desired remote voltage if
this bus is controlling another bus.
Columns 91-98   Maximum MVAR or voltage limit (F)
Columns 99-106  Minimum MVAR or voltage limit (F)
Columns 107-114 Shunt conductance G (per unit) (F) *
Columns 115-122 Shunt susceptance B (per unit) (F) *
Columns 124-127 Remote controlled bus number
*/
void cdfBusPrint (FILE *fp, int areaNum, gridBus *bus)
{
  int type = static_cast<int> (bus->getType ());
  fprintf (fp, "%4u %-12s %2u%3u %2d %6.4f%7.2f",
           bus->getUserID (), bus->getName ().c_str (), areaNum, bus->getInt ("zone"), type,
           bus->getVoltage (), bus->getAngle () * 180.0 / kPI);

  double P = bus->get ("p", gridUnits::MW);
  double Q = bus->get ("q", gridUnits::MW);
  double genP = -bus->get ("genreal", gridUnits::MW);
  double genQ = -bus->get ("genreactive", gridUnits::MW);

  fprintf (fp, "%9.2f%9.2f%9.2f%8.2f", P, Q, genP, genQ);
  double vmax = (std::min)(bus->get ("vmax"), deflim1);
  double vmin = (std::max)(bus->get ("vmin"), 0.0);
  if (type != 0)
    {
      vmax = bus->get ("qmax");
      vmin = bus->get ("qmin");
    }
  if (vmax >= deflim1)
    {
      vmax = 0.0;
    }
  if (vmin < -kHalfBigNum)
    {
      vmin = 0.0;
    }



  if ((type == 1)||(type == 3))
    {
      fprintf (fp," %7.1f %6.3f%8.2f%8.2f",
               bus->get ("basevoltage"), bus->get ("vtarget"), vmax, vmin);
    }
  else
    {
      fprintf (fp, " %7.1f %6.3f%8.4f%8.4f",
               bus->get ("basevoltage"), bus->get ("vtarget"), vmax, vmin);
    }

  double yp = bus->get ("yp", gridUnits::MW);
  double yq = -bus->get ("yq", gridUnits::MW);
  fprintf (fp,"%8.4f%8.4f %4d\n", yp, yq, 0);
}

/*
Columns  1- 4   Tap bus number (I) *
For transformers or phase shifters, the side of the model
the non-unity tap is on
Columns  6- 9   Z bus number (I) *
For transformers and phase shifters, the side of the model
the device impedance is on.
Columns 11-12   Load flow area (I)
Columns 14-15   Loss zone (I)
Column  17      Circuit (I) * (Use 1 for single lines)
Column  19      Type (I) *
0 - Transmission line
1 - Fixed tap
2 - Variable tap for voltage control (TCUL, LTC)
3 - Variable tap (turns ratio) for MVAR control
4 - Variable phase angle for MW control (phase shifter)
Columns 20-29   Branch resistance R, per unit (F) *
Columns 30-39   Branch reactance X, per unit (F) * No zero impedance lines
Columns 40-50   Line charging B, per unit (F) * (total line charging, +B)
Columns 51-55   Line MVA rating No 1 (I) Left justify!
Columns 57-61   Line MVA rating No 2 (I) Left justify!
Columns 63-67   Line MVA rating No 3 (I) Left justify!
Columns 69-72   Control bus number
Column  74      Side (I)
0 - Controlled bus is one of the terminals
1 - Controlled bus is near the tap side
2 - Controlled bus is near the impedance side (Z bus)
Columns 77-82   Transformer final turns ratio (F)
Columns 84-90   Transformer (phase shifter) final angle (F)
Columns 91-97   Minimum tap or phase shift (F)
Columns 98-104  Maximum tap or phase shift (F)
Columns 106-111 Step size (F)
Columns 113-119 Minimum voltage, MVAR or MW limit (F)
Columns 120-126 Maximum voltage, MVAR or MW limit (F)
*/

void cdfLinkPrint (FILE *fp,int areaNum, acLine *lnk)
{
  int type = 0;
  int cbus = 0;
  double maxVal = 0.0;
  double minVal = 0.0;
  double minAdj = 0.0;
  double maxAdj = 0.0;
  double ssize = 0.0;
  if (dynamic_cast<adjustableTransformer *> (lnk))
    {
      auto alnk = static_cast<adjustableTransformer *> (lnk);
      type = alnk->getInt ("control_mode");
      switch ( type)
        {
        case 0:
          type = 1;
          maxVal = 0;
          minVal = 0;
        case 1:
          type = 2;
          maxVal = alnk->get ("vmax");
          minVal = alnk->get ("vmin");
          minAdj = alnk->get ("mintap");
          maxAdj = alnk->get ("maxtap");
          ssize = alnk->get ("stepsize");
        case 2:
          type = 3;
          maxVal = alnk->get ("qmax", gridUnits::MW);
          minVal = alnk->get ("qmin", gridUnits::MW);
          minAdj = alnk->get ("mintap");
          maxAdj = alnk->get ("maxtap");
          ssize = alnk->get ("stepsize");
        case 3:
          type = 4;
          maxVal = alnk->get ("pmax", gridUnits::MW);
          minVal = alnk->get ("pmin", gridUnits::MW);
          minAdj = alnk->get ("mintapangle");
          maxAdj = alnk->get ("maxtapangle");
          ssize = alnk->get ("stepsize");
        }
      cbus = alnk->getInt ("controlbusid");

    }
  else
    {
      if (lnk->getTap () != 1.0)
        {
          type = 1;
        }
      else if (lnk->getTapAngle () != 0.0)
        {
          type = 4;
        }
    }

  fprintf (fp, "%4d %4d %2d %2d 1 %1d",
           lnk->getBus (1)->getUserID (), lnk->getBus (2)->getUserID (), lnk->getInt ("zone"), areaNum, type);
  fprintf (fp, "%10.6f%10.6f%11.5f",
           lnk->get ("r"), lnk->get ("x"),lnk->get ("b", gridUnits::MW));


  double rat1 = lnk->get ("ratinga", gridUnits::MW);
  double rat2 = lnk->get ("ratingb", gridUnits::MW);
  double rat3 = lnk->get ("erating", gridUnits::MW);
  if ((rat1 < 0) || (rat1 > deflim1))
    {
      rat1 = 0.0;
    }
  if ((rat2 < 0) || (rat2 > deflim1))
    {
      rat2 = 0.0;
    }
  if ((rat3 < 0) || (rat3 > deflim1))
    {
      rat3 = 0.0;
    }
  fprintf (fp, "%5d %5d %5d %4d %1d  ",
           static_cast<int> (rat1), static_cast<int> (rat2), static_cast<int> (rat3),cbus, 0);
  switch (type)
    {
    case 0:
      fprintf (fp, "0.0       0.0 0.0    0.0     0.0    0.0   0.0\n");
      break;
    case 1:
      fprintf (fp, "%6.4f    0.0 0.0    0.0     0.0    0.0   0.0\n",
               lnk->getTap ());
      break;
    case 2:
    case 3:
      fprintf (fp, "%6.4f %6.1f %6.4f %6.4f%7.5f %6.4f %6.4f\n",
               lnk->getTap (), 0.0, minAdj, maxAdj, ssize, minVal, maxVal);
      break;
    case 4:
      fprintf (fp, "%6.4f%7.2f%7.4f%7.2f %6.2f%7.1f%7.1f\n",
               lnk->getTap (), lnk->getTapAngle () * 180.0 / kPI, minAdj, maxAdj, ssize, minVal, maxVal);
      break;
    }


}

void savePowerFlowCdf (gridDynSimulation *gds, const std::string &fname)
{

  FILE *fp = fopen (fname.c_str (), "w");
  if (fp == nullptr)
    {
      return;
    }

  double basePower = gds->get ("basepower");
  //Title Data
  fprintf (fp, " 0 /0 /0  %20s %5d 2016  %27s\n", ("GridDyn " + griddyn_version).c_str (), static_cast<int> (basePower),gds->getName ().c_str ());

  //Bus Data
  fprintf (fp, "BUS DATA FOLLOWS\n");
  index_t nn = 0;
  gridBus *bus = gds->getBus (nn);
  while (bus != nullptr)
    {
      cdfBusPrint (fp, 1, bus);
      ++nn;
      bus = gds->getBus (nn);
    }

  gridArea *Area = gds->getArea (0);


  index_t mm = 0;
  while (Area != nullptr)
    {
      nn = 0;
      bus = Area->getBus (nn);
      while (bus != nullptr)
        {
          cdfBusPrint (fp, Area->getUserID (), bus);
          ++nn;
          bus = Area->getBus (nn);
        }
      ++mm;
      Area = gds->getArea (mm);
    }
  fprintf (fp, "-999\n");

  //Line Data
  fprintf (fp, "BRANCH DATA FOLLOWS\n");
  nn = 0;
  gridLink *lnk = gds->getLink (nn);
  while (lnk != nullptr)
    {

      cdfLinkPrint (fp,1, static_cast<acLine *> (lnk));
      ++nn;
      lnk = gds->getLink (nn);
    }
  mm = 0;
  Area = gds->getArea (mm);
  while (Area != nullptr)
    {
      nn = 0;
      lnk = Area->getLink (nn);
      while (lnk != nullptr)
        {
          cdfLinkPrint (fp, Area->getUserID (), static_cast<acLine *> (lnk));
          ++nn;
          lnk = Area->getLink (nn);
        }
      ++mm;
      Area = gds->getArea (mm);
    }

  fprintf (fp,"-999\n");
  fprintf (fp, "LOSS ZONES FOLLOWS \n");
  fprintf (fp, "1 %s\n", gds->getName ().c_str ());
  fprintf (fp, "-99\n");
  fprintf (fp, "INTERCHANGE DATA FOLLOWS ");
  fprintf (fp, "-9\n");
  fprintf (fp, "TIE LINES FOLLOWS				0 ITEMS\n");
  fprintf (fp, "-999\n");
  fprintf (fp, "END OF DATA");

  fclose (fp);
}


void savePowerFlowXML (gridDynSimulation *gds, const std::string &fname)
{
  ticpp::Document doc (fname);
  ticpp::Element *busE;
  ticpp::Element *linkE;
  ticpp::Element *prop;
  ticpp::Text *propval;
  ticpp::Declaration *decl = new ticpp::Declaration ("0.5", "", "");
  doc.LinkEndChild (decl);



  ticpp::Comment *comment = new ticpp::Comment ();
  comment->SetValue ("Power Flow Result output");
  doc.LinkEndChild (comment);

  ticpp::Element *sol = new ticpp::Element ("PowerFlow");
  doc.LinkEndChild (sol);

  ticpp::Element *buses = new ticpp::Element ("buses");
  prop = new ticpp::Element ("count");
  auto buscount = gds->get ("buscount");
  propval = new ticpp::Text (buscount);
  prop->LinkEndChild (propval);
  buses->LinkEndChild (prop);

  sol->LinkEndChild (buses);
  index_t nn = 0;
  gridBus *bus = gds->getBus (nn);
  while (bus != nullptr)
    {
      busE = new ticpp::Element ("bus");
      prop = new ticpp::Element ("name");
      propval = new ticpp::Text (bus->getName ());
      prop->LinkEndChild (propval);
      busE->LinkEndChild (prop);

      prop = new ticpp::Element ("index");
      propval = new ticpp::Text (nn);
      prop->LinkEndChild (propval);
      busE->LinkEndChild (prop);

      prop = new ticpp::Element ("voltage");
      propval = new ticpp::Text (bus->getVoltage ());
      prop->LinkEndChild (propval);
      busE->LinkEndChild (prop);

      prop = new ticpp::Element ("angle");
      propval = new ticpp::Text (bus->getAngle ());
      prop->LinkEndChild (propval);
      busE->LinkEndChild (prop);

      buses->LinkEndChild (busE);
      ++nn;
      bus = gds->getBus (nn);
    }

  ticpp::Element *links = new ticpp::Element ("links");

  prop = new ticpp::Element ("count");
  auto lnkcnt = gds->get ("linkcount");
  propval = new ticpp::Text (lnkcnt);
  prop->LinkEndChild (propval);
  links->LinkEndChild (prop);

  sol->LinkEndChild (links);
  nn = 0;
  gridLink *lnk = gds->getLink (nn);
  while (lnk != nullptr)
    {
      linkE = new ticpp::Element ("link");
      prop = new ticpp::Element ("name");
      propval = new ticpp::Text (lnk->getName ());
      prop->LinkEndChild (propval);
      linkE->LinkEndChild (prop);

      prop = new ticpp::Element ("index");
      propval = new ticpp::Text (nn);
      prop->LinkEndChild (propval);
      linkE->LinkEndChild (prop);

      prop = new ticpp::Element ("Bus1");
      propval = new ticpp::Text (lnk->getBus (1)->getUserID ());
      prop->LinkEndChild (propval);
      linkE->LinkEndChild (prop);

      prop = new ticpp::Element ("Bus2");
      propval = new ticpp::Text (lnk->getBus (2)->getUserID ());
      prop->LinkEndChild (propval);
      linkE->LinkEndChild (prop);

      prop = new ticpp::Element ("RealImpedance");
      propval = new ticpp::Text (lnk->get ("r"));
      prop->LinkEndChild (propval);
      linkE->LinkEndChild (prop);

      prop = new ticpp::Element ("ImagImpedance");
      propval = new ticpp::Text (lnk->get ("x"));
      prop->LinkEndChild (propval);
      linkE->LinkEndChild (prop);

      prop = new ticpp::Element ("RealIn");
      propval = new ticpp::Text (lnk->getRealPower ());
      prop->LinkEndChild (propval);
      linkE->LinkEndChild (prop);

      prop = new ticpp::Element ("RealOut");
      propval = new ticpp::Text (lnk->getRealPower (2));
      prop->LinkEndChild (propval);
      linkE->LinkEndChild (prop);


      links->LinkEndChild (linkE);
      ++nn;
      lnk = gds->getLink (nn);
    }
  doc.SaveFile ();
}


void saveBusData(gridDynSimulation *gds, const std::string &fname)
{
	std::vector<double> V;
	std::vector<double> A;
	std::vector<double> Pgen;
	std::vector<double> Qgen;
	std::vector<double> Pload;
	std::vector<double> Qload;
	std::vector<std::string> bname;
	gds->getVoltage(V);
	gds->getAngle(A);
	gds->getBusGenerationReal(Pgen);
	gds->getBusGenerationReactive(Qgen);
	gds->getBusLoadReactive(Qload);
	gds->getBusLoadReal(Pload);
	gds->getBusName(bname);

	std::ofstream out(fname);
	out << "Name, Voltage, angle, Pg, Qg, Pl, Ql\n";
	for (size_t aa = 0; aa < V.size(); ++aa)
	{
		out << bname[aa] << ", " << V[aa] << ", " << A[aa];
		out << ", " << Pgen[aa] << ", " << Qgen[aa] << ", ";
		out << Pload[aa] << ", " << Qload[aa] << "\n";
	}
}

void saveLineData(gridDynSimulation *gds, const std::string &fname)
{
	std::vector<double> loss;
	std::vector<double> P1;
	std::vector<double> Q1;
	std::vector<double> P2;
	std::vector<double> Q2;
	std::vector<std::string> linkname;
	gds->getLinkLoss(loss);
	gds->getLinkRealPower(P1, 0, 1);
	gds->getLinkRealPower(P2, 0, 2);
	gds->getLinkReactivePower(Q1, 0, 1);
	gds->getLinkReactivePower(Q2, 0, 2);
	gds->getLinkName(linkname);

	std::ofstream out(fname);
	out << "Name, P1, Q1, P2, Q2, loss\n";
	for (size_t aa = 0; aa < P1.size(); ++aa)
	{
		out << linkname[aa] << ", " << P1[aa] << ", " << Q1[aa];
		out << ", " << P2[aa] << ", " << Q2[aa] << ", ";
		out << loss[aa]<<"\n";
	}
}

void savePowerFlowBinary (gridDynSimulation *, const std::string & /*fname*/)
{

}


void saveState (gridDynSimulation *gds, const std::string &fname, const solverMode &sMode,bool append)
{
  boost::filesystem::path filePath (fname);
  if (fname.empty ())
    {
      auto stateFile = gds->getString ("statefile");
      if (stateFile.empty ())
        {
          std::cerr << "no file specified" << std::endl;
          return;
        }
      else
        {
          filePath = boost::filesystem::path (stateFile);
        }
    }

  std::string ext = convertToLowerCase (filePath.extension ().string ());

  if (ext == ".xml")
    {
      saveStateXML (gds, fname, sMode);
    }
  else if ((ext == ".bin") || (ext == ".dat"))
    {
      saveStateBinary (gds, fname, sMode,append);
    }
  else
    {
      saveStateBinary (gds, fname, sMode,append);
    }
}

void saveStateXML (gridDynSimulation *, const std::string & /*fname*/, const solverMode &)
{

}



void saveStateBinary (gridDynSimulation *gds, const std::string &fname, const solverMode &iMode, bool append)
{


  const solverMode &sMode = gds->getCurrentMode (iMode);
  auto sd = gds->getSolverInterface (sMode);
  if (!sd)
    {
      return;
    }
  double *statedata = sd->state_data ();
  auto dsize = sd->size ();

  std::uint32_t index = gds->getInt("residcount");
  if (fname.empty ())
    {
      auto stateFile = gds->getString ("statefile");
	  auto s=writeVector(gds->getCurrentTime(), index, 0, iMode.offsetIndex, dsize, statedata, stateFile,append);
	  if ((s == FUNCTION_EXECUTION_SUCCESS)&&(hasDifferential(iMode)))
	  {
		  writeVector(gds->getCurrentTime(), index, 1, iMode.offsetIndex, dsize, sd->deriv_data(), stateFile);
	  }
	  
    }
  else
    {
	  auto s = writeVector(gds->getCurrentTime(), index, 0, iMode.offsetIndex, dsize, statedata, fname, append);
	  if ((s == FUNCTION_EXECUTION_SUCCESS) && (hasDifferential(iMode)))
	  {
		  writeVector(gds->getCurrentTime(), index, 1, iMode.offsetIndex, dsize, sd->deriv_data(), fname);
	  }
    }
}


int writeVector(double time, std::uint32_t code, std::uint32_t index, std::uint32_t key, std::uint32_t numElements, const double *data, const std::string&filename, bool append)
{
	std::ofstream  bFile;
	if (append)
	{
		bFile.open(filename.c_str(), std::ios::out | std::ios::binary | std::ios::app);
	}
	else
	{
		bFile.open(filename.c_str(), std::ios::out | std::ios::binary);
	}
	if (!bFile.is_open())
	{
		return (FUNCTION_EXECUTION_FAILURE);
	}
	code &= 0x0000FFFF; //make sure we don't change the data type
	bFile.write((char *)(&time), sizeof(double));
	bFile.write((char *)(&code), sizeof(std::uint32_t));
	bFile.write((char *)(&index), sizeof(std::uint32_t));
	bFile.write((char *)(&key), sizeof(std::uint32_t));
	bFile.write((char *)(&numElements), sizeof(std::uint32_t));
	bFile.write((char *)(data), sizeof(double) * numElements);
	return FUNCTION_EXECUTION_SUCCESS;
}

int writeArray(double time, std::uint32_t code,std::uint32_t index, std::uint32_t key, arrayData<double> *a1, const std::string&filename, bool append)
{
	std::ofstream  bFile;
	if (append)
	{
		bFile.open(filename.c_str(), std::ios::out | std::ios::binary | std::ios::app);
	}
	else
	{
		bFile.open(filename.c_str(), std::ios::out | std::ios::binary);
	}
	if (!bFile.is_open())
	{
		return FUNCTION_EXECUTION_FAILURE;
	}
	code &= 0x0000FFFF;
	code |= 0x00010000;
	bFile.write((char *)(&time), sizeof(double));
	bFile.write((char *)(&code), sizeof(std::uint32_t));
	bFile.write((char *)(&index), sizeof(std::uint32_t));
	bFile.write((char *)(&key), sizeof(std::uint32_t));
	std::uint32_t numElements = a1->size();
	bFile.write((char *)(&numElements), sizeof(std::uint32_t));
	a1->start();
	for (size_t nn = 0; nn < numElements; ++nn)
	{
		auto el = a1->next();
		bFile.write((char *)(&el), sizeof(data_triple<double>));
	}
	return FUNCTION_EXECUTION_SUCCESS;
}

void loadState (gridDynSimulation *gds, const std::string &fname, const solverMode &sMode)
{
  boost::filesystem::path filePath (fname);
  if (fname.empty ())
    {
      auto stateFile = gds->getString ("statefile");
      if (stateFile.empty ())
        {
          std::cerr << "no file specified" << std::endl;
          return;
        }
      else
        {
          filePath = boost::filesystem::path (stateFile);
        }
    }

  if (!boost::filesystem::exists (filePath))
    {
      std::cerr << "file does not exist" << std::endl;
      return;
    }

  std::string ext = convertToLowerCase (filePath.extension ().string ());

  if (ext == ".xml")
    {
      loadStateXML (gds, fname,sMode);
    }
  else if ((ext == ".bin")||(ext == ".dat"))
    {
      loadStateBinary (gds, fname,sMode);
    }
  else
    {
      loadStateBinary (gds, fname,sMode);
    }


}

void loadStateBinary (gridDynSimulation *gds, const std::string &fname, const solverMode &iMode)
{
  const solverMode &sMode = gds->getCurrentMode (iMode);
  auto sd = gds->getSolverInterface (sMode);
  if (!sd)
    {
      return;
    }

  std::ifstream  bFile;
  if (fname.empty ())
    {
      auto stateFile = gds->getString ("statefile");

      bFile.open (stateFile.c_str (), std::ios::in | std::ios::binary);
    }
  else
    {
      bFile.open (fname.c_str (), std::ios::in | std::ios::binary);
    }
  if (!bFile.is_open ())
    {
      gds->log (gds, GD_ERROR_PRINT, "Unable to open file for writing:" + fname);
      return;
    }
  unsigned int dsize;
  bFile.read ((char *)(&dsize), sizeof(int));
  if (sd->size () != dsize)
    {
      if (!sd->isInitialized ())
        {
          sd->allocate (dsize);
        }
      else
        {
          gds->log (gds, GD_ERROR_PRINT, "statefile does not match solverMode in size");
          return;
        }
    }
  //TODO:: PT this index should be checked at some point, just not sure what to do with it now, might be used for automatic solverMode location
  //instead of what is done currently.
  unsigned int oi;
  bFile.read ((char *)(&(oi)), sizeof(int));
  bFile.read ((char *)(sd->state_data ()), sizeof(double) * dsize);
  if (isDynamic (sMode))
    {
      bFile.read ((char *)(sd->deriv_data ()), sizeof(double) * dsize);
    }
}

void loadStateXML (gridDynSimulation *, const std::string & /*fname*/, const solverMode &)
{

}



void loadPowerFlow (gridDynSimulation *gds, const std::string &fname)
{
  boost::filesystem::path filePath (fname);
  std::string ext = convertToLowerCase (filePath.extension ().string ());
  //get rid of the . on the extension if it has one
  if (ext[0] == '.')
    {
      ext.erase (0,1);
    }
  if (ext == "xml")
    {
      loadPowerFlowXML (gds,fname);
    }
  else if (ext == "csv")
    {
      loadPowerFlowCSV (gds,fname);
    }
  else if ((ext == "cdf") || (ext == "txt"))
    {
      loadPowerFlowCdf (gds,fname);
    }
  else
    {
      loadPowerFlowBinary (gds,fname);
    }
}

void loadPowerFlowCdf (gridDynSimulation *, const std::string & /*fname*/)
{

}

void loadPowerFlowCSV (gridDynSimulation *, const std::string & /*fname*/)
{

}

void loadPowerFlowBinary (gridDynSimulation *, const std::string & /*fname*/)
{

}

void loadPowerFlowXML (gridDynSimulation *gds, const std::string &fname)
{


  ticpp::Document doc (fname);
  doc.LoadFile ();
  ticpp::Element   *flow = doc.FirstChildElement ();
  ticpp::Element   *buses = flow->FirstChildElement ("buses");
  ticpp::Element   *links = flow->FirstChildElement ("links");
  //check to make sure the buscount is correct
  ticpp::Element   *cd = buses->FirstChildElement ("count");
  int count;
  cd->FirstChild ()->GetValue (&count);
  auto busCount = gds->getInt ("buscount");
  if (count != busCount)
    {
      return;
    }
  //check to make sure the link count is correct
  cd = links->FirstChildElement ("count");
  cd->FirstChild ()->GetValue (&count);
  auto linkCount = gds->getInt ("linkcount");
  if (count != linkCount)
    {
      return;
    }
  //loop over the buses
  cd = buses->FirstChildElement ("bus",false);
  while (cd != nullptr)
    {
      index_t kk;
      double val1, val2;
      ticpp::Element   *prop = cd->FirstChildElement ("index",false);
      prop->FirstChild ()->GetValue (&kk);

      prop = cd->FirstChildElement ("voltage",false);
      prop->FirstChild ()->GetValue (&val1);


      prop = cd->FirstChildElement ("angle",false);
      prop->FirstChild ()->GetValue (&val2);
      auto bus = gds->getBus (kk);
      bus->setVoltageAngle (val1,val2);
      cd = cd->NextSiblingElement ("bus",false);

    }

}



void captureJacState (gridDynSimulation *gds, const std::string &fname,const solverMode &iMode)
{
  std::ofstream  bFile (fname.c_str (), std::ios::out | std::ios::binary);
  if (!bFile.is_open ())
    {
      gds->log (gds, GD_ERROR_PRINT, "Unable to open file for writing:" + fname);
    }
//writing the state vector
  const solverMode &sMode = gds->getCurrentMode (iMode);
  auto sd = gds->getSolverInterface (sMode);
  arrayDataSparse ad;
  stateData sD (gds->getCurrentTime (), sd->state_data (), sd->deriv_data ());

  sD.cj = 10000;

  gds->jacobianElements (&sD, &ad, sMode);

  stringVec stateNames;
  gds->getStateName (stateNames, sMode);

  count_t dsize = sd->size ();


  bFile.write ((char *)(&dsize), sizeof(unsigned int));
  for (auto &stN : stateNames)
    {
      unsigned int stnSize = static_cast<unsigned int> (stN.length ());
      bFile.write ((char *)(&stnSize), sizeof(int));
      bFile.write (stN.c_str (), stnSize);
    }

//write the state vector
  bFile.write ((char *)(sd->state_data ()), dsize * sizeof(double));
//writing the Jacobian Matrix
  dsize = ad.size ();
  bFile.write ((char *)(&dsize), sizeof(count_t));

  for (index_t kk = 0; kk < dsize; ++kk)
    {
      index_t r = ad.rowIndex (kk);
      index_t c = ad.colIndex (kk);
      double val = ad.val (kk);
      bFile.write ((char *)(&r), sizeof(index_t));
      bFile.write ((char *)(&c), sizeof(index_t));
      bFile.write ((char *)(&val), sizeof(double));
    }

  bFile.close ();
}


void saveJacobian (gridDynSimulation *gds, const std::string &fname,const solverMode &iMode)
{


  std::ofstream  bFile (fname.c_str (), std::ios::out | std::ios::binary);
  if (!bFile.is_open ())
    {
      gds->log (gds, GD_ERROR_PRINT, "Unable to open file for writing:" + fname);
    }
  //writing the state vector
  const solverMode &sMode = gds->getCurrentMode (iMode);
  auto sd = gds->getSolverInterface (sMode);

  arrayDataSparse ad;

  stateData sD (gds->getCurrentTime (), sd->state_data (), sd->deriv_data ());

  sD.cj = 10000;
  gds->jacobianElements (&sD, &ad, sMode);


  stringVec stateNames;
  gds->getStateName (stateNames, sMode);

  count_t dsize = sd->size ();
  bFile.write ((char *)(&dsize), sizeof(count_t));
  for (auto &stN : stateNames)
    {
      unsigned int stnSize = static_cast<unsigned int> (stN.length ());
      bFile.write ((char *)(&stnSize), sizeof(int));
      bFile.write (stN.c_str (), stnSize);
    }
  //writing the Jacobian Matrix
  dsize = ad.size ();
  bFile.write ((char *)(&dsize), sizeof(count_t));

  for (unsigned kk = 0; kk < dsize; ++kk)
    {
      index_t r = ad.rowIndex (kk);
      index_t c = ad.colIndex (kk);
      double val = ad.val (kk);
      bFile.write ((char *)(&r), sizeof(index_t));
      bFile.write ((char *)(&c), sizeof(index_t));
      bFile.write ((char *)(&val), sizeof(double));
    }
}
