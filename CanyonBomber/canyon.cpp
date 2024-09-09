#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <sstream> 
#include "Glut.h"  
#include "canyon.h"

const int windowWidth = 800;
const int windowHeight = 600;
const int numBlocks = 10;
const int blockWidth = 80;
const int blockHeight = 20;
const int bombSize = 10;
const int maxLives = 3;
bool gameWon = false; 
int score = 0;
int lives = maxLives;
int level = 1; 
bool gameOver = false;
bool gameStarted = false;

struct Block {
    int x, y;
    bool destroyed;
};

struct Bomb {
    int x, y;
    bool active;
};

std::vector<Block> blocks;
Bomb bomb;
int planeX = windowWidth / 2;
int planeY = windowHeight - 50;

void initBlocks() {
    blocks.clear();
    switch (level) {
    case 1:
        // prvi nivo
        for (int i = 0; i < numBlocks; ++i) {
            Block block;
            block.x = i * blockWidth;
            block.y = windowHeight / 2;
            block.destroyed = false;
            blocks.push_back(block);
        }
        break;
    case 2:
        // drugi nivo
        for (int i = 0; i < numBlocks / 2; ++i) {
            Block block;
            block.x = i * blockWidth;
            block.y = windowHeight / 2 + blockHeight;
            block.destroyed = false;
            blocks.push_back(block);
        }
        for (int i = numBlocks / 2; i < numBlocks; ++i) {
            Block block;
            block.x = i * blockWidth;
            block.y = windowHeight / 2;
            block.destroyed = false;
            blocks.push_back(block);
        }
        break;
    case 3:
        // treci nivo
        int columns = 3;
        int distance = blockWidth; 

        for (int col = 0; col < columns; ++col) {
            int startX = col * (windowWidth / columns) + distance / 2;
            for (int i = 0; i < numBlocks / columns; ++i) {
                Block block;
                block.x = startX;
                block.y = windowHeight / 2 + i * (blockHeight + distance);
                block.destroyed = false;
                blocks.push_back(block);
            }
        }
        break;

    }
}

void initBomb() {
    bomb.active = false;
}
void clearBoard() {
    blocks.clear();
}


void resetGame() {
    score = 0;
    lives = maxLives;
    level = 1;
    gameOver = false;
    gameStarted = false;
    initBlocks();
    initBomb();
}

void initRendering() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, windowWidth, 0.0, windowHeight);
    resetGame();
}

void drawText(const char* text, float x, float y, float r, float g, float b, void* font) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    while (*text) {
        glutBitmapCharacter(font, *text);
        text++;
    }
}

void drawPlane() {
    glPushMatrix();
    glTranslatef(planeX, planeY, 0.0);

    // Draw the body of the helicopter (smaller)
    glColor3f(0.0, 0.0, 1.0); // Blue
    glBegin(GL_POLYGON);
    glVertex2f(-15, 8);
    glVertex2f(15, 8);
    glVertex2f(18, 5);
    glVertex2f(18, -5);
    glVertex2f(15, -8);
    glVertex2f(-15, -8);
    glVertex2f(-18, -5);
    glVertex2f(-18, 5);
    glEnd();

    // Draw the cockpit (adjusted)
    glColor3f(0.5, 0.5, 1.0); // Light blue
    glBegin(GL_POLYGON);
    glVertex2f(-8, 8);
    glVertex2f(8, 8);
    glVertex2f(8, 0);
    glVertex2f(-8, 0);
    glEnd();

    // Draw the rotor
    glColor3f(0.0, 1.0, 0.0); // Green
    glBegin(GL_QUADS);
    glVertex2f(-30, 12);
    glVertex2f(30, 12);
    glVertex2f(30, 8);
    glVertex2f(-30, 8);
    glEnd();

    // Draw the tail
    glColor3f(0.0, 0.0, 1.0); // Blue
    glBegin(GL_POLYGON);
    glVertex2f(15, 5);
    glVertex2f(30, 5);
    glVertex2f(35, 0);
    glVertex2f(30, -5);
    glVertex2f(15, -5);
    glEnd();

    // Draw the tail rotor
    glColor3f(0.0, 1.0, 0.0); // Green
    glBegin(GL_QUADS);
    glVertex2f(30, 6);
    glVertex2f(35, 6);
    glVertex2f(35, -6);
    glVertex2f(30, -6);
    glEnd();

    // Draw the landing skids
    glColor3f(0.3, 0.3, 0.3); // Gray
    glBegin(GL_LINES);
    glVertex2f(-12, -10);
    glVertex2f(-20, -15);
    glVertex2f(12, -10);
    glVertex2f(20, -15);
    glVertex2f(-20, -15);
    glVertex2f(20, -15);
    glEnd();

    glPopMatrix();
}



void drawBomb() {
    if (bomb.active) {
        glPushMatrix();
        glTranslatef(bomb.x, bomb.y, 0.0);
        glColor3f(1.0, 1.0, 0.0); // Yellow color
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0.0, 0.0); // Center of circle
        int numSegments = 100;
        for (int i = 0; i <= numSegments; ++i) {
            float angle = 2.0f * 3.14 * i / numSegments;
            float dx = (bombSize / 2.0) * cosf(angle);
            float dy = (bombSize / 2.0) * sinf(angle);
            glVertex2f(dx, dy);
        }
        glEnd();
        glPopMatrix();
    }
}


void drawBlocks() {
    for (const auto& block : blocks) {
        if (!block.destroyed) {
            glPushMatrix();
            glTranslatef(block.x, block.y, 0.0);
            if (level == 1) {
                glColor3f(0.0, 1.0, 0.0); // Green
            }
            else if (level == 2) {
                if (block.y == windowHeight / 2) {
                    glColor3f(1.0, 0.0, 0.0); // Red
                }
                else {
                    glColor3f(0.0, 0.0, 1.0); // Blue
                }
            }
            else if (level == 3) {
                glColor3f(1.0, 0.0, 0.0); // Red
            }
            glBegin(GL_QUADS);
            glVertex2f(0, blockHeight);
            glVertex2f(blockWidth, blockHeight);
            glVertex2f(blockWidth, 0);
            glVertex2f(0, 0);
            glEnd();
            glPopMatrix();
        }
    }
}

void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (gameOver) {
        if (gameWon) {
            // Display "You Win!" message
            drawText("You Win!", windowWidth / 2 - 50, windowHeight / 2, 0.0, 1.0, 0.0, GLUT_BITMAP_TIMES_ROMAN_24);
            
        }
        else {
            // Display "Game Over!" message
            drawText("Game Over!", windowWidth / 2 - 50, windowHeight / 2, 1.0, 0.0, 0.0, GLUT_BITMAP_TIMES_ROMAN_24);
            // Display "Press R to try again" message
            drawText("Press R to try again", windowWidth / 2 - 75, windowHeight / 2 - 30, 1.0, 1.0, 1.0, GLUT_BITMAP_HELVETICA_18);
        }
    }
    else {
        drawPlane();
        drawBomb();
        drawBlocks();

        // Display score, lives, and level
        drawText(("Score: " + std::to_string(score)).c_str(), 10, windowHeight - 20, 1.0, 1.0, 1.0, GLUT_BITMAP_HELVETICA_18);
        drawText(("Lives: " + std::to_string(lives)).c_str(), 10, windowHeight - 40, 1.0, 1.0, 1.0, GLUT_BITMAP_HELVETICA_18);
        drawText(("Level: " + std::to_string(level)).c_str(), 10, windowHeight - 60, 1.0, 1.0, 1.0, GLUT_BITMAP_HELVETICA_18);
    }

    glutSwapBuffers();
}

void dropBomb() {
    if (!bomb.active) {
        bomb.active = true;
        bomb.x = planeX;
        bomb.y = planeY;
    }
}

void updateBomb() {
    if (bomb.active) {
        bomb.y -= 5;
        if (bomb.y <= 0) {
            bomb.active = false;
            lives--;
            if (lives <= 0) {
                gameOver = true;
            }
        }

        for (auto& block : blocks) {
            if (!block.destroyed &&
                bomb.x > block.x && bomb.x < block.x + blockWidth &&
                bomb.y > block.y && bomb.y < block.y + blockHeight) {
                block.destroyed = true;
                bomb.active = false;
                score += 10;
                break;
            }
        }
    }
}



void update(int value) {
    if (gameStarted && !gameOver) {
        updateBomb();
        
        bool allBlocksDestroyed = true;
        for (const auto& block : blocks) {
            if (!block.destroyed) {
                allBlocksDestroyed = false;
                break;
            }
        }
        if (allBlocksDestroyed) {
            if (level == 3) {
                gameWon = true; 
                gameOver = true; 
            }
            else {
               
                level++;
                if (level > 3) {
                    level = 1; 
                }
                clearBoard();
                initBlocks();
            }
        }
        glutPostRedisplay();
        glutTimerFunc(30, update, 0);
    }
}


void handleKeypress(unsigned char key, int x, int y) {
    switch (key) {
    case 27: // ESC key
        std::exit(0);
        break;
    case ' ':
        if (!gameStarted) {
            gameStarted = true;
            glutTimerFunc(30, update, 0);
        }
        else {
            dropBomb();
        }
        break;
    case 'r':
        resetGame();
        break;
    case 'a':
        planeX -= 10;
        break;
    case 'd':
        planeX += 10;
        break;
    }
}

void resizeWindow(int w, int h) {
    double scale, center;
    double windowXmin, windowXmax, windowYmin, windowYmax;

    glViewport(0, 0, w, h);

    w = (w == 0) ? 1 : w;
    h = (h == 0) ? 1 : h;
    if ((double)windowWidth / w < (double)windowHeight / h) {
        scale = ((double)windowHeight / h) / ((double)windowWidth / w);
        center = (double)windowWidth / 2;
        windowXmin = center - (center)*scale;
        windowXmax = center + (windowWidth - center) * scale;
        windowYmin = 0;
        windowYmax = windowHeight;
    }
    else {
        scale = ((double)windowWidth / w) / ((double)windowHeight / h);
        center = (double)windowHeight / 2;
        windowYmin = center - (center)*scale;
        windowYmax = center + (windowHeight - center) * scale;
        windowXmin = 0;
        windowXmax = windowWidth;
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(windowXmin, windowXmax, windowYmin, windowYmax, -1, 1);
}

void setUpLevel(int level) {
    initBlocks();
    initBomb();
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(10, 60);
    glutInitWindowSize(800, 600);

    glutCreateWindow("Canyon Bomber");

    initRendering();

    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(resizeWindow);
    glutDisplayFunc(drawScene);

    glutMainLoop();
    return 0;
}
