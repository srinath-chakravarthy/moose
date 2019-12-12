# MooseObject

All user-facing objects in MOOSE derive from `MooseObject`, this allows for a common structure
for all applications and is the basis for the modular design of MOOSE.

There are two basic forms of a MooseObject, a basic form and a template form for [!ac](AD).

!---

## Basic Header: CustomObject.h

```C++
#pragma once

#include "BaseObject.h"

class CustomObject : public BaseObject
{
public:
  static InputParameters validParams();

  CustomObject(const InputParameters & parameters);

protected:

  virtual Real doSomething() override;

  const Real & _scale;
};
```

!---

## Basic Source: CustomObject.C

```C++
#include "CustomObject.h"

registerMooseObject("CustomApp", CustomObject);

InputParameters
CustomObject::validParams()
{
  InputParameters params = BaseObject::validParams();
  params.addClassDescription("The CustomObject does something with a scale parameter.");
  params.addParam<Real>("scale", 1, "A scale factor for use when doing something.");
  return params;
}

CustomObject::CustomObject(const InputParameters & parameters) :
    BaseObject(parameters),
    _scale(getParam<Real>("scale"))
{
}

double
CustomObject::doSomething()
{
  // Do some sort of import calculation here that needs a scale factor
  return _scale;
}
```

!---

## AD Header: ADCustomObject.h

```cpp
#pragma once

#include "ADBaseObject.h"

template <ComputeStage compute_stage>
class ADCustomObject : public ADBaseObject<compute_stage>
{
public:
  static InputParameters validParams();

  ADCustomObject(const InputParameters & parameters);

protected:
  virtual ADReal doSomething() override;

  const Real & _scale;

  usingBaseObjectMembers;
};
```

!---

## AD Source: ADCustomObject.C

```cpp
#include "ADCustomObject.h"

registerADMooseObject("MooseApp", ADCustomObject);

template <ComputeStage compute_stage>
InputParameters
ADCustomObject<compute_stage>::validParams()
{
    InputParameters params = ADCustomObject<
    params.addParam<Real>("scale", 1, "A scale factor for use when doing something.");
    params.addClassDescription("The ADCustomObject does something with a scale parameter.");
    params;
)

template <ComputeStage compute_stage>
ADCustomObject<compute_stage>::ADCustomObject(const InputParameters & parameters)
  : ADBaseObject<compute_stage>(parameters),
    _scale(adGetParam<Real>("scale"))

{
}

template <ComputeStage compute_stage>
ADReal
ADCustomObject<compute_stage>::doSomething()
{
  return _scale;
}

adBaseClass(ADCustomObject);
```
