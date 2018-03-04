#ifndef TG_CONFIGURATION_H
#define TG_CONFIGURATION_H

class Configuration {
public:
    // General
    bool program_running = true;
    // Movement
    const float mouse_sensitivity = 0.05f;
    const float movement_speed = 6.00f;
    // Perspective
    const float perspective_near = 0.1f;
    const float perspective_far = 5000.0f;
    // ROAMTerrain
    const float roam_chunk_length = 1500.0f;
    const int roam_max_triangles = 7500;
    const float roam_tex_repeat_factor = 8.0f;
    // Grass
    const float grass_height = 4.5f;
    const float grass_width = 9.0f;
    const float grass_y_adjust = -0.5f;
    const float grass_separation = 25.0f;
    // Noise
    const float noise_divisor = 2048.0f;
    const float noise_amplitude = 265.0f;
};

#endif