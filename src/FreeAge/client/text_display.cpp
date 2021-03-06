// Copyright 2020 The FreeAge authors
// This file is part of FreeAge, licensed under the new BSD license.
// See the COPYING file in the project root for the license text.

#include "FreeAge/client/text_display.hpp"

#include <cmath>

#include <QImage>
#include <QOpenGLFunctions_3_2_Core>
#include <QPainter>

TextDisplay::~TextDisplay() {
  QOpenGLFunctions_3_2_Core* f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_2_Core>();
  
  if (textureInitialized) {
    f->glDeleteTextures(1, &textureId);
  }
}

void TextDisplay::Render(const QFont& font, const QRgb& color, const QString& text, const QRect& rect, int alignmentFlags, GLuint bufferObject, UIShader* uiShader, int widgetWidth, int widgetHeight, QOpenGLFunctions_3_2_Core* f) {
  if (font != this->font ||
      text != this->text ||
      alignmentFlags != this->alignmentFlags) {
    this->font = font;
    this->text = text;
    this->alignmentFlags = alignmentFlags;
    
    UpdateTexture(f);
  }
  
  // Render the texture.
  ShaderProgram* program = uiShader->GetProgram();
  program->UseProgram(f);
  f->glUniform1i(uiShader->GetTextureLocation(), 0);  // use GL_TEXTURE0
  f->glBindTexture(GL_TEXTURE_2D, textureId);
  
  f->glUniform2f(uiShader->GetTexTopLeftLocation(), 0, 0);
  f->glUniform2f(uiShader->GetTexBottomRightLocation(), 1, 1);
  
  f->glUniform2f(
      uiShader->GetSizeLocation(),
      2.f * textureWidth / static_cast<float>(widgetWidth),
      2.f * textureHeight / static_cast<float>(widgetHeight));
  
  f->glUniform4f(uiShader->GetModulationColorLocation(), qRed(color) / 255.f, qGreen(color) / 255.f, qBlue(color) / 255.f, qAlpha(color) / 255.f);
  
  float leftX;
  if (alignmentFlags & Qt::AlignLeft) {
    leftX = rect.x();
  } else if (alignmentFlags & Qt::AlignHCenter) {
    leftX = rect.x() + 0.5f * rect.width() - 0.5f * textureWidth;
  } else if (alignmentFlags & Qt::AlignRight) {
    leftX = rect.x() + rect.width() - textureWidth;
  } else {
    LOG(ERROR) << "Missing horizontal alignment for text rendering.";
    leftX = rect.x();
  }
  
  float topY;
  if (alignmentFlags & Qt::AlignTop) {
    topY = rect.y();
  } else if (alignmentFlags & Qt::AlignVCenter) {
    topY = rect.y() + 0.5f * rect.height() - 0.5f * textureHeight;
  } else if (alignmentFlags & Qt::AlignBottom) {
    topY = rect.y() + rect.height() - textureHeight;
  } else {
    LOG(ERROR) << "Missing vertical alignment for text rendering.";
    topY = rect.y();
  }
  
  leftX = std::round(leftX);
  topY = std::round(topY);
  
  bounds = QRect(leftX, topY, textureWidth, textureHeight);
  
  int elementSizeInBytes = 3 * sizeof(float);
  f->glBindBuffer(GL_ARRAY_BUFFER, bufferObject);
  float* data = static_cast<float*>(f->glMapBufferRange(GL_ARRAY_BUFFER, 0, elementSizeInBytes, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT));
  data[0] = leftX;
  data[1] = topY;
  data[2] = 0.f;
  f->glUnmapBuffer(GL_ARRAY_BUFFER);
  
  program->SetPositionAttribute(
      3,
      GetGLType<float>::value,
      3 * sizeof(float),
      0,
      f);
  
  f->glDrawArrays(GL_POINTS, 0, 1);
  
  CHECK_OPENGL_NO_ERROR();
}

void TextDisplay::UpdateTexture(QOpenGLFunctions_3_2_Core* f) {
  // Compute the text size.
  // Note that we currently allocate a dummy image in order to get the correct font
  // metrics for drawing to QImages via a QPainter on that image. Is there a more efficient way to
  // get the correct font metrics?
  QImage dummyImage(1, 1, QImage::Format_RGBA8888);
  QPainter dummyPainter(&dummyImage);
  dummyPainter.setFont(font);
  QFontMetrics fontMetrics = dummyPainter.fontMetrics();
  QRect constrainRect(0, 0, 0, 0);
  QRect boundingRect = fontMetrics.boundingRect(constrainRect, alignmentFlags, text);
  textureWidth = boundingRect.width();
  textureHeight = boundingRect.height();
  dummyPainter.end();
  
  // Render the text into an image with that size.
  QImage textImage(textureWidth, textureHeight, QImage::Format_RGBA8888);
  textImage.fill(qRgba(0, 0, 0, 0));
  QPainter painter(&textImage);
  painter.setPen(qRgba(255, 255, 255, 255));
  painter.setFont(font);
  painter.drawText(textImage.rect(), alignmentFlags, text);
  painter.end();
  
  // Upload the rendered image to a texture.
  if (!textureInitialized) {
    f->glGenTextures(1, &textureId);
    f->glBindTexture(GL_TEXTURE_2D, textureId);
    
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    textureInitialized = true;
  } else {
    f->glBindTexture(GL_TEXTURE_2D, textureId);
  }
  
  f->glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  f->glTexImage2D(
      GL_TEXTURE_2D,
      0, GL_RGBA,
      textImage.width(), textImage.height(),
      0, GL_BGRA, GL_UNSIGNED_BYTE,
      textImage.scanLine(0));
}
