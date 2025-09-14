#ifndef content
#define content

#include "graphics.h"
#include "textures.h"
#include "vector2.h"

#pragma region Player
Vector2 playerPosition;
Vector2 playerVelocity;
float playerRotation = 0;
Vector2 oldPos;
float oldRot = 0;
int playerImmune = 50;

int enemiesActive = 0;

Vector2 playerCenter()
{
  return Vector2(playerPosition.X + 7.0 / 2.0, playerPosition.Y + 10.0 / 2.0);
}
#pragma endregion

#pragma region Super
class NPC
{
  public:
  float width;
  float height;
  Vector2 position;
  Vector2 velocity;
  int lifeMax;
  int life;
  int damage;
  float rotation;
  float scale;
  bool friendly;
  bool hostile;
  int immune;
  Vector2 oldPos;
  float oldRot;
  const bool* texture;
  int value;

  NPC(float posX, float posY):
    position(posX, posY),
    velocity(0, 0),
    lifeMax(100),
    damage(5),
    width(10),
    height(10),
    friendly(false),
    hostile(false),
    immune(0),
    rotation(0),
    scale(1),
    value(0) {}

  Vector2 Center()
  {
    return Vector2(position.X + width / 2, position.Y + height / 2);
  }

  virtual void AI()
  {
  }

  virtual bool PreKill()
  {
    return true;
  }

  virtual void Kill()
  {
  }

  virtual bool PreDraw() // Preferably override if texture width != height
  {
    return true;
  }
};
NPC* npc[24];

template <typename Type>
void NewNPC(float posX, float posY)
{
  for (int i = 0; i < 24; i++)
  {
    //NPC* npc0 = npc[i];
    if (npc[i] == nullptr)
    {
      NPC* newNPC = new Type(posX, posY);
      npc[i] = newNPC;
      NPC* npc0 = npc[i];
      npc0->life = npc0->lifeMax;
      npc0->position.X -= (float)npc0->width / 2.0;
      npc0->position.Y -= (float)npc0->height / 2.0;
      npc0->oldPos = npc0->position;
      npc0->oldRot = npc0->rotation;
      if (npc0->PreDraw())
      {
        draw(npc0->position.X, npc0->position.Y, npc0->width, npc0->height, npc0->texture, npc0->rotation, npc0->scale);
      }
      enemiesActive++;
      return;
    }
  }
}

class Projectile
{
  public:
  float width;
  float height;
  Vector2 position;
  Vector2 velocity;
  int damage;
  float rotation;
  float scale;
  bool friendly;
  bool hostile;
  int timeLeft;
  int penetrate;
  int immunity;
  Vector2 oldPos;
  float oldRot;
  const bool* texture;
  bool laser = false;

  Projectile(float posX, float posY,
             float velX, float velY,
             int dmg,
             bool isFriendly, bool isHostile,
             float rot, float scl):
    timeLeft(500),
    position(posX, posY),
    velocity(velX, velY),
    width(10),
    height(10),
    damage(dmg),
    friendly(isFriendly),
    hostile(isHostile),
    penetrate(1),
    immunity(0),
    rotation(rot),
    scale(scl),
    oldPos(0, 0),
    oldRot(0) {}

  Vector2 Center()
  {
    return Vector2(position.X + width / 2, position.Y + height / 2);
  }

  NPC* TargetClosestFriendly()
  {
    int dist = 0;
    NPC* npc2 = nullptr;
    for (int i = 0; i < 24; i++)
    {
      NPC* npc0 = npc[i];
      if (npc[i] == nullptr)
      {
        break;
      }
      if (npc0->friendly && (Distance(Center(), npc0->Center()) < dist || dist == 0))
      {
        npc2 = npc[i];
        dist = Distance(Center(), npc0->Center());
      }
    }
    return npc2;
  }

  NPC* TargetClosestHostile()
  {
    int dist = 0;
    NPC* npc2 = nullptr;
    for (int i = 0; i < 24; i++)
    {
      NPC* npc0 = npc[i];
      if (npc[i] == nullptr)
      {
        break;
      }
      if (npc0->hostile && (Distance(Center(), npc0->Center()) < dist || dist == 0))
      {
        npc2 = npc[i];
        dist = Distance(Center(), npc0->Center());
      }
    }
    return npc2;
  }

  virtual void AI()
  {
  }

  virtual void Kill()
  {
  }

  virtual int FindFrame()
  {
    return 1;
  }

  virtual bool PreDraw() // Preferably override if texture width != height
  {
    return true;
  }
};
Projectile* proj[100];

template <typename Type>
void NewProjectile(float posX, float posY, float velX, float velY, int damage, bool friendly, bool hostile, float rotation = 0, float scale = 1)
{
  for (int i = 0; i < 100; i++)
  {
    if (proj[i] == nullptr)
    {
      Projectile* newProjectile = new Type(posX, posY, velX, velY, damage, friendly, hostile, rotation, scale);
      proj[i] = newProjectile;
      Projectile* proj0 = proj[i];
      proj0->position.X -= (float)proj0->width / 2.0;
      proj0->position.Y -= (float)proj0->height / 2.0;
      proj0->oldPos = proj0->position;
      proj0->oldRot = proj0->rotation;
      if (proj0->PreDraw())
      {
        draw(proj0->position.X, proj0->position.Y, proj0->width, proj0->height, proj0->texture, proj0->rotation, proj0->scale);
      }
      return;
    }
  }
}
#pragma endregion

#pragma region Projectiles
class Bullet : public Projectile
{
  public:
  Bullet(float posX, float posY,
         float velX, float velY,
         int dmg,
         bool isFriendly, bool isHostile,
         float rot, float scl) : Projectile(posX, posY, velX, velY, dmg, isFriendly, isHostile, rot, scl)
  {
    width = 1;
    height = 1; // 3
    texture = bullet;
  }
  void AI() override
  {
  }
  void Kill() override
  {
  }
  bool PreDraw() override
  {
    int texW = 1;
    int texH = 3;
    int posX = Center().X - (float)texW / 2.0;
    int posY = Center().Y - (float)texH / 2.0;
    draw(posX, posY, texW, texH, texture, rotation, scale);
    return false;
  }
};
class BulletSmall : public Projectile
{
  public:
  BulletSmall(float posX, float posY,
         float velX, float velY,
         int dmg,
         bool isFriendly, bool isHostile,
         float rot, float scl) : Projectile(posX, posY, velX, velY, dmg, isFriendly, isHostile, rot, scl)
  {
    width = 1;
    height = 1; // 3
    texture = bulletS;
  }
  void AI() override
  {
  }
  void Kill() override
  {
  }
  bool PreDraw() override
  {
    int texW = 1;
    int texH = 2;
    int posX = Center().X - (float)texW / 2.0;
    int posY = Center().Y - (float)texH / 2.0;
    draw(posX, posY, texW, texH, texture, rotation, scale);
    return false;
  }
};
class Explosion : public Projectile
{
  public:
  Explosion(float posX, float posY,
         float velX, float velY,
         int dmg,
         bool isFriendly, bool isHostile,
         float rot, float scl) : Projectile(posX, posY, velX, velY, dmg, isFriendly, isHostile, rot, scl)
  {
    width = 17;
    height = 17;
    texture = explosion;
    timeLeft = 28;
    penetrate = -1;
  }
  void AI() override
  {
    if (timeLeft <= 16)
    {
      damage = 0;
    }
  }
  void Kill() override
  {
  }
  int FindFrame() override
  {
    return ceil((29.0 - (float)timeLeft) / 4.0);
  }
};
class Flames : public Projectile
{
  int ai, ai2;
  public:
  Flames(float posX, float posY,
         float velX, float velY,
         int dmg,
         bool isFriendly, bool isHostile,
         float rot, float scl) : Projectile(posX, posY, velX, velY, dmg, isFriendly, isHostile, rot, scl)
  {
    width = 17;
    height = 17;
    immunity = 0;
    texture = explosion;
    timeLeft = 14;
    penetrate = -1;
  }
  void AI() override
  {
    if (timeLeft <= 6)
    {
      scale += 0.12;
      if (timeLeft <= 5)
      {
        if (timeLeft == 4)
        {
          damage *= 0.67;
        }
        else if (timeLeft <= 3)
        {
          damage = 0;
        }
        else
        {
          damage *= 0.75;
        }
      }
    }
  }
    
  void Kill() override
  {
  }

  int FindFrame() override
  {
    return ceil((15.0 - (float)timeLeft) / 2.0);
  }
};
class Rocket : public Projectile
{
  public:
  Rocket(float posX, float posY,
         float velX, float velY,
         int dmg,
         bool isFriendly, bool isHostile,
         float rot, float scl) : Projectile(posX, posY, velX, velY, dmg, isFriendly, isHostile, rot, scl)
  {
    width = 3;
    height = 3; // 8
    texture = rocket;
  }
  void AI() override
  {
    if (velocity.Length() < 4)
    {
      Vector2 v1(0, -0.1);
      velocity = velocity.Add(v1.RotatedBy(rotation));
    }
    else
    {
      velocity = Normalize(velocity).Multiply(4);
    }
    if (timeLeft % 17 == 16)
    {
      NewProjectile<Explosion>(Center().X, Center().Y, 0, random(21) * 0.01, 0, false, false, 0, 0.4 + random(21) * 0.01);
    }
  }
  void Kill() override
  {
    NewProjectile<Explosion>(Center().X, Center().Y, 0, 0, damage / 3, true, false);
  }
  bool PreDraw() override
  {
    int texW = 5;
    int texH = 8;
    int posX = Center().X - (float)texW / 2.0;
    int posY = Center().Y - (float)texH / 2.0;
    draw(posX, posY, texW, texH, texture, rotation, scale);
    return false;
  }
};
class Laser : public Projectile
{
  public:
  Laser(float posX, float posY,
         float velX, float velY,
         int dmg,
         bool isFriendly, bool isHostile,
         float rot, float scl) : Projectile(posX, posY, velX, velY, dmg, isFriendly, isHostile, rot, scl)
  {
    penetrate = 1;
    timeLeft = 1;
    width = 1;
    height = 1;
    texture = laserByte;
    laser = true;
  }
  void AI() override
  {
    if (position.Y > -32)
    {
      NewProjectile<Laser>(Center().X, position.Y + 1, 0, 0, damage, true, false, 0, 1);
    }
  }
  void Kill() override
  {
  }
};
class RocketMini : public Projectile
{
  public:
  RocketMini(float posX, float posY,
         float velX, float velY,
         int dmg,
         bool isFriendly, bool isHostile,
         float rot, float scl) : Projectile(posX, posY, velX, velY, dmg, isFriendly, isHostile, rot, scl)
  {
    width = 4;
    height = 4; // 8
    texture = miniRocket;
    timeLeft = 225;
  }
  void AI() override
  {
    rotation = ToDegrees(velocity.ToRotation()) + 90.0;
    if (timeLeft == 207)
    {
      NewProjectile<Explosion>(Center().X, Center().Y, 0, 0, 0, false, false, 0, 0.2 + random(16) * 0.01);
    }
    else if (timeLeft < 207)
    {
      NPC* target = TargetClosestHostile();
      if (target != nullptr)
      {
        velocity = Normalize(target->Center().Subtract(Center())).Multiply(2);
      }
    }
  }
  void Kill() override
  {
    NewProjectile<Explosion>(Center().X, Center().Y, 0, 0, damage / 3, true, false, 0, 0.4);
  }
  bool PreDraw() override
  {
    int texW = 4;
    int texH = 5;
    int posX = Center().X - (float)texW / 2.0;
    int posY = Center().Y - (float)texH / 2.0;
    draw(posX, posY, texW, texH, texture, rotation, scale);
    return false;
  }
};
class Plasma : public Projectile
{
  public:
  Plasma(float posX, float posY,
         float velX, float velY,
         int dmg,
         bool isFriendly, bool isHostile,
         float rot, float scl) : Projectile(posX, posY, velX, velY, dmg, isFriendly, isHostile, rot, scl)
  {
    width = 9;
    height = 9; // 3
    texture = plasmaBall;
  }
  void AI() override
  {
    if (scale < 1)
    {
      scale += 0.05;
    }
  }
  void Kill() override
  {
    NewProjectile<Explosion>(Center().X, Center().Y, 0, 0, damage * 0.17, true, false);
  }
};
class BM : public Projectile
{
  public:
  int ai, ai2 = random(1, 5);
  BM(float posX, float posY,
         float velX, float velY,
         int dmg,
         bool isFriendly, bool isHostile,
         float rot, float scl) : Projectile(posX, posY, velX, velY, dmg, isFriendly, isHostile, rot, scl)
  {
    width = 3;
    height = 3; // 3
    texture = stormBM;
  }
  void AI() override
  {
    if (ai % 5 == 0)
    {
      ai2++;
      if (ai2 % 5 == 0)
      {
        ai2 = 1;
      }
    }
    ai++;
  }
  void Kill() override
  {
    NewProjectile<Flames>(Center().X, Center().Y, 0, 0, 0, false, false, 0, 0.2);
  }
  bool PreDraw() override
  {
    int texW = 3;
    int texH = 8;
    int posX = Center().X - (float)texW / 2.0;
    int posY = Center().Y - (float)texH / 2.0;
    draw(posX, posY, texW, texH, texture, rotation, scale, ai2);
    return false;
  }
};
class Rail : public Projectile
{
  public:
  int ai = 1;
  Rail(float posX, float posY,
         float velX, float velY,
         int dmg,
         bool isFriendly, bool isHostile,
         float rot, float scl) : Projectile(posX, posY, velX, velY, dmg, isFriendly, isHostile, rot, scl)
  {
    penetrate = -1;
    timeLeft = 1;
    width = 3;
    height = 3;
    texture = rg;
    laser = true;
  }
  void AI() override
  {
    if (ai == 1)
    {
      ai = 2;
    }
    else
    {
      ai = 1;
    }
    int posY = position.Y - 1;
    if (posY > -32)
    {
      NewProjectile<Rail>(Center().X, posY, 0, 0, damage, true, false, 0, 1);
    }
  }
  virtual int FindFrame()
  {
    return ai;
  }
  void Kill() override
  {
  }
};
class RocketMiniEvil : public Projectile
{
  public:
  RocketMiniEvil(float posX, float posY,
         float velX, float velY,
         int dmg,
         bool isFriendly, bool isHostile,
         float rot, float scl) : Projectile(posX, posY, velX, velY, dmg, isFriendly, isHostile, rot, scl)
  {
    width = 4;
    height = 4; // 8
    texture = miniRocket;
    timeLeft = 200;
  }
  void AI() override
  {
    rotation = ToDegrees(velocity.ToRotation()) + 90.0;
    if (timeLeft == 182)
    {
      NewProjectile<Explosion>(Center().X, Center().Y, 0, 0, 0, false, false, 0, 0.2 + random(16) * 0.01);
    }
    else if (timeLeft >= 172 && timeLeft < 182)
    {
      velocity = Normalize(playerCenter().Subtract(Center())).Multiply(2);
    }
  }
  void Kill() override
  {
    NewProjectile<Explosion>(Center().X, Center().Y, 0, 0, 0, true, false, 0, 0.4);
  }
  bool PreDraw() override
  {
    int texW = 4;
    int texH = 5;
    int posX = Center().X - (float)texW / 2.0;
    int posY = Center().Y - (float)texH / 2.0;
    draw(posX, posY, texW, texH, texture, rotation, scale);
    return false;
  }
};
class VuVProj : public Projectile
{
  public:
  VuVProj(float posX, float posY,
         float velX, float velY,
         int dmg,
         bool isFriendly, bool isHostile,
         float rot, float scl) : Projectile(posX, posY, velX, velY, dmg, isFriendly, isHostile, rot, scl)
  {
    width = 1;
    height = 1; // 3
    texture = vuvproj;
  }
  void AI() override
  {
  }
  void Kill() override
  {
  }
  bool PreDraw() override
  {
    int texW = 3;
    int texH = 6;
    int posX = Center().X - (float)texW / 2.0;
    int posY = Center().Y - (float)texH / 2.0;
    draw(posX, posY, texW, texH, texture, rotation, scale);
    return false;
  }
};
class TroniWave : public Projectile
{
  public:
  TroniWave(float posX, float posY,
         float velX, float velY,
         int dmg,
         bool isFriendly, bool isHostile,
         float rot, float scl) : Projectile(posX, posY, velX, velY, dmg, isFriendly, isHostile, rot, scl)
  {
    width = 3;
    height = 3; // 3
    texture = wave;
    penetrate = 3;
    timeLeft = 250;
  }
  void AI() override
  {
    if (position.X + velocity.X < 0 || position.X + velocity.X > 128)
    {
      velocity.X *= -1;
    }
    if (position.Y + velocity.Y < 0 || position.Y + velocity.Y > 64)
    {
      velocity.Y *= -1;
    }
  }
  void Kill() override
  {
  }
  bool PreDraw() override
  {
    int texW = 6;
    int texH = 4;
    int posX = Center().X - (float)texW / 2.0;
    int posY = Center().Y - (float)texH / 2.0;
    draw(posX, posY, texW, texH, texture, rotation, scale);
    return false;
  }
};
class TroniWaveBig : public Projectile
{
  public:
  int ai = 0;
  TroniWaveBig(float posX, float posY,
         float velX, float velY,
         int dmg,
         bool isFriendly, bool isHostile,
         float rot, float scl) : Projectile(posX, posY, velX, velY, dmg, isFriendly, isHostile, rot, scl)
  {
    width = 64;
    height = 8; // 3
    texture = waveBig;
    penetrate = 3;
    timeLeft = 75;
  }
  void AI() override
  {
    if (ai >= 50)
    {
      velocity.Y = 1.8;
      scale = 0.625 + (ai - 50) * 0.055;
    }
    ai++;
  }
  void Kill() override
  {
  }
  /*bool PreDraw() override
  {
    int texW = 6;
    int texH = 4;
    int posX = Center().X - (float)texW / 2.0;
    int posY = Center().Y - (float)texH / 2.0;
    draw(posX, posY, texW, texH, texture, rotation, scale);
    return false;
  }*/
};
#pragma endregion

#pragma region NPCs
class Enemy1 : public NPC
{
  public:
  int ai;
  Enemy1(float posX, float posY) : NPC(posX, posY)
  {
    value = 16;
    lifeMax = 120;
    damage = 16;
    width = 10; // 11
    height = 10;
    texture = enemy;
    hostile = true;
    ai = 0;
  }
  void AI() override
  {
    float dist = Distance(Center(), playerCenter());
    if (dist > 55)
    {
      Vector2 maxSpeed = Normalize(playerCenter().Subtract(Center())).Multiply(0.75);
      float increase = 2.5;
      velocity = velocity.Multiply(increase - 1).Add(maxSpeed).Divide(increase);
    }
		else
		{
			velocity = velocity.Divide(1.1);
		}

    Vector2 vector(playerCenter().Y - Center().Y, playerCenter().X - Center().X);
    rotation = ToDegrees(atan2(-vector.Y, vector.X) + PI);
    if (ai >= 60)
    {
        Vector2 vel = Vector2(0, -2).RotatedBy(rotation);
        NewProjectile<Bullet>(Center().X, Center().Y, vel.X, vel.Y, 6, false, true, rotation, 1);
        ai = 0;
    }
    if (ai < 60)
    {
      ai++;
    }
  }
  void Kill() override
  {
    NewProjectile<Explosion>(Center().X, Center().Y, 0, 0, 0, false, false);
  }
  bool PreDraw() override
  {
    int texW = 11;
    int texH = 10;
    int posX = Center().X - (float)texW / 2.0;
    int posY = Center().Y - (float)texH / 2.0;
    draw(posX, posY, texW, texH, texture, rotation, scale);
    return false;
  }
};
class Enemy2 : public NPC
{
  public:
  int ai;
  Enemy2(float posX, float posY) : NPC(posX, posY)
  {
    value = 84;
    lifeMax = 230;
    damage = 16;
    width = 9;
    height = 9; // 9
    texture = enemy2;
    hostile = true;
    ai = 0;
  }
  void AI() override
  {
    Vector2 targetVec = Vector2(64, 16);
    if (ai >= 150)
    {
      targetVec = playerCenter();
      if (ai >= 225)
      {
        targetVec = Vector2(64, 16);
        ai = 0;
      }
    }
    ai++;
    float acc = 0.075;
    float turnSpeed = 2;
    float maxSpd = 2.5;
    if (Sign(targetVec.X - Center().X) == 1)
    {
      if (velocity.X < 0)
      {
        velocity.X += acc;
      }
      else if (velocity.X < maxSpd)
      {
        velocity.X += acc * turnSpeed;
      }
      else
      {
        velocity.X = maxSpd;
      }
    }
    else if (Sign(targetVec.X - Center().X) == -1)
    {
      if (velocity.X > 0)
      {
        velocity.X -= acc;
      }
      else if (velocity.X > -maxSpd)
      {
        velocity.X -= acc * turnSpeed;
      }
      else
      {
        velocity.X = -maxSpd;
      }
    }
    if (Sign(targetVec.Y - Center().Y) == 1)
    {
      if (velocity.Y < 0)
      {
        velocity.Y += acc;
      }
      else if (velocity.Y < maxSpd)
      {
        velocity.Y += acc * turnSpeed;
      }
      else
      {
        velocity.Y = maxSpd;
      }
    }
    else if (Sign(targetVec.Y - Center().Y) == -1)
    {
      if (velocity.Y > 0)
      {
        velocity.Y -= acc;
      }
      else if (velocity.Y > -maxSpd)
      {
        velocity.Y -= acc * turnSpeed;
      }
      else
      {
        velocity.Y = -maxSpd;
      }
    }
    if (velocity.Length() > maxSpd)
    {
      velocity = Normalize(velocity).Multiply(maxSpd);
    }
    rotation = ToDegrees(velocity.ToRotation()) + 90.0;
  }  
  void Kill() override
  {
    NewProjectile<Explosion>(Center().X, Center().Y, 0, 0, 0, false, false);
  }
  bool PreDraw() override
  {
    int texW = 9;
    int texH = 14;
    int posX = Center().X - (float)texW / 2.0;
    int posY = Center().Y - (float)texH / 2.0;
    draw(posX, posY, texW, texH, texture, rotation, scale);
    return false;
  }
};
class Enemy3 : public NPC
{
  public:
  int ai;
  Enemy3(float posX, float posY) : NPC(posX, posY)
  {
    value = 42;
    lifeMax = 210;
    damage = 19;
    width = 9; // 11
    height = 9;
    texture = enemy3;
    hostile = true;
    ai = 0;
  }
  void AI() override
  {
    float dist = Distance(Center(), playerCenter());
    if (dist > 50)
    {
      Vector2 maxSpeed = Normalize(playerCenter().Subtract(Center())).Multiply(0.9);
      float increase = 2.75;
      velocity = velocity.Multiply(increase - 1).Add(maxSpeed).Divide(increase);
    }
		else
		{
			velocity = velocity.Divide(1.025);
		}
    if (dist < 35)
    {
      Vector2 maxSpeed = Normalize(Center().Subtract(playerCenter())).Multiply(0.9 - (35 - dist) / 233);
      float increase = 2.75;
      velocity = velocity.Multiply(increase - 1).Add(maxSpeed).Divide(increase);
    }

    Vector2 vector(playerCenter().Y - Center().Y, playerCenter().X - Center().X);
    rotation = ToDegrees(atan2(-vector.Y, vector.X) + PI);
    if (ai >= 92)
    {
      if (ai % 6 == 2)
      {
        Vector2 vel = Vector2(0, -2).RotatedBy(rotation);
        NewProjectile<BulletSmall>(Center().X, Center().Y, vel.X, vel.Y, 4, false, true, rotation, 1);
      }
      if (ai >= 110)
      {
        ai = 0;
      }
    }
    ai++;
  }
  void Kill() override
  {
    NewProjectile<Explosion>(Center().X, Center().Y, 0, 0, 0, false, false);
  }
  bool PreDraw() override
  {
    int texW = 9;
    int texH = 12;
    int posX = Center().X - (float)texW / 2.0;
    int posY = Center().Y - (float)texH / 2.0;
    draw(posX, posY, texW, texH, texture, rotation, scale);
    return false;
  }
};
class LordFunken : public NPC
{
  public:
  int ai = -50;
  int ai2 = 1;
  Vector2 bfg;
  float bfgRot = 0;
  int cannonL = 1;
  int cannonL2 = 1;
  int cannonR = 1;
  int cannonR2 = 1;
  int turretLFrame = 1;
  int turretRFrame = 1;
  int killTimer = 0;

  LordFunken(float posX, float posY) : NPC(posX, posY)
  {
    value = 2488;
    lifeMax = 8000;
    damage = 70;
    width = 116; // 11
    height = 27;
    texture = funken;
    hostile = true;
    position = Vector2(64, -61);
    bfg = Vector2(Center().X - 3, position.Y + 23);
  }
  void AI() override
  {
    if (ai < 0) // Intro
    {
      position = Vector2(6, -5 + ai * 1.12);
    }
    else if (ai <= 600)
    {
      Vector2 vBfg(playerCenter().Y - (position.Y + 24), playerCenter().X - Center().X);
      bfgRot = ToDegrees(atan2(-vBfg.Y, vBfg.X));
      if (bfgRot < -50)
      {
        bfgRot = -50;
      }
      else if (bfgRot > 50)
      {
        bfgRot = 50;
      }

      int dmg = 9;
      if (ai % 200 == 128)
      {
        turretLFrame = 2;
        turretRFrame = 4;
        Vector2 vel = Vector2(0, 1.5).RotatedBy(-35);
        NewProjectile<Bullet>(position.X + 42, position.Y + 24, vel.X, vel.Y, dmg, false, true, -35, 1);
        NewProjectile<Bullet>(position.X + 75, position.Y + 24, -vel.X, vel.Y, dmg, false, true, 35, 1);
      }
      else if (ai % 200 == 140)
      {
        turretLFrame = 3;
        turretRFrame = 5;
        Vector2 vel = Vector2(0, 1.5).RotatedBy(-70);
        NewProjectile<Bullet>(position.X + 42, position.Y + 24, vel.X, vel.Y, dmg, false, true, -70, 1);
        NewProjectile<Bullet>(position.X + 75, position.Y + 24, -vel.X, vel.Y, dmg, false, true, 70, 1);
      }
      else if (ai % 200 == 152)
      {
        turretLFrame = 2;
        turretRFrame = 4;
        Vector2 vel = Vector2(0, 1.5).RotatedBy(-35);
        NewProjectile<Bullet>(position.X + 42, position.Y + 24, vel.X, vel.Y, dmg, false, true, -35, 1);
        NewProjectile<Bullet>(position.X + 75, position.Y + 24, -vel.X, vel.Y, dmg, false, true, 35, 1);
      }
      else if (ai % 200 == 164)
      {
        turretLFrame = 1;
        turretRFrame = 1;
        NewProjectile<Bullet>(position.X + 42, position.Y + 24, 0, 1.5, dmg, false, true, 0, 1);
        NewProjectile<Bullet>(position.X + 75, position.Y + 24, 0, 1.5, dmg, false, true, 0, 1);
      }
      else if (ai % 200 == 176)
      {
        turretLFrame = 4;
        turretRFrame = 2;
        Vector2 vel = Vector2(0, 1.5).RotatedBy(35);
        NewProjectile<Bullet>(position.X + 42, position.Y + 24, vel.X, vel.Y, dmg, false, true, 35, 1);
        NewProjectile<Bullet>(position.X + 75, position.Y + 24, -vel.X, vel.Y, dmg, false, true, -35, 1);
      }
      else if (ai % 200 == 188)
      {
        turretLFrame = 1;
        turretRFrame = 1;
        NewProjectile<Bullet>(position.X + 42, position.Y + 24, 0, 1.5, dmg, false, true, 0, 1);
        NewProjectile<Bullet>(position.X + 75, position.Y + 24, 0, 1.5, dmg, false, true, 0, 1);
      }
      else if (ai == 200 || ai == 400)
      {
        Vector2 vel = Vector2(0, -2).RotatedBy(bfgRot + 180.0);
        NewProjectile<Rocket>(Center().X, position.Y + 24, vel.X, vel.Y, 35, false, true, bfgRot + 180.0, 1);
      }

      position = Vector2(6, -5);
    }
    else if (ai <= 640) // Transición
    {
      bfgRot = 0;
      position = Vector2(6, -5 - 47 * (ai - 600) / 40); //6, 55
    }
    else if (ai <= 680) // Transición
    {
      position = Vector2(6, 102 - 47 * (ai - 640) / 40); //6, 55
    }
    else if (ai <= 1030)
    {
      if (ai == 780 || ai == 880)
      {
        NewProjectile<RocketMiniEvil>(position.X + 25, position.Y + 2, -1.4142, -1.4142, 22, false, true, -45, 1);
        cannonL = 2;
      }
      else if (ai == 785 || ai == 885)
      {
        cannonL = 3;
      }
      else if (ai == 790 || ai == 890)
      {
        cannonL = 1;
      }
      else if (ai == 805 || ai == 905)
      {
        NewProjectile<RocketMiniEvil>(position.X + 35, position.Y + 1, -1.4142, -1.4142, 22, false, true, -45, 1);
        cannonL2 = 2;
      }
      else if (ai == 810 || ai == 910)
      {
        cannonL2 = 3;
      }
      else if (ai == 815 || ai == 915)
      {
        cannonL2 = 1;
      }
      else if (ai == 830 || ai == 930)
      {
        NewProjectile<RocketMiniEvil>(position.X + 79, position.Y + 1, 1.4142, -1.4142, 22, false, true, 45, 1);
        cannonR = 2;
      }
      else if (ai == 835 || ai == 935)
      {
        cannonR = 3;
      }
      else if (ai == 840 || ai == 940)
      {
        cannonR = 1;
      }
      else if (ai == 855 || ai == 955)
      {
        NewProjectile<RocketMiniEvil>(position.X + 89, position.Y + 2, 1.4142, -1.4142, 22, false, true, 45, 1);
        cannonR2 = 2;
      }
      else if (ai == 860 || ai == 960)
      {
        cannonR2 = 3;
      }
      else if (ai == 865 || ai == 965)
      {
        cannonR2 = 1;
      }
      position = Vector2(6, 55);
    }
    else if (ai <= 1070) // Transición
    {
      position = Vector2(6, 55 + 47 * (ai - 1030) / 40); //6, 55
    }
    else if (ai <= 1110) // Transición
    {
      position = Vector2(6, -52 + 47 * (ai - 1070) / 40); //6, 55
    }
    else
    {
      ai = 0;
    }
    ai++;

    // BFG pos
    bfg = Vector2(Center().X - 3, position.Y + 23);
    if (bfgRot >= 1)
    {
      bfg.X++;
    }

    if (ai % 4 == 0)
    {
      ai2++;
      if (ai2 > 3)
      {
        ai2 = 1;
      }
    }
  }
  bool PreKill() override
  {
    PreDraw();
    damage = 0;
    if (killTimer >= 96)
    {
      return true;
    }
    if (killTimer % 12 == 0)
    {
      NewProjectile<Explosion>(position.X + random(0, width), position.Y + random(0, height), 0, 0, 0, false, false);
    }
    killTimer++;
    return false;
  }
  void Kill() override
  {
    NewProjectile<Explosion>(Center().X - width / 4, Center().Y, 0, 0, 0, false, false, 0, 2);
    NewProjectile<Explosion>(Center().X, Center().Y, 0, 0, 0, false, false, 0, 2);
    NewProjectile<Explosion>(Center().X + width / 4, Center().Y, 0, 0, 0, false, false, 0, 2);
  }
  bool PreDraw() override
  {
    draw(Center().X - 64, Center().Y - 28, 128, 45, texture);

    draw(position.X + 10, position.Y + 29, 9, 17, funkenJet, 0, 1, ai2);
    draw(position.X + 97, position.Y + 29, 9, 17, funkenJet, 0, 1, ai2);
    
    draw(position.X + 20, position.Y - 4, 7, 7, funkenL, 0, 1, cannonL);
    draw(position.X + 30, position.Y - 5, 7, 7, funkenL, 0, 1, cannonL2);
    draw(position.X + 79, position.Y - 5, 7, 7, funkenR, 0, 1, cannonR);
    draw(position.X + 89, position.Y - 4, 7, 7, funkenR, 0, 1, cannonR2);

    drawOrigin(bfg.X, bfg.Y, 6, 10, funkenBig, Vector2(3, 1), bfgRot, 1, 1);

    draw(position.X + 39, position.Y + 23, 5, 5, funkenGun, 0, 1, turretLFrame);
    draw(position.X + 72, position.Y + 23, 5, 5, funkenGun, 0, 1, turretRFrame);
    return false;
  }
};

class Vuv : public NPC
{
  public:
  int ai = 0;
  int ai2 = 1;
  int ai3 = 0;
  Vuv(float posX, float posY) : NPC(posX, posY)
  {
    value = 22;
    lifeMax = 70;
    damage = 27;
    width = 10;
    height = 10; // 9
    texture = vuvspawn;
    hostile = true;
  }
  void AI() override
  {
    if (ai % 10 == 0)
    {
      if (ai2 == 1)
      {
        ai2 = 2;
      }
      else
      {
        ai2 = 1;
      }
    }
    Vector2 vec = playerCenter();
    if (ai >= 175 && ai < 225)
    {
      if (ai == 175)
      {
        ai3 = random(15, 114);
      }
      vec = Vector2(ai3, 10);
    }
    else if (ai >= 225)
    {
      ai = 0;
    }
    if (Distance(vec, Center()) > 5)
    {
      Vector2 maxSpeed = Normalize(vec.Subtract(Center())).Multiply(2.1);
      int increase = 60;
      velocity = velocity.Multiply(increase - 1).Add(maxSpeed).Divide(increase);
    }
    ai++;
  }
  void Kill() override
  {
    NewProjectile<Explosion>(Center().X, Center().Y, 0, 0, 0, false, false);
  }
  bool PreDraw() override
  {
    int texW = 16;
    int texH = 16;
    int posX = Center().X - (float)texW / 2.0;
    int posY = Center().Y - (float)texH / 2.0;
    draw(posX, posY, texW, texH, texture, 0, 1, ai2);
    return false;
  }
};
class VuVUlubu : public NPC
{
  public:
  int ai = -64;
  int ai2 = 1;
  int killTimer = 0;
  VuVUlubu(float posX, float posY) : NPC(posX, posY)
  {
    value = 2886;
    lifeMax = 9500;
    damage = 98;
    width = 98; // 11
    height = 20;
    texture = vuvu;
    hostile = true;
    position = Vector2(8, -32);
  }
  void AI() override
  {
    if (ai % 10 == 0)
    {
      if (ai2 == 1)
      {
        ai2 = 2;
      }
      else
      {
        ai2 = 1;
      }
    }
    if (ai < 0) // Intro
    {
      position = Vector2(8, ai * 0.5);
    }
    else if (ai <= 400)
    {
      position = Vector2(8, 0);
      if (ai % 200 > 125 && ai % 5 == 0)
      {
        float deg = random(-30, 31);
        Vector2 v = Vector2(0, 1.5).RotatedBy(deg);
        NewProjectile<VuVProj>(random(position.X, position.X + width), 0, v.X, v.Y, 19, false, true, deg, 1);
      }
    }
    else if (ai <= 600)
    {
      if (ai >= 450 && ai % 75 == 0)
      {
        NewNPC<Vuv>(random(position.X, position.X + width), 0);
      }
    }
    else
    {
      ai = 0;
    }
    ai++;
  }
  void Kill() override
  {
    NewProjectile<Explosion>(Center().X - width / 4, Center().Y, 0, 0, 0, false, false, 0, 2);
    NewProjectile<Explosion>(Center().X, Center().Y, 0, 0, 0, false, false, 0, 2);
    NewProjectile<Explosion>(Center().X + width / 4, Center().Y, 0, 0, 0, false, false, 0, 2);
  }
  bool PreDraw() override
  {
    draw(0, position.Y, 118, 32, texture, 0, 1, ai2);
    return false;
  }
  bool PreKill() override
  {
    PreDraw();
    damage = 0;
    if (killTimer >= 96)
    {
      return true;
    }
    if (killTimer % 12 == 0)
    {
      NewProjectile<Explosion>(position.X + random(0, width), position.Y + random(0, height), 0, 0, 0, false, false);
    }
    killTimer++;
    return false;
  }
};

class Tronica : public NPC
{
  public:
  int ai = -56;
  int ai2 = 1;
  int killTimer = 0;
  Vector2 vector = Vector2(0, 0);
  float rot = 0;
  Vector2 vel = Vector2(0, 0);
  Tronica(float posX, float posY) : NPC(posX, posY)
  {
    value = 2234;
    lifeMax = 7250;
    damage = 65;
    width = 94; // 11
    height = 19;
    texture = tronik;
    hostile = true;
    position = Vector2(17, -28);
  }
  void AI() override
  {
    if (ai % 10 == 0)
    {
      if (ai2 == 1)
      {
        ai2 = 2;
      }
      else
      {
        ai2 = 1;
      }
    }
    if (ai < 0) // Intro
    {
      position = Vector2(17, ai * 0.5);
    }
    else if (ai <= 303)
    {
      if (ai >= 197 && (ai % 50 == 47 || ai % 50 == 0 || ai % 50 == 3))
      {
        if (ai % 50 == 47)
        {
          vector = Vector2(playerCenter().Y - Center().Y, playerCenter().X - Center().X);
          rot = ToDegrees(atan2(-vector.Y, vector.X));
          vel = Vector2(0, 1.25).RotatedBy(rot);
        }
        NewProjectile<TroniWave>(Center().X, Center().Y, vel.X, vel.Y, 19, false, true, rot, 1);
      }
    }
    else if (ai <= 525)
    {
      if (ai >= 375 && ai % 75 == 0)
      {
        for (int i = 0; i < 3; i++)
        {
          rot = random(-60, 61);
          vel = Vector2(0, 1.25).RotatedBy(rot);
          NewProjectile<TroniWave>(Center().X, Center().Y, vel.X, vel.Y, 19, false, true, -rot, 1);
        }
      }
    }
    else if (ai <= 775)
    {
      if (ai > 625 && ai % 10 == 5)
      {
        NewProjectile<Rocket>(random(position.X, position.X + width), position.Y, 0, 0.75, 34, false, true, 180.0, 1);
      }
    }
    else if (ai == 900)
    {
      NewProjectile<TroniWaveBig>(Center().X, 6, 0, 0.44, 55, false, true, 0, 0.4);
    }
    else if (ai >= 1050)
    {
      ai = 0;
    }
    ai++;
  }
  void Kill() override
  {
    NewProjectile<Explosion>(Center().X - width / 4, Center().Y, 0, 0, 0, false, false, 0, 2);
    NewProjectile<Explosion>(Center().X, Center().Y, 0, 0, 0, false, false, 0, 2);
    NewProjectile<Explosion>(Center().X + width / 4, Center().Y, 0, 0, 0, false, false, 0, 2);
  }
  bool PreDraw() override
  {
    draw(14, position.Y, 100, 28, texture, 0, 1);
    draw(23, position.Y + 8, 15, 21, tronijet, 0, 1, ai2);
    draw(90, position.Y + 8, 15, 21, tronijet, 0, 1, ai2);
    return false;
  }
  bool PreKill() override
  {
    PreDraw();
    damage = 0;
    if (killTimer >= 96)
    {
      return true;
    }
    if (killTimer % 12 == 0)
    {
      NewProjectile<Explosion>(position.X + random(0, width), position.Y + random(0, height), 0, 0, 0, false, false);
    }
    killTimer++;
    return false;
  }
};

class EnemyBig2 : public NPC
{
  public:
  int ai;
  EnemyBig2(float posX, float posY) : NPC(posX, posY)
  {
    value = 332;
    lifeMax = 1450;
    damage = 20;
    width = 16; // 11
    height = 16;
    texture = enemyBig2;
    hostile = true;
    ai = 0;
  }
  void AI() override
  {
    float dist = Distance(Center(), playerCenter());
    if (dist > 70)
    {
      Vector2 maxSpeed = Normalize(playerCenter().Subtract(Center())).Multiply(0.75);
      float increase = 2.5;
      velocity = velocity.Multiply(increase - 1).Add(maxSpeed).Divide(increase);
    }
		else
		{
			velocity = velocity.Divide(1.1);
		}

    Vector2 vector(playerCenter().Y - (Center().Y + 11), playerCenter().X - Center().X);
    if (ai >= 90)
    {
      Vector2 velocity = Vector2(0, -3).RotatedBy(ToDegrees(atan2(-vector.Y, vector.X) + PI));
      NewProjectile<BM>(Center().X, Center().Y + 11, velocity.X, velocity.Y, 17, false, true, ToDegrees(atan2(-vector.Y, vector.X) + PI), 1);
      ai = 0;
    }
    if (ai < 90)
    {
      ai++;
    }
  }
  void Kill() override
  {
    NewProjectile<Explosion>(Center().X, Center().Y, 0, 0, 0, false, false, 0, 2);
  }
  bool PreDraw() override
  {
    int texW = 24;
    int texH = 21;
    int posX = Center().X - (float)texW / 2.0;
    //int posY = Center().Y - (float)texH / 2.0;
    draw(posX, position.Y, texW, texH, texture, rotation, scale);
    return false;
  }
};
class EnemyBig1 : public NPC
{
  public:
  int ai = 0;
  int killTimer = 0;
  EnemyBig1(float posX, float posY) : NPC(posX, posY)
  {
    value = 387;
    lifeMax = 1880;
    damage = 7;
    width = 17; // 11
    height = 17;
    texture = enemyBig;
    hostile = true;
  }
  bool PreKill() override
  {
    PreDraw();
    damage = 0;
    if (killTimer >= 30)
    {
      return true;
    }
    if (killTimer % 10 == 0)
    {
      NewProjectile<Explosion>(position.X + random(0, width), position.Y + random(0, height), 0, 0, 0, false, false, 0, 0.7);
    }
    killTimer++;
    return false;
  }
  void AI() override
  {
    float dist = Distance(Center(), playerCenter());
    if (dist > 75)
    {
      Vector2 maxSpeed = Normalize(playerCenter().Subtract(Center())).Multiply(1);
      float increase = 4;
      velocity = velocity.Multiply(increase - 1).Add(maxSpeed).Divide(increase);
    }
		else
		{
			velocity = velocity.Divide(1.01);
		}
    if (dist < 50)
    {
      Vector2 maxSpeed = Normalize(Center().Subtract(playerCenter())).Multiply(1 - (50 - dist) / 500);
      float increase = 4;
      velocity = velocity.Multiply(increase - 1).Add(maxSpeed).Divide(increase);
    }

    Vector2 vector(playerCenter().Y - Center().Y, playerCenter().X - Center().X);
    rotation = ToDegrees(atan2(-vector.Y, vector.X) + PI);
    /*if (random(480) == 0)
    {
      NewNPC<Enemy1>(Center().X, Center().Y);
    }*/
    if (ai >= 80)
    {
      Vector2 vel = Vector2(0, -3).RotatedBy(rotation);
      NewProjectile<BM>(Center().X, Center().Y, vel.X, vel.Y, 14, false, true, rotation, 1);
      ai = 0;
    }
    if (ai < 80)
    {
      ai++;
    }
  }
  void Kill() override
  {
    NewProjectile<Explosion>(Center().X, Center().Y, 0, 0, 0, false, false, 0, 2);
  }
  bool PreDraw() override
  {
    int texW = 17;
    int texH = 25;
    int posX = Center().X - (float)texW / 2.0;
    int posY = Center().Y - (float)texH / 2.0;
    draw(posX, posY, texW, texH, texture, rotation, scale);
    return false;
  }
};
#pragma endregion
#endif