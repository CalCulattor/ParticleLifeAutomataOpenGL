#include <vector>
#include <array>
#include <algorithm>
#include <map>

#include "Particle.h"

class ParticleSystem{

    public:
    static constexpr int x_screen_size = 800;
    static constexpr int y_screen_size = 800;
    static constexpr int grid_size = 2;
    static constexpr int grid_number = ((x_screen_size + grid_size - 1) / grid_size) * ((y_screen_size + grid_size - 1) / grid_size);
    static const int grid_x = (x_screen_size + grid_size - 1) / grid_size;
    static const int grid_y = (y_screen_size + grid_size - 1) / grid_size;
    int types_count = 0;
    const float friction = 0.8;
    const float beta = 8;
    const float alpha = 18;
    const float repell_constant = 5;
    const float interaction_constant = 0.1;
    std::vector<Particle> m_Particles = {};
    std::array<std::vector<Particle*>, grid_number>* grid = nullptr;
    std::vector<int> m_Types_id = {};
    std::map<std::pair<int, int>, float> m_Interactions = {};
    float* f_Interactions = nullptr;
    int particle_count = 0;

    ParticleSystem();

    ~ParticleSystem();
    
    void postSpawnCleanup();

    void spawn(const float new_x, const float new_y, const int n_type);

    void addInteraction(const int id1, const int id2, float value);

    int convertId(const int id);

    static int particleGridCoordinate(Particle& p);

    static int pointGridCoordinate(int x, int y);

    static float calculateDistance2(const float x1, const float y1, const float x2, const float y2);

    void calculateVelocityAndMove(Particle& p);

    float calculateFractialAcceleration(const Particle& p1, const Particle& p2, const float distance2) const;

    void calculateAcceleration(Particle& p);

    void update();
};