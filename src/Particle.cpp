#include "Particle.h"

Particle::Particle(const float n_x, const float n_y, const int n_type) {
    x = n_x;
    y = n_y;
    vx = 0.0f;
    vy = 0.0f;
    ax = 0.0f;
    ay = 0.0f;
    type_Id = n_type;
}
