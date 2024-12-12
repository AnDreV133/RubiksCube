// Rubik Cube in OpenGL

#include <GL/gl.h>
#include <GL/freeglut.h>
#include <vector>
#include <ostream>
#include <iostream>
#include <iomanip>

using namespace std;

struct cube_rotate {
    GLfloat angle, x, y, z;
};

struct rot_edge_params {
    int is_vertical, is_close_edge, is_positive_rot;
};

GLfloat angle, fAspect, cube_size;
GLint rot_x, rot_y, x_0, x_k, y_0, y_k, z_0, z_k, gap;
vector<cube_rotate> cube_rotations[3][3][3];

int lastMouseX = 0;
int lastMouseY = 0;
bool mouseDragging = false;

void load_visualization_parameters();

void reset_selected_face();

void rotate_edge(rot_edge_params params) {
    GLfloat rotate_angle = params.is_positive_rot ? 90 : -90;

    // Поворот грани по параметрам
    reset_selected_face();
    if (params.is_vertical && params.is_close_edge) {
        x_k = x_0;
    } else if (!params.is_vertical && params.is_close_edge) {
        y_k = y_0;
    } else if (params.is_vertical && !params.is_close_edge) {
        x_0 = x_k;
    } else if (!params.is_vertical && !params.is_close_edge) {
        y_0 = y_k;
    }

    // Формирование параметров поворота грани
    vector<cube_rotate> face[3][3];
    int indexI;
    int indexJ;
    cube_rotate rotation;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j) {
            indexI = i;
            indexJ = j;

            if (x_0 == x_k) {
                rotation = {-rotate_angle, 1.0, 0.0, 0.0};
                face[indexJ][indexI] = cube_rotations[x_k][i][j];
            }

            if (y_0 == y_k) {
                rotation = {rotate_angle, 0.0, 1.0, 0.0};
                face[indexJ][indexI] = cube_rotations[i][y_k][j];
            }

            if (z_0 == z_k) {
                rotation = {-rotate_angle, 0.0, 0.0, 1.0};
                face[indexJ][indexI] = cube_rotations[i][j][z_k];
            }

            face[indexJ][indexI].push_back(rotation);
        }

    // Копируем получившийся face в cube_rotations
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j) {
            indexI = i;
            indexJ = j;

            if (x_0 == x_k)
                cube_rotations[x_k][i][j] = face[indexI][indexJ];

            if (y_0 == y_k)
                cube_rotations[i][y_k][j] = face[indexI][indexJ]; // покрутить i j

            if (z_0 == z_k)
                cube_rotations[i][j][z_k] = face[indexI][indexJ];
        }
}

void reset_selected_face() {
    x_0 = 0;
    x_k = 2;
    y_0 = 0;
    y_k = 2;
    z_0 = 0;
    z_k = 2;
}

void set_camera() {
    gluLookAt(0, 80, 200, 0, 0, 0, 0, 1, 0);
}

void draw_axes(float length);

void draw_rubiks_cube();

void draw_cube(int x, int y, int z);

// Главная функция отрисовки
void draw_func() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Установка камеры
    set_camera();

    // Поворот сцены
    glRotatef(rot_x, 1.0, 0.0, 0.0); // rotate in y axis
    glRotatef(rot_y, 0.0, 1.0, 0.0); // rotate in x axis

    // Отрисовка координат
    draw_axes(cube_size * 1.5f + 20.0f);
    // Отрисовка кубика Рубика
    draw_rubiks_cube();

    glutSwapBuffers();
}

void draw_axes(float length) {
    glBegin(GL_LINES);

    // Ось X (красный цвет)
    glColor3f(1.0f, 0.0f, 0.0f); // Красный
    glVertex3f(0.0f, 0.0f, 0.0f); // Начало оси
    glVertex3f(length, 0.0f, 0.0f); // Конец оси

    // Ось Y (зеленый цвет)
    glColor3f(0.0f, 1.0f, 0.0f); // Зеленый
    glVertex3f(0.0f, 0.0f, 0.0f); // Начало оси
    glVertex3f(0.0f, length, 0.0f); // Конец оси

    // Ось Z (синий цвет)
    glColor3f(0.0f, 0.0f, 1.0f); // Синий
    glVertex3f(0.0f, 0.0f, 0.0f); // Начало оси
    glVertex3f(0.0f, 0.0f, length); // Конец оси

    glEnd();
}

// Отрисовка кубика Рубика
void draw_rubiks_cube() {
    // Отрисовка кубиков
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            for (int k = 0; k < 3; ++k)
                draw_cube(i, j, k);

    // Отрисовка цилиндров внутри кубика Рубика
    int cylinder_half_size = cube_size * 1.4 + gap;
    int cylinder_size = cylinder_half_size * 2;
    int cylinder_radius = cylinder_half_size;

    // Отрисовка цилиндров
    glPushMatrix();

    // Черный цвет для цилиндров
    glColor3f(0.0f, 0.0f, 0.0f);

    // Цилиндр вдоль оси X
    glPushMatrix();
    glTranslatef(-cylinder_half_size, 0, 0);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f); // Поворот вокруг оси Y, чтобы цилиндр был вдоль оси X
    glutSolidCylinder(cylinder_radius, cylinder_size, 16, 1); // Высота 3.0f, чтобы проходить через весь куб
    glPopMatrix();

    // Цилиндр вдоль оси Y
    glPushMatrix();
    glTranslatef(0, -cylinder_half_size, 0);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Поворот вокруг оси X, чтобы цилиндр был вдоль оси Y
    glutSolidCylinder(cylinder_radius, cylinder_size, 16, 1); // Высота 3.0f, чтобы проходить через весь куб
    glPopMatrix();

    // Цилиндр вдоль оси Z
    glPushMatrix();
    glTranslatef(0, 0, -cylinder_half_size);
    glutSolidCylinder(cylinder_radius, cylinder_size, 16, 1); // Цилиндр по умолчанию вдоль оси Z
    glPopMatrix();

    glPopMatrix();
}

// Отрисовка кубика
void draw_cube(int x, int y, int z) {
    vector<cube_rotate> lrot = cube_rotations[x][y][z];

    glPushMatrix();

    // Установка позиции кубика
    glTranslatef(
            (x - 1) * (cube_size + gap),
            (y - 1) * (cube_size + gap),
            (z - 1) * (cube_size + gap)
    );

    // Поворот кубика по его истории поворотов
    for(int i = lrot.size() - 1; i >= 0; --i)
        glRotatef(lrot[i].angle, lrot[i].x, lrot[i].y, lrot[i].z);

    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);  // front
    glNormal3f(0.0, 0.0, 1.0);  
    glVertex3f(cube_size / 2, cube_size / 2, cube_size / 2);
    glVertex3f(-cube_size / 2, cube_size / 2, cube_size / 2);
    glVertex3f(-cube_size / 2, -cube_size / 2, cube_size / 2);
    glVertex3f(cube_size / 2, -cube_size / 2, cube_size / 2);
    glEnd();

    glColor3f(1.0f, 0.5f, 0.0f);
    glBegin(GL_QUADS);  // back
    glNormal3f(0.0, 0.0, -1.0);  
    glVertex3f(cube_size / 2, cube_size / 2, -cube_size / 2);
    glVertex3f(cube_size / 2, -cube_size / 2, -cube_size / 2);
    glVertex3f(-cube_size / 2, -cube_size / 2, -cube_size / 2);
    glVertex3f(-cube_size / 2, cube_size / 2, -cube_size / 2);
    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);  // left
    glNormal3f(-1.0, 0.0, 0.0);  
    glVertex3f(-cube_size / 2, cube_size / 2, cube_size / 2);
    glVertex3f(-cube_size / 2, cube_size / 2, -cube_size / 2);
    glVertex3f(-cube_size / 2, -cube_size / 2, -cube_size / 2);
    glVertex3f(-cube_size / 2, -cube_size / 2, cube_size / 2);
    glEnd();

    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);  // right
    glNormal3f(1.0, 0.0, 0.0);  
    glVertex3f(cube_size / 2, cube_size / 2, cube_size / 2);
    glVertex3f(cube_size / 2, -cube_size / 2, cube_size / 2);
    glVertex3f(cube_size / 2, -cube_size / 2, -cube_size / 2);
    glVertex3f(cube_size / 2, cube_size / 2, -cube_size / 2);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);  // top
    glNormal3f(0.0, 1.0, 0.0);  
    glVertex3f(-cube_size / 2, cube_size / 2, -cube_size / 2);
    glVertex3f(-cube_size / 2, cube_size / 2, cube_size / 2);
    glVertex3f(cube_size / 2, cube_size / 2, cube_size / 2);
    glVertex3f(cube_size / 2, cube_size / 2, -cube_size / 2);
    glEnd();

    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);  // bottom
    glNormal3f(0.0, -1.0, 0.0);  
    glVertex3f(-cube_size / 2, -cube_size / 2, -cube_size / 2);
    glVertex3f(cube_size / 2, -cube_size / 2, -cube_size / 2);
    glVertex3f(cube_size / 2, -cube_size / 2, cube_size / 2);
    glVertex3f(-cube_size / 2, -cube_size / 2, cube_size / 2);
    glEnd();

    glPopMatrix();
} 

// Инициализация параметров отрисовки
void init_func() {
    cube_size = 30.0;
    rot_x = 0.0;
    rot_y = 0.0;
    gap = 4;

    // Задаем свет
    GLfloat ambient_light[4] = {0.2, 0.2, 0.2, 1.0};

    // Задаём параметры материала
    GLfloat specularity[4] = {1.0, 1.0, 1.0, 1.0};
    GLint material_specularity = 60;

    // Цвет заднего фона
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glShadeModel(GL_SMOOTH);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularity);
    glMateriali(GL_FRONT, GL_SHININESS, material_specularity);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_light);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    angle = 45;
}

void load_visualization_parameters() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(angle, fAspect, 0.4, 500);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void reshape_func(GLsizei w, GLsizei h) {
    if (h == 0) h = 1;

    glViewport(0, 0, w, h);

    fAspect = (GLfloat) w / (GLfloat) h;

    load_visualization_parameters();
}

// Обработка нажатий клавиш
void keyboard_func(unsigned char key, int x, int y) {
    switch (key) {
        case 'Q':
        case 'q':
            rotate_edge({1, 1, 1});
            break;
        case 'W':
        case 'w':
            rotate_edge({1, 0, 1});
            break;
        case 'A':
        case 'a':
            rotate_edge({1, 1, 0});
            break;
        case 'S':
        case 's':
            rotate_edge({1, 0, 0});
            break;
        case 'E':
        case 'e':
            rotate_edge({0, 0, 0});
            break;
        case 'R':
        case 'r':
            rotate_edge({0, 0, 1});
            break;
        case 'D':
        case 'd':
            rotate_edge({0, 1, 0});
            break;
        case 'F':
        case 'f':
            rotate_edge({0, 1, 1});
            break;
        default:
            break;
    }

    glutPostRedisplay();
}

void mouse_func(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mouseDragging = true;
            lastMouseX = x;
            lastMouseY = y;
        } else if (state == GLUT_UP) {
            mouseDragging = false;
        }
    }
}

void mouse_motion_func(int x, int y) {
    if (mouseDragging) {
        // Вычисляем разницу в координатах мыши
        int deltaX = x - lastMouseX;
        int deltaY = y - lastMouseY;

        // Обновляем углы поворота
        rot_y += deltaX * 0.5f; // Чувствительность поворота
        rot_x += deltaY * 0.5f;

        // Сохраняем текущие координаты мыши
        lastMouseX = x;
        lastMouseY = y;

        // Перерисовываем сцену
        load_visualization_parameters();
        glutPostRedisplay();
    }
}

void mouse_wheel_func(int wheel, int direction, int x, int y) {
    if (direction > 0) {
        if (angle >= 10) angle -= 5; // Zoom-in
    } else {
        if (angle <= 130) angle += 5; // Zoom-out
    }
    load_visualization_parameters();
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(400, 350);
    glutCreateWindow("Rubik's Cube 3D");
    glutDisplayFunc(draw_func);
    glutReshapeFunc(reshape_func);

    glutMouseFunc(mouse_func);
    glutMotionFunc(mouse_motion_func); // Функция для отслеживания движения мыши
    glutMouseWheelFunc(mouse_wheel_func);
    glutKeyboardFunc(keyboard_func);

    init_func();
    glutMainLoop();

    return 0;
}
