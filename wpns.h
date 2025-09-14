#ifndef wpns
#define wpns
class Weapon
{
  public:
  //int cooldown;
  int level;
  String name;
  String desc;
  //int damage;
  bool autofire;
  int ai;
  int useAnimation = 0;
  int useAnim = 0;
  int charge2 = 0;
  int chargeFire = 0;
  int chargeMax = 0;
  int cd = 0;
  bool hold = false;
  Weapon(int lvl) : level(lvl) {}

  virtual int damage()
  {
    return 1;
  }
  virtual int cooldown()
  {
    return 1;
  }
  virtual float velocity()
  {
    return 1;
  }
  virtual void Shoot()
  {
  }
  virtual int cost()
  {
    return 0;
  }
};

class AntiAirCannon : public Weapon
{
  public: AntiAirCannon(int lvl) : Weapon(lvl)
  {
    name = "Canon antiaereo";
    desc = "El arma para aeronaves basica.";
  }
  int damage() override
  {
    if (level == 0)
    {
      return 14;
    }
    return (14.0 * (1.0 + (float)(level - 1) * (1.0 / 9.0)));
  }
  int cooldown() override
  {
    return 8;
  }
  float velocity() override
  {
    return -3;
  }
  int cost() override
  {
    if (level == 0)
    {
      return 1500;
    }
    else if (level == 10)
    {
      return 0;
    }
    return 500 + 25 * (level - 1);
  }
  void Shoot() override
  {
    NewProjectile<Bullet>(playerCenter().X, playerCenter().Y, 0, velocity(), damage(), true, false, 0, 1);
  }
};

class MachineGun : public Weapon
{
  public: MachineGun(int lvl) : Weapon(lvl)
  {
    name = "Ametralladora";
    desc = "Un par de torretas automaticas.";
    ai = random(2);
    autofire = true;
  }
  int damage() override
  {
    if (level == 0)
    {
      return 10;
    }
    return 10.0 * (1.0 + (float)(level - 1) * (1.0 / 9.0));
  }
  int cooldown() override
  {
    return 6;
  }
  float velocity() override
  {
    return -2.5;
  }
  int cost() override
  {
    if (level == 0)
    {
      return 1500;
    }
    else if (level == 10)
    {
      return 0;
    }
    return 500 + 25 * (level - 1);
  }
  void Shoot() override
  {
    if (ai == 0)
    {
      NewProjectile<BulletSmall>(playerCenter().X - 2, playerCenter().Y, 0, velocity(), damage(), true, false, 0, 1);
      ai = 1;
    }
    else
    {
      NewProjectile<BulletSmall>(playerCenter().X + 2, playerCenter().Y, 0, velocity(), damage(), true, false, 0, 1);
      ai = 0;
    }
  }
};

class Flamethrower : public Weapon
{
  public: Flamethrower(int lvl) : Weapon(lvl)
  {
    name = "Lanzallamas";
    desc = "Gran dano a corto alcance.";
    autofire = true;
  }
  int damage() override
  {
    if (level == 0)
    {
      return 11;
    }
    return 11 * (1.0 + (float)(level - 1) * (1.0 / 9.0));
  }
  int cooldown() override
  {
    return 3;
  }
  float velocity() override
  {
    return -2;
  }
  int cost() override
  {
    if (level == 0)
    {
      return 2500;
    }
    else if (level == 10)
    {
      return 0;
    }
    return 600 + 30 * (level - 1);
  }
  void Shoot() override
  {
    NewProjectile<Flames>(playerCenter().X, playerPosition.Y + 1, 0, velocity(), damage(), true, false, 0, 0.7);
  }
};
class RocketLauncher : public Weapon
{
  public: RocketLauncher(int lvl) : Weapon(lvl)
  {
    name = "Lanzacohetes";
    desc = "Un poderoso cohete con dano de area.";
  }
  int damage() override
  {
    if (level == 0)
    {
      return 240;
    }
    return 240.0 * (1.0 + (float)(level - 1) * (1.0 / 9.0));
  }
  int cooldown() override
  {
    return 100;
  }
  float velocity() override
  {
    return -0.4;
  }
  int cost() override
  {
    if (level == 0)
    {
      return 3500;
    }
    else if (level == 10)
    {
      return 0;
    }
    return 750 + 40 * (level - 1);
  }
  void Shoot() override
  {
    NewProjectile<Rocket>(playerCenter().X, playerCenter().Y, 0, velocity(), damage() * 0.75, true, false, 0, 1);
  }
};
class LaserCannon : public Weapon
{
  public: LaserCannon(int lvl) : Weapon(lvl)
  {
    name = "Canon laser";
    desc = "Dispara un potente laser continuo.";
    autofire = true;
  }
  int damage() override
  {
    if (level == 0)
    {
      return 12;
    }
    return 12.0 * (1.0 + (float)(level - 1.0) * (1.0 / 9.0));
  }
  int cooldown() override
  {
    return 1;
  }
  float velocity() override
  {
    return -64;
  }
  int cost() override
  {
    if (level == 0)
    {
      return 3500;
    }
    else if (level == 10)
    {
      return 0;
    }
    return 750 + 40 * (level - 1);
  }
  void Shoot() override
  {
    NewProjectile<Laser>(playerCenter().X, playerPosition.Y + 1, 0, 0, damage(), true, false, 0, 1);
  }
};
class MissilePod : public Weapon
{
  public: MissilePod(int lvl) : Weapon(lvl)
  {
    name = "Torreta de misiles";
    desc = "Lanza un bombardeo de pequenos cohetes aurodirigidos.";
    useAnimation = 25;
  }
  int damage() override
  {
    if (level == 0)
    {
      return 120;
    }
    return 120.0 * (1.0 + (float)(level - 1.0) * (1.0 / 9.0));
  }
  int cooldown() override
  {
    return 100;
  }
  float velocity() override
  {
    return -0.8;
  }
  int cost() override
  {
    if (level == 0)
    {
      return 4500;
    }
    else if (level == 10)
    {
      return 0;
    }
    return 900 + 55 * (level - 1);
  }
  void Shoot() override
  {
    if (useAnim == 25)
    {
      Vector2 vel = Vector2(0, -1).RotatedBy(-60);
      NewProjectile<RocketMini>(playerCenter().X, playerCenter().Y, vel.X, vel.Y, damage() * 0.25, true, false, 0, 1);
    }
    else if (useAnim == 17)
    {
      Vector2 vel = Vector2(0, -1).RotatedBy(60);
      NewProjectile<RocketMini>(playerCenter().X, playerCenter().Y, vel.X, vel.Y, damage() * 0.25, true, false, 0, 1);
    }
    else if (useAnim == 9)
    {
      Vector2 vel = Vector2(0, -1).RotatedBy(-105);
      NewProjectile<RocketMini>(playerCenter().X, playerCenter().Y, vel.X, vel.Y, damage() * 0.25, true, false, 0, 1);
    }
    else if (useAnim == 1)
    {
      Vector2 vel = Vector2(0, -1).RotatedBy(105);
      NewProjectile<RocketMini>(playerCenter().X, playerCenter().Y, vel.X, vel.Y, damage() * 0.25, true, false, 0, 1);
    }
  }
};
class PlasmaCannon : public Weapon
{
  public: PlasmaCannon(int lvl) : Weapon(lvl)
  {
    name = "Canon de plasma";
    desc = "Un arma capaz de eyectar poderosas esferas de plasma.";
  }
  int damage() override
  {
    if (level == 0)
    {
      return 93;
    }
    return 93.0 * (1.0 + (float)(level - 1.0) * (1.0 / 9.0));
  }
  int cooldown() override
  {
    return 33;
  }
  float velocity() override
  {
    return -2;
  }
  int cost() override
  {
    if (level == 0)
    {
      return 4500;
    }
    else if (level == 10)
    {
      return 0;
    }
    return 900 + 55 * (level - 1);
  }
  void Shoot() override
  {
    NewProjectile<Flames>(playerCenter().X, playerPosition.Y, 0, 0, 0, true, false);
    NewProjectile<Plasma>(playerCenter().X, playerCenter().Y, 0, velocity(), damage() * 0.83, true, false, 0, 0.5);
  }
};
class StormRifle : public Weapon
{
  public: StormRifle(int lvl) : Weapon(lvl)
  {
    name = "Rifle tormenta";
    desc = "Dispara potentes rayos de energia a gran velocidad.";
  }
  int damage() override
  {
    if (level == 0)
    {
      return 28;
    }
    return 28.0 * (1.0 + (float)(level - 1) * (1.0 / 9.0));
  }
  int cooldown() override
  {
    return 5;
  }
  float velocity() override
  {
    return -6;
  }
  int cost() override
  {
    if (level == 0)
    {
      return 6500;
    }
    else if (level == 10)
    {
      return 0;
    }
    return 1000 + 75 * (level - 1);
  }
  void Shoot() override
  {
    NewProjectile<Flames>(playerCenter().X, playerPosition.Y, 0, 0, 0, false, false, 0, 0.2);
    NewProjectile<BM>(playerCenter().X, playerCenter().Y, 0, velocity(), damage(), true, false, 0, 1);
  }
};
class Railgun : public Weapon
{
  public: Railgun(int lvl) : Weapon(lvl)
  {
    name = "Canon de riel";
    desc = "Carga un poderoso rayo capaz de devastar todo en su camino.";
    chargeFire = 43;
    chargeMax = 50;
  }
  int damage() override
  {
    if (level == 0)
    {
      return 720;
    }
    return 720.0 * (1.0 + (float)(level - 1.0) * (1.0 / 9.0));
  }
  int cooldown() override
  {
    return 150;
  }
  float velocity() override
  {
    return -64;
  }
  int cost() override
  {
    if (level == 0)
    {
      return 10000;
    }
    else if (level == 10)
    {
      return 0;
    }
    return 2500 + 125 * (level - 1);
  }
  void Shoot() override
  {
    if (charge2 == 43)
    {
      NewProjectile<Flames>(playerCenter().X, playerPosition.Y, 0, 0, 0, false, false, 0, 0.75);
      NewProjectile<Laser>(playerCenter().X, playerPosition.Y + 1, 0, 0, 0, true, false, 0, 1);
    }
    else if (charge2 == 44 || charge2 > 48)
    {
      NewProjectile<Laser>(playerCenter().X, playerPosition.Y + 1, 0, 0, 0, true, false, 0, 1);
    }
    else if (charge2 > 44)
    {
      NewProjectile<Rail>(playerCenter().X, playerPosition.Y + 1, 0, 0, damage(), true, false, 0, 1);
    }
    /*else
    {
      NewProjectile<Flames>(playerCenter().X, playerPosition.Y, 0, 0, 0, false, false, 0, 0.2 + charge2 / 200);
    }*/
  }
};
#endif