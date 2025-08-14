#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

float initial_vertices[3][2] = {
    {0.0, 100.0},
    {-86.6, -50.0},
    {86.6, -50.0}
};
float base_vertices[3][2];
float triangle_vertices[3][2];

int animation_choice = 0;
int current_step = 0;
const int total_steps = 120;

float trans_x, trans_y;
float scale_x, scale_y;
float rotation_angle;
int reflection_axis;
float shear_x, shear_y;

void display();
void init();
void animate_one_step();
void reset_triangle();
void print_menu();
void platform_sleep(int milliseconds);

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("2D Transformations - Console Input");

    init();

    glutDisplayFunc(display);

    memcpy(triangle_vertices, initial_vertices, sizeof(initial_vertices));
    memcpy(base_vertices, initial_vertices, sizeof(initial_vertices));

    char choice;
    while (1) {
        display();
        print_menu();
        printf("Enter your choice: ");

        scanf(" %c", &choice);

        if (choice >= '1' && choice <= '5') {
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

        if (animation_choice != 0) {
            for (current_step = 0; current_step <= total_steps; current_step++) {
                animate_one_step();
                display();
                platform_sleep(16);
            }
            animation_choice = 0;
        }
    }
    return 0;
}

void init() {
    glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-WINDOW_WIDTH / 2.0, WINDOW_WIDTH / 2.0, -WINDOW_HEIGHT / 2.0, WINDOW_HEIGHT / 2.0);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINES);
    glVertex2f(-WINDOW_WIDTH / 2.0, 0.0);
    glVertex2f(WINDOW_WIDTH / 2.0, 0.0);
    glVertex2f(0.0, -WINDOW_HEIGHT / 2.0);
    glVertex2f(0.0, WINDOW_HEIGHT / 2.0);
    glEnd();

    glColor3f(0.2f, 0.6f, 1.0f);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 3; ++i) {
        glVertex2f(triangle_vertices[i][0], triangle_vertices[i][1]);
    }
    glEnd();

    glutSwapBuffers();
}

void reset_triangle() {
    memcpy(triangle_vertices, initial_vertices, sizeof(initial_vertices));
    memcpy(base_vertices, initial_vertices, sizeof(initial_vertices));
}

void animate_one_step() {
    if (animation_choice == 0) return;

    float progress = (float)current_step / total_steps;

    switch (animation_choice) {
        case 1:
            for (int i = 0; i < 3; i++) {
                triangle_vertices[i][0] = base_vertices[i][0] + trans_x * progress;
                triangle_vertices[i][1] = base_vertices[i][1] + trans_y * progress;
            }
            break;
        case 2:
            {
                float current_sx = 1.0f + (scale_x - 1.0f) * progress;
                float current_sy = 1.0f + (scale_y - 1.0f) * progress;
                for (int i = 0; i < 3; i++) {
                    triangle_vertices[i][0] = base_vertices[i][0] * current_sx;
                    triangle_vertices[i][1] = base_vertices[i][1] * current_sy;
                }
            }
            break;
        case 3:
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
        case 4:
            {
                float reflect_factor = 1.0f - 2.0f * progress;
                if (reflection_axis == 1) {
                    for (int i = 0; i < 3; i++) {
                        triangle_vertices[i][0] = base_vertices[i][0];
                        triangle_vertices[i][1] = base_vertices[i][1] * reflect_factor;
                    }
                } else {
                    for (int i = 0; i < 3; i++) {
                        triangle_vertices[i][0] = base_vertices[i][0] * reflect_factor;
                        triangle_vertices[i][1] = base_vertices[i][1];
                    }
                }
            }
            break;
        case 5:
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

void platform_sleep(int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}
