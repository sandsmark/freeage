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
  
  NumBuildings
};

QSize GetBuildingSize(BuildingType type);
QRect GetBuildingOccupancy(BuildingType type);
QString GetBuildingName(BuildingType type);

/// TODO: This needs to consider the player's civilization and researched technologies
ResourceAmount GetBuildingCost(BuildingType type);
