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
    // ------------------------------------------------------------
// Material variation
// ------------------------------------------------------------

// Fraction of circles that represent graphite rod ends.
// 0.18 means around 18 percent of generated pieces.
constexpr int GRAPHITE_NUMBER = 10;
constexpr float BULLDOZER_BLADE_THICKNESS = 0.018f;
constexpr float BULLDOZER_BLADE_PUSH = 0.08f;


constexpr float BULLDOZER_MASS = 1.0f;
constexpr float BULLDOZER_RECOIL_MULTIPLIER = 0.35f;

constexpr float BULLDOZER_LOAD_DRAG = 8.0f;

// Extra slowdown per object currently touching the blade.
constexpr float BULLDOZER_LOAD_DRAG_PER_CONTACT = 2.0f;

// Graphite rod ends are slightly smaller and denser.
constexpr float MIN_GRAPHITE_RADIUS = 0.018f;
constexpr float MAX_GRAPHITE_RADIUS = 0.040f;

constexpr float CONCRETE_DENSITY = 1.0f;
constexpr float GRAPHITE_DENSITY = 1.8f;

// Graphite slides a bit more easily than rough concrete rubble.
constexpr float CONCRETE_FRICTION_MULTIPLIER = 1.0f;
constexpr float GRAPHITE_FRICTION_MULTIPLIER = 0.65f;

// Graphite pieces bounce slightly less.
constexpr float CONCRETE_RESTITUTION_MULTIPLIER = 1.0f;
constexpr float GRAPHITE_RESTITUTION_MULTIPLIER = 0.75f;

// Graphite rendering color
constexpr float GRAPHITE_RED = 0.58f;
constexpr float GRAPHITE_GREEN = 0.08f;
constexpr float GRAPHITE_BLUE = 0.475f;
// ------------------------------------------------------------
// Bulldozer settings
// ------------------------------------------------------------

constexpr float BULLDOZER_RADIUS = 0.06f;
constexpr float BULLDOZER_BLADE_WIDTH = 0.18f;
constexpr float BULLDOZER_BLADE_OFFSET = 0.08f;

constexpr float BULLDOZER_START_X = 0.0f;
constexpr float BULLDOZER_START_Y = -0.82f;

constexpr float BULLDOZER_MAX_SPEED = 0.45f;
constexpr float BULLDOZER_ACCELERATION = 0.75f;
constexpr float BULLDOZER_ROTATION_SPEED = 2.2f;
constexpr float BULLDOZER_FRICTION = 0.85f;

// ------------------------------------------------------------
// Neural network input/output settings
// ------------------------------------------------------------

// Sightlines + rotation x/y + velocity x/y + acceleration x/y
constexpr int SIGHTLINE_COUNT = 9;
constexpr int NETWORK_INPUT_COUNT = SIGHTLINE_COUNT + 6;

constexpr int NETWORK_HIDDEN_COUNT = 16;
constexpr int NETWORK_OUTPUT_COUNT = 2;

// Output 0: rotation command, -1 to 1
// Output 1: forward/backward command, -1 to 1

constexpr float SIGHTLINE_LENGTH = 0.75f;
constexpr float SIGHTLINE_FOV_RADIANS = 2.2f;

// ------------------------------------------------------------
// Bulldozer rendering
// ------------------------------------------------------------

constexpr float BULLDOZER_RED = 0.85f;
constexpr float BULLDOZER_GREEN = 0.70f;
constexpr float BULLDOZER_BLUE = 0.20f;

constexpr float BULLDOZER_BLADE_RED = 0.95f;
constexpr float BULLDOZER_BLADE_GREEN = 0.95f;
constexpr float BULLDOZER_BLADE_BLUE = 0.80f;

}

#endif
