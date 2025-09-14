#ifndef helper
#define helper

int Sign(float num)
{
  if (num > 0)
  {
    num = 1;
  }
  else if (num == 0)
  {
    num = 0;
  }
  else
  {
    num = -1;
  }
  return num;
}

float ToDegrees(float num)
{
  return num * (180.0 / PI);
}

float ToRadians(float num)
{
  return num * PI / 180;
}

bool Intersects(float posX, float posY, int width, int height, float posX2, float posY2, int width2, int height2)
{
  return posX + width > posX2 && posX < posX2 + width2 && posY + height > posY2 && posY < posY2 + height2;
}

#endif