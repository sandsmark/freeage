// Copyright 2020 The FreeAge authors
// This file is part of FreeAge, licensed under the new BSD license.
// See the COPYING file in the project root for the license text.

#include "FreeAge/client/minimap.hpp"

#include "FreeAge/client/map.hpp"

Minimap::~Minimap() {
  if (haveTexture) {
    QOpenGLFunctions_3_2_Core* f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_2_Core>();
    f->glDeleteTextures(1, &textureId);
    haveTexture = false;
  }
  if (haveGeometryBuffersBeenInitialized) {
    QOpenGLFunctions_3_2_Core* f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_2_Core>();
    f->glDeleteBuffers(1, &vertexBuffer);
    haveGeometryBuffersBeenInitialized = false;
  }
}

void Minimap::Update(Map* map, const std::vector<QRgb>& playerColors, QOpenGLFunctions_3_2_Core* f) {
  if (!haveTexture) {
    f->glGenTextures(1, &textureId);
    f->glBindTexture(GL_TEXTURE_2D, textureId);
    
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    f->glTexImage2D(
        GL_TEXTURE_2D,
        0, GL_RGBA,
        map->GetWidth(), map->GetHeight(),
        0, GL_BGRA, GL_UNSIGNED_BYTE,
        nullptr);
    
    CHECK_OPENGL_NO_ERROR();
    haveTexture = true;
  }
  
  // TODO: Restrict updates to the parts of the texture that actually changed?
  // TODO: Use a color palette to reduce the amount of data transferred to the GPU on updates?
  // TODO: Updates could be written to one texture in a background thread while another texture is used for rendering
  
  // Render terrain
  QRgb* data = new QRgb[map->GetWidth() * map->GetHeight()];
  for (int y = 0; y < map->GetHeight(); ++ y) {
    const int* viewCountRow = &map->viewCountAt(0, 0) + y * map->GetWidth();
    QRgb* dataRow = data + y * map->GetWidth();
    for (int x = 0; x < map->GetWidth(); ++ x) {
      int viewCountValue = viewCountRow[x];
      int differences =
          std::abs(map->elevationAt(x, y) - map->elevationAt(x + 1, y)) +
          std::abs(map->elevationAt(x, y) - map->elevationAt(x, y + 1)) +
          std::abs(map->elevationAt(x, y) - map->elevationAt(x + 1, y + 1));
      
      // TODO: How should slopes be colored? Use some kind of lighting simulation, as on the actual terrain?
      dataRow[x] = (viewCountValue < 0) ? qRgb(0, 0, 0) : ((differences > 0) ? qRgb(25, 135, 14) : qRgb(51, 151, 39));
    }
  }
  
  // Render buildings and units
  for (const auto& item : map->GetObjects()) {
    if (item.second->isBuilding()) {
      ClientBuilding* building = AsBuilding(item.second);
      if (map->ComputeMaxViewCountForBuilding(building) < 0) {
        continue;
      }
      
      const QPoint& baseTile = building->GetBaseTile();
      
      if (IsTree(building->GetType())) {
        data[baseTile.x() + map->GetWidth() * baseTile.y()] = qRgb(21, 118, 21);
      } else if (building->GetType() == BuildingType::ForageBush) {
        data[baseTile.x() + map->GetWidth() * baseTile.y()] = qRgb(176, 217, 139);  // TODO: Check actual color; enlarge drawing?
      } else if (building->GetType() == BuildingType::GoldMine) {
        data[baseTile.x() + map->GetWidth() * baseTile.y()] = qRgb(255, 255, 0);  // TODO: Check actual color; enlarge drawing?
      } else if (building->GetType() == BuildingType::StoneMine) {
        data[baseTile.x() + map->GetWidth() * baseTile.y()] = qRgb(127, 127, 127);  // TODO: Check actual color; enlarge drawing?
      } else if (building->GetPlayerIndex() != kGaiaPlayerIndex) {
        constexpr int growSize = 0;
        
        QSize buildingSize = GetBuildingSize(building->GetType());
        
        int minX = std::max(0, baseTile.x() - growSize);
        int minY = std::max(0, baseTile.y() - growSize);
        int maxX = std::min(map->GetWidth() - 1, baseTile.x() + buildingSize.width() - 1 + growSize);
        int maxY = std::min(map->GetHeight() - 1, baseTile.y() + buildingSize.height() - 1 + growSize);
        
        for (int y = minY; y <= maxY; ++ y) {
          for (int x = minX; x <= maxX; ++ x) {
            data[x + map->GetWidth() * y] = playerColors[building->GetPlayerIndex()];
          }
        }
      }
    } else if (item.second->isUnit()) {
      ClientUnit* unit = AsUnit(item.second);
      if (map->IsUnitInFogOfWar(unit)) {
        continue;
      }
      
      constexpr int growSize = 0;
      
      int minX = std::max<int>(0, unit->GetMapCoord().x() - growSize);
      int minY = std::max<int>(0, unit->GetMapCoord().y() - growSize);
      int maxX = std::min<int>(map->GetWidth() - 1, unit->GetMapCoord().x() + growSize);
      int maxY = std::min<int>(map->GetHeight() - 1, unit->GetMapCoord().y() + growSize);
      
      for (int y = minY; y <= maxY; ++ y) {
        for (int x = minX; x <= maxX; ++ x) {
          data[x + map->GetWidth() * y] = playerColors[unit->GetPlayerIndex()];
        }
      }
    }
  }
  
  f->glBindTexture(GL_TEXTURE_2D, textureId);
  f->glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  f->glTexImage2D(
        GL_TEXTURE_2D,
        0, GL_RGBA,
        map->GetWidth(), map->GetHeight(),
        0, GL_BGRA, GL_UNSIGNED_BYTE,
        data);
  
  delete[] data;
}

void Minimap::Render(const QPointF& topLeft, float uiScale, const std::shared_ptr<MinimapShader>& shader, QOpenGLFunctions_3_2_Core* f) {
  if (!haveGeometryBuffersBeenInitialized) {
    f->glGenBuffers(1, &vertexBuffer);
    memset(oldVertexData, 0, 24 * sizeof(float));
    haveGeometryBuffersBeenInitialized = true;
  }
  
  shader->GetProgram()->UseProgram(f);
  
  f->glUniform1i(shader->GetTextureLocation(), 0);  // use GL_TEXTURE0
  f->glBindTexture(GL_TEXTURE_2D, textureId);
  
  // Update vertices?
  f->glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  
  float data[] = {
    // Top
    static_cast<float>(topLeft.x() + uiScale * 480),
    static_cast<float>(topLeft.y() + uiScale * 37),
    0.f,
    1.f,
    // Right
    static_cast<float>(topLeft.x() + uiScale * 824.5f),
    static_cast<float>(topLeft.y() + uiScale * 221.5f),
    1.f,
    1.f,
    // Bottom
    static_cast<float>(topLeft.x() + uiScale * 480),
    static_cast<float>(topLeft.y() + uiScale * 408),
    1.f,
    0.f,
    // Top
    static_cast<float>(topLeft.x() + uiScale * 480),
    static_cast<float>(topLeft.y() + uiScale * 37),
    0.f,
    1.f,
    // Bottom
    static_cast<float>(topLeft.x() + uiScale * 480),
    static_cast<float>(topLeft.y() + uiScale * 408),
    1.f,
    0.f,
    // Left
    static_cast<float>(topLeft.x() + uiScale * 136.5f),
    static_cast<float>(topLeft.y() + uiScale * 221.5f),
    0.f,
    0.f
  };
  
  bool haveChange = false;
  for (int i = 0; i < 24; ++ i) {
    if (data[i] != oldVertexData[i]) {
      haveChange = true;
      break;
    }
  }
  int numVertices = 6;
  int elementSizeInBytes = 4 * sizeof(float);
  if (haveChange) {
    f->glBufferData(GL_ARRAY_BUFFER, numVertices * elementSizeInBytes, data, GL_STATIC_DRAW);
    CHECK_OPENGL_NO_ERROR();
  }
  
  // Render
  shader->GetProgram()->SetPositionAttribute(
      2,
      GetGLType<float>::value,
      elementSizeInBytes,
      0,
      f);
  shader->GetProgram()->SetTexCoordAttribute(
      3,
      GetGLType<float>::value,
      elementSizeInBytes,
      2 * sizeof(float),
      f);
  
  f->glDrawArrays(GL_TRIANGLES, 0, numVertices);
  CHECK_OPENGL_NO_ERROR();
}
