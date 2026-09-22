#include <vector>
#include <array>
#include <unordered_map>
#include <algorithm>

#include "Particle.h"

class ParticleSystem{

    public:
    static constexpr int x_screen_size = 800;
    static constexpr int y_screen_size = 800;
    static constexpr int grid_size = 2;
    static constexpr int grid_number = ((x_screen_size + grid_size - 1) / grid_size) * ((y_screen_size + grid_size - 1) / grid_size);
    static const int grid_x = (x_screen_size + grid_size - 1) / grid_size;
    static const int grid_y = (y_screen_size + grid_size - 1) / grid_size;
    const float friction = 0.9;
    const float beta = 20;
    const float alpha = 40;
    const float repell_constant = 3;
    const float interaction_constant = 0.09;
    std::vector<Particle> m_Particles = {};
    std::array<std::vector<Particle*>, grid_number>* grid = nullptr;
    int particle_count = 0;

    ~ParticleSystem();

    void postSpawnCleanup();

    void spawn(const float new_x, const float new_y);

    static int particleGridCoordinate(Particle& p);

    static int pointGridCoordinate(int x, int y);

    static float calculateDistance2(const float x1, const float y1, const float x2, const float y2);

    void calculateVelocityAndMove(Particle& p);

    float calculateFractialAcceleration(const Particle& p1, const Particle& p2, const float distance2) const;

    void calculateAcceleration(Particle& p);

    void update();
};