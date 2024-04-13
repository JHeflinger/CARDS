#include "projectile.h"
#include <stdlib.h>

Projectile* GenerateDefaultProjectile(Vector2 position, Vector2 velocity) {
	Projectile* projectile = calloc(1, sizeof(Projectile));
	projectile->position = position;
	projectile->velocity = velocity;
	projectile->size.x = 10.0f;
	projectile->size.y = 10.0f;
	projectile->rotation = 0.0f;
	projectile->lifetime = 1.0f;
	projectile->enable_flags = ENABLE_PROJECTILE_NONE;
	return projectile;
}
