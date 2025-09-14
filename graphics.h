#ifndef graphics
#define graphics

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "vector2.h"

bool screenPixels[128][64];
Adafruit_SSD1306 display(128, 64, &SPI, 21, 20, 17);

void clearScreen()
{
  for (int y = 0; y < 64; y++)
  {
    for (int x = 0; x < 128; x++)
    {
      if (screenPixels[x][y])
      {
        display.drawPixel(x, y, 0);
        screenPixels[x][y] = false;
      }
    }
  }
}
void clear(int positionX, int positionY, int width, int height)
{
  display.fillRect(positionX, positionY, width, height, 0);
}
void write(int positionX, int positionY, int size, String text)
{
  display.setTextSize(size);
  display.setCursor(positionX, positionY);
  display.print(text);
}
void draw(float positionX, float positionY, int width, int height, const bool* texture, float rotation = 0, float scale = 1, int frame = 1)
{
  int d90 = round(rotation / 90.0) * 90;
  if (fabs(rotation - d90) < 1.0)
  {
    rotation = d90;  // Ajustar si está dentro del margen
  }

  int f = (frame - 1) * height;
  for (int y = 0; y < height; y++)
  {
    int y0 = (y + f) * width;
    for (int x = 0; x < width; x++)
    {
      if (texture[y0 + x])
      {
        if (rotation != 0 || scale != 1)
        {
          Vector2 vector((float)x, (float)y);
          if (rotation != 0)
          {
            vector = vector.RotateAround((float)width / 2.0, (float)height / 2.0, rotation);
          }
          float rot0 = fmod(abs(rotation), 90.0) * PI / 180.0;

          float scaleRot = scale / max(cos(rot0), sin(rot0)) > 1 ? scale / max(cos(rot0), sin(rot0)) : 1;

          float newX0 = vector.X + ((float)x - (float)width / 2.0) * (scale - 1.0);
          float newY0 = vector.Y + ((float)y - (float)height / 2.0) * (scale - 1.0);

          int x02 = newX0 + positionX;
          int y02 = newY0 + positionY;
          for (int x03 = x02; x03 < x02 + scaleRot; x03++)
          {
            for (int y03 = y02; y03 < y02 + scaleRot; y03++)
            {
              if (x03 >= 0 && x03 < 128 && y03 >= 0 && y03 < 64)
              {
                screenPixels[x03][y03] = true;
              }
            }
          }
          if ((int)scaleRot > 1)
          {
            display.fillRect(x02, y02, scaleRot, scaleRot, 1);
          }
          else
          {
            display.drawPixel(x02, y02, 1);
          }
        }
        else if (x + positionX >= 0 && x + positionX < 128 && y + positionY >= 0 && y + positionY < 64)
        {
          int x02 = x + positionX;
          int y02 = y + positionY;
          display.drawPixel(x02, y02, 1);
          screenPixels[x02][y02] = true;
        }
      }
    }
  }
}
void drawOrigin(float positionX, float positionY, int width, int height, const bool* texture, Vector2 origin = Vector2(0, 0), float rotation = 0, float scale = 1, int frame = 1)
{
  int d90 = round(rotation / 90.0) * 90;
  if (fabs(rotation - d90) < 1.0)
  {
    rotation = d90;  // Ajustar si está dentro del margen
  }

  int f = (frame - 1) * height;
  for (int y = 0; y < height; y++)
  {
    int y0 = (y + f) * width;
    for (int x = 0; x < width; x++)
    {
      if (texture[y0 + x])
      {
        if (rotation != 0 || scale != 1)
        {
          Vector2 vector((float)x, (float)y);
          if (rotation != 0)
          {
            vector = vector.RotateAround(origin.X, origin.Y, rotation);
          }
          float rot0 = fmod(abs(rotation), 90.0) * PI / 180.0;

          float scaleRot = scale / max(cos(rot0), sin(rot0)) > 1 ? scale / max(cos(rot0), sin(rot0)) : 1;

          float newX0 = vector.X + ((float)x - (float)width / 2.0) * (scale - 1.0);
          float newY0 = vector.Y + ((float)y - (float)height / 2.0) * (scale - 1.0);

          int x02 = newX0 + positionX;
          int y02 = newY0 + positionY;
          for (int x03 = x02; x03 < x02 + scaleRot; x03++)
          {
            for (int y03 = y02; y03 < y02 + scaleRot; y03++)
            {
              if (x03 >= 0 && x03 < 128 && y03 >= 0 && y03 < 64)
              {
                screenPixels[x03][y03] = true;
              }
            }
          }
          if ((int)scaleRot > 1)
          {
            display.fillRect(x02, y02, scaleRot, scaleRot, 1);
          }
          else
          {
            display.drawPixel(x02, y02, 1);
          }
        }
        else if (x + positionX >= 0 && x + positionX < 128 && y + positionY >= 0 && y + positionY < 64)
        {
          int x02 = x + positionX;
          int y02 = y + positionY;
          display.drawPixel(x02, y02, 1);
          screenPixels[x02][y02] = true;
        }
      }
    }
  }
}
#endif