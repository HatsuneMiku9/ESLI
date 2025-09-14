#ifndef vector2
#define vector2

#include <Arduino.h>
#include <math.h>
#include "helper.h"

struct Vector2
{
  float X, Y;

  Vector2(): X(0), Y(0) {}
  Vector2(float val): X(val), Y(val) {}
  Vector2(float x, float y): X(x), Y(y) {}
  /*Vector2 Zero()
  {
    return Vector2(0, 0);
  }*/

  float ToRotation()
  {
    /*float angle = atan2(Y, X);
    if (angle < 0)
    {
      angle += 2 * PI;
    }
    return angle;*/
    return atan2(Y, X);
  }
  Vector2 Add(Vector2 vector)
  {
    return Vector2(X + vector.X, Y + vector.Y);
  }
  Vector2 Subtract(Vector2 vector)
  {
    return Vector2(X - vector.X, Y - vector.Y);
  }
  Vector2 Multiply(float val)
  {
    return Vector2(X * val, Y * val);
  }
  Vector2 Divide(float val)
  {
    return Vector2(X / val, Y / val);
  }

  Vector2 RotateAround(float originX, float originY, float angle)
  {
    angle *= (PI / 180.0);

    // Trasladar este punto al origen relativo al centro
    float X2 = X - originX;
    float Y2 = Y - originY;

    // Aplicar rotación
    float X3 = X2 * cos(angle) - Y2 * sin(angle);
    float Y3 = X2 * sin(angle) + Y2 * cos(angle);

    return Vector2(X3 + originX, Y3 + originY);
  }
  Vector2 RotatedBy(float num)
  {
    return Vector2(X * cos(ToRadians(num)) - Y * sin(ToRadians(num)), X * sin(ToRadians(num)) + Y * cos(ToRadians(num)));
  }

  float Length()
  {
    return sqrt(X * X + Y * Y);
  }
};

Vector2 Normalize(Vector2 vector)
{
  float num = sqrt(vector.X * vector.X + vector.Y * vector.Y);
  return Vector2(vector.X / num, vector.Y / num);
}
float Distance(Vector2 vector, Vector2 vec2)
{
  return sqrt((vector.X - vec2.X) * (vector.X - vec2.X) + (vector.Y - vec2.Y) * (vector.Y - vec2.Y));
}

#endif