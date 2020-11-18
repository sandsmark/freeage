#pragma once

#include <QString>

struct GameDataPrivate;
namespace genie {
class Unit;
}

class GameData
{
public:
  static bool initialize(const QString &gamePath);

  static const genie::Unit &unitData(const quint32 id, const quint32 civ);
  static const std::vector<genie::Unit> &allUnitData(const quint32 civ);

private:
  GameData();
  ~GameData();
  static GameDataPrivate &p();
};

