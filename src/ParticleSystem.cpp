#include <cmath>
#include <bit>

#include "ParticleSystem.h"

ParticleSystem::~ParticleSystem(){
    delete grid;
}

void ParticleSystem::postSpawnCleanup() {
    particle_count = m_Particles.size();
    grid = new std::array<std::vector<Particle*>, grid_number>;
    for (int i = 0; i < particle_count; i++) {
        if (particleGridCoordinate(m_Particles[i]) != -1) {
            (*grid)[particleGridCoordinate(m_Particles[i])].push_back(&m_Particles[i]);
        }
    }
}

void ParticleSystem::spawn(const float new_x, const float new_y) {
    m_Particles.push_back(Particle(new_x, new_y));
}

int ParticleSystem::particleGridCoordinate(Particle& p) {
    if (int(p.x / grid_size) >= grid_x || int(p.y / grid_size) >= grid_y || p.x < 0 || p.y < 0) return -1;
    return int(p.x / grid_size) + int(p.y / grid_size) * grid_x;
}

int ParticleSystem::pointGridCoordinate(int x, int y) {
    if (x >= grid_x || y >= grid_y || x < 0 || y < 0) return -1;
    return x + y * grid_x;
}

float ParticleSystem::calculateDistance2(const float x1, const float y1, const float x2, const float y2) {
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

void ParticleSystem::calculateVelocityAndMove(Particle& p) {
    p.vx += p.ax;
    p.vy += p.ay;
    p.vx *= friction;
    p.vy *= friction;

    if (p.x + 1 > x_screen_size && p.vx > 0) p.vx *= -1;

    if (p.x - 1 < 0 && p.vx < 0) p.vx *= -1;

    if (p.y + 1 > y_screen_size && p.vy > 0) p.vy *= -1;

    if (p.y - 1 < 0 && p.vy < 0) p.vy *= -1;

    p.x += p.vx;
    p.y += p.vy;
}

float ParticleSystem::calculateFractialAcceleration(const Particle& p1, const Particle& p2, const float distance2) const {
    float distance = std::sqrt(distance2);
    float inv_distance = 1 / distance;
    float c = 0.0f;
    if (distance <= beta) {
        c = repell_constant * (distance / beta - 1) * interaction_constant * inv_distance;
    } else if (distance <= alpha) {
        c = (1 - (std::fabs((2 * distance - alpha - beta)) / (alpha - beta))) * interaction_constant * inv_distance;
    }
    return c;
}

void ParticleSystem::calculateAcceleration(Particle& p) {
    float ax = 0, ay = 0;
    const float alpha2 = alpha * alpha;
    const int radius = (alpha + grid_size - 1) / grid_size;
    const int r2 = radius * radius;
    const float exact_x = p.x;
    const float exact_y = p.y;
    const int x = int(exact_x) / grid_size;
    const int y = int(exact_y) / grid_size;
    for (int dy = -radius; dy <= radius; ++dy) {
        int dy2 = dy * dy;
        for (int dx = -radius; dx <= radius; ++dx) {
            int tmp_point = pointGridCoordinate(x + dx, y + dy);
            if (dx * dx + dy2 <= r2 && tmp_point != -1) {
                for(const auto* m_p : (*grid)[tmp_point]) {
                    float px = m_p->x;
                    float py = m_p->y;
                    float a = exact_x - px;
                    float b = exact_y - py;
                    float distance2 = a * a + b * b;
                    if (m_p != &p && distance2 > 0.001f && distance2 < alpha2) {
                        float c = calculateFractialAcceleration(p, *m_p, distance2);
                        ax += -a * c;
                        ay += -b * c;
                    }
                }
            }
        }
    }
    p.ax = ax;
    p.ay = ay;
}

void ParticleSystem::update()
{
    for (int i = 0; i < particle_count; ++i) calculateAcceleration(m_Particles[i]);
    for (int i = 0; i < particle_count; ++i) calculateVelocityAndMove(m_Particles[i]);

    for (auto& cell : *grid) cell.clear();
    for (int i = 0; i < particle_count; ++i) {
        int cell = particleGridCoordinate(m_Particles[i]);
        if (cell != -1)
            (*grid)[cell].push_back(&m_Particles[i]);
    }
}