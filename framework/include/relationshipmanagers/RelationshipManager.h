//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "MooseObject.h"
#include "InputParameters.h"
#include "MooseMesh.h"

#include "libmesh/ghosting_functor.h"

// Forward declarations
class RelationshipManager;
class MooseMesh;

template <>
InputParameters validParams<RelationshipManager>();

/**
 * RelationshipManagers are used for describing what kinds of non-local resources are needed for an
 * object's calculations. Non-local resources include geometric element information, or solution
 * information that may be more than a single side-neighbor away in a mesh. This includes
 * physically disconnected elements that might be needed for contact or mortar calculations.
 */
class RelationshipManager : public MooseObject, public libMesh::GhostingFunctor
{
public:
  static InputParameters validParams();

  RelationshipManager(const InputParameters & parameters);

  /**
   * Called before this RM is attached.  Will only be called once.
   */
  void init()
  {
    if (!_inited)
      internalInit();
    _inited = true;
  }

  /**
   * Whether or not this RM has been inited
   */
  bool inited() { return _inited; }

  /**
   * The object (or Action) this RelationshipManager was built for
   */
  virtual const std::vector<std::string> & forWhom() const { return _for_whom; }

  /**
   * Add another name to for_whom
   */
  void addForWhom(const std::string & for_whom) { _for_whom.push_back(for_whom); }

  /**
   * Method for returning relationship manager information (suitable for console output).
   */
  virtual std::string getInfo() const = 0;

  /**
   * Getter for returning the enum type for this RM.
   */
  Moose::RelationshipManagerType getType() const { return _rm_type; }

  /**
   * Check to see if an RM is of a given type
   *
   * This is here so that the Boolean logic doesn't have to get spread everywhere in the world
   */
  bool isType(const Moose::RelationshipManagerType & type) const
  {
    return (_rm_type & type) == type;
  }

  virtual bool operator==(const RelationshipManager & /*rhs*/) const
  {
    mooseError("Comparison operator required for this RelationshipManager required");
  }

  /**
   * Whether or not this RM can be attached to the Mesh early if it's geometric.
   *
   * Note: this is unused if this RM is purely Algebraic
   */
  bool attachGeometricEarly() { return _attach_geometric_early; }

  /**
   * Whether this should be placed on the undisplaced or displaced systems
   *
   * Note: this says 'mesh' but that's just to match the colloquial term
   * in MOOSE.  If this thing is algebraic then it's going to the DofMap
   */
  bool useDisplacedMesh() const { return _use_displaced_mesh; }

  /**
   * Set the dof map
   */
  void setDofMap(const DofMap & dof_map) { _dof_map = &dof_map; }

protected:
  /**
   * Called before this RM is attached.  Only called once
   */
  virtual void internalInit() = 0;

  /// Whether or not this has been initialized
  bool _inited = false;

  /// Reference to the Mesh object
  MooseMesh & _mesh;

  /// Pointer to DofMap (may be null if this is geometric only). This is useful for setting coupling
  /// matrices in call-backs from DofMap::reinit
  const DofMap * _dof_map;

  /// Boolean indicating whether this RM can be attached early (e.g. all parameters are known
  /// without the need for inspecting things like variables or other parts of the system that
  /// are not available.
  const bool _attach_geometric_early;

  /// The type of RM this object is. Note that the underlying enum is capable of holding
  /// multiple values simultaneously, so you must use bitwise operators to test values.
  Moose::RelationshipManagerType _rm_type;

  /// The name of the object that requires this RelationshipManager
  std::vector<std::string> _for_whom;

  /// Which system this should go to (undisplaced or displaced)
  const bool _use_displaced_mesh;

  /// What type of systems this RM can be applied to
  const Moose::RMSystemType _system_type;

public:
  /**
   * Whether \p input_rm is geometric
   */
  static bool isGeometric(Moose::RelationshipManagerType input_rm);

  /**
   * Whether \p input_rm is algebraic
   */
  static bool isAlgebraic(Moose::RelationshipManagerType input_rm);

  /**
   * Whether \p input_rm is coupling
   */
  static bool isCoupling(Moose::RelationshipManagerType input_rm);

  /**
   * A relationship manager type that is geometric and algebraic
   */
  static Moose::RelationshipManagerType geo_and_alg;

  /**
   * A relationship manager type that is geometric, algebraic, and coupling
   */
  static Moose::RelationshipManagerType geo_alg_and_coupl;

  /**
   * This returns an \p InputParameters object containing an \p ElementSideNeighborLayers
   * relationship manager with zero layers of ghosting. While zero layers may seem foolish, this is
   * actually very useful for building the correct sparsity pattern between intra-element degrees of
   * freedom. Since this object only has meaning for building the sparsity pattern, the relationsip
   * manager type contained within the returned \p InputParameters object will be COUPLING
   */
  static InputParameters zeroLayerGhosting();

  /**
   * This returns an \p InputParameters object containing an \p ElementSideNeighborLayers
   * relationship manager with one side layer of ghosting.
   * @param rm_type The type of relationship manager that should be added. This can be GEOMETRIC,
   *                ALGEBRAIC, COUPLING or a combination of the three
   */
  static InputParameters oneLayerGhosting(Moose::RelationshipManagerType rm_type);
};
