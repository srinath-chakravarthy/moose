//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef METHANEFLUIDPROPERTIESTEST_H
#define METHANEFLUIDPROPERTIESTEST_H

#include "MooseObjectUnitTest.h"
#include "MethaneFluidProperties.h"

class MethaneFluidPropertiesTest : public MooseObjectUnitTest
{
public:
  MethaneFluidPropertiesTest() : MooseObjectUnitTest("MooseUnitApp")
  {
    registerObjects(_factory);
    buildObjects();
  }

protected:
  void registerObjects(Factory & factory) { registerUserObject(MethaneFluidProperties); }

  void buildObjects()
  {
    InputParameters uo_pars = _factory.getValidParams("MethaneFluidProperties");
    _fe_problem->addUserObject("MethaneFluidProperties", "fp", uo_pars);
    _fp = &_fe_problem->getUserObject<MethaneFluidProperties>("fp");
  }

  const MethaneFluidProperties * _fp;
};

#endif // METHANEFLUIDPROPERTIESTEST_H
