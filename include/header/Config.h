#ifndef CONFIG_H
#define CONFIG_H

namespace Config
{
    // ------------------------------------------------------------
    // Window settings
    // ------------------------------------------------------------

    constexpr int WINDOW_WIDTH = 900;
    constexpr int WINDOW_HEIGHT = 900;

    // ------------------------------------------------------------
    // Simulation bounds
    //
    // These values describe the square roof area in OpenGL-style
    // normalized coordinates.
    //
    // Left   = -1.0
    // Right  =  1.0
    // Bottom = -1.0
    // Top    =  1.0
    // ------------------------------------------------------------

    constexpr float SIMULATION_LEFT = -1.0f;
    constexpr float SIMULATION_RIGHT = 1.0f;
    constexpr float SIMULATION_BOTTOM = -1.0f;
    constexpr float SIMULATION_TOP = 1.0f;

    constexpr float SIMULATION_WIDTH =
        SIMULATION_RIGHT - SIMULATION_LEFT;

    constexpr float SIMULATION_HEIGHT =
        SIMULATION_TOP - SIMULATION_BOTTOM;

    // ------------------------------------------------------------
    // Rubble generation
    // ------------------------------------------------------------

    constexpr int RUBBLE_COUNT = 80;

    constexpr float MIN_RUBBLE_RADIUS = 0.015f;
    constexpr float MAX_RUBBLE_RADIUS = 0.055f;

    // Initial random movement speed.
    // Small values are best because the simulation area is only -1 to 1.
    constexpr float INITIAL_SPEED = 0.25f;

    // ------------------------------------------------------------
    // Physics settings
    // ------------------------------------------------------------

    // Maximum timestep used by Simulation::update.
    // Prevents unstable jumps if the window freezes or is dragged.
    constexpr float MAX_DELTA_TIME = 1.0f / 30.0f;

    // Linear friction.
    // Higher value means rubble slows down faster.
    constexpr float FRICTION_COEFFICIENT = 0.18f;

    // Speeds smaller than this are snapped to zero to avoid endless sliding.
    constexpr float STOP_SPEED = 0.005f;

    // Circle-circle bounce.
    // 0.0 = no bounce
    // 1.0 = perfectly elastic
    constexpr float COLLISION_RESTITUTION = 0.35f;

    // Wall bounce.
    constexpr float WALL_RESTITUTION = 0.25f;

    // ------------------------------------------------------------
    // Rendering settings
    // ------------------------------------------------------------

    constexpr int CIRCLE_SEGMENTS = 32;

    constexpr float RUBBLE_RED = 0.45f;
    constexpr float RUBBLE_GREEN = 0.43f;
    constexpr float RUBBLE_BLUE = 0.40f;

    constexpr float ROOF_RED = 0.12f;
    constexpr float ROOF_GREEN = 0.12f;
    constexpr float ROOF_BLUE = 0.12f;

    constexpr float BOUNDARY_RED = 0.75f;
    constexpr float BOUNDARY_GREEN = 0.75f;
    constexpr float BOUNDARY_BLUE = 0.75f;
}

#endif
