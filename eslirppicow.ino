#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include "utils.h"

#include "vector2.h"
#include "graphics.h"
#include "helper.h"
#include "content.h"
#include "textures.h"
#include "wpns.h"

#define SCREEN_ADDRESS 0x3C
#define EEPROM_SIZE 64
#define MONEY_ADDRESS 60
#define WPN1_CUR_ADDRESS 4
#define WPN2_CUR_ADDRESS 8
int LVL_ADDRESSES[9] = {12, 16, 20, 24, 28, 32, 36, 40, 44};
int LVL2_ADDRESSES[3] = {48, 52, 56};
//int LVL_ADDRESSES[9] = {13, 14, 15, 16, 17, 18, 19, 20, 21};

#define button 14
#define button2 15
#define joystickX A0
#define joystickY A2

#pragma region Initialization
// De los controles
bool useButton = false;
bool useButton2 = false;
int joyDelay = 0;

// De la navegación entre menús
int intro;
int mode = 0;
bool load = false;
int option = 0;
int option2 = 0;

int timer = 0;
int timer2 = 0;
int disparos = 0;
int dEnemies = 0;
int dBosses = 0;
int namePos = 0;
char charPos = 65;
char newName[5] = "    ";

int playerLifeMax = 100;
int playerLife = 100;
int lifeRegen = 250;
int lifeRegen2 = 50;
int regenTimer = 0;
float playerVel = 1;

int lvls2[3] = {1, 1, 1};

int lvCost(int lv)
{
  return 2500 + 1250 * (lv - 1);
}

int pts = 0;
int money = 0;
//int wpnAct[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
int lvls[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
Weapon* weapons[9] = {};
int playerWeapon[2] = {0, 1};
int wpn = 0;
#pragma endregion

//Projectile* proj[100];
//NPC* npc[24];

void setup()
{
  display.begin(SCREEN_ADDRESS, true);
  display.clearDisplay();
  display.setTextColor(1);
  draw(10, 16, 30, 32, eslabs);
  write(46, 16, 3, F("ESLI"));
  write(46, 40, 1, F("Laboratorios"));
  display.display();

  bool flag = false;
  Serial.begin(9600);
  EEPROM.begin(EEPROM_SIZE);

  pinMode(2, OUTPUT);
  pinMode(7, OUTPUT);
  
  configWifi();
  initWifi();

  delay(1000);
  /*for (int i = 0; i < EEPROM.length(); i += 4)
  {
    EEPROM.write(i, 0);
  }*/

  #pragma region EEPROM
    EEPROM.get(MONEY_ADDRESS, money);
    if (money < 0)
    {
      money = 0;
      EEPROM.put(MONEY_ADDRESS, money);
      flag = true;
    }
    for (int i = 0; i < 3; i++)
    {
      EEPROM.get(LVL2_ADDRESSES[i], lvls2[i]);
      if (lvls2[i] < 1 || lvls2[i] > 6)
      {
        lvls2[i] = 1;
        EEPROM.put(LVL2_ADDRESSES[i], lvls2[i]);
        flag = true;
      }
    }

    EEPROM.get(WPN1_CUR_ADDRESS, playerWeapon[0]);
    if (playerWeapon[0] < 0 || playerWeapon[0] > 8)
    {
      playerWeapon[0] = 0;
      EEPROM.put(WPN1_CUR_ADDRESS, playerWeapon[0]);
      flag = true;
    }
    EEPROM.get(WPN2_CUR_ADDRESS, playerWeapon[1]);
    if (playerWeapon[1] < 0 || playerWeapon[1] > 8)
    {
      playerWeapon[1] = 1;
      EEPROM.put(WPN2_CUR_ADDRESS, playerWeapon[1]);
      flag = true;
    }

    for (int i = 0; i < 9; i++)
    {
      EEPROM.get(LVL_ADDRESSES[i], lvls[i]);
      if (i <= 1)
      {
        if (lvls[i] < 1 || lvls[i] > 10)
        {
          lvls[i] = 1;
          EEPROM.put(LVL_ADDRESSES[i], lvls[i]);
          flag = true;
        }
      }
      else
      {
        if (lvls[i] < 0 || lvls[i] > 10)
        {
          lvls[i] = 0;
          EEPROM.put(LVL_ADDRESSES[i], lvls[i]);
          flag = true;
        }
      }
    }
    if (flag)
    {
      EEPROM.commit();
    }
  #pragma endregion

  weapons[0] = new AntiAirCannon(lvls[0]);
  weapons[1] = new MachineGun(lvls[1]);
  weapons[2] = new Flamethrower(lvls[2]);
  weapons[3] = new RocketLauncher(lvls[3]);
  weapons[4] = new LaserCannon(lvls[4]);
  weapons[5] = new MissilePod(lvls[5]);
  weapons[6] = new PlasmaCannon(lvls[6]);
  weapons[7] = new StormRifle(lvls[7]);
  weapons[8] = new Railgun(lvls[8]);

  pinMode(button, INPUT);
  pinMode(joystickX, INPUT);
  pinMode(joystickY, INPUT);

  delay(3000);
  digitalWrite(2, LOW);
  digitalWrite(7, LOW);
  display.clearDisplay();
}

void loop()
{
  if (!digitalRead(button))
  {
    useButton = false;
  }
  if (!digitalRead(button2))
  {
    useButton2 = false;
  }
  if (mode == 0)
  {
    if (!load)
    {
      draw(23, 37 + option * 10, 3, 5, arrow);
      write(49, 36, 1, F("Jugar"));
      write(43, 46, 1, F("Armeria"));
      write(28, 56, 1, F("Personalizar"));
      display.display();
      load = true;
    }
    float titleRot = sin(millis() / 33 * PI / 180.0) * 7.5;
    float titleScale = 1.0 + sin(millis() / 50 * PI / 180.0) * 0.2;
    clear(0, 0, 128, 36);
    draw(12, 12, 96, 15, ESLI, titleRot, titleScale);
    // Seleccionar opción
    if (digitalRead(button) && !useButton)
    {
      mode = option + 1;
      option = 0;
      option2 = 0;
      load = false;
      display.clearDisplay();
      useButton = true;
    }
    if (joyDelay <= 0)
    {
      if (analogRead(joystickY) < 256 && option < 2)
      {
        clear(23, 37 + option * 10, 3, 5);
        option++;
        draw(23, 37 + option * 10, 3, 5, arrow);
        joyDelay = 6;
      }
      else if (analogRead(joystickY) >= 768 && option > 0)
      {
        clear(23, 37 + option * 10, 3, 5);
        option--;
        draw(23, 37 + option * 10, 3, 5, arrow);
        joyDelay = 6;
      }
    }
    else
    {
      joyDelay--;
    }
    delay(20);
    display.display();
  }
  else if (mode == 1)
  {
    if (!load)
    {
      StartPlayer();
      pts = 0;
      timer = 0;
      timer2 = 0;
      disparos = 0;
      dEnemies = 0;
      dBosses = 0;
      playerPosition.X = 59;
      playerPosition.Y = 48;
      draw(playerPosition.X - 1, playerPosition.Y - 2, 9, 13, plyr);
      //NewNPC<LordFunken>(20, 20);
      //NewNPC<VuVUlubu>(108, 20);
      //NewNPC<Tronica>(20, 20);
      load = true;
    }

    clearScreen();

    UpdateProjectile();
    UpdateNPC();

    #pragma region Player
      #pragma region Movement
      playerVelocity.X = 0;
      playerVelocity.Y = 0;
      if (analogRead(joystickX) < 256 && playerPosition.X > -2)
      {
        playerVelocity.X = -playerVel;
        if (playerRotation > -18)
        {
          playerRotation -= playerVel;
        }
      }
      else if (analogRead(joystickX) >= 768 && playerPosition.X < 123)
      {
        playerVelocity.X = playerVel;
        if (playerRotation < 18)
        {
          playerRotation += playerVel;
        }
      }
      else
      {
        if (playerRotation > 0)
        {
          playerRotation -= playerVel;
        }
        else if (playerRotation < 0)
        {
          playerRotation += playerVel;
        }
      }
      if (analogRead(joystickY) >= 768 && playerPosition.Y > -3)
      {
        playerVelocity.Y = -playerVel;
      }
      else if (analogRead(joystickY) < 256 && playerPosition.Y < 57)
      {
        playerVelocity.Y = playerVel;
      }

      if (playerVelocity.X != 0 || playerVelocity.Y != 0)
      {
        if (playerVelocity.X != 0 && playerVelocity.Y != 0)
        {
          playerVelocity.X *= 0.7071;
          playerVelocity.Y *= 0.7071;
        }
        playerPosition.X += playerVelocity.X;
        playerPosition.Y += playerVelocity.Y;
      }

      if (oldPos.X != playerPosition.X || oldPos.Y != playerPosition.Y || oldRot != playerRotation)
      {
        oldRot = playerRotation;
        oldPos = playerPosition;
      }
      #pragma endregion

    if (regenTimer > 0)
    {
      regenTimer--;
    }
    if (playerLife < playerLifeMax && regenTimer == 0)
    {
      playerLife++;
      regenTimer = lifeRegen2;
    }

    if (playerImmune > 0)
    {
      playerImmune--;
    }
    if (playerImmune % 6 < 3)
    {
      draw(playerPosition.X - 1, playerPosition.Y - 2, 9, 13, plyr, playerRotation);
    }

    Weapon* curWpn = weapons[playerWeapon[wpn]];
    if ((digitalRead(button) && curWpn->cd <= 0 && !useButton) ||
    (!curWpn->hold && curWpn->charge2 >= curWpn->chargeFire && curWpn->charge2 < curWpn->chargeMax)) // Disparar
    {
      if (curWpn->useAnimation > 0)
      {
        curWpn->useAnim = curWpn->useAnimation;
      }
      if (curWpn->chargeMax == 0)
      {
        curWpn->cd = curWpn->cooldown();
        useButton = !curWpn->autofire;
        curWpn->Shoot();
        disparos++;
      }
      else // Channel
      {
        if (curWpn->charge2 < curWpn->chargeFire)
        {
          curWpn->charge2++;
        }
        else if (!curWpn->hold)
        {
          curWpn->Shoot();
          if (curWpn->charge2 < curWpn->chargeMax)
          {
            curWpn->charge2++;
          }
          else
          {
            curWpn->cd = curWpn->cooldown();
            useButton = !curWpn->autofire;
            curWpn->charge2 = 0;
          }
        }
        else
        {
          curWpn->Shoot();
        }
      }
    }
    else
    {
      if (curWpn->charge2 > curWpn->chargeFire)
      {
        curWpn->cd = curWpn->cooldown();
        curWpn->charge2 = 0;
      }
      else if (curWpn->charge2 < curWpn->chargeMax)
      {
        curWpn->charge2 = 0;
      }

      if (curWpn->useAnim > 0)
      {
        curWpn->Shoot();
        curWpn->useAnim--;
      }
      if (curWpn->cd > 0)
      {
        curWpn->cd--;
      }
    }
    if (weapons[playerWeapon[!wpn]]->chargeFire > 0 && weapons[playerWeapon[!wpn]]->charge2 > weapons[playerWeapon[!wpn]]->chargeFire)
    {
      weapons[playerWeapon[!wpn]]->cd = weapons[playerWeapon[!wpn]]->cooldown();
    }
    weapons[playerWeapon[!wpn]]->charge2 = 0;
    weapons[playerWeapon[!wpn]]->useAnim = 0;
    if (weapons[playerWeapon[!wpn]]->cd > 0)
    {
      weapons[playerWeapon[!wpn]]->cd--;
    }
    if (digitalRead(button2) && !useButton2)
    {
      //if (playerWeapon[wpn] != playerWeapon[!wpn] && playerWeapon[wpn] != -1)
      //{
        wpn = !wpn;
      //}
      useButton2 = true;
    }
    #pragma endregion
    
    clear(104, 0, 25, 17);
    if (playerLife < 10)
    {
      write(116, 0, 1, (String)playerLife + "%");
    }
    else if (playerLife < 100)
    {
      write(110, 0, 1, (String)playerLife + "%");
    }
    else
    {
      write(104, 0, 1, (String)playerLife + "%");
    }

    Weapon* wpn1 = weapons[playerWeapon[0]];
    if (wpn1->cooldown() > 5)
    {
      display.drawRect(105, 8, 23, 3, 1);
      float cd = 22.0 * ((float)wpn1->cooldown() - (float)wpn1->cd) / (float)wpn1->cooldown();
      if (wpn1->chargeMax > 0)
      {
        if (wpn1->charge2 == 0)
        {
          cd = 22.0 * (float)wpn1->cd / (float)wpn1->cooldown();
        }
        else
        {
          if (wpn1->charge2 < wpn1->chargeFire)
          {
            cd = 22.0 * (float)wpn1->charge2 / (float)wpn1->chargeFire;
          }
          else
          {
            cd = 22.0;
          }
        }
      }
      display.drawLine(105, 9, 105 + cd, 9, 1);
    }
    else
    {
      display.fillRect(105, 8, 23, 3, 1);
    }

    Weapon* wpn2 = weapons[playerWeapon[1]];
    if (wpn2->cooldown() > 5)
    {
      display.drawRect(105, 12, 23, 3, 1);
      float cd2 = 22.0 * ((float)wpn2->cooldown() - (float)wpn2->cd) / (float)wpn2->cooldown();
      if (wpn2->chargeMax > 0)
      {
        if (wpn2->charge2 == 0)
        {
          cd2 = 22.0 * (float)wpn2->cd / (float)wpn2->cooldown();
        }
        else
        {
          if (wpn2->charge2 < wpn2->chargeFire)
          {
            cd2 = 22.0 * (float)wpn2->charge2 / (float)wpn2->chargeFire;
          }
          else
          {
            cd2 = 22.0;
          }
        }
      }
      display.drawLine(105, 13, 105 + cd2, 13, 1);
    }
    else
    {
      display.fillRect(105, 12, 23, 3, 1);
    }

    #pragma region Spawns
    if (timer2 < 1500)
    {
      if (timer2 == 150 || timer2 == 375 || timer2 == 600 || timer2 == 900 || timer2 == 1125 || timer2 == 1350)
      {
        NewNPC<Enemy1>(random(0, 128), -16);
      }
      timer2++;
    }
    else if (timer2 == 1500)
    {
      if (enemiesActive == 0)
      {
        timer2++;
      }
    }
    else if (timer2 < 1900)
    {
      if (timer2 % 100 == 0)
      {
        NewNPC<Enemy1>(random(0, 128), -16);
      }
      if (timer2 == 1899)
      {
        NewNPC<Enemy3>(random(0, 128), -16);
      }
      timer2++;
    }
    else if (timer2 == 1900)
    {
      if (enemiesActive == 0)
      {
        timer2++;
      }
    }
    else if (timer2 < 2300)
    {
      if (timer2 % 100 == 0)
      {
        NewNPC<Enemy1>(random(0, 128), -16);
      }
      if (timer2 == 2299)
      {
        NewNPC<Enemy3>(random(0, 128), -16);
      }
      timer2++;
    }
    else if (timer2 == 2300)
    {
      if (enemiesActive == 0)
      {
        timer2++;
      }
    }
    else if (timer2 < 2600)
    {
      if (timer2 % 100 == 0)
      {
        NewNPC<Enemy1>(random(0, 128), -16);
      }
      if (timer2 == 2599)
      {
        NewNPC<Enemy3>(random(0, 128), -16);
      }
      timer2++;
    }
    else if (timer2 == 2600)
    {
      if (enemiesActive == 0)
      {
        timer2++;
      }
    }
    else if (timer2 < 2900)
    {
      if (timer2 == 2799)
      {
        NewNPC<Enemy3>(random(0, 128), -16);
      }
      if (timer2 == 2899)
      {
        NewNPC<Enemy3>(random(0, 128), -16);
      }
      timer2++;
    }
    else if (timer2 == 2900)
    {
      if (enemiesActive <= 1)
      {
        timer2++;
      }
    }
    else if (timer2 < 3000)
    {
      if (timer2 == 2999)
      {
        NewNPC<Enemy3>(random(0, 128), -16);
      }
      timer2++;
    }
    else if (timer2 == 3000)
    {
      if (enemiesActive <= 0)
      {
        timer2++;
      }
    }
    else if (timer2 < 3150)
    {
      if (timer2 == 3149)
      {
        NewNPC<Enemy2>(random(0, 128), -16);
      }
      timer2++;
    }
    else if (timer2 == 3150)
    {
      if (enemiesActive <= 0)
      {
        timer2++;
      }
    }
    else if (timer2 < 3300)
    {
      if (timer2 == 3250)
      {
        NewNPC<Enemy2>(random(0, 128), -16);
        NewNPC<Enemy3>(random(0, 128), -16);
      }
      else if (timer2 == 3299)
      {
        NewNPC<Enemy1>(random(0, 128), -16);
      }
      timer2++;
    }
    else if (timer2 == 3300)
    {
      if (enemiesActive <= 0)
      {
        timer2++;
      }
    }
    else if (timer2 < 3350)
    {
      if (timer2 == 3349)
      {
        NewNPC<LordFunken>(64, -61);
      }
      timer2++;
    }
    else if (timer2 == 3350)
    {
      if (enemiesActive <= 2)
      {
        if (timer % 750 == 500)
        {
          NewNPC<Enemy1>(-16 + random(2) * 160, random(-16, 17));
        }
      }
      if (enemiesActive <= 0)
      {
        timer2++;
      }
    }
    else if (timer2 < 3450)
    {
      if (timer2 == 3400)
      {
        NewNPC<Enemy1>(random(0, 128), -16);
        NewNPC<Enemy1>(random(0, 128), -16);
      }
      else if (timer2 == 3449)
      {
        NewNPC<Enemy2>(random(0, 128), -16);
      }
      timer2++;
    }
    else if (timer2 == 3450)
    {
      if (enemiesActive <= 0)
      {
        timer2++;
      }
    }
    else if (timer2 < 3500)
    {
      if (timer2 == 3499)
      {
        NewNPC<EnemyBig1>(64, -16);
      }
      timer2++;
    }
    else if (timer2 == 3500)
    {
      if (enemiesActive <= 3)
      {
        if (timer % 150 == 75 && random(3) == 0)
        {
          NewNPC<Enemy1>(random(0, 128), -16);
        }
      }
      if (enemiesActive <= 0)
      {
        timer2++;
      }
    }
    else if (timer2 < 3950)
    {
      if (timer2 < 3949 && timer2 % 75 == 50)
      {
        switch (random(3))
        {
          case 0:
          case 1:
            NewNPC<Enemy1>(random(0, 128), -16);
            break;
          default:
            NewNPC<Enemy3>(random(0, 128), -16);
            break;
        }
      }
      else if (timer2 == 3949)
      {
        NewNPC<Enemy2>(random(0, 128), -16);
      }
      timer2++;
    }
    else if (timer2 == 3950)
    {
      if (enemiesActive <= 1)
      {
        timer2++;
      }
    }
    else if (timer2 < 4050)
    {
      if (timer2 == 4049)
      {
        NewNPC<EnemyBig1>(64, -16);
      }
      timer2++;
    }
    else if (timer2 == 4050)
    {
      if (enemiesActive <= 3)
      {
        if (timer % 150 == 75 && random(3) == 0)
        {
          NewNPC<Enemy1>(random(0, 128), -16);
        }
      }
      if (enemiesActive <= 0)
      {
        timer2++;
      }
    }
    else if (timer2 < 4100)
    {
      if (timer2 == 4099)
      {
        NewNPC<VuVUlubu>(8, -32);
      }
      timer2++;
    }
    else if (timer2 == 4100)
    {
      if (enemiesActive <= 3)
      {
        if (timer % 750 == 500)
        {
          switch (random(3))
          {
            case 0:
            case 1:
              NewNPC<Enemy1>(random(0, 128), -16);
              break;
            default:
              NewNPC<Enemy3>(random(0, 128), -16);
              break;
          }
        }
      }
      if (enemiesActive <= 0)
      {
        timer2++;
      }
    }
    else if (timer2 < 4250)
    {
      if (timer2 == 4150)
      {
        NewNPC<EnemyBig1>(64, -16);
      }
      else if (timer2 == 4200)
      {
        NewNPC<Enemy3>(random(0, 128), -16);
      }
      else if (timer2 == 4249)
      {
        NewNPC<Enemy1>(random(0, 128), -16);
      }
      timer2++;
    }
    else if (timer2 == 4250)
    {
      if (enemiesActive <= 0)
      {
        timer2++;
      }
    }
    else if (timer2 < 4550)
    {
      if (timer2 == 4350)
      {
        NewNPC<Enemy2>(random(0, 128), -16);
      }
      else if (timer2 == 4400)
      {
        NewNPC<Enemy3>(random(0, 128), -16);
      }
      else if (timer2 == 4450)
      {
        NewNPC<Enemy3>(random(0, 128), -16);
      }
      else if (timer2 == 4549)
      {
        NewNPC<Enemy3>(random(0, 128), -16);
      }
      timer2++;
    }
    else if (timer2 == 4550)
    {
      if (enemiesActive <= 1)
      {
        timer2++;
      }
    }
    else if (timer2 < 4700)
    {
      if (timer2 == 4699)
      {
        NewNPC<EnemyBig2>(16, -16);
        NewNPC<EnemyBig2>(112, -16);
      }
      timer2++;
    }
    else if (timer2 == 4700)
    {
      if (enemiesActive <= 3)
      {
        if (timer % 200 == 100 && random(3) == 0)
        {
          switch (random(3))
          {
            case 0:
            case 1:
              NewNPC<Enemy1>(random(0, 128), -16);
              break;
            default:
              NewNPC<Enemy3>(random(0, 128), -16);
              break;
          }
        }
      }
      if (enemiesActive <= 0)
      {
        timer2++;
      }
    }
    else if (timer2 < 4950)
    {
      if (timer2 == 4800)
      {
        NewNPC<Enemy2>(random(0, 128), -16);
      }
      else if (timer2 == 4875)
      {
        NewNPC<Enemy2>(random(0, 128), -16);
      }
      else if (timer2 == 4949)
      {
        NewNPC<Enemy2>(random(0, 128), -16);
      }
      timer2++;
    }
    else if (timer2 == 4950)
    {
      if (enemiesActive <= 0)
      {
        timer2++;
      }
    }
    else if (timer2 < 5150)
    {
      if (timer2 == 5050)
      {
        NewNPC<EnemyBig2>(16, -16);
        NewNPC<EnemyBig2>(112, -16);
      }
      else if (timer2 == 5149)
      {
        NewNPC<Enemy2>(random(0, 128), -16);
      }
      timer2++;
    }
    else if (timer2 == 5150)
    {
      if (enemiesActive <= 3)
      {
        if (timer % 200 == 100 && random(3) == 0)
        {
          switch (random(3))
          {
            case 0:
            case 1:
              NewNPC<Enemy1>(random(0, 128), -16);
              break;
            default:
              NewNPC<Enemy3>(random(0, 128), -16);
              break;
          }
        }
      }
      if (enemiesActive <= 0)
      {
        timer2++;
      }
    }
    else if (timer2 < 5200)
    {
      if (timer2 == 5199)
      {
        NewNPC<Tronica>(17, -28);
      }
      timer2++;
    }
    else if (timer2 == 5200)
    {
      if (enemiesActive <= 3)
      {
        if (timer % 1000 == 700)
        {
          switch (random(3))
          {
            case 0:
            case 1:
              NewNPC<Enemy1>(random(0, 128), -16);
              break;
            default:
              NewNPC<Enemy3>(random(0, 128), -16);
              break;
          }
        }
      }
      if (enemiesActive <= 4)
      {
        if (timer % 2500 == 1500)
        {
          NewNPC<EnemyBig2>(112, -16);
        }
      }
      if (enemiesActive <= 0)
      {
        timer2++;
      }
    }
    else
    {
      // Infinito
      if (timer2 < 6700)
      {
        if (enemiesActive < 6)
        {
          if (timer % 60 == 0 && random(4) == 0)
          {
            NewNPC<Enemy1>(random(0, 128), -16);
          }
          if (timer % 90 == 0 && random(6) == 0)
          {
            NewNPC<Enemy3>(random(0, 128), -16);
          }
          timer2++;
        }
      }
      else if (timer2 < 8200)
      {
        if (enemiesActive < 6)
        {
          if (timer % 120 == 0 && random(6) == 0)
          {
            NewNPC<Enemy1>(random(0, 128), -16);
          }
          if (timer % 90 == 0 && random(5) == 0)
          {
            NewNPC<Enemy3>(random(0, 128), -16);
          }
          if (timer % 180 == 0 && random(7) == 0)
          {
            NewNPC<Enemy2>(random(0, 128), -16);
          }
          if (timer % 120 == 0 && random(7) == 0)
          {
            NewNPC<EnemyBig1>(64, -16);
          }
          timer2++;
        }
      }
      else if (timer2 < 9700)
      {
        if (enemiesActive < 7)
        {
          if (timer % 130 == 0 && random(5) == 0)
          {
            NewNPC<Enemy1>(random(0, 128), -16);
          }
          if (timer % 90 == 0 && random(6) == 0)
          {
            NewNPC<Enemy3>(random(0, 128), -16);
          }
          if (timer % 190 == 0 && random(6) == 0)
          {
            NewNPC<Enemy2>(random(0, 128), -16);
          }
          if (timer % 470 == 0 && random(7) == 0)
          {
            NewNPC<EnemyBig1>(64, -16);
          }
          if (timer % 770 == 0 && random(6) == 0)
          {
            NewNPC<EnemyBig2>(16, -16);
            NewNPC<EnemyBig2>(112, -16);
          }
          timer2++;
        }
      }
      else
      {
        if (enemiesActive < 8)
        {
          if (timer % 110 == 0 && random(5) == 0)
          {
            NewNPC<Enemy1>(random(0, 128), -16);
          }
          if (timer % 190 == 0 && random(5) == 0)
          {
            NewNPC<Enemy3>(random(0, 128), -16);
          }
          if (timer % 240 == 0 && random(5) == 0)
          {
            NewNPC<Enemy2>(random(0, 128), -16);
          }
          if (timer % 430 == 0 && random(6) == 0)
          {
            NewNPC<EnemyBig1>(random(0, 128), -16);
          }
          if (timer % 740 == 0 && random(6) == 0)
          {
            NewNPC<EnemyBig2>(16, -16);
            NewNPC<EnemyBig2>(112, -16);
          }
        }
      }
    }
    #pragma endregion

    timer++;
    if (timer % 10 == 0)
    {
      pts++;
    }
    display.display();
    delay(20);
  }
  else if (mode == 2)
  {
    if (!load)
    {
      ArmoryGun();
      load = true;
    }

    if (joyDelay <= 0)
    {
      if (analogRead(joystickX) < 256)
      {
        option--;
        if (option < 0)
        {
          option = 8;
        }
        ArmoryGun();
        joyDelay = 200;
      }
      else if (analogRead(joystickX) >= 768)
      {
        option++;
        if (option > 8)
        {
          option = 0;
        }
        ArmoryGun();
        joyDelay = 200;
      }
    }
    else
    {
      joyDelay--;
    }

    if (digitalRead(button) && !useButton)
    {
      if (weapons[option]->level < 10 && money >= weapons[option]->cost())
      {
        money -= weapons[option]->cost();
        lvls[option]++;
        weapons[option]->level++;
        ArmoryGun();
      }
      useButton = true;
      delay(200);
    }

    if (digitalRead(button2) && !useButton2)
    {
      Save();
      mode = 0;
      option = 0;
      option2 = 0;
      load = false;
      display.clearDisplay();
      useButton2 = true;
    }
    delay(1);
  }
  else if (mode == 3)
  {
    if (!load)
    {
      CustomizeLoad();
      load = true;
    }

    if (joyDelay <= 0)
    {
      if (analogRead(joystickX) < 256)
      {
        if (option2 == 0)
        {
          if (option == 0)
          {
            option = 1;
          }
          else
          {
            option = 0;
          }
          joyDelay = 200;
        }
        else if (option == 0)
        {
          if (option2 == 1)
          {
            do
            {
              playerWeapon[0]--;
              if (playerWeapon[0] < 0)
              {
                playerWeapon[0] = 8;
              }
            }
            while (weapons[playerWeapon[0]]->level == 0 || playerWeapon[0] == playerWeapon[1]);
          }
          else if (option2 == 2)
          {
            do
            {
              playerWeapon[1]--;
              if (playerWeapon[1] < 0)
              {
                playerWeapon[1] = 8;
              }
            }
            while (weapons[playerWeapon[1]]->level == 0 || playerWeapon[0] == playerWeapon[1]);
          }
        }
        CustomizeLoad();
        joyDelay = 200;
      }
      else if (analogRead(joystickX) >= 768)
      {
        if (option2 == 0)
        {
          if (option == 0)
          {
            option = 1;
          }
          else
          {
            option = 0;
          }
          joyDelay = 200;
        }
        else if (option == 0)
        {
          if (option2 == 1)
          {
            do
            {
              playerWeapon[0]++;
              if (playerWeapon[0] > 8)
              {
                playerWeapon[0] = 0;
              }
            }
            while (weapons[playerWeapon[0]]->level == 0 || playerWeapon[0] == playerWeapon[1]);
          }
          else if (option2 == 2)
          {
            do
            {
              playerWeapon[1]++;
              if (playerWeapon[1] > 8)
              {
                playerWeapon[1] = 0;
              }
            }
            while (weapons[playerWeapon[1]]->level == 0 || playerWeapon[0] == playerWeapon[1]);
          }
        }
        CustomizeLoad();
        joyDelay += 200;
      }
      else if (analogRead(joystickY) < 256)
      {
        if (option == 0)
        {
          if (option2 < 2)
          {
            option2++;
          }
        }
        else
        {
          if (option2 < 3)
          {
            option2++;
          }
        }
        CustomizeLoad();
        joyDelay += 200;
      }
      else if (analogRead(joystickY) >= 768)
      {
        if (option2 > 0)
        {
          option2--;
        }
        CustomizeLoad();
        joyDelay = 200;
      }
    }
    else
    {
      joyDelay--;
    }

    if (digitalRead(button) && !useButton)
    {
      if (lvls2[option2 - 1] < 6 && money >= lvCost(lvls2[option2 - 1]))
      {
        money -= lvCost(lvls2[option2 - 1]);
        lvls2[option2 - 1]++;
        CustomizeLoad();
      }
      useButton = true;
      delay(200);
    }

    if (digitalRead(button2) && !useButton2)
    {
      Save();
      mode = 0;
      option = 0;
      option2 = 0;
      load = false;
      display.clearDisplay();
      useButton2 = true;
    }
    delay(1);
  }
  else if (mode == 4)
  {
    if (!load)
    {
      display.clearDisplay();
      write(19, 2, 1, F("Juego terminado"));
      write(22, 16, 1, F("Tu puntuacion:"));
      if (pts < 10)
      {
        write(58, 26, 2, (String)pts);
      }
      else if (pts < 100)
      {
        write(52, 26, 2, (String)pts);
      }
      else if (pts < 1000)
      {
        write(46, 26, 2, (String)pts);
      }
      else if (pts < 10000)
      {
        write(40, 26, 2, (String)pts);
      }
      else if (pts < 100000)
      {
        write(34, 26, 2, (String)pts);
      }
      else if (pts < 1000000)
      {
        write(28, 26, 2, (String)pts);
      }
      else
      {
        write(22, 26, 2, (String)pts);
      }
      for (int i = 0; i < 4; i++)
      {
        draw(52 + i * 6, 45, 5, 3, arrU);
        draw(52 + i * 6, 57, 5, 3, arrD);
      }
      write(52, 49, 1, "A");
      display.display();
      load = true;
    }

    if (joyDelay <= 0)
    {
      if (analogRead(joystickY) < 256)
      {
        charPos--;
        if (charPos < 65)
        {
          charPos = 90;
        }
        clear(52 + namePos * 6, 49, 24, 8);
        write(52 + namePos * 6, 49, 1, (String)charPos);
        display.display();
        joyDelay = 5;
      }
      else if (analogRead(joystickY) >= 768)
      {
        charPos++;
        if (charPos > 90)
        {
          charPos = 65;
        }
        clear(52 + namePos * 6, 49, 24, 8);
        write(52 + namePos * 6, 49, 1, (String)charPos);
        display.display();
        joyDelay = 5;
      }
    }
    else
    {
      joyDelay--;
    }

    if (digitalRead(button) && !useButton)
    {
      newName[namePos] = charPos;
      namePos++;
      charPos = 65;
      useButton = true;
      if (namePos >= 4)
      {
        Send();
        namePos = 0;
        charPos = 65;
        for (int i = 0; i < 5; i++)
        {
          newName[i] = 0;
        }

        mode = 0;
        option = 0;
        option2 = 0;
        load = false;
        display.clearDisplay();
        useButton = true;
        useButton2 = true;
      }
      else
      {
        write(52 + namePos * 6, 49, 1, (String)charPos);
        display.display();
      }
    }
    delay(20);
  }
}

void UpdateProjectile()
{
  for (int i = 0; i < 100; i++)
  {
    Projectile* proj0 = proj[i];
    if (proj0 == nullptr)
    {
      break;
    }

    if (proj0->friendly)
    {
      for (int j = 0; j < 24; j++)
      {
        NPC* npc0 = npc[j];
        if (npc0 == nullptr || proj0->penetrate == 0)
        {
          break;
        }

        if (Intersects(
        proj0->position.X + proj0->width * (1.0 - proj0->scale),
        proj0->position.Y + proj0->height * (1.0 - proj0->scale),
        proj0->width, proj0->height,
        npc0->position.X + npc0->width * (1.0 - npc0->scale),
        npc0->position.Y + npc0->height * (1.0 - npc0->scale),
        npc0->width, npc0->height)) // mega mamada
        {
          proj0->penetrate--;
          if (npc0->immune <= 0 || (proj0->penetrate == 0 && !proj0->laser))
          {
            npc0->life -= proj0->damage;
          }
          if ((proj0->penetrate != 0 || proj0->laser) && npc0->immune == 0)
          {
            npc0->immune = 5;
          }
        }
      }
    }
    if (proj0->hostile)
    {
      if (Intersects(
      proj0->position.X + proj0->width * (1.0 - proj0->scale),
      proj0->position.Y + proj0->height * (1.0 - proj0->scale),
      proj0->width, proj0->height, playerPosition.X, playerPosition.Y, 7, 10) && playerImmune == 0 && proj0->damage > 0)
      {
        proj0->penetrate--;
        playerLife -= proj0->damage;
        regenTimer = lifeRegen;
        if ((proj0->penetrate != 0 || proj0->laser) && playerImmune == 0)
        {
          playerImmune = 50;
        }
        if (playerLife <= 0)
        {
          EndGame();
          return;
        }
      }
    }

    if (proj0->timeLeft <= 0 || proj0->penetrate == 0 || proj0->position.X <= -(proj0->width + 64) || proj0->position.Y <= -(proj0->height + 64) || proj0->position.X > 192 || proj0->position.Y > 128)
    {
      KillProjectile(i);
      i--;
      continue;
    }

    proj0->position.X += proj0->velocity.X;
    proj0->position.Y += proj0->velocity.Y;

    if (proj0->oldPos.X != proj0->position.X || proj0->oldPos.Y != proj0->position.Y || proj0->oldRot != proj0->rotation)
    {
      proj0->oldPos = proj0->position;
      proj0->oldRot = proj0->rotation;
    }

    proj0->AI();

    if (proj0->PreDraw())
    {
      draw(proj0->position.X, proj0->position.Y, proj0->width, proj0->height, proj0->texture, proj0->rotation, proj0->scale, proj0->FindFrame());
    }

    proj0->timeLeft--;
  }
}

void KillProjectile(int num)
{
  if (proj[num] == nullptr)
  {
    return;
  }
  proj[num]->Kill();
  delete proj[num];
  proj[num] = nullptr;

  for (int i = num; i < 100 - 1; i++)
  {
    proj[i] = proj[i + 1];
  }

  proj[100 - 1] = nullptr;
}


void UpdateNPC()
{
  for (int i = 0; i < 24; i++)
  {
    NPC* npc0 = npc[i];
    if (npc0 == nullptr)
    {
      break;
    }

    if (npc0->life <= 0)
    {
      if (npc0->PreKill())
      {
        KillNPC(i);
        i--;
      }
      continue;
    }

    if (npc0->hostile)
    {
      if (Intersects(
      npc0->position.X + npc0->width * (1.0 - npc0->scale),
      npc0->position.Y + npc0->height * (1.0 - npc0->scale),
      npc0->width, npc0->height, playerPosition.X, playerPosition.Y, 7, 10) && playerImmune == 0 && npc0->damage > 0)
      {
        playerLife -= npc0->damage;
        regenTimer = lifeRegen;
        playerImmune = 50;
        if (playerLife <= 0)
        {
          EndGame();
          return;
        }
        // Reducir vida y hacer que el jugador muerapene
      }
    }

    npc0->position.X += npc0->velocity.X;
    npc0->position.Y += npc0->velocity.Y;

    if (npc0->oldPos.X != npc0->position.X || npc0->oldPos.Y != npc0->position.Y || npc0->oldRot != npc0->rotation)
    {
      npc0->oldRot = npc0->rotation;
      npc0->oldPos = npc0->position;
    }

    if (npc0->immune > 0)
    {
      npc0->immune--;
    }

    npc0->AI();

    if (npc0->PreDraw())
    {
      draw(npc0->position.X, npc0->position.Y, npc0->width, npc0->height, npc0->texture, npc0->rotation, npc0->scale);
    }

    if (npc0->position.X <= -(npc0->width + 64) || npc0->position.Y <= -(npc0->height + 64) || npc0->position.X > 192 || npc0->position.Y > 128)
    {
      KillNPC(i);
    }
  }
}

void KillNPC(int num)
{
  if (npc[num] == nullptr)
  {
    return;
  }
  npc[num]->Kill();
  if (playerLife > 0)
  {
    money += npc[num]->value;
    pts += npc[num]->value;
    if (npc[num]->lifeMax >= 5000)
    {
      dBosses++;
    }
    dEnemies++;
  }
  delete npc[num];
  npc[num] = nullptr;

  for (int i = num; i < 24 - 1; i++)
  {
    npc[i] = npc[i + 1];
  }

  npc[24 - 1] = nullptr;
  enemiesActive--;
}

void EndGame()
{
  for (int i = 0; i < 24; i++)
  {
    KillNPC(0);
  }
  for (int i = 0; i < 100; i++)
  {
    KillProjectile(0);
  }

  //money = 500000;

  regenTimer = 0;
  playerPosition = Vector2(0, 0);
  playerVelocity = Vector2(0, 0);
  playerRotation = 0;
  oldPos = Vector2(0, 0);
  oldRot = 0;
  playerImmune = 50;

  weapons[playerWeapon[0]]->cd = 0;
  weapons[playerWeapon[0]]->charge2 = 0;
  weapons[playerWeapon[0]]->useAnim = 0;
  weapons[playerWeapon[1]]->cd = 0;
  weapons[playerWeapon[1]]->charge2 = 0;
  weapons[playerWeapon[1]]->useAnim = 0;
  wpn = 0;

  Save();

  mode = 4;
  useButton = true;
  useButton2 = true;
  option = 0;
  option2 = 0;
  load = false;
  display.clearDisplay();
}

void Send()
{
  jsonProject["gamertag"] = newName;
  jsonProject["points"] = pts;
  jsonProject["shots"] = disparos;
  jsonProject["destroyedEnemies"] = dEnemies;
  jsonProject["destroyedBosses"] = dBosses;
  jsonProject["gameTime"] = timer / 50;
  serializeJson(jsonProject, jsonProjectPayload);

  sendDataToAngelAPI(jsonProjectPayload);
  sendDataToConcursoAPI(disparos, dEnemies, timer / 50);

  delay(500);
}

void ArmoryGun()
{
  display.clearDisplay();
  draw(2, 9, 3, 5, arrow2);
  display.drawRect(7, 3, 17, 17, 1);
  draw(8, 4, 15, 15, guns, 0, 1, option + 1);
  draw(26, 9, 3, 5, arrow);

  write(32, 0, 1, "DMG: " + (String)weapons[option]->damage());
  write(32, 8, 1, "RoF: " + (String)(50.0 / (float)weapons[option]->cooldown()));
  write(32, 16, 1, "VEL: " + (String)(-weapons[option]->velocity()));

  write(0, 24, 1, weapons[option]->name);
  write(0, 32, 1, weapons[option]->desc);

  write(0, 57, 1, "$" + (String)money);

  if (weapons[option]->level == 10)
  {
    write(110, 16, 1, "MAX");
  }
  else if (weapons[option]->level > 0)
  {
    write(98, 16, 1, "LV. " + (String)weapons[option]->level);
  }
  if (weapons[option]->cost() > 0)
  {
    if (weapons[option]->cost() < 10)
    {
      write(116, 0, 1, "$" + (String)weapons[option]->cost());
    }
    else if (weapons[option]->cost() < 100)
    {
      write(110, 0, 1, "$" + (String)weapons[option]->cost());
    }
    else if (weapons[option]->cost() < 1000)
    {
      write(104, 0, 1, "$" + (String)weapons[option]->cost());
    }
    else if (weapons[option]->cost() < 10000)
    {
      write(98, 0, 1, "$" + (String)weapons[option]->cost());
    }
    else
    {
      write(92, 0, 1, "$" + (String)weapons[option]->cost());
    }
    
    if (weapons[option]->level == 0)
    {
      write(86, 57, 1, F("Comprar"));
    }
    else
    {
      write(86, 57, 1, F("Mejorar"));
    }
  }
  display.display();
}

void CustomizeLoad()
{
  StartPlayer();
  display.clearDisplay();
  draw(4, 14, 27, 36, esliship);
  if (option == 0)
  {
    write(13, 0, 1, "Seleccionar armas");
    display.drawRect(42, 11, 17, 17, 1);
    draw(43, 12, 15, 15, guns, 0, 1, playerWeapon[0] + 1);
    if (playerWeapon[1] != -1)
    {
      display.drawRect(42, 36, 17, 17, 1);
      draw(43, 37, 15, 15, guns, 0, 1, playerWeapon[1] + 1);
    }
    if (option2 == 0)
    {
      draw(8, 1, 3, 5, arrow2);
      draw(116, 1, 3, 5, arrow);
    }
    else if (option2 == 1)
    {
      draw(37, 17, 3, 5, arrow2);
      draw(61, 17, 3, 5, arrow);
      write(0, 57, 1, weapons[playerWeapon[0]]->name);
    }
    else
    {
      draw(37, 42, 3, 5, arrow2);
      draw(61, 42, 3, 5, arrow);
      write(0, 57, 1, weapons[playerWeapon[1]]->name);
    }
    write(68, 12, 1, "Arma");
    write(68, 20, 1, "principal");
    write(68, 37, 1, "Arma");
    write(68, 45, 1, "secundaria");
  }
  else
  {
    write(28, 0, 1, "Mejorar nave");

    write(50, 11, 1, "Armadura: " + (String)playerLife);

    write(38, 28, 1, "Velocidad: " + (String)playerVel);

    write(38, 45, 1, "Recuperacion: " + (String)(lvls2[2]));

    for (int i = 0; i < 3; i++)
    {
      if (lvls2[i] <= 5)
      {
        if (lvCost(lvls2[i]) < 10000)
        {
          write(98, 19 + i * 17, 1, "$" + (String)lvCost(lvls2[i]));
        }
        else
        {
          write(92, 19 + i * 17, 1, "$" + (String)lvCost(lvls2[i]));
        }
      }
    }

    write(0, 57, 1, "$" + (String)money);

    if (option2 == 0)
    {
      draw(23, 1, 3, 5, arrow2);
      draw(101, 1, 3, 5, arrow);
    }
    else if (option2 == 1)
    {
      draw(33, 13, 3, 5, arrow);
    }
    else if (option2 == 2)
    {
      draw(33, 29, 3, 5, arrow);
    }
    else
    {
      draw(33, 45, 3, 5, arrow);
    }
  }
  display.display();
}

void Save()
{
  EEPROM.put(MONEY_ADDRESS, money);
  EEPROM.put(WPN1_CUR_ADDRESS, playerWeapon[0]);
  EEPROM.put(WPN2_CUR_ADDRESS, playerWeapon[1]);
  for (int i = 0; i < 9; i++)
  {
    EEPROM.put(LVL_ADDRESSES[i], lvls[i]);
  }
  for (int i = 0; i < 3; i++)
  {
    EEPROM.put(LVL2_ADDRESSES[i], lvls2[i]);
  }
  EEPROM.commit();
}

void StartPlayer()
{
  playerLifeMax = 100.0 * (1.0 + (float)(lvls2[0] - 1) * (0.1));
  playerLife = playerLifeMax;
  playerVel = 1.0 * (1.0 + (float)(lvls2[1] - 1) * (0.15));
  lifeRegen = 250 - 150 * ((float)(lvls2[2] - 1) * (0.2));
  lifeRegen2 = 50 - 25 * ((float)(lvls2[2] - 1) * (0.2));
}