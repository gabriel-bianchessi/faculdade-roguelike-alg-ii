#pragma once
#include "Weapon.h"
#include "raylib.h"

// Origem no cabo; eixo X local aponta para a ponta/cano.
void drawWeapon(WeaponType type, Vector2 origin, Vector2 direction, float scale = 1);
