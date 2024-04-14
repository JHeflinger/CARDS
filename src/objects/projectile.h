#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "raylib.h"
#include <stdlib.h>

typedef void (*ProjectileBehavior)(void*, size_t); // return: void | void* parameters | size_t num args

typedef enum {
	ENABLE_PROJECTILE_NONE          = 1 << 0,
	ENABLE_PROJECTILE_ON_SPAWN      = 1 << 1,
	ENABLE_PROJECTILE_ON_UPDATE     = 1 << 2,
	ENABLE_PROJECTILE_ON_COLLISION  = 1 << 3,
	ENABLE_PROJECTILE_ON_HIT        = 1 << 4,
	ENABLE_PROJECTILE_ON_DEATH      = 1 << 5
} ProjectileFlag;

typedef struct {
	Vector2 position;
	Vector2 velocity;
	Vector2 size;
	float rotation;
	float lifetime;
	ProjectileBehavior on_spawn;
	ProjectileBehavior on_update;
	ProjectileBehavior on_collision;
	ProjectileBehavior on_hit;
	ProjectileBehavior on_death;
	ProjectileFlag enable_flags;
} Projectile;

Projectile* GenerateDefaultProjectile(Vector2 position, Vector2 velocity);

#endif
