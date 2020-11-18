#include "game_data.hpp"

#include <QString>
#include <QDebug>

#include <genie/dat/DatFile.h>

struct GameDataPrivate
{
  bool initialize(const QString &gamePath) {
    if (initialized) {
      qWarning() << "Already initialized";
    }
    initialized = false;

    try {
      dat.load(gamePath.toStdString() + "/resources/_common/dat/empires2_x2_p1.dat");
    } catch (const std::exception &e) {
      qWarning() << "Failed to load dat" << e.what();
      return false;
    }

    initialized = true;
    return true;
  }
  bool initialized = false;
  genie::DatFile dat;
};

bool GameData::initialize(const QString &gamePath)
{
  return p().initialize(gamePath);
}

const std::vector<genie::Unit> &GameData::allUnitData(const quint32 civ)
{
  static const std::vector<genie::Unit> nullUnit;
  if (!p().initialized) {
    qWarning() << "Missing unitdata";
    return nullUnit;
  }
  if (civ >= p().dat.Civs.size()) {
    qWarning() << "civ" << civ << "out of range";
    return nullUnit;
  }
  return p().dat.Civs[civ].Units;
}

const genie::Unit &GameData::unitData(const quint32 id, const quint32 civ)
{
  static const genie::Unit nullUnit;
  if (!p().initialized) {
    qWarning() << "Missing unitdata";
    return nullUnit;
  }
  if (civ >= p().dat.Civs.size()) {
    qWarning() << "civ" << civ << "out of range";
    return nullUnit;
  }
  if (id >= p().dat.Civs[civ].Units.size()) {
    qWarning() << "unit" << id << "out of range";
  }

  return p().dat.Civs[civ].Units[id];
}

GameData::GameData()
{
}

GameData::~GameData()
{
  // out of line so std::unique_ptr works
}

GameDataPrivate &GameData::p()
{
  // Threadsafe, not that it matters much
  static GameDataPrivate instance;
  return instance;
}
