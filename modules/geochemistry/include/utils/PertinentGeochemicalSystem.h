//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "GeochemicalDatabaseReader.h"
#include <unordered_map>
#include "DenseMatrix.h"
#include <libmesh/dense_vector.h>

/**
 * Data structure designed to hold information related to sorption via surface complexation.
 * Since this is uncommon in geochemistry models, and this information only needs to be retrieved
 * once per node at the start of the Newton process, it uses an std::map, which is slow
 * compared with a std::vector, but saves memory compared with storing a lot of "zeroes" for every
 * non-sorbing species in the model
 */
struct SurfaceComplexationInfo
{
  SurfaceComplexationInfo(){};

  Real surface_area;
  std::map<std::string, Real> sorption_sites;
};

/**
 * Data structure to hold all relevant information from the database file.
 * Generally, the database file contains information on a lot more species than any numerical model
 * considers.  The ModelGeochemicalDatabase only holds the minimal information required for the
 * numerical model.  It also holds the information as std::vector and DenseMatrix data structures
 * for numerical efficiency.  This makes the ModelGeochemicalDatabase a little more obscure compared
 * with the database file, but considering this information is used hundreds, maybe millions, of
 * times per node during a single timestep, numerical efficiency is paramount.
 */
struct ModelGeochemicalDatabase
{
  ModelGeochemicalDatabase(){};

  /// temperatures defined in the database
  std::vector<Real> temperatures;

  /**
   * basis_species_index[name] = index of the basis species, within all ModelGeochemicalDatabase
   * internal datastrcutres, with given name
   */
  std::unordered_map<std::string, unsigned> basis_species_index;

  /// basis_species_name[j] = name of the j^th basis species
  std::vector<std::string> basis_species_name;

  /// basis_species_mineral[j] = true iff the j^th basis species is a mineral
  std::vector<bool> basis_species_mineral;

  /// basis_species_gas[j] = true iff the j^th basis species is a gas
  std::vector<bool> basis_species_gas;

  /// all quantities have a charge (mineral charge = 0, gas charge = 0, oxide charge = 0)
  std::vector<Real> basis_species_charge;

  /// all quantities have a molecular weight (g)
  std::vector<Real> basis_species_weight;

  /// all quantities have an ionic radius (Angstrom) for computing activity (mineral radius = 0, gas radius = 0, surface species radius = 0)
  std::vector<Real> basis_species_radius;

  /// all quantities have a molecular weight (g)
  std::vector<Real> basis_species_molecular_weight;

  /// all quantities have a molecular volume (cm^3) (only nonzero for minerals, however)
  std::vector<Real> basis_species_molecular_volume;

  /**
   * eqm_species_index[name] = index of the equilibrium species (secondary aqueous species, redox
   * couples in equilibrium with the aqueous solution, minerals in equilibrium with the aqueous
   * solution, gases in equilibrium with the aqueous solution) within all ModelGeochemicalDatabase
   * internal datastrcutres, with given name
   */
  std::unordered_map<std::string, unsigned> eqm_species_index;

  /// eqm_species_name[i] = name of the i^th eqm species
  std::vector<std::string> eqm_species_name;

  /// eqm_species_mineral[i] = true iff the i^th equilibrium species is a mineral
  std::vector<bool> eqm_species_mineral;

  /// eqm_species_gas[i] = true iff the i^th equilibrium species is a gas
  std::vector<bool> eqm_species_gas;

  /// all quantities have a charge (mineral charge = 0, gas charge = 0, oxide charge = 0)
  std::vector<Real> eqm_species_charge;

  /// all quantities have an ionic radius (Angstrom) for computing activity (mineral radius = 0, gas radius = 0, surface species radius = 0)
  std::vector<Real> eqm_species_radius;

  /// all quantities have a molecular weight (g)
  std::vector<Real> eqm_species_molecular_weight;

  /// all quantities have a molecular volume (cm^3) (only nonzero for minerals, however)
  std::vector<Real> eqm_species_molecular_volume;

  /**
   * eqm_stoichiometry(i, j) = stoichiometric coefficient for equilibrium species "i" in terms of
   * the basis species "j"
   */
  DenseMatrix<Real> eqm_stoichiometry;

  /**
   * eqm_log10K(i, j) = log10(equilibrium constant) for i^th equilibrium species at the j^th
   * temperature point
   */
  DenseMatrix<Real> eqm_log10K;

  /**
   * surface_sorption_name[k] = name of the mineral involved in surface sorption.  Each of these
   * will be associated with a unique surface potential.
   */
  std::vector<std::string> surface_sorption_name;

  /**
   * surface_sorption_area[k] = specific surface area [m^2/g] for the k^th mineral involved in
   * surface sorption.  Each mineral involved in surface sorption must have a surface_sorption_area
   * prescribed it (in the database) and will be associated with a unique surface potential.
   */
  std::vector<Real> surface_sorption_area;

  /// surface_sorption_related[j] = true iff the j^th equilibrium species is involved in surface sorption
  std::vector<bool> surface_sorption_related;

  /**
   * surface_sorption_number[j] = the index of the surface potential that should be used to modify
   * the equilibrium constant for the j^th equilibrium species.  surface_sorption_number is only
   * meaningful if surface_sorption_related[j] = true.  0 <= surface_sorption_number[:] <
   * surface_sorption_name.size() = number of minerals involved in surface sorption = number of
   * surface potentials in the simulation = surface_sorption_area.size().
   */
  std::vector<unsigned> surface_sorption_number;

  /**
   * the name of the species on the left-hand side of the redox equations.  Upon creation of the
   * model this is e-, but it may change due to swaps
   */
  std::string redox_lhs;

  /**
   * redox_stoichiometry(i, j) = stoichiometric coefficients for i^th redox species that is in
   * disequilibrium in terms of the j basis species.  These equations are all written with their
   * left-hand sides being e-.
   * For instance, the database contains the reactions
   * Fe+++ = -0.5H20 + Fe++ + H+ + 0.25*O2(aq)
   * e- = 0.5H2O - 0.25O2(aq)- H+
   * The first is a redox reaction, and assume the user has specified it is in disquilibrium (by
   * specifying it is in the basis).  If Fe++, H+ and O2(aq) are also in the basis then the two
   * equations yield the single redox equation
   * e- = -Fe+++ + Fe++
   * Then redox_stoichiometry will be -1 for j = Fe+++, and +1 for j = Fe++.
   */
  DenseMatrix<Real> redox_stoichiometry;

  /**
   * redox_log10K(i, j) = log10(equilibrium constant) for i^th redox species at the j^th temperature
   * point
   */
  DenseMatrix<Real> redox_log10K;

  /**
   * Holds info on surface complexation, if any, in the model.  Note this is slow compared with
   * storing information in a std::vector or DenseMatrix, but it also saves memory over storing a
   * lot of "zeroes" for all species not involved in surface complexation
   */
  std::unordered_map<std::string, SurfaceComplexationInfo> surface_complexation_info;

  /**
   * Holds info on gas fugacity "chi" parameters.  Note this is slow compared with storing info in a
   * DenseMatrix, but it also saves memory over storing a lot of "zeroes" for non-gas species.  I
   * believe the slowness will not present a problem because fugacity parameters are typically only
   * queried at the start of the simulation, or at most once per timestep.  If the lookup becomes
   * burdensome, change from unordered_map to DenseMatrix
   */
  std::unordered_map<std::string, std::vector<Real>> gas_chi;

  /**
   * kin_species_index[name] = index of the kinetic species, within all ModelGeochemicalDatabase
   * internal datastrcutres, with given name
   */
  std::unordered_map<std::string, unsigned> kin_species_index;

  /// kin_species_name[j] = name of the j^th kinetic species
  std::vector<std::string> kin_species_name;

  /// kin_species_mineral[j] = true iff the j^th kinetic species is a mineral
  std::vector<bool> kin_species_mineral;

  /// all kinetic quantities have a charge (mineral charge = 0)
  std::vector<Real> kin_species_charge;

  /// all quantities have a molecular weight (g)
  std::vector<Real> kin_species_molecular_weight;

  /// all quantities have a molecular volume (cm^3) (only nonzero for minerals, however)
  std::vector<Real> kin_species_molecular_volume;

  /**
   * kin_stoichiometry(i, j) = stoichiometric coefficient for kinetic species "i" in terms of
   * the basis species "j"
   */
  DenseMatrix<Real> kin_stoichiometry;
};

/**
 * Constructs and stores a minimal amount of information that is pertinent to the user-defined
 * geochemical system.  Most importantly, all basis species, secondary species, mineral species,
 * etc, that are defined in the geochemical database but are irrelevant to the user-defined system
 * are eliminated from further consideration.  This reduces the amount of information considerably.
 * The final result is stored in a ModelGeochemicalDatabase structure.  This structure is designed
 * to be computationally efficient.  A "getter" that copies this structure is provided as a public
 * method.  It is intended that a MOOSE simulation will construct one PertinentGeochemicalSystem
 * object, and then copy the information to the nodes during the initial setup.  If different nodes
 * are allowed different swaps (likely to be the case) each node will need a different copy of the
 * ModelGeochemicalDatabase structure.
 */
class PertinentGeochemicalSystem
{
public:
  /**
   * @param db the database reader, which will have parsed the database file
   *
   * @param basis_species A list of basis components relevant to the aqueous-equilibrium problem.
   * "H2O" must appear first in this list.  No element must appear more than once in this list.
   * These components must be chosen from the "basis species" in the database, the sorbing sites (if
   * any) and the decoupled redox states that are in disequilibrium (if any).  Any redox pair that
   * is not in this list or the kinetic_redox list, will be assumed to be at equilibrium with the
   * aqueous solution and will be considered a secondary species.  All these species, except H2O,
   * may be later swapped out of this list, either by a manual user-prescribed swap (and replaced by
   * a mineral or a gas of fixed fugacity, for instance), or during the numerical solve.
   *
   * @param minerals A list of minerals that are in equilibrium with the aqueous solution.  This can
   * only include the "minerals" in the database file.  No element can appear more than once in this
   * list.  Their equilibrium reaction must consist of only the basis_species, and
   * secondary species and non-kinetically-controlled redox couples that can be expressed in terms
   * of the basis_species.  If they are also "sorbing minerals" in the database then their sorption
   * sites must consist of the basis_species only.  During simulation, the user can compute the
   * saturation index of these minerals, and these minerals can be "swapped" into the basis if
   * desired (or required during the numerical solve).  If the user performs a manual "swap" then an
   * initial condition must be provided for the mineral.  The user choose whether these minerals are
   * allowed to precipitate or not - that is, they can be "supressed".  This list, along with the
   * kinetic_minerals list, comprises the entire list of minerals in the problem: all others are
   * eliminated from consideration.
   *
   * @param gases A list of gases that are in equilibrium with the aqueous solution and can have
   * their fugacities fixed, at least at some time and spatial location.  All members of this list
   * must be a "gas" in the database file.  No gas must appear more than once in this list.  The
   * equilibrium reaction of each gas must involve only the basis_species, or secondary species or
   * non-kinetically-controlled redox couples that can be expressed in terms of the basis_species.
   *
   * @param kinetic_minerals A list of minerals that whose dynamics are governed by a rate law.
   * These are not in equilibrium with the aqueous solution.  This can only include the "minerals"
   * in the database file.  No element can appear more than once in this list.  Their equilibrium
   * reaction must involve only the basis_species, or secondary species or
   * non-kinetically-controlled redox couples that can be expressed in terms of the basis_species.
   * If they are also "sorbing minerals" in the database then their sorption sites must consist of
   * the basis_speices only.  No members of this list must be in the minerals list. They can never
   * be "swapped" into the basis, nor can they be "supressed".
   *
   * @param kinetic_redox A list of redox pairs whose dynamics are governed by a rate law.
   * These are not in equilibrium with the aqueous solution.  Each element of this list must appear
   * in the "redox couples" section of the database.  No element can appear more than once in this
   * list.  Their reaction must involve only the basis_species, or secondary species or
   * non-kinetically-controlled redox couples that can be expressed in terms of the basis_species.
   * No members of this list must be in the basis_species list.  They can never be "swapped" into
   * the basis.
   *
   * @param kinetic_surface_species A list of surface sorbing species whose dynamics are governed by
   * a rate law.  These are not in equilibrium with the aqueous solution.  All elements of this list
   * must appear as a "surface species" in the database.  No member must appear more than twice in
   * this list. Their reaction must involve only the basis_species, or secondary species or
   * non-kinetically-controlled redox couples that can be expressed in terms of the basis_species.
   * They can never be "swapped" into the basis.
   *
   * @param redox_ox The name of the oxygen species, eg O2(aq), that appears in redox reactions. For
   * redox pairs that are in disequilibrium to be correctly recorded, and hence their Nernst
   * potentials to be computed easily, redox_ox must be a basis_species and it must appear in the
   * reaction for each redox pair.
   *
   * @param redox_e The name of the free electron, eg e-.  For redox pairs that are in
   * disequilibrium to be correctly recorded, and hence their Nernst potentials to be computed
   * easily, the equilibrium reaction for redox_e must involve redox_ox, and the basis species must
   * be chosen so that redox_e is an equilibrium species.
   */
  PertinentGeochemicalSystem(const GeochemicalDatabaseReader & db,
                             const std::vector<std::string> & basis_species,
                             const std::vector<std::string> & minerals,
                             const std::vector<std::string> & gases,
                             const std::vector<std::string> & kinetic_minerals,
                             const std::vector<std::string> & kinetic_redox,
                             const std::vector<std::string> & kinetic_surface_species,
                             const std::string & redox_ox,
                             const std::string & redox_e);

  /// Return a reference to the ModelGeochemicalDatabase structure
  const ModelGeochemicalDatabase & modelGeochemicalDatabase() const;

private:
  /// The database
  GeochemicalDatabaseReader _db;

  /// given a species name, return its index in the corresponding "info" std::vector
  std::unordered_map<std::string, unsigned> _basis_index;

  /// a vector of all relevant species
  std::vector<GeochemistryBasisSpecies> _basis_info;

  /// given a species name, return its index in the corresponding "info" std::vector
  std::unordered_map<std::string, unsigned> _mineral_index;

  /// a vector of all relevant species
  std::vector<GeochemistryMineralSpecies> _mineral_info;

  /// given a species name, return its index in the corresponding "info" std::vector
  std::unordered_map<std::string, unsigned> _gas_index;

  /// a vector of all relevant species
  std::vector<GeochemistryGasSpecies> _gas_info;

  /// given a species name, return its index in the corresponding "info" std::vector
  std::unordered_map<std::string, unsigned> _kinetic_mineral_index;

  /// a vector of all relevant species
  std::vector<GeochemistryMineralSpecies> _kinetic_mineral_info;

  /// given a species name, return its index in the corresponding "info" std::vector
  std::unordered_map<std::string, unsigned> _kinetic_redox_index;

  /// a vector of all relevant species
  std::vector<GeochemistryRedoxSpecies> _kinetic_redox_info;

  /// given a species name, return its index in the corresponding "info" std::vector
  std::unordered_map<std::string, unsigned> _kinetic_surface_index;

  /// a vector of all relevant species
  std::vector<GeochemistrySurfaceSpecies> _kinetic_surface_info;

  /// given a species name, return its index in the corresponding "info" std::vector
  std::unordered_map<std::string, unsigned> _secondary_index;

  /// a vector of all relevant species
  std::vector<GeochemistryEquilibriumSpecies> _secondary_info;

  /**
   * The name of the oxygen in all disequilibrium-redox equations, eg O2(aq), which must be a basis
   * species if disequilibrium redox reactions are to be recorded
   */
  const std::string _redox_ox;

  /// The name of the free electron involved in redox reactions
  const std::string _redox_e;

  /// The important datastructure built by this class
  ModelGeochemicalDatabase _model;

  /**
   * using the basis_species list, this method builds _basis_index and _basis_info
   */
  void buildBasis(const std::vector<std::string> & basis_species);

  /**
   * using the minerals list, this method builds _mineral_index and _mineral_info
   */
  void buildMinerals(const std::vector<std::string> & minerals);

  /**
   * using the gas list, this method builds _gas_index and _gas_info
   */
  void buildGases(const std::vector<std::string> & gases);

  /**
   * using the kinetic_minerals list, this method builds _kinetic_mineral_index and
   * _kinetic_mineral_info
   */
  void buildKineticMinerals(const std::vector<std::string> & kinetic_minerals);

  /**
   * using the kinetic_redox list, this method builds _kinetic_redox_index and
   * _kinetic_redox_info
   */
  void buildKineticRedox(const std::vector<std::string> & kinetic_redox);

  /**
   * using the kinetic_surface list, this method builds _kinetic_surface_index and
   * _kinetic_surface_info
   */
  void buildKineticSurface(const std::vector<std::string> & kinetic_surface);

  /**
   * Extract all relevant "redox couples" and "secondary species" and "surface species" from the
   * database. These are all species whose reaction involves only the basis_species and are not
   * kinetically controlled or already in the basis_species list.
   */
  void buildSecondarySpecies();

  /**
   * Check that all minerals in mineral_info have reactions that involve only the
   * basis_species or secondary_species.   Check that if a mineral in this list is also a
   * "sorbing mineral", its sorbing sites are present in the basis_species list
   */
  void checkMinerals(const std::vector<GeochemistryMineralSpecies> & mineral_info) const;

  /**
   * Check that all gases in the "gases" list have reactions that involve only the
   * basis_species or secondary_species.
   */
  void checkGases() const;

  /**
   * Check that all kinetic redox species in the _kinetic_redox list have reactions that involve
   * only the basis_species or secondary_species.
   */
  void checkKineticRedox() const;

  /**
   * Check that all kinetic surface species in the _kinetic_surface_species list have reactions that
   * involve only the basis_species or secondary_species.
   */
  void checkKineticSurfaceSpecies() const;

  /**
   * Fully populate the ModelGeochemicalDatabase
   */
  void createModel();
};
