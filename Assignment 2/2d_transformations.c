/**
 * @file 2d_transformations.c
 * @brief A C program to demonstrate 2D transformations using OpenGL.
 *
 * This program is driven entirely by the console. It visualizes Translation,
 * Scaling, Rotation, Reflection, and Shearing on a 2D triangle with animation.
 *
 * @author Gemini
 *
 * To Compile:
 * gcc 2d_transformations.c -o transform -lglut -lGL -lGLU -lm
 *
 * To Run:
 * ./transform
 */

#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include <string.h>

// Include headers for Sleep/usleep
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// For M_PI constant on some compilers
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --- Global Variables ---

// Window dimensions
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

// The vertices of our triangle.
// We use three arrays to manage state:
// - initial_vertices: The absolute original state for resetting.
// - base_vertices: The state before the current animation starts.
// - triangle_vertices: The vertices being actively modified and drawn.
float initial_vertices[3][2] = {
    {0.0, 100.0},    // Top vertex (made smaller)
    {-86.6, -50.0},  // Bottom-left vertex
    {86.6, -50.0}    // Bottom-right vertex
};
float base_vertices[3][2];
float triangle_vertices[3][2];


// Animation state variables
int animation_choice = 0;   // Which transformation to animate (0=none)
int current_step = 0;       // The current frame of the animation
const int total_steps = 120; // Total frames for any animation

// User-defined transformation parameters
float trans_x, trans_y;       // Translation factors
float scale_x, scale_y;       // Scaling factors
float rotation_angle;         // Rotation angle in degrees
int reflection_axis;          // 1 for X-axis, 2 for Y-axis
float shear_x, shear_y;       // Shearing factors

// --- Function Prototypes ---

void display();
void init();
void animate_one_step();
void reset_triangle();
void print_menu();
void platform_sleep(int milliseconds);


/**
 * @brief Main function: sets up GLUT and runs the main console-driven application loop.
 */
int main(int argc, char** argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("2D Transformations - Console Input");

    // Custom initialization
    init();

    // Register the display function. No keyboard func needed.
    glutDisplayFunc(display);

    // Initialize all vertex arrays to the starting state
    memcpy(triangle_vertices, initial_vertices, sizeof(initial_vertices));
    memcpy(base_vertices, initial_vertices, sizeof(initial_vertices));

    char choice;
    // This is the main application loop, driven by console input.
    while (1) {
        display(); // Draw the current state
        print_menu();
        printf("Enter your choice: ");

        // Read a single char, the space before %c consumes whitespace (like newlines)
        scanf(" %c", &choice);

        // Prepare for a new transformation
        if (choice >= '1' && choice <= '5') {
            // Set the base for the new animation to the current triangle state
            memcpy(base_vertices, triangle_vertices, sizeof(triangle_vertices));
        }

        switch (choice) {
            case '1':
                printf("Action: Translation\n");
                printf("Enter translation vector (tx ty): ");
                scanf("%f %f", &trans_x, &trans_y);
                animation_choice = 1;
                break;
            case '2':
                printf("Action: Scaling\n");
                printf("Enter scaling factors (sx sy): ");
                scanf("%f %f", &scale_x, &scale_y);
                animation_choice = 2;
                break;
            case '3':
                printf("Action: Rotation\n");
                printf("Enter rotation angle in degrees: ");
                scanf("%f", &rotation_angle);
                animation_choice = 3;
                break;
            case '4':
                printf("Action: Reflection\n");
                printf("Reflect about which axis? (1 for X-axis, 2 for Y-axis): ");
                scanf("%d", &reflection_axis);
                animation_choice = (reflection_axis == 1 || reflection_axis == 2) ? 4 : 0;
                break;
            case '5':
                printf("Action: Shearing\n");
                printf("Enter x-shear and y-shear factors (shx shy): ");
                scanf("%f %f", &shear_x, &shear_y);
                animation_choice = 5;
                break;
            case 'r':
            case 'R':
                printf("Action: Resetting triangle to original position.\n");
                reset_triangle();
                animation_choice = 0;
                break;
            case 'q':
            case 'Q':
                printf("Exiting program.\n");
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
                animation_choice = 0;
                break;
        }

        // If a valid animation was chosen, run the animation loop
        if (animation_choice != 0) {
            for (current_step = 0; current_step <= total_steps; current_step++) {
                animate_one_step(); // Calculate one frame
                display();          // Render the frame
                platform_sleep(16); // Pause for ~60 FPS
            }
            animation_choice = 0; // Reset for next input
        }
    }
    return 0;
}


/**
 * @brief Initializes OpenGL states, projection, and background color.
 */
void init() {
    glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-WINDOW_WIDTH / 2.0, WINDOW_WIDTH / 2.0, -WINDOW_HEIGHT / 2.0, WINDOW_HEIGHT / 2.0);
}

/**
 * @brief The main display callback function.
 */
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw X and Y axes
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINES);
    glVertex2f(-WINDOW_WIDTH / 2.0, 0.0);
    glVertex2f(WINDOW_WIDTH / 2.0, 0.0);
    glVertex2f(0.0, -WINDOW_HEIGHT / 2.0);
    glVertex2f(0.0, WINDOW_HEIGHT / 2.0);
    glEnd();

    // Draw the triangle
    glColor3f(0.2f, 0.6f, 1.0f);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 3; ++i) {
        glVertex2f(triangle_vertices[i][0], triangle_vertices[i][1]);
    }
    glEnd();

    glutSwapBuffers();
}

/**
 * @brief Resets the triangle to its absolute original position.
 */
void reset_triangle() {
    memcpy(triangle_vertices, initial_vertices, sizeof(initial_vertices));
    memcpy(base_vertices, initial_vertices, sizeof(initial_vertices));
}

/**
 * @brief Calculates the vertex positions for a single frame of an animation.
 * This function no longer uses timers; it's called directly from the main loop.
 */
void animate_one_step() {
    if (animation_choice == 0) return;

    float progress = (float)current_step / total_steps;

    switch (animation_choice) {
        case 1: // Translation
            for (int i = 0; i < 3; i++) {
                triangle_vertices[i][0] = base_vertices[i][0] + trans_x * progress;
                triangle_vertices[i][1] = base_vertices[i][1] + trans_y * progress;
            }
            break;
        case 2: // Scaling
            {
                float current_sx = 1.0f + (scale_x - 1.0f) * progress;
                float current_sy = 1.0f + (scale_y - 1.0f) * progress;
                for (int i = 0; i < 3; i++) {
                    triangle_vertices[i][0] = base_vertices[i][0] * current_sx;
                    triangle_vertices[i][1] = base_vertices[i][1] * current_sy;
                }
            }
            break;
        case 3: // Rotation
            {
                float current_angle_rad = (rotation_angle * M_PI / 180.0) * progress;
                float cos_val = cos(current_angle_rad);
                float sin_val = sin(current_angle_rad);
                for (int i = 0; i < 3; i++) {
                    float ox = base_vertices[i][0];
                    float oy = base_vertices[i][1];
                    triangle_vertices[i][0] = ox * cos_val - oy * sin_val;
                    triangle_vertices[i][1] = ox * sin_val + oy * cos_val;
                }
            }
            break;
        case 4: // Reflection
            {
                float reflect_factor = 1.0f - 2.0f * progress;
                if (reflection_axis == 1) { // Reflect about X-axis
                    for (int i = 0; i < 3; i++) {
                        triangle_vertices[i][0] = base_vertices[i][0];
                        triangle_vertices[i][1] = base_vertices[i][1] * reflect_factor;
                    }
                } else { // Reflect about Y-axis
                    for (int i = 0; i < 3; i++) {
                        triangle_vertices[i][0] = base_vertices[i][0] * reflect_factor;
                        triangle_vertices[i][1] = base_vertices[i][1];
                    }
                }
            }
            break;
        case 5: // Shearing
            {
                float current_shx = shear_x * progress;
                float current_shy = shear_y * progress;
                for (int i = 0; i < 3; i++) {
                    float ox = base_vertices[i][0];
                    float oy = base_vertices[i][1];
                    triangle_vertices[i][0] = ox + current_shx * oy;
                    triangle_vertices[i][1] = oy + current_shy * ox;
                }
            }
            break;
    }
}

/**
 * @brief Prints the user menu to the console.
 */
void print_menu() {
    printf("\n\n========= 2D Transformation Menu =========\n");
    printf("  1: Translate\n");
    printf("  2: Scale\n");
    printf("  3: Rotate\n");
    printf("  4: Reflect\n");
    printf("  5: Shear\n");
    printf("  R: Reset to Original Position\n");
    printf("  Q: Quit Program\n");
    printf("==========================================\n");
}

/**
 * @brief A cross-platform sleep function.
 * @param milliseconds The duration to sleep for.
 */
void platform_sleep(int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000); // usleep takes microseconds
#endif
}
