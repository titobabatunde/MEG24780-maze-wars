#include <stdio.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <vector>
#include "fssimplewindow.h"
#include <string>
#include "ysglfontdata.h"


using namespace std;

const double PI = 3.1415927;

int maxi(int a, int b)
{
    if (a > b) return a;
    return b;
}

void PhysicalCoordToScreenCoord(int& sx, int& sy, double px, double py)
{
    sx = (int)(px * 10.0);
    sy = 600 - (int)(py * 10.0);
}

void GetLocalTimeHourMinSec(int& hour, int& min, int& sec) {

    struct tm* localTime;
    time_t t = time(NULL);
    localTime = localtime(&t);
    hour = localTime->tm_hour;
    min = localTime->tm_min;
    sec = localTime->tm_sec;
}

void DrawRect(int x1, int y1, int x2, int y2, int fill)
{
    if (0 != fill)
    {
        glBegin(GL_QUADS);
    }
    else
    {
        glBegin(GL_LINE_LOOP);
    }

    glVertex2i(x1, y1);
    glVertex2i(x2, y1);
    glVertex2i(x2, y2);
    glVertex2i(x1, y2);

    glEnd();
}

void Drawtank(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, int fill) {
    if (0 != fill)
    {
        glBegin(GL_QUADS);
    }
    else
    {
        glBegin(GL_LINE_LOOP);
    }

    glVertex2i(x1, y1);
    glVertex2i(x2, y2);
    glVertex2i(x3, y3);
    glVertex2i(x4, y4);

    glEnd();
}

void DrawLine(int x1, int y1, int x2, int y2)
{
    glBegin(GL_LINES);

    glVertex2i(x1, y1);
    glVertex2i(x2, y2);

    glEnd();
}

void DrawCircle(int cx, int cy, int rad, int fill)
{
    const double YS_PI = 3.1415927;

    if (0 != fill)
    {
        glBegin(GL_POLYGON);
    }
    else
    {
        glBegin(GL_LINE_LOOP);
    }

    int i;
    for (i = 0; i < 64; i++)
    {
        double angle = (double)i * YS_PI / 32.0;
        double x = (double)cx + cos(angle) * (double)rad;
        double y = (double)cy + sin(angle) * (double)rad;
        glVertex2d(x, y);
    }

    glEnd();
}




class Maze
{
public:
    const int thickness = 1;
    const int percentage = 25;
    const int min_active = 40;
    int cells[8][6];
    int walls[9][7][2];

    void Initialize_Cells();
    void Initialize_Walls();
    void assign_walls();

    void Draw();
    int check_maze();
    void check_neighbor(int i, int j);
    void Draw_Active_Cells();
};

void Maze::Initialize_Cells()
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            cells[i][j] = 0;
        }
    }
}

void Maze::Initialize_Walls()
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            for (int k = 0; k < 2; k++)
            {
                walls[i][j][k] = 0;
            }
        }
    }

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            for (int k = 0; k < 2; k++)
            {
                if ((0 == i || 8 == i) && j < 6)
                {
                    walls[i][j][1] = 1;
                }

                if ((0 == j || 6 == j) && i < 8)
                {
                    walls[i][j][0] = 1;
                }
            }
        }
    }
}

void Maze::assign_walls()
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            if (0 == walls[i][j][0])
            {
                int p = rand() % 100;
                if (p < percentage)
                {
                    walls[i][j][0] = 1;
                }
            }
            if (0 == walls[i][j][1])
            {
                int p = rand() % 100;
                if (p < percentage)
                {
                    walls[i][j][1] = 1;
                }
            }
            if (0 == walls[i + 1][j][1])
            {
                int p = rand() % 100;
                if (p < percentage)
                {
                    walls[i + 1][j][1] = 1;
                }
            }
            if (0 == walls[i][j + 1][0])
            {
                int p = rand() % 100;
                if (p < percentage)
                {
                    walls[i][j + 1][0] = 1;
                }
            }
        }
    }
}


void Maze::Draw()
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            if (1 == walls[i][j][0])
            {
                glColor3f(0, 0, 0);
                glBegin(GL_LINE_STRIP);
                glVertex2i(100 * i, 100 * j);
                glVertex2i(100 * (i + 1), 100 * j);
                glEnd();
            }
            if (1 == walls[i][j][1])
            {
                glColor3f(0, 0, 0);
                glBegin(GL_LINE_STRIP);
                glVertex2i(100 * i, 100 * j);
                glVertex2i(100 * i, 100 * (j + 1));
                glEnd();
            }
        }
    }
}


void Maze::check_neighbor(int i, int j)
{
    if (0 == walls[i + 1][j][1] && 0 == cells[i + 1][j])
    {
        cells[i + 1][j] = 1;
        check_neighbor(i + 1, j);
    }
    if (0 == walls[i][j + 1][0] && 0 == cells[i][j + 1])
    {
        cells[i][j + 1] = 1;
        check_neighbor(i, j + 1);
    }
    if (0 == walls[i][j][1])
    {
        if (0 == cells[i - 1][j])
        {
            cells[i - 1][j] = 1;
            check_neighbor(i - 1, j);
        }
    }
    if (0 == walls[i][j][0])
    {
        if (0 == cells[i][j - 1])
        {
            cells[i][j - 1] = 1;
            check_neighbor(i, j - 1);
        }
    }
}

int Maze::check_maze()
{
    Initialize_Cells();
    int x = rand() % 8;
    int y = rand() % 6;
    check_neighbor(x, y);

    int active = 0;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            if (1 == cells[i][j])
            {
                active++;
            }
        }
    }
    //printf("active cells: %d \n", active);
    int valid = 0;
    if (active > min_active)
    {
        valid = 1;
    }
    return valid;
}

void Maze::Draw_Active_Cells()
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            if (0 == cells[i][j])
            {
                glColor3f(0, 0, 0);
                glBegin(GL_QUADS);
                glVertex2i(100 * i, 100 * j);
                glVertex2i(100 * (i + 1), 100 * j);
                glVertex2i(100 * (i + 1), 100 * (j + 1));
                glVertex2i(100 * i, 100 * (j + 1));
                glEnd();
            }
        }
    }
}

class Property {
public:
    int numBullet = 1;
    int color[3] = { 0,0,0 };
    double speed = 10;
    int size = 3;
    int time = 3;
};

class Tank {

public:

    int width = 4;
    int length = 2;
    int barrel_length = 3;
    double angle = 6;
    bool tankState;
    int R, G, B;
    double artX;
    double artY;
    double artAngle;
    bool laser = false;
    double X_points[4]; // X_Uprigtht, X_Lowrigtht, X_Lowleft, X_Upleft
    double Y_points[4]; // Y_Uprigtht, Y_Lowrigtht, Y_Lowleft, Y_Upleft
    double CircleX[2], CircleY[2];
    double speed = 0.5;
    Property* BulletProperty;

    void Initialize(int cells[8][6], double angle, Property* property);
    void Color(int r, int g, int b);
    void Calculatepoints(double x, double y, double angle);
    bool CheckCollisionmaze(int walls[9][7][2]);
    bool CheckCollisiontank(double adversary_X, double adversary_Y);
    void Draw(int tanknum);
    void Laser(int walls[9][7][2]);
    void Move(int tanknum, double adversary_X, double adversary_Y, int walls[9][7][2]);

    bool CheckHitByBullet(double bulletX, double bulletY, int rad);

};

bool Tank::CheckHitByBullet(double bulletX, double bulletY, int rad) {
    int cx, cy;
    PhysicalCoordToScreenCoord(cx, cy, bulletX, bulletY);

    int tankcx1, tankcy1;
    int tankcx2, tankcy2;

    PhysicalCoordToScreenCoord(tankcx1, tankcy1, CircleX[0], CircleY[0]);
    PhysicalCoordToScreenCoord(tankcx2, tankcy2, CircleX[1], CircleY[1]);

    double tankRad = sqrt((double)(tankcx1 - tankcx2) * (tankcx1 - tankcx2) + (tankcy1 - tankcy2) * (tankcy1 - tankcy2));
    tankRad /= 2;

    double distance = sqrt((double)(tankcx1 - cx) * (tankcx1 - cx) + (tankcy1 - cy) * (tankcy1 - cy));

    if (distance <= (tankRad + rad)) {
        return true;
    }

    distance = sqrt((double)(tankcx2 - cx) * (tankcx2 - cx) + (tankcy2 - cy) * (tankcy2 - cy));

    if (distance <= (tankRad + rad)) {
        return true;
    }

    return false;

}

void Tank::Calculatepoints(double x, double y, double angle) {

    X_points[0] = x + width / 2 * cos(angle) + length / 2 * sin(angle);
    Y_points[0] = y + width / 2 * sin(angle) - length / 2 * cos(angle);

    X_points[1] = x - width / 2 * cos(angle) + length / 2 * sin(angle);
    Y_points[1] = y - width / 2 * sin(angle) - length / 2 * cos(angle);

    X_points[2] = x - width / 2 * cos(angle) - length / 2 * sin(angle);
    Y_points[2] = y - width / 2 * sin(angle) + length / 2 * cos(angle);

    X_points[3] = x + width / 2 * cos(angle) - length / 2 * sin(angle);
    Y_points[3] = y + width / 2 * sin(angle) + length / 2 * cos(angle);

    CircleX[0] = x + cos(angle);
    CircleY[0] = y + sin(angle);

    CircleX[1] = x - cos(angle);
    CircleY[1] = y - sin(angle);

}

void Tank::Initialize(int cells[8][6], double angle, Property* property) {
    bool done = false;
    while (done == false)
    {
        int x = rand() % 8;
        int y = rand() % 6;

        if (cells[x][y] == 1)
        {
            artX = 10 * x + 5;
            artY = 60 - (10 * y + 5);
            done = true;
        }
    }
    artAngle = angle;
    tankState = true;
    Calculatepoints(artX, artY, artAngle);
    BulletProperty = property;
}

void Tank::Color(int r, int g, int b) {
    R = r;
    G = g;
    B = b;
}

void Tank::Draw(int tanknum)
{
    int x1, y1, x2, y2, x3, y3, x4, y4;
    int tx, ty;
    int cx1, cx2, cy1, cy2;

    PhysicalCoordToScreenCoord(tx, ty, artX, artY);
    PhysicalCoordToScreenCoord(x1, y1, X_points[0], Y_points[0]);
    PhysicalCoordToScreenCoord(x2, y2, X_points[1], Y_points[1]);
    PhysicalCoordToScreenCoord(x3, y3, X_points[2], Y_points[2]);
    PhysicalCoordToScreenCoord(x4, y4, X_points[3], Y_points[3]);


    glColor3ub(R, G, B);
    Drawtank(x1, y1, x2, y2, x3, y3, x4, y4, 1);

    int bx, by;
    PhysicalCoordToScreenCoord(bx, by, artX + barrel_length * cos(artAngle), artY + barrel_length * sin(artAngle));

    if (tanknum == 1) {
        glColor3ub(255, 215, 0);
        DrawLine(tx, ty, bx, by);
        DrawCircle(tx, ty, 5, 1);
    }
    else if (tanknum == 2) {
        glColor3ub(0, 255, 0);
        DrawLine(tx, ty, bx, by);
        DrawCircle(tx, ty, 5, 1);
    }
}

void Tank::Laser(int walls[9][7][2])
{
    if (!laser) return;
    const int lr = 300;
    int x0, y0;
    double xl[lr], yl[lr];
    PhysicalCoordToScreenCoord(x0, y0, artX, artY);
    xl[0] = x0;
    yl[0] = y0;
    double a = artAngle;
    for (int i = 1; i < lr; i++)
    {
        xl[i] = xl[i - 1] + 2 * cos(a);
        yl[i] = yl[i - 1] - 2 * sin(a);
        int cx1, cx2, cy1, cy2;
        cx1 = floor(xl[i - 1] / 100);
        cx2 = floor(xl[i] / 100);
        cy1 = floor(yl[i - 1] / 100);
        cy2 = floor(yl[i] / 100);
        if (cx1!=cx2 && walls[maxi(cx1, cx2)][cy1][1])
        {
            if (a > 0) a = PI - a;
            else a = -PI - a;
            xl[i] = 2 * xl[i - 1] - xl[i];
        }
        if (cy1 != cy2 && walls[cx1][maxi(cy1, cy2)][0])
        {
            a = -a;
            yl[i] = 2 * yl[i - 1] - yl[i];
        }
    }
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0xff00);
    for (int i = 0; i < lr; i++)
    {
        glVertex2d(xl[i], yl[i]);
    }
    glEnd();
    glDisable(GL_LINE_STIPPLE);
}

void Tank::Move(int tanknum, double adversary_X, double adversary_Y, int walls[9][7][2]) {

    double X = adversary_X;
    double Y = adversary_Y;
    /* Control player 1 */
    if (tanknum == 1) {

        if (FsGetKeyState(FSKEY_LEFT))
        {
            artAngle -= angle;
            if (!(CheckCollisiontank(X, Y) && CheckCollisionmaze(walls))) {
                artAngle += angle;
            }
        }

        if (FsGetKeyState(FSKEY_RIGHT))
        {
            artAngle += angle;
            if (!(CheckCollisiontank(X, Y) && CheckCollisionmaze(walls))) {
                artAngle -= angle;
            }
        }

        if (FsGetKeyState(FSKEY_UP))
        {
            artX += speed * cos(artAngle);
            artY += speed * sin(artAngle);
            if (!(CheckCollisiontank(X, Y) && CheckCollisionmaze(walls))) {
                artX -= speed * cos(artAngle);
                artY -= speed * sin(artAngle);
            }
        }

        if (FsGetKeyState(FSKEY_DOWN))
        {
            artX -= speed * cos(artAngle);
            artY -= speed * sin(artAngle);
            if (!(CheckCollisiontank(X, Y) && CheckCollisionmaze(walls))) {
                artX += speed * cos(artAngle);
                artY += speed * sin(artAngle);
            }
        }
    }
    else {
        /* Control player 2 */
        if (FsGetKeyState(FSKEY_A))
        {
            artAngle -= angle;
            if (!(CheckCollisiontank(X, Y) && CheckCollisionmaze(walls))) {
                artAngle += angle;
            }
        }

        if (FsGetKeyState(FSKEY_D))
        {
            artAngle += angle;
            if (!(CheckCollisiontank(X, Y) && CheckCollisionmaze(walls))) {
                artAngle -= angle;
            }
        }

        if (FsGetKeyState(FSKEY_W))
        {
            artX += speed * cos(artAngle);
            artY += speed * sin(artAngle);
            if (!(CheckCollisiontank(X, Y) && CheckCollisionmaze(walls))) {
                artX -= speed * cos(artAngle);
                artY -= speed * sin(artAngle);
            }
        }

        if (FsGetKeyState(FSKEY_S))
        {
            artX -= speed * cos(artAngle);
            artY -= speed * sin(artAngle);
            if (!(CheckCollisiontank(X, Y) && CheckCollisionmaze(walls))) {
                artX += speed * cos(artAngle);
                artY += speed * sin(artAngle);
            }
        }
    }
}

bool Tank::CheckCollisionmaze(int walls[9][7][2]) {

    int x[4], y[4];

    Calculatepoints(artX, artY, artAngle);
    PhysicalCoordToScreenCoord(x[0], y[0], X_points[0], Y_points[0]);
    PhysicalCoordToScreenCoord(x[1], y[1], X_points[1], Y_points[1]);
    PhysicalCoordToScreenCoord(x[2], y[2], X_points[2], Y_points[2]);
    PhysicalCoordToScreenCoord(x[3], y[3], X_points[3], Y_points[3]);

    double barrelX = artX + barrel_length * cos(artAngle);
    double barrelY = artY + barrel_length * sin(artAngle);

    for (int i = 0; i < 9; i++)
    {

        for (int j = 0; j < 7; j++)
        {

            if (1 == walls[i][j][0])
            {
                for (int k = 0; k < 4; k++) {
                    if (((i * 100) <= x[k]) && (x[k] <= ((i + 1) * 100)) && ((j * 100 - 3) <= y[k]) && (y[k] <= (j * 100 + 3))) {
                        return false;
                    }
                    if (((i * 100) <= barrelX) && (barrelX <= ((i + 1) * 100)) && ((j * 100) <= barrelY) && (barrelY <= (j * 100))) {
                        return false;
                    }
                }
            }

            if (1 == walls[i][j][1])
            {
                for (int k = 0; k < 4; k++) {
                    if (((j * 100) <= y[k]) && (y[k] <= ((j + 1) * 100)) && ((i * 100 - 3) <= x[k]) && (x[k] <= (i * 100 + 3))) {
                        return false;
                    }
                    if (((j * 100) <= barrelY) && (barrelY <= ((j + 1) * 100)) && ((i * 100) <= barrelX) && (barrelX <= (i * 100))) {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

bool Tank::CheckCollisiontank(double adversary_X, double adversary_Y) {

    int cx, cy, cdx, cdy;
    double rad = sqrt(width * width + length * length) / 2 * 10;
    PhysicalCoordToScreenCoord(cdx, cdy, adversary_X, adversary_Y);
    PhysicalCoordToScreenCoord(cx, cy, artX, artY);

    const double YS_PI = 3.1415927;

    for (int i = 0; i < 180; i++) {

        double ad_ang = (double)i * YS_PI / 90.0;
        int ad_X = (double)cdx + cos(ad_ang) * rad;
        int ad_Y = (double)cdy + sin(ad_ang) * rad;

        for (int j = 0; j < 180; j++) {

            double ang = (double)j * YS_PI / 90.0;
            int X = (double)cx + cos(ang) * rad;
            int Y = (double)cy + sin(ang) * rad;

            if (X == ad_X && Y == ad_Y) return false;
        }
    }
    return true;
}



class Bullet {

public:
    int bulletState;
    double bulletX, bulletY, bulletVx, bulletVy;
    double prebulletX, prebulletY;
    double angle;
    int start_time;
    Property* bulletProperty;

    int bullet_alpha; // 255

    void Fire(double x, double y, double angle, Property* property);
    void Move(double dt);
    void Draw();
    bool bounceOnWall(int wall[9][7][2]);
    void bounceOnMaze(const Maze& m, double dt);
    bool fade();
};

void Bullet::Fire(double x, double y, double ang, Property* property) {
    bulletProperty = property;
    bulletProperty->speed = property->speed;
    bulletX = x;
    bulletY = y;
    angle = ang;
    bulletVx = bulletProperty->speed * cos(angle);
    bulletVy = bulletProperty->speed * sin(angle);

    int hour, min, sec;
    GetLocalTimeHourMinSec(hour, min, sec);
    start_time = hour * 3600 + min * 60 + sec;

    bullet_alpha = 255;


}

void Bullet::Move(double dt) {

    prebulletX = bulletX;
    prebulletY = bulletY;

    bulletX += bulletVx * dt;
    bulletY += bulletVy * dt;
}

void Bullet::Draw() {
    int sx, sy;
    PhysicalCoordToScreenCoord(sx, sy, bulletX, bulletY);
    int r = bulletProperty->color[0], g = bulletProperty->color[1], b = bulletProperty->color[2];
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4ub(r, g, b, bullet_alpha);
    DrawCircle(sx, sy, bulletProperty->size, 1);
    glDisable(GL_BLEND);
}

void Bullet::bounceOnMaze(const Maze& m, double dt) {
    int cx, cy;
    int rad = bulletProperty->size;
    PhysicalCoordToScreenCoord(cx, cy, bulletX, bulletY);

    const double YS_PI = 3.1415927;

    int i;
    for (i = 0; i < 180; i++)
    {
        double ang = (double)i * YS_PI / 90; // 32
        int x = (double)cx + cos(ang) * rad;
        int y = (double)cy + sin(ang) * rad;

        int wallX = x / 100;
        int wallY = y / 100;

        if (m.walls[wallX][wallY][0] == 1 && y % 100 == 0 && m.walls[wallX][wallY][1] == 1 && x % 100 == 0) {
            bulletVx = -bulletVx;
            break;
        }

        if (m.walls[wallX][wallY][0] == 1 && y % 100 == 0) {
            bulletVy = -bulletVy;

            break;
        }
        else if (m.walls[wallX][wallY][1] == 1 && x % 100 == 0) {
            bulletVx = -bulletVx;
            break;
        }
    }
}

bool Bullet::bounceOnWall(int wall[9][7][2]) {
    int i1, i2, j1, j2;
    i1 = int();
    double rad = bulletProperty->size;


    int x1, y1, x2, y2;
    PhysicalCoordToScreenCoord(x1, y1, prebulletX, prebulletY);
    PhysicalCoordToScreenCoord(x2, y2, bulletX, bulletY);
    i1 = int(floor(x1 / 100.0));
    j1 = int(floor(y1 / 100.0));
    i2 = int(floor(x2 / 100.0));
    j2 = int(floor(y2 / 100.0));

    if (i1 != i2 && wall[maxi(i1, i2)][j1][1]) {
        bulletVx = -bulletVx;
        bulletX = prebulletX;
        return true;
    }

    if (j1 != j2 && wall[i1][maxi(j1, j2)][0]) {
        bulletVy = -bulletVy;
        bulletY = prebulletY;
        return true;
    }
    return false;
}


bool Bullet::fade() {
    int hour, min, sec;
    GetLocalTimeHourMinSec(hour, min, sec);
    int cur_time = hour * 3600 + min * 60 + sec;

    if (cur_time - start_time > bulletProperty->time - 1) {
        bullet_alpha -= 5;
    }

    if (cur_time - start_time > bulletProperty->time) {
        bulletState = 0;
        return true;
    }
    return false;
}

void setmap(Maze& m, int map[600][800]) {
    printf("Set map\n");
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            if (1 == m.walls[i][j][0])
            {
                for (int l = 0; l < 100; l++)
                {
                    map[j * 100][(i * 100) + l] = 1;
                }
            }

            if (1 == m.walls[i][j][1])
            {
                for (int l = 0; l < 100; l++)
                {
                    map[(j * 100) + l][(i * 100)] = 2;
                }
            }
        }
    }

}

class Draw
{
public:
    int key1 = -1;
    int key2 = -1;
    int timecounter = 0;
    // Helper Functions
    void DrawRect(int x1, int y1, int x2, int y2, int fill);
    void DrawRectf(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int fill);
    void DrawLine(int x1, int y1, int x2, int y2);
    void DrawLines(double x1[], double x2[], int size);
    void DrawCircle(int cx, int cy, int rad, int fill);
    void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int fill);
    int RotateX(int x, int y, int cx, int cy, double radians);
    int RotateY(int x, int y, int cx, int cy, double radians);
    int RotateXd(double x, double y, double cx, double cy, double radians);
    int RotateYd(double x, double y, double cx, double cy, double radians);
    int CheckMouse(int mx, int my, int cx[], int cy[], int w[], int size);
    void DrawHeader(int cx, int cy, int truckH, int truckW, int player);
    void DrawTitle(int cx, int cy);
    void DrawEnd(int player, int cx, int cy);
    void DrawKeyOptions(int cx, int cy, int keyH, int player);
    int CheckKey(int click);
    int GetKey1(void);
    int GetKey2(void);
    void GetBonusItemColors(int bonusN);
    void DrawBonusFeature(int cx, int cy, int boxH, int bonusN);

    // Main Functions
    // void DrawMaze(int cx[], int cy[], int gridH); //Confused
    void DrawTruck(int cx, int cy, int player, int truckH, int truckW, int angle, int bonus);
    void DrawBonusItem(int cx, int cy, int boxH, int bonusN);
    void DrawFirstWindow(int keyX[], int keyY[],
        int keyH, int keySize,
        int truckX[], int truckY[],
        int truckSize,
        int truckW, int truckH);
    void DrawScore(int score1, int score2, int cx, int cy);

    //Testing & Debugging
    void DrawBonus(int x, int y, int bonusN);
    int TestingMainMenu(void);
    void TestingExplosion(void);
    void TestingScore(void);
    void TestingTruck(void);
    void TestingEnd(int player);
    // not sure about CannonBall & CannonBallTrajectory (will be used?)


};

int Draw::GetKey1(void)
{
    return key1;
}

int Draw::GetKey2(void)
{
    return key2;
}

void Draw::DrawScore(int score1, int score2, int cx, int cy)
{
    /*
     * Score1: player 1 score
     * Score2: player 2 score
     * cx: x location of player 1
     * cy: y location of player 2
     * This function draws the score of each player
     */
    std::string t1 = std::to_string(score1);
    char const* player1 = t1.c_str();

    std::string t2 = std::to_string(score2);
    char const* player2 = t2.c_str();

    glColor3ub(0, 0, 0);
    glRasterPos2d(cx, cy);
    YsGlDrawFontBitmap16x20(player1);

    glColor3ub(0, 0, 0);
    DrawRect(cx - 4, cy - 24, cx + 35, cy + 6, 0);


    glColor3ub(90, 90, 90);
    glRasterPos2d(cx - 35, cy);
    YsGlDrawFontBitmap16x20("P1");

    glColor3ub(0, 0, 0);
    glRasterPos2d(cx, cy + 30);
    YsGlDrawFontBitmap16x20(player2);

    glColor3ub(0, 0, 0);
    DrawRect(cx - 4, cy + 6, cx + 35, cy + 36, 0);

    glColor3ub(0, 0, 255);
    glRasterPos2d(cx - 35, cy + 30);
    YsGlDrawFontBitmap16x20("P2");
}

void Draw::DrawHeader(int cx, int cy, int truckH, int truckW, int player)
{
    if (player == 1)
    {
        glColor3ub(0, 0, 0);
        glRasterPos2d(cx - (int)(truckW * ((double)5 / 12)), cy - truckH);
        YsGlDrawFontBitmap16x20("Player 1");
    }
    if (player == 2)
    {
        glColor3ub(0, 0, 0);
        glRasterPos2d(cx - (int)(truckW * ((double)5 / 12)), cy - truckH);
        YsGlDrawFontBitmap16x20("Player 2");
    }
}

void Draw::DrawTitle(int cx, int cy)
{
    glColor3ub(255, 0, 0);
    glRasterPos2d(cx - (cx / 4), cy);
    YsGlDrawFontBitmap20x32("Maze Wars");

}

void Draw::DrawEnd(int player, int cx, int cy)
{
    (player == 1) ? glColor3ub(90, 90, 90) : glColor3ub(0, 0, 255); // grey or blue
    int totalwidth = 448;
    glRasterPos2d(cx - (totalwidth / 2), cy);
    if (player == 1)
    {
        YsGlDrawFontBitmap32x48("Player 1 won!!");
    }
    if (player == 2)
    {
        YsGlDrawFontBitmap32x48("Player 2 won!!");
    }

    int truckW = 150;
    int truckH = 100;
    int truckX = 400;
    int truckY = 250;

    DrawTruck(truckX, truckY, player, truckH, truckW, 0.0, 0);
}

void Draw::DrawKeyOptions(int cx, int cy, int keyH, int option)
{
    // cx=125,cy=450,keyH=50
    if (option == 1)
    {
        glColor3ub(0, 0, 0);
        // bottom left corner
        // glRasterPos2d(66,434);
        glRasterPos2d(cx - (int)(keyH * ((double)7 / 6)), cy - (int)(keyH * ((double)1 / 3)));
        YsGlDrawFontBitmap16x20("Q");

        glRasterPos2d(cx - (int)(keyH * ((double)1 / 8)), cy - (int)(keyH * ((double)1 / 3)));
        YsGlDrawFontBitmap16x20("W");

        glRasterPos2d(cx - (int)(keyH * ((double)7 / 6)), cy + (int)(keyH * ((double)2 / 3)));
        YsGlDrawFontBitmap16x20("A");

        glRasterPos2d(cx - (int)(keyH * ((double)1 / 8)), cy + (int)(keyH * ((double)2 / 3)));
        YsGlDrawFontBitmap16x20("S");

        glRasterPos2d(cx + (int)(keyH * ((double)5 / 6)), cy + (int)(keyH * ((double)2 / 3)));
        YsGlDrawFontBitmap16x20("D");

        //Q
        glColor3ub(0, 0, 0);
        DrawRect(cx - (int)(keyH * ((double)3 / 2)),
            cy - keyH,
            cx - (int)((double)keyH / 2),
            cy, 0);
        // W
        glColor3ub(0, 0, 0);
        DrawRect(cx - (int)(keyH * ((double)1 / 2)),
            cy - keyH,
            cx + (int)((double)keyH / 2),
            cy, 0);
        // A
        glColor3ub(0, 0, 0);
        DrawRect(cx - (int)(keyH * ((double)3 / 2)),
            cy,
            cx - (int)((double)keyH / 2),
            cy + keyH, 0);
        // S
        glColor3ub(0, 0, 0);
        DrawRect(cx - (int)(keyH * ((double)1 / 2)),
            cy,
            cx + (int)((double)keyH / 2),
            cy + keyH, 0);
        // D
        glColor3ub(0, 0, 0);
        DrawRect(cx + (int)(keyH * ((double)1 / 2)),
            cy,
            cx + (int)(keyH * ((double)3 / 2)),
            cy + keyH, 0);
    }

    // cx=650,cy=450,keyH=50
    if (option == 2)
    {
        glColor3ub(0, 0, 0);
        // bottom left corner
        glRasterPos2d(cx - (int)(keyH * ((double)7 / 4)), cy - (int)(keyH * ((double)1 / 3)));
        YsGlDrawFontBitmap16x20("Space");

        // Up Arrow
        DrawTriangle(cx + (int)(keyH * ((double)1 / 3)),
            cy - (int)(keyH * ((double)1 / 3)),
            cx + (int)(keyH * ((double)1 / 2)),
            cy - (int)(keyH * ((double)2 / 3)),
            cx + (int)(keyH * ((double)2 / 3)),
            cy - (int)(keyH * ((double)1 / 3)), 1);
        // Left Arrow
        DrawTriangle(cx - (int)(keyH * ((double)1 / 3)),
            cy + (int)(keyH * ((double)1 / 3)),
            cx - (int)(keyH * ((double)2 / 3)),
            cy + (int)(keyH * ((double)1 / 2)),
            cx - (int)(keyH * ((double)1 / 3)),
            cy + (int)(keyH * ((double)2 / 3)), 1);
        //Down Arrow
        DrawTriangle(cx + (int)(keyH * ((double)1 / 3)),
            cy + (int)(keyH * ((double)1 / 3)),
            cx + (int)(keyH * ((double)1 / 2)),
            cy + (int)(keyH * ((double)2 / 3)),
            cx + (int)(keyH * ((double)2 / 3)),
            cy + (int)(keyH * ((double)1 / 3)), 1);
        // Right Arrow
        DrawTriangle(cx + (int)(keyH * ((double)4 / 3)),
            cy + (int)(keyH * ((double)1 / 3)),
            cx + (int)(keyH * ((double)5 / 3)),
            cy + (int)(keyH * ((double)1 / 2)),
            cx + (int)(keyH * ((double)4 / 3)),
            cy + (int)(keyH * ((double)2 / 3)), 1);

        // SPACE
        glColor3ub(0, 0, 0);
        DrawRect(cx - (keyH * 2),
            cy - keyH,
            cx,
            cy, 0);
        // Up Arrow
        glColor3ub(0, 0, 0);
        DrawRect(cx,
            cy - keyH,
            cx + keyH,
            cy, 0);
        // Left Arrow
        glColor3ub(0, 0, 0);
        DrawRect(cx - keyH,
            cy,
            cx,
            cy + keyH, 0);
        // Down Arrow
        glColor3ub(0, 0, 0);
        DrawRect(cx,
            cy,
            cx + keyH,
            cy + keyH, 0);
        // Right Arrow
        glColor3ub(0, 0, 0);
        DrawRect(cx + keyH,
            cy,
            cx + (2 * keyH),
            cy + keyH, 0);
    }
}

int Draw::RotateXd(double x, double y, double cx, double cy, double radians)
{
    // clockwise rotation
    return cx + ((x - cx) * cos(radians)) - ((y - cy) * sin(radians));
}

int Draw::RotateYd(double x, double y, double cx, double cy, double radians)
{
    //clockwise rotation
    return cy + ((x - cx) * sin(radians)) + ((y - cy) * cos(radians));
}

void Draw::DrawLines(double x1[], double y1[], int size)
{
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < size; i++)
    {
        glVertex2d(x1[i], y1[i]);
    }

    glEnd();
}

int Draw::RotateX(int x, int y, int cx, int cy, double radians)
{
    return cx + (int)round((double)(x - cx) * cos(radians)) - (int)round((double)(y - cy) * sin(radians));
}

int Draw::RotateY(int x, int y, int cx, int cy, double radians)
{
    return cy + (int)round((double)(x - cx) * sin(radians)) + (int)round((double)(y - cy) * cos(radians));
}

void Draw::DrawBonusFeature(int cx, int cy, int boxH, int bonusN)
{
    //glColor3ub(0, 0, 0);
    // glRasterPos2d(cx + (int)(boxH * ((double)4 / 6)), cy);
    int xd, yd;
    xd = cx - int(boxH / 4.0);
    yd = cy + int(boxH / 4.0);
    glRasterPos2d(xd, yd);
    switch (bonusN) //7
    {
     glRasterPos2d(cx, cy);
    case 0:
        
        YsGlDrawFontBitmap16x20("B");
        break;
    case 1:
        YsGlDrawFontBitmap16x20("S");
        break;
    case 2:
        YsGlDrawFontBitmap16x20("B");
        break;
    case 3:
        YsGlDrawFontBitmap16x20("S");
        break;
    case 4:
        YsGlDrawFontBitmap16x20("L");
        break;
    }
}

void Draw::GetBonusItemColors(int bonusN)
{
    switch (bonusN) //7
    {
    case 0:
        glColor3ub(255, 0, 0);     //red
        break;
    case 1:
        glColor3ub(255, 0, 0);     //red
        //glColor3ub(160, 32, 240);  //lavendar
        break;
    case 2:
        glColor3ub(255, 16, 240);  //neon-pink
        break;
    case 3:
        glColor3ub(255, 16, 240);  //neon-pink
        //glColor3ub(13, 152, 186);  //blue-green
        break;
    case 4:
        glColor3ub(255, 16, 240);  //neon-pink
        //glColor3ub(255, 140, 0);   //dark-orange
        break;
    case 5:
        glColor3ub(154, 205, 50);  //yellow-green
        break;
    case 6:
        glColor3ub(128, 128, 128); //grey
        break;
    case 7: // player 1
        glColor3ub(255, 215, 0);   //gold
        break;
    case 8: // player 2
        glColor3ub(0, 255, 0);     //green
        break;
    }
}
void Draw::DrawBonusItem(int cx, int cy, int boxH, int bonusN)
{
    // draw.DrawBonusItem(600, 250, 100, 1);
    /*
     * cx: bonus item x center
     * cy: bonus item y center
     * bonusN: the type of bonus item
     * boxH: box height and width
     * This function draws a bonus item (7 total)
     */
    const double PI = 3.1415927;

    // Top Ribbon
    // (bonusN==1) ? glColor3ub(255,0,0) : glColor3ub(160,32,240);
    GetBonusItemColors(bonusN);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++)
    {
        double radians = (double)i * (PI / 180.0);
        double x = (((double)boxH * ((double)5 / 12)) * sin(4.0 * radians) * sin(radians)) + (double)cx;
        double y = (((double)boxH * ((double)5 / 12)) * sin(4.0 * radians) * cos(radians)) + (double)cy - ((double)boxH / 2);
        glVertex2d(x, y);
    }
    glEnd();

    // Top Ribbon Outline
    glColor3ub(0, 0, 0);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; i++)
    {
        double radians = (double)i * (PI / 180.0);
        double x = (((double)boxH * ((double)5 / 12)) * sin(4.0 * radians) * sin(radians)) + (double)cx;
        double y = (((double)boxH * ((double)5 / 12)) * sin(4.0 * radians) * cos(radians)) + (double)cy - ((double)boxH / 2);
        glVertex2d(x, y);
    }
    glEnd();

    // Back of Gift Box
    glColor3ub(255, 215, 0);
    DrawRect(cx - (int)((double)boxH / 2),
        cy - (int)((double)boxH / 2),
        cx + (int)((double)boxH / 2),
        cy + (int)((double)boxH / 2), 1);
    // Back of Gift Box Outline
    glColor3ub(0, 0, 0);
    DrawRect(cx - (int)((double)boxH / 2),
        cy - (int)((double)boxH / 2),
        cx + (int)((double)boxH / 2),
        cy + (int)((double)boxH / 2), 0);

    // Horizontal Ribbon
    GetBonusItemColors(bonusN);
    DrawRect(cx - (int)((double)boxH / 2),
        cy - (int)((double)boxH / 12),
        cx + (int)((double)boxH / 2),
        cy + (int)((double)boxH / 12), 1);
    // Horizontal Ribbon Outline
    glColor3ub(0, 0, 0);
    DrawRect(cx - (int)((double)boxH / 2),
        cy - (int)((double)boxH / 12),
        cx + (int)((double)boxH / 2),
        cy + (int)((double)boxH / 12), 0);

    // Vertical Ribbon
    GetBonusItemColors(bonusN);
    DrawRect(cx - (int)((double)boxH / 12),
        cy - (int)((double)boxH / 2),
        cx + (int)((double)boxH / 12),
        cy + (int)((double)boxH / 2), 1);
    // Vertical Ribbon Outline
    glColor3ub(0, 0, 0);
    DrawRect(cx - (int)((double)boxH / 12),
        cy - (int)((double)boxH / 2),
        cx + (int)((double)boxH / 12),
        cy + (int)((double)boxH / 2), 0);
}

void Draw::DrawTruck(int cx, int cy, int player, int truckH, int truckW, int angle, int bonusN)
{
    //ONLY CANNON SHAFT ROTATES CLOCKWISE
    //draw.DrawTruck(200, 250, 1, 100, 150, 30, 0);
    /*
     * cx: truck x center
     * cy: truck y center
     * player: player number
     * truckH: truck height
     * truckW: truck width
     * angle: angle of cannon shaft (clockwise)
     * bonus: the type of bonus item collected,
       shaft display changes based on bonus items
     * This function draws the truck
     */
    const double PI = 3.1415927;
    double radians = angle * (PI / 180.0);
    // (condition) ? expressionTrue : expressionFalse;
    // Truck body
    (player == 1) ? glColor3ub(90, 90, 90) : glColor3ub(0, 0, 255); // grey or blue
    DrawRect(cx - (int)((double)truckW / 2),
        cy - (int)((double)truckH / 2),
        cx + (int)((double)truckW / 2),
        cy + (int)((double)truckH / 2), 1);
    glColor3ub(0, 0, 0);
    DrawLine(cx - (int)((double)truckW / 2),
        cy - (int)((double)truckH / 2),
        cx - (int)((double)truckW / 2),
        cy + (int)((double)truckH / 2));
    DrawLine(cx + (int)((double)truckW / 2),
        cy - (int)((double)truckH / 2),
        cx + (int)((double)truckW / 2),
        cy + (int)((double)truckH / 2));

    //Truck Wheels
    int x, y1, y2;
    x = cx - (int)((double)truckW / 2);
    //printf("%d\n",x);
    y1 = cy - (int)((double)truckH / 2);
    y2 = cy + (int)(truckH * ((double)3 / 8));
    for (double i = 1; i <= 6; i++)
    {
        glColor3ub(0, 0, 0);
        DrawRect(x, y1, (int)(x + ((double)truckW / 6)), (int)(y1 + ((double)truckH / 8)), 0);

        glColor3ub(0, 0, 0);
        DrawRect(x, y2, (int)(x + ((double)truckW / 6)), (int)(y2 + ((double)truckH / 8)), 0);
        x += (int)((double)truckW / 6);
        //printf("%d\n",x);
    }

    // Truck Shaft
    // (player==1) ? glColor3ub(255,215,0) : glColor3ub(0,255,0); // gold or green
    (player == 1) ? glColor3ub(255, 215, 0) : glColor3ub(0, 255, 0);
    DrawRectf(RotateXd((double)cx, (double)cy - ((double)truckH / 8), (double)cx, (double)cy, radians),
        RotateYd((double)cx, (double)cy - ((double)truckH / 8), (double)cx, (double)cy, radians),
        RotateXd((double)cx + ((double)truckW * ((double)2 / 3)), (double)cy - ((double)truckH / 8), (double)cx, (double)cy, radians),
        RotateYd((double)cx + ((double)truckW * ((double)2 / 3)), (double)cy - ((double)truckH / 8), (double)cx, (double)cy, radians),
        RotateXd((double)cx + ((double)truckW * ((double)2 / 3)), (double)cy + ((double)truckH / 8), (double)cx, (double)cy, radians),
        RotateYd((double)cx + ((double)truckW * ((double)2 / 3)), (double)cy + ((double)truckH / 8), (double)cx, (double)cy, radians),
        RotateXd((double)cx, (double)cy + ((double)truckH / 8), (double)cx, (double)cy, radians),
        RotateYd((double)cx, (double)cy + ((double)truckH / 8), (double)cx, (double)cy, radians), 1);

    glColor3ub(0, 0, 0);
    DrawRectf(RotateXd((double)cx, (double)cy - ((double)truckH / 8), (double)cx, (double)cy, radians),
        RotateYd((double)cx, (double)cy - ((double)truckH / 8), (double)cx, (double)cy, radians),
        RotateXd((double)cx + ((double)truckW * ((double)2 / 3)), (double)cy - ((double)truckH / 8), (double)cx, (double)cy, radians),
        RotateYd((double)cx + ((double)truckW * ((double)2 / 3)), (double)cy - ((double)truckH / 8), (double)cx, (double)cy, radians),
        RotateXd((double)cx + ((double)truckW * ((double)2 / 3)), (double)cy + ((double)truckH / 8), (double)cx, (double)cy, radians),
        RotateYd((double)cx + ((double)truckW * ((double)2 / 3)), (double)cy + ((double)truckH / 8), (double)cx, (double)cy, radians),
        RotateXd((double)cx, (double)cy + ((double)truckH / 8), (double)cx, (double)cy, radians),
        RotateYd((double)cx, (double)cy + ((double)truckH / 8), (double)cx, (double)cy, radians), 0);

    //Truck Center
    (player == 1) ? glColor3ub(255, 215, 0) : glColor3ub(0, 255, 0); // gold or green
    DrawCircle(cx, cy, (int)((double)truckH / 4), 1);

    glColor3ub(0, 0, 0);
    DrawCircle(cx, cy, (int)((double)truckH / 4), 0);
}

void Draw::DrawRect(int x1, int y1, int x2, int y2, int fill)
{
    if (0 != fill)
    {
        glBegin(GL_QUADS);
    }
    else
    {
        glBegin(GL_LINE_LOOP);
    }

    glVertex2i(x1, y1);
    glVertex2i(x2, y1);
    glVertex2i(x2, y2);
    glVertex2i(x1, y2);

    glEnd();
}

void Draw::DrawRectf(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int fill)
{
    //full rectangle
    if (0 != fill)
    {
        glBegin(GL_QUADS);
    }
    else
    {
        glBegin(GL_LINE_LOOP);
    }

    glVertex2d(x1, y1);
    glVertex2d(x2, y2);
    glVertex2d(x3, y3);
    glVertex2d(x4, y4);

    glEnd();
}

void Draw::DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int fill)
{
    if (0 != fill)
    {
        glBegin(GL_TRIANGLES);
    }
    else
    {
        glBegin(GL_LINE_LOOP);
    }
    glVertex2i(x1, y1);
    glVertex2i(x2, y2);
    glVertex2i(x3, y3);
    glEnd();


}

void Draw::DrawLine(int x1, int y1, int x2, int y2)
{
    glBegin(GL_LINES);

    glVertex2i(x1, y1);
    glVertex2i(x2, y2);

    glEnd();
}

void Draw::DrawCircle(int cx, int cy, int rad, int fill)
{
    const double PI = 3.1415927;
    if (0 != fill)
    {
        glBegin(GL_POLYGON);
    }
    else
    {
        glBegin(GL_LINE_LOOP);
    }

    int i;
    for (i = 0; i < 64; i++)
    {
        double angle = (double)i * PI / 32.0;
        double x = (double)cx + cos(angle) * (double)rad;
        double y = (double)cy + sin(angle) * (double)rad;
        glVertex2d(x, y);
    }

    glEnd();
}

int Draw::CheckMouse(int mx, int my, int cx[], int cy[], int w[], int size)
{
    //centers
    int idx = -1;
    for (int i = 0; i < size; i++)
    {
        int rx = mx - (cx[i] - (w[i] / 2));
        int ry = my - (cy[i] - (w[i] / 2));
        if (0 <= rx && rx <= w[i] && 0 <= ry && ry <= w[i])
        {
            idx = i + 1;
            break;
        }
    }
    return idx;
}

int Draw::CheckKey(int click)
{
    if (click > 0)
    {
        if (key1 < 0)
        {
            key1 = click;
        }
        if (key2 < 0 && click != key1 && key1>0)
        {
            key2 = click;
        }
    }
    else
    {
        return 0;
    }

    if (key1 > 0 && key2 > 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void Draw::DrawFirstWindow(int keyX[], int keyY[], int keyH, int keySize, int truckX[], int truckY[], int truckSize, int truckW, int truckH)
{
    /*
     * keyX[]: x locations of type of input keys available
     * keyY[]: y locations of type of input keys available
     * keyH: key height and width
     * keySize: size of key array
     * truckX[]: x locations of trucks (player 1 and 2)
     * truckY[]: y locations of trucks (player 1 and 2)
     * truckSize: size of truck array
     * truckH: truck height
     * truckW: truck width
     * This function displays the trucks for each player and the keys available to each player
     */
     // centers of keys
     // int cx[]={key1X,key2X,key3X};
     // int cy[]={key1Y,key2Y,key3Y};
     // int w[]={3*keyH, 4*keyH, 4*keyH};
    int w[] = { 3 * keyH, 4 * keyH };
    /*
    draw.DrawKeyOptions(125,450,50,1);
    draw.DrawKeyOptions(650,450,50,2);
    draw.DrawKeyOptions(400,450,50,3);
     */
    DrawTitle(400, 50);
    int clicked = -1;
    int click = -1;
    for (int i = 0; i < keySize; i++)
    {
        DrawKeyOptions(keyX[i], keyY[i], keyH, i + 1);
    }
    for (int i = 0; i < truckSize; i++)
    {
        // void DrawTruck(int cx, int cy, int player, int truckH, int truckW, int angle, int bonus);
        // printf("player %d\n",i+1);
        DrawTruck(truckX[i], truckY[i], i+1, truckH, truckW, 0.0, 0);
        DrawHeader(truckX[i], truckY[i], truckH, truckW, i + 1);
    }

    /*
    int lb, mb, rb, mx, my;
    if (FSMOUSEEVENT_LBUTTONDOWN == FsGetMouseEvent(lb, mb, rb, mx, my))
    {
        click = CheckMouse(mx, my, keyX, keyY, w, keySize);
    }

    clicked = CheckKey(click);

    if (key1 > 0)
    {
        DrawHeader(keyX[key1 - 1] - keyH, keyY[key1 - 1] - (keyH / 2), keyH, keyH, 1);
    }
    if (key2 > 0)
    {
        DrawHeader(keyX[key2 - 1] - keyH, keyY[key2 - 1] - (keyH / 2), keyH, keyH, 2);
    }*/


    // return clicked;
}

void Draw::DrawBonus(int x, int y, int bonusN)
{
    DrawBonusItem(x, y, 30, bonusN);
    DrawBonusFeature(x, y, 30, bonusN);
}

void Draw::TestingScore(void)
{
    DrawScore(10, 20, 100, 550);
}

void Draw::TestingTruck(void)
{
    // DrawTruck(int cx, int cy, int player, int truckH, int truckW, int angle, int bonusN)
    DrawTruck(200, 250, 1, 100, 150, 30, 2);
}

int Draw::TestingMainMenu(void)
{
    int keyX[] = { 600,200 };
    int keySize = 2;
    int keyY[] = { 450,450 };
    int keyH = 50;

    int truckW = 150;
    int truckH = 100;
    int truckX[] = { 200,600 };
    int truckY[] = { 250,250 };
    int truckSize = 2;

    DrawFirstWindow(keyX, keyY, keyH, keySize, truckX, truckY, truckSize, truckW, truckH);
    return 0;
}

void Draw::TestingEnd(int player)
{
    DrawEnd(player, 430, 400);
}



class BonusItem
{
public:
    int type = -1, types = 5;
    int x_grid, y_grid;
    int receiver = -2, enemy = -2;
    double timer = 0, time = 15;
    void Appear(int cells[8][6]);
    void Draw(class Draw draw);
    void CheckReceived(vector<Tank*>& tanks);
    void Step(double dt, vector<Tank*>& tanks);
    void Apply(vector<Tank*>& tanks);
    void Finish(vector<Tank*>& tanks);

    //    Draw *draw = new Draw();
    //    void DrawBonusFeature(int cx, int cy, int boxH, int bonusN);
    //    void DrawBonusItem(int cx, int cy, int boxH, int bonusN);
    //    void GetBonusItemColors(int bonusN);
};

void BonusItem::Appear(int cells[8][6])
{
    if (receiver > -2) return;
    if (rand() % 1000 > 0) return;
    x_grid = rand() % 8;
    y_grid = rand() % 6;
    if (cells[x_grid][y_grid] == 0) return;
    receiver = -1;
    enemy = -1;
    type = rand() % types;
}

void BonusItem::Draw(class Draw draw)
{
    if (receiver != -1) return;
    glColor3f(0.0, 0.0, 0.0);

    int x, y;
    x = 50 + 100 * x_grid;
    y = 50 + 100 * y_grid;
    draw.DrawBonus(x, y, type);
    /*switch (type) {

    case 0:
        YsGlDrawFontBitmap12x16("B-");
        break;

    case 1:
        YsGlDrawFontBitmap12x16("S-");
        break;

    case 2:
        YsGlDrawFontBitmap12x16("B+");
        break;

    case 3:
        YsGlDrawFontBitmap12x16("S+");
        break;

    default:
        break;

    }*/

}

void BonusItem::CheckReceived(vector<Tank*>& tanks)
{
    if (receiver != -1) return;
    for (int i = 0; i < 2; i++)
    {
        int x, y;
        PhysicalCoordToScreenCoord(x, y, tanks[i]->artX, tanks[i]->artY);
        int xb, yb;
        xb = 50 + 100 * x_grid;
        yb = 50 + 100 * y_grid;
        int dx, dy, d;
        dx = xb - x;
        dy = yb - y;
        d = sqrt(dx * dx + dy * dy);
        if (d < 30) // check based on x and y
        {
            receiver = i;
            enemy = (i + 1) % 2;
            timer = time;
            Apply(tanks);
            return;
        }


    }
}

void BonusItem::Step(double dt, vector<Tank*>& tanks)
{
    if (receiver < 0) return;
    timer -= dt;
    if (timer < 0)
    {
        timer = 0;
        Finish(tanks);
    }
}

void BonusItem::Apply(vector<Tank*>& tanks)
{
    if (receiver < 0) return;
    switch (type) {

    case 0:
        tanks[enemy]->BulletProperty->speed *= 0.6;
        break;

    case 1:
        tanks[enemy]->speed *= 0.6;
        break;

    case 2:
        tanks[receiver]->BulletProperty->speed *= 1.4;
        break;

    case 3:
        tanks[receiver]->speed *= 1.4;
        break;

    case 4:
        tanks[receiver]->laser = true;
        break;


    default:
        break;
    }
}

void BonusItem::Finish(vector<Tank*>& tanks)
{
    switch (type) {

    case 0:
        tanks[enemy]->BulletProperty->speed /= 0.6;
        break;

    case 1:
        tanks[enemy]->speed /= 0.6;
        break;

    case 2:
        tanks[receiver]->BulletProperty->speed /= 1.4;
        break;

    case 3:
        tanks[receiver]->speed /= 1.4;
        break;
    
    case 4:
        tanks[receiver]->laser = false;
        break;

    default:
        break;

    }
    type = -1;
    receiver = -2;
    enemy = -2;
}

int main(void) {

    Draw draw; // declare draw class
    srand(time(NULL));
    FsOpenWindow(16, 16, 800, 700, 1);
    int score1 = 0;
    int score2 = 0;
    int clicked = 0;
    int endgame = 3;

    while (draw.timecounter < 200) {
        FsPollDevice();
        int key = FsInkey();
        if (key == FSKEY_ESC)
        {
            return 0;
        }
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        clicked = draw.TestingMainMenu();
        draw.timecounter++;

        FsSwapBuffers();
        FsSleep(20);
    }


    while (score1 < endgame && score2 < endgame)
    {
        Maze m;
        int valid = 0;
        while (0 == valid)
        {
            //printf("Initialize a maze...");
            m.Initialize_Cells();
            m.Initialize_Walls();
            m.assign_walls();
            valid = m.check_maze();
        }

        int explosionCount = 0;
        double expX[10], expY[10];
        int expT[10];

        Property* tank1Property = new Property();
        Property* tank2Property = new Property();
        tank1Property->color[0] = 255;
        tank1Property->color[1] = 215;
        tank1Property->color[2] = 0;

        tank2Property->color[0] = 0;
        tank2Property->color[1] = 255;
        tank2Property->color[2] = 0;

        vector<Tank*> tanks;

        Tank* tank1 = new Tank();
        tank1->Initialize(m.cells, PI / 6.0, tank1Property);
        tank1->Color(90, 90, 90);

        Tank* tank2 = new Tank();
        tank2->Initialize(m.cells, PI / 6.0, tank2Property);
        tank2->Color(0, 0, 255);

        double tanks_distance = 0;
        tanks_distance = pow((tank1->artX - tank2->artX), 2) + pow((tank1->artY - tank2->artY), 2);
        while (tanks_distance < 1600)
        {
            tank2->Initialize(m.cells, PI / 6.0, tank2Property);
            tanks_distance = pow((tank1->artX - tank2->artX), 2) + pow((tank1->artY - tank2->artY), 2);
        }

        tanks.push_back(tank1);
        tanks.push_back(tank2);

        // Initialize bullets
        vector<Bullet*> bulletsTank1;
        int nBullet = 5;
        int nBulletCurr = 0;

        for (int i = 0; i < nBullet; i++) {
            Bullet* bullet = new Bullet();
            bullet->bulletState = 0;
            bulletsTank1.push_back(bullet);
        }

        vector<Bullet*> bulletsTank2;
        int nBullet2 = 5;
        int nBulletCurr2 = 0;

        for (int i = 0; i < nBullet2; i++) {
            Bullet* bullet = new Bullet();
            bullet->bulletState = 0;
            bulletsTank2.push_back(bullet);
        }

        int n_bonus = 5;
        vector <BonusItem*> bonuses;
        for (int i = 0; i < n_bonus; i++) {
            BonusItem* bonus = new BonusItem();
            bonuses.push_back(bonus);
        }

        while ((tank2->tankState && tank1->tankState) || explosionCount > 0)
        {
            FsPollDevice();

            int key = FsInkey();
            if (FSKEY_ESC == key)
            {
                return 0;
            }

            if (FSKEY_SPACE == key) {
                if (nBulletCurr < nBullet) {
                    nBulletCurr++;
                    for (int i = 0; i < bulletsTank1.size(); i++) {
                        if (bulletsTank1[i]->bulletState == 0) {
                            bulletsTank1[i]->bulletState = 1;
                            bulletsTank1[i]->Fire(tank1->artX, tank1->artY, tank1->artAngle, tank1Property);
                            break;
                        }
                    }
                }
            }

            if (FSKEY_Q == key) {
                if (nBulletCurr2 < nBullet2) {
                    nBulletCurr2++;
                    for (int i = 0; i < bulletsTank2.size(); i++) {
                        if (bulletsTank2[i]->bulletState == 0) {
                            bulletsTank2[i]->bulletState = 1;
                            bulletsTank2[i]->Fire(tank2->artX, tank2->artY, tank2->artAngle, tank2Property);
                            break;
                        }
                    }
                }
            }

            tank1->Move(1, tank2->artX, tank2->artY, m.walls);
            tank2->Move(2, tank1->artX, tank1->artY, m.walls);

            tank1->Calculatepoints(tank1->artX, tank1->artY, tank1->artAngle);
            tank2->Calculatepoints(tank2->artX, tank2->artY, tank2->artAngle);

            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

            m.Draw_Active_Cells();
            m.Draw();
            draw.DrawScore(score1, score2, 400, 650);

            if (tank1->tankState)
            {
                tank1->Laser(m.walls);
                tank1->Draw(1);
            }
            if (tank2->tankState)
            {
                tank2->Laser(m.walls);
                tank2->Draw(2);
            }

            for (int i = 0; i < bulletsTank1.size(); i++) {
                if (bulletsTank1[i]->bulletState == 1) {
                    bulletsTank1[i]->Move(0.05);
                    bulletsTank1[i]->bounceOnWall(m.walls);
                    // check hit tank2
                    if (tank2->tankState && tank2->CheckHitByBullet(bulletsTank1[i]->bulletX, bulletsTank1[i]->bulletY, bulletsTank1[i]->bulletProperty->size)) {

                        explosionCount = 100;
                        bulletsTank1[i]->bulletState = 0;
                        nBulletCurr--;
                        for (int i = 0; i < 10; ++i)
                        {
                            expX[i] = tank2->artX + (double)(rand() % 50 - 25) / 10.0;
                            expY[i] = tank2->artY + (double)(rand() % 50 - 25) / 10.0;
                            expT[i] = i * 5;
                        }

                        tank2->tankState = false;
                        printf("Tank2 die \n");
                        score1++;
                    }
                }
            }

            for (int i = 0; i < bulletsTank2.size(); i++) {
                if (bulletsTank2[i]->bulletState == 1) {
                    bulletsTank2[i]->Move(0.05);
                    bulletsTank2[i]->bounceOnWall(m.walls);
                    // check hit tank1
                    if (tank1->tankState && tank1->CheckHitByBullet(bulletsTank2[i]->bulletX, bulletsTank2[i]->bulletY, bulletsTank2[i]->bulletProperty->size)) {
                        tank1->tankState = false;
                        explosionCount = 200;
                        bulletsTank2[i]->bulletState = 0;
                        nBulletCurr2--;
                        for (int i = 0; i < 10; ++i)
                        {
                            expX[i] = tank1->artX + (double)(rand() % 50 - 25) / 10.0;
                            expY[i] = tank1->artY + (double)(rand() % 50 - 25) / 10.0;
                            expT[i] = i * 5;
                        }
                        printf("Tank1 die \n");
                        score2++;
                    }
                }
            }


            for (int i = 0; i < nBullet; i++) {
                if (bulletsTank1[i]->bulletState == 1)
                {
                    bulletsTank1[i]->Draw();
                    if (bulletsTank1[i]->fade()) {
                        nBulletCurr--;
                    }
                }
            }

            for (int i = 0; i < nBullet2; i++) {
                if (bulletsTank2[i]->bulletState == 1)
                {
                    bulletsTank2[i]->Draw();

                    if (bulletsTank2[i]->fade()) {
                        nBulletCurr2--;
                    }
                }
            }

            if (0 < explosionCount)
            {
                for (int i = 0; i < 10; ++i)
                {
                    expT[i] += 3;
                    expT[i] %= 50;
                }
                --explosionCount;
            }

            if (0 < explosionCount)
            {
                for (int i = 0; i < 10; ++i)
                {
                    int x, y;
                    PhysicalCoordToScreenCoord(x, y, expX[i], expY[i]);
                    glColor3ub(rand() % 256, rand() % 256, rand() % 256);
                    DrawCircle(x, y, expT[i], 1);
                }
            }

            for (int i = 0; i < n_bonus; i++)
            {
                bonuses[i]->Appear(m.cells);
                bonuses[i]->Draw(draw);
                bonuses[i]->CheckReceived(tanks);
                bonuses[i]->Step(0.040, tanks);
            }

            FsSwapBuffers();
            FsSleep(20);


        }

        while (score1 == endgame || score2 == endgame) {

            FsPollDevice();
            int key = FsInkey();
            int wonplayer = 0;
            if (key == FSKEY_ESC)
            {
                return 0;
            }
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            if (score1 == endgame) wonplayer = 1;
            else if (score2 == endgame) wonplayer = 2;
            draw.TestingEnd(wonplayer);
            draw.DrawScore(score1, score2, 390, 480);
            FsSwapBuffers();
            FsSleep(20);

        }
    }
    return 0;
}
