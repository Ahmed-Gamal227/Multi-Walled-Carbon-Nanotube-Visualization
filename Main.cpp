#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <iostream>

const float PI = 3.14159265358979323846f;
const float SQRT3 = 1.7320508075688772f; // sqrt(3)

struct Point3D {
    float x, y, z;
    Point3D(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}
};

// Parameters for multi-walled nanotube
const int layers = 3;                 // Number of concentric tubes
const int n = 6;                      // Chirality parameter (n,n)
const float a = 0.246f;               // Graphene lattice constant (nm)
const float bondLength = 0.142f;      // C-C bond length (nm)
const float interlayerDistance = 0.34f; // Distance between layers (nm)
const int lengthSegments = 20;        // Number of unit cells along length

std::vector<Point3D> atoms;
std::vector<std::pair<int, int>> bonds;

void createMultiWalledNanotube() {
    atoms.clear();
    bonds.clear();

    for (int layer = 0; layer < layers; ++layer) {
        float layerRadius = (n * a) / (2 * PI) + layer * interlayerDistance;
        float layerCircumference = 2 * PI * layerRadius;
        float scaledA = layerCircumference / n;

        for (int i = 0; i < lengthSegments; ++i) {
            for (int j = 0; j < 2 * n; ++j) {
                float x_sheet, y_sheet;

                if (j % 2 == 0) {
                    x_sheet = (j / 2) * scaledA;
                    y_sheet = i * (SQRT3 * bondLength);
                }
                else {
                    x_sheet = (j / 2) * scaledA + scaledA / 2;
                    y_sheet = i * (SQRT3 * bondLength) + bondLength * SQRT3 / 2;
                }

                float theta = x_sheet / layerRadius;
                float x = layerRadius * cos(theta);
                float y = layerRadius * sin(theta);
                float z = y_sheet - (lengthSegments * SQRT3 * bondLength) / 2;

                atoms.emplace_back(x, y, z);
            }
        }
    }

    // Create bonds within each layer
    for (int layer = 0; layer < layers; ++layer) {
        int layerOffset = layer * lengthSegments * 2 * n;

        for (int i = 0; i < lengthSegments; ++i) {
            for (int j = 0; j < 2 * n; ++j) {
                int current = layerOffset + i * 2 * n + j;

                if (j % 2 == 0) {
                    if (j < 2 * n - 1) {
                        bonds.emplace_back(current, current + 1);
                    }
                }
                else {
                    int next = (j == 2 * n - 1) ? current - (2 * n - 1) : current + 1;
                    bonds.emplace_back(current, next);
                }

                if (i < lengthSegments - 1) {
                    if (j % 2 == 0) {
                        bonds.emplace_back(current, current + 2 * n);
                    }
                    else {
                        bonds.emplace_back(current, current + 2 * n - 1);
                        if (j < 2 * n - 1) {
                            bonds.emplace_back(current, current + 2 * n + 1);
                        }
                        else {
                            bonds.emplace_back(current, current + 1);
                        }
                    }
                }
            }
        }
    }

    // Create interlayer bonds (van der Waals connections)
    if (layers > 1) {
        for (int layer = 0; layer < layers - 1; ++layer) {
            int outerOffset = (layer + 1) * lengthSegments * 2 * n;
            int innerOffset = layer * lengthSegments * 2 * n;

            for (int i = 0; i < lengthSegments; i += 2) {
                for (int j = 0; j < 2 * n; j += 2) {
                    int innerAtom = innerOffset + i * 2 * n + j;
                    int outerAtom = outerOffset + i * 2 * n + j;

                    // Connect vertically aligned atoms between layers
                    bonds.emplace_back(innerAtom, outerAtom);
                }
            }
        }
    }
}

void init() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // Enhanced lighting
    GLfloat light0_position[] = { 5.0f, 5.0f, 10.0f, 1.0f };
    GLfloat light1_position[] = { -5.0f, 5.0f, -10.0f, 1.0f };
    GLfloat white_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat lmodel_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);

    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white_light);
    glLightfv(GL_LIGHT1, GL_SPECULAR, white_light);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    // Material properties
    GLfloat mat_specular[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    GLfloat mat_shininess[] = { 100.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    createMultiWalledNanotube();
}

void drawCarbonAtom(float x, float y, float z, int layer) {
    glPushMatrix();
    glTranslatef(x, y, z);

    // Different colors for different layers
    GLfloat carbon_color[4];
    if (layer == 0) {
        carbon_color[0] = 0.8f; carbon_color[1] = 0.2f; carbon_color[2] = 0.2f; // Red
    }
    else if (layer == 1) {
        carbon_color[0] = 0.2f; carbon_color[1] = 0.8f; carbon_color[2] = 0.2f; // Green
    }
    else {
        carbon_color[0] = 0.2f; carbon_color[1] = 0.2f; carbon_color[2] = 0.8f; // Blue
    }
    carbon_color[3] = 1.0f;

    GLfloat carbon_emission[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, carbon_color);
    glMaterialfv(GL_FRONT, GL_EMISSION, carbon_emission);

    glutSolidSphere(0.08f, 16, 16);
    glPopMatrix();
}

void drawBond(Point3D p1, Point3D p2, bool interlayer) {
    GLfloat bond_color[4];
    if (interlayer) {
        bond_color[0] = 0.8f; bond_color[1] = 0.8f; bond_color[2] = 0.0f; // Yellow for interlayer
    }
    else {
        bond_color[0] = 0.7f; bond_color[1] = 0.7f; bond_color[2] = 0.7f; // Gray for intralayer
    }
    bond_color[3] = 1.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, bond_color);

    Point3D center((p1.x + p2.x) / 2, (p1.y + p2.y) / 2, (p1.z + p2.z) / 2);
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    float dz = p2.z - p1.z;
    float length = sqrt(dx * dx + dy * dy + dz * dz);

    float angle = acos(dz / length) * 180.0f / PI;
    float axisX = -dy;
    float axisY = dx;

    glPushMatrix();
    glTranslatef(center.x, center.y, center.z);
    glRotatef(angle, axisX, axisY, 0.0f);

    GLUquadric* quadric = gluNewQuadric();
    gluCylinder(quadric, interlayer ? 0.02f : 0.04f, interlayer ? 0.02f : 0.04f, length, 8, 1);
    gluDeleteQuadric(quadric);

    glPopMatrix();
}

// Camera control variables
float cameraAngleX = 20.0f;
float cameraAngleY = 0.0f;
float cameraDistance = 25.0f;
bool isPaused = false;
float rotationAngle = 0.0f;

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Camera positioning
    float camX = cameraDistance * sin(cameraAngleY * PI / 180) * cos(cameraAngleX * PI / 180);
    float camY = cameraDistance * sin(cameraAngleX * PI / 180);
    float camZ = cameraDistance * cos(cameraAngleY * PI / 180) * cos(cameraAngleX * PI / 180);

    gluLookAt(camX, camY, camZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    // Rotate when not paused
    if (!isPaused) {
        rotationAngle += 0.3f;
        if (rotationAngle > 360.0f) rotationAngle -= 360.0f;
    }
    glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f);

    // Draw all bonds
    for (const auto& bond : bonds) {
        // Determine if this is an interlayer bond
        int atomsPerLayer = lengthSegments * 2 * n;
        bool interlayer = (bond.first / atomsPerLayer) != (bond.second / atomsPerLayer);
        drawBond(atoms[bond.first], atoms[bond.second], interlayer);
    }

    // Draw all atoms with layer-specific colors
    for (size_t i = 0; i < atoms.size(); ++i) {
        int layer = i / (lengthSegments * 2 * n);
        drawCarbonAtom(atoms[i].x, atoms[i].y, atoms[i].z, layer);
    }

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0f, (float)w / (float)h, 1.0f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void idle() {
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:
        cameraAngleX += 2.0f;
        if (cameraAngleX > 89.0f) cameraAngleX = 89.0f;
        break;
    case GLUT_KEY_DOWN:
        cameraAngleX -= 2.0f;
        if (cameraAngleX < -89.0f) cameraAngleX = -89.0f;
        break;
    case GLUT_KEY_LEFT:
        cameraAngleY -= 2.0f;
        break;
    case GLUT_KEY_RIGHT:
        cameraAngleY += 2.0f;
        break;
    }
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case '+':
    case '=':
        cameraDistance -= 1.0f;
        if (cameraDistance < 10.0f) cameraDistance = 10.0f;
        break;
    case '-':
    case '_':
        cameraDistance += 1.0f;
        if (cameraDistance > 50.0f) cameraDistance = 50.0f;
        break;
    case 'r':
        cameraAngleX = 20.0f;
        cameraAngleY = 0.0f;
        cameraDistance = 25.0f;
        break;
    case ' ':
        isPaused = !isPaused;
        break;
    case 'l':
        // Toggle layer visibility (could be extended)
        break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 800);
    glutCreateWindow("Multi-Walled Carbon Nanotube Visualization");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutSpecialFunc(specialKeys);
    glutKeyboardFunc(keyboard);

    std::cout << "Controls:\n";
    std::cout << "Arrow Keys: Rotate view\n";
    std::cout << "+/-: Zoom in/out\n";
    std::cout << "Space: Pause/Resume rotation\n";
    std::cout << "R: Reset view\n";

    glutMainLoop();
    return 0;
}