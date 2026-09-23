class Particle {

    public:
    float x, y;
    float vx, vy;
    float ax, ay;
    float inv_Mass;
    int type_Id;
    int grid_position;

    Particle() = default;
    Particle(const float n_x, const float n_y, const int n_type);
};
