// Copyright 2020 The FreeAge authors
// This file is part of FreeAge, licensed under the new BSD license.
// See the COPYING file in the project root for the license text.

#pragma once

#include <QRect>
#include <QSize>
#include <QString>

#include "FreeAge/common/resources.hpp"

/// Building types. The numbers must be sequential, starting from zero,
/// since they are used to index into a std::vector of Sprite.
enum class BuildingType {
  // Player buildings
  TownCenter = 0,
  // TODO: use the proper annexes
  TownCenterBack,    // Not used as building, just for loading the sprite
  TownCenterCenter,  // Not used as building, just for loading the sprite
  TownCenterFront,   // Not used as building, just for loading the sprite
  TownCenterMain,    // Not used as building, just for loading the sprite
  
  House,
  Mill,
  MiningCamp,
  LumberCamp,
  Dock,
  
  Barracks,
  Outpost,
  PalisadeWall,
  PalisadeGate,
  
  // Gaia "buildings"
  TreeOak,
  FirstTree = TreeOak,
  LastTree = TreeOak,
  
  ForageBush,
  GoldMine,
  StoneMine,
  
  NumBuildings
};

bool IsTree(BuildingType type, const int civ = 0);

QSize GetBuildingSize(BuildingType type, const int civ = 0);
QRect GetBuildingOccupancy(BuildingType type, const int civ = 0);
QString GetBuildingName(BuildingType type, const int civ = 0);
double GetBuildingConstructionTime(BuildingType type, const int civ = 0);

/// TODO: This needs to consider the player's civilization and researched technologies
ResourceAmount GetBuildingCost(BuildingType type, const int civ = 0);

/// Returns whether the given building type acts as a drop-off point for the given resource type.
bool IsDropOffPointForResource(BuildingType building, ResourceType resource, const int civ = 0);

/// TODO: This needs to consider the player's civilization and researched technologies
u32 GetBuildingMaxHP(BuildingType type, const int civ = 0);
u32 GetBuildingMeleeArmor(BuildingType type, const int civ = 0);

/// Returns the max number of the given building type that the player can build.
/// Returns -1 if unlimited.
/// TODO: This needs to consider the player's age.
/// TODO: use infinity instead of -1 ?
int GetBuildingMaxInstances(BuildingType type, const int civ = 0);

int GetBuildingProvidedPopulationSpace(BuildingType type, const int civ = 0);

float GetBuildingLineOfSight(BuildingType type, const int civ = 0);
