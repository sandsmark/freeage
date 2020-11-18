// Copyright 2020 The FreeAge authors
// This file is part of FreeAge, licensed under the new BSD license.
// See the COPYING file in the project root for the license text.

#include "FreeAge/common/building_types.hpp"

#include <QObject>

#include <genie/dat/Unit.h>

#include "FreeAge/common/logging.hpp"
#include "FreeAge/common/game_data.hpp"

// from aokts
// TODO: support loading the xml format of aokts directly
static int unitIds(const BuildingType type)
{
  switch (type) {
  case BuildingType::TownCenter:
    return 109;
  case BuildingType::House:
    return 70;
  case BuildingType::Mill:
    return 68;
  case BuildingType::MiningCamp:
    return 584;
  case BuildingType::LumberCamp:
    return 562;
  case BuildingType::Dock:
    return 805;
  case BuildingType::Barracks:
    return 12;
  case BuildingType::Outpost:
    return 598;
  case BuildingType::PalisadeWall:
    return 72;
  case BuildingType::PalisadeGate:
    return 523;
  case BuildingType::ForageBush:
    return 59;
  case BuildingType::GoldMine:
    return 66;
  case BuildingType::StoneMine:
    return 102;
  case BuildingType::TreeOak:
    return 349;
  case BuildingType::TownCenterBack:
    return 618;  // guessing annex 1
  case BuildingType::TownCenterCenter:
    return 619; // guessing annex 2
  case BuildingType::TownCenterFront:
    return 620; // guessing annex 3
  case BuildingType::TownCenterMain:
        return 621; // guessing head unit
  default:
    LOG(ERROR) << "Invalid type given: " << static_cast<int>(type);
    break;
  }

  return -1;
}

QSize GetBuildingSize(BuildingType type, const int civ) {
  // TODO: Load this from some data file?
  
  if (static_cast<int>(type) >= static_cast<int>(BuildingType::FirstTree) &&
      static_cast<int>(type) <= static_cast<int>(BuildingType::LastTree)) {
    return QSize(1, 1);
  }
  const genie::Unit &gunit = GameData::unitData(unitIds(type), civ);
  return QSize(gunit.Size.x, gunit.Size.y);
}

QRect GetBuildingOccupancy(BuildingType type, const int civ) {
  const genie::Unit &gunit = GameData::unitData(unitIds(type), civ);
  return QRect(0, 0, gunit.ClearanceSize.x, gunit.ClearanceSize.y);
}

QString GetBuildingName(BuildingType type, const int civ) {
  switch (type) {
  case BuildingType::TownCenter: return QObject::tr("Town Center");
  case BuildingType::TownCenterBack: LOG(ERROR) << "GetBuildingName() called on BuildingType::TownCenterBack"; return "";
  case BuildingType::TownCenterCenter: LOG(ERROR) << "GetBuildingName() called on BuildingType::TownCenterCenter"; return "";
  case BuildingType::TownCenterFront: LOG(ERROR) << "GetBuildingName() called on BuildingType::TownCenterFront"; return "";
  case BuildingType::TownCenterMain: LOG(ERROR) << "GetBuildingName() called on BuildingType::TownCenterMain"; return "";
  case BuildingType::House: return QObject::tr("House");
  
  case BuildingType::Mill: return QObject::tr("Mill");
  case BuildingType::MiningCamp: return QObject::tr("Mining Camp");
  case BuildingType::LumberCamp: return QObject::tr("Lumber Camp");
  case BuildingType::Dock: return QObject::tr("Dock");
  
  case BuildingType::Barracks: return QObject::tr("Barracks");
  case BuildingType::Outpost: return QObject::tr("Outpost");
  case BuildingType::PalisadeWall: return QObject::tr("Palisade Wall");
  case BuildingType::PalisadeGate: return QObject::tr("Palisade Gate");
  
  case BuildingType::TreeOak: return QObject::tr("Oak Tree");
  case BuildingType::ForageBush: return QObject::tr("Forage Bush");
  case BuildingType::GoldMine: return QObject::tr("Gold Mine");
  case BuildingType::StoneMine: return QObject::tr("Stone Mine");
  case BuildingType::NumBuildings: LOG(ERROR) << "GetBuildingName() called on BuildingType::NumBuildings"; return "";
  }
  return "";
}

double GetBuildingConstructionTime(BuildingType type, const int civ) {
  switch (type) {
  case BuildingType::TownCenter: break;
  case BuildingType::TownCenterBack: LOG(ERROR) << "GetBuildingConstructionTime() called on BuildingType::TownCenterBack"; break;
  case BuildingType::TownCenterCenter: LOG(ERROR) << "GetBuildingConstructionTime() called on BuildingType::TownCenterCenter"; break;
  case BuildingType::TownCenterFront: LOG(ERROR) << "GetBuildingConstructionTime() called on BuildingType::TownCenterFront"; break;
  case BuildingType::TownCenterMain: LOG(ERROR) << "GetBuildingConstructionTime() called on BuildingType::TownCenterMain"; break;
  case BuildingType::House: break;

  case BuildingType::Mill: break;
  case BuildingType::MiningCamp: break;
  case BuildingType::LumberCamp: break;
  case BuildingType::Dock: break;

  case BuildingType::Barracks: break;
  case BuildingType::Outpost: break;
  case BuildingType::PalisadeWall: break;
  case BuildingType::PalisadeGate: break;

  case BuildingType::TreeOak: LOG(ERROR) << "GetBuildingConstructionTime() called on BuildingType::TreeOak"; break;
  case BuildingType::ForageBush: LOG(ERROR) << "GetBuildingConstructionTime() called on BuildingType::ForageBush"; break;
  case BuildingType::GoldMine: LOG(ERROR) << "GetBuildingConstructionTime() called on BuildingType::GoldMine"; break;
  case BuildingType::StoneMine: LOG(ERROR) << "GetBuildingConstructionTime() called on BuildingType::StoneMine"; break;
  case BuildingType::NumBuildings: LOG(ERROR) << "GetBuildingConstructionTime() called on BuildingType::NumBuildings"; break;
  }

  return GameData::unitData(unitIds(type), civ).Creatable.TrainTime;
}

ResourceAmount GetBuildingCost(BuildingType type, const int civ) {
  const std::array<genie::unit::Creatable::ResourceCost, 3> &costs =
      GameData::unitData(unitIds(type), civ).Creatable.ResourceCosts;

  ResourceAmount amount;
  for (const genie::unit::Creatable::ResourceCost &res : costs) {
    switch (res.Type) {
    case genie::ResourceStoreMode::GiveResourceType:
    case genie::ResourceStoreMode::GiveAndTakeResourceType:
    case genie::ResourceStoreMode::BuildingResourceType:
      break;
    default:
      continue;
    }
    switch(genie::ResourceType(res.Type)) {
    case genie::ResourceType::WoodStorage:
      amount.wood() += res.Amount;
      break;
    case genie::ResourceType::GoldStorage:
      amount.gold() += res.Amount;
      break;
    case genie::ResourceType::StoneStorage:
      amount.stone() += res.Amount;
      break;
    case genie::ResourceType::FoodStorage:
      amount.food() += res.Amount;
      break;
    default:
      continue;
    }
  }

  return amount;
}

bool IsDropOffPointForResource(BuildingType building, ResourceType resource, const int civ) {
  // TODO: this is the wrong way around, the units have the dropsites they can use
//  if (GameData::unitData(unitIds(building), civ).Class != genie::Unit::BuildingClass) {
//    return false;
//  }

  genie::ResourceType gresource = genie::ResourceType::InvalidResource;
  switch(resource) {
  case ResourceType::Wood:
    gresource = genie::ResourceType::WoodStorage;
    break;
  case ResourceType::Gold:
    gresource = genie::ResourceType::GoldStorage;
    break;
  case ResourceType::Stone:
    gresource = genie::ResourceType::StoneStorage;
    break;
  case ResourceType::Food:
    gresource = genie::ResourceType::FoodStorage;
    break;
  default:
    return false;
  }
  const int resourceId = int(gresource);

  const int16_t id = unitIds(building);
  for (const genie::Unit &gunit : GameData::allUnitData(civ)) {
    for (const genie::Task &task : gunit.Action.TaskList) {
      if (task.ActionType != genie::ActionType::GatherRebuild) {
        continue;
      }
      if (task.ResourceOut != resourceId) {
        continue;
      }
      if (std::find(gunit.Action.DropSites.begin(), gunit.Action.DropSites.end(), id) == gunit.Action.DropSites.end()) {
        continue;
      }

      return true;
    }
  }

  return false;
}

u32 GetBuildingMaxHP(BuildingType type, const int civ) {
  return GameData::unitData(unitIds(type), civ).HitPoints;
}

u32 GetBuildingMeleeArmor(BuildingType type, const int civ) {
  const genie::Unit &gunit = GameData::unitData(unitIds(type), civ);
  return gunit.Combat.DisplayedMeleeArmour;

  // When the rest here is fixed
//  for (const genie::unit::AttackOrArmor &armor : gunit.Combat.Armours) {
//    if (armor.Class == genie::unit::AttackOrArmor::BaseMelee) {
//      return armor.Amount;
//    }
//  }
}

int GetBuildingMaxInstances(BuildingType type, const int civ) {
  if (type == BuildingType::TownCenter) { // TODO: add wonder
    return 1;
  } else if (type >= BuildingType::House && type <= BuildingType::PalisadeGate) {
    return -1; // unlimited
  }
  return 0;
}

int GetBuildingProvidedPopulationSpace(BuildingType type, const int civ) {
  const std::array<genie::Unit::ResourceStorage, 3> &storages =
      GameData::unitData(unitIds(type), civ).ResourceStorages;

  int amount = 0;
  for (const genie::Unit::ResourceStorage &res : storages) {
    if (res.Type != int(genie::ResourceType::PopulationHeadroom)) {
        continue;
    }

    if (res.Paid != genie::ResourceStoreMode::BuildingResourceType) {
      continue;
    }

    amount += res.Amount;
  }
  return amount;
}

float GetBuildingLineOfSight(BuildingType type, const int civ) {
  return GameData::unitData(unitIds(type), civ).LineOfSight;
}

bool IsTree(BuildingType type, const int civ) {
  return GameData::unitData(unitIds(type), civ).Class == genie::Unit::Tree;
}
