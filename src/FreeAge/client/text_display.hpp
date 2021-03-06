// Copyright 2020 The FreeAge authors
// This file is part of FreeAge, licensed under the new BSD license.
// See the COPYING file in the project root for the license text.

#pragma once

#include <QFont>
#include <QRect>
#include <QRgb>
#include <QString>

#include "FreeAge/client/opengl.hpp"
#include "FreeAge/client/shader_ui.hpp"

/// Helper class for text rendering, based on Qt's text rendering.
/// It works by drawing the text to a CPU image first and then transferring this image to the GPU.
/// Pro: Since this uses Qt's text renderer, it can probably deal with any kinds of obscure languages correctly.
/// Con: Transferring the text images to the GPU is slow. This operation must be done every time the text changes.
class TextDisplay {
 public:
  /// Deallocates OpenGL resources, thus needs an active OpenGL context.
  ~TextDisplay();
  
  void Render(const QFont& font, const QRgb& color, const QString& text, const QRect& rect, int alignmentFlags, GLuint bufferObject, UIShader* uiShader, int widgetWidth, int widgetHeight, QOpenGLFunctions_3_2_Core* f);
  
  /// Returns the bounds of the last rendered text.
  inline const QRect& GetBounds() const { return bounds; }
  
 private:
  void UpdateTexture(QOpenGLFunctions_3_2_Core* f);
  
  
  QString text;
  QFont font;
  int alignmentFlags = -1;
  
  bool textureInitialized = false;
  GLuint textureId;
  int textureWidth;
  int textureHeight;
  
  QRect bounds;
};
