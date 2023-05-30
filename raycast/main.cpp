#include <SFML/Graphics.hpp>
#include <iostream>

#define PI atan(1)*4
#define PI2 PI / 2
#define PI32 3 * PI / 2
#define TAU atan(1)*8
#define SCREENX 800
#define SCREENY 800
#define mapX 16
#define mapY 16

float xPos = 400;
float yPos = 400;
float rot = 0;

float xJeff = 450;
float yJeff = 450;

sf::Image wallImg;
sf::Texture wallTex;

sf::Image jeffImg;
sf::Texture jeffTex;

int* generateMap();
void drawMap(sf::RenderWindow*, int*);
void drawPlayer(sf::RenderWindow*);
void drawScreen(sf::RenderWindow*, int*);
void playerInputs(int, int*, float);
void raycast(float, int*, float*, int*, int, int);
float getDistance(float, float, float, float);
void draw3D(sf::RenderWindow*, int*);
void bubble(int*, float*, float*);

int main() {
    // create the window
    sf::RenderWindow window(sf::VideoMode(SCREENX, SCREENY), "RayCast");
    window.setFramerateLimit(30);

    // Convert map picture to map array
    int* map = generateMap();

    // Load wall texture
    wallImg.loadFromFile("wall.png");
    wallTex.loadFromImage(wallImg);

    // Load Jeff texture
    jeffImg.loadFromFile("wall2.png");
    jeffTex.loadFromImage(jeffImg);

    sf::Clock clock;

    // run the program as long as the window is open
    while (window.isOpen()) {
        float currentTime = clock.restart().asSeconds();

        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event)) {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }

        playerInputs(100, map, currentTime);

        drawScreen(&window, map);

        // end the current frame
        window.display();
    }

    // Free memory
    free(map);

    return 0;
}

int* generateMap() {
    sf::Image mapImg;
    mapImg.loadFromFile("map.png");
    int* map = (int*)calloc(mapX * mapY, sizeof(int));
    for (int i = 0; i < mapX; i++) {
        for (int j = 0; j < mapY; j++) {
            sf::Color pixel = mapImg.getPixel(i, j);

            if (pixel == sf::Color(0, 0, 0)) {
                map[j * mapX + i] = 1;
            }
            else {
                map[j * mapX + i] = 0;
            }
        }
    }

    return map;
}

void drawMap(sf::RenderWindow *window, int *map) {
    sf::VertexArray square(sf::Quads, 4);
    sf::Color wallCol;
    for (int i = 0; i < mapX; i++) {
        for (int j = 0; j < mapY; j++) {
            square[0].position = sf::Vector2f(i * SCREENY / mapY, j * SCREENY / mapY);
            square[1].position = sf::Vector2f(i * SCREENY / mapY, (j + 1) * SCREENY / mapY);
            square[2].position = sf::Vector2f((i + 1) * SCREENY / mapY, (j + 1) * SCREENY / mapY);
            square[3].position = sf::Vector2f((i + 1) * SCREENY / mapY, j * SCREENY / mapY);

            if (map[j * mapX + i] == 1) {
                wallCol = sf::Color(0, 0, 0);
            }
            else {
                wallCol = sf::Color(200, 255, 255);
            }

            square[0].color = wallCol;
            square[1].color = wallCol;
            square[2].color = wallCol;
            square[3].color = wallCol;

            (*window).draw(square);
        }
    }
}

void drawPlayer(sf::RenderWindow* window) {
    // Draw player cirlce
    sf::CircleShape player(5.f);
    player.setPosition(xPos - 5, yPos - 5);
    player.setFillColor(sf::Color(255, 255, 0));
    (*window).draw(player);

    // Draw player direction
    sf::VertexArray line(sf::Lines, 2);
    line[0].position = sf::Vector2f(xPos, yPos);
    line[1].position = sf::Vector2f(xPos + 20 * cos(rot), yPos - 20 * sin(rot));

    line[0].color = sf::Color(255, 255, 0);
    line[1].color = sf::Color(255, 255, 0);
    (*window).draw(line);
}

void drawScreen(sf::RenderWindow* window, int* map) {
    // Fill the window background
    (*window).clear(sf::Color::Color(77, 77, 77, 255));

    // 3D view
    draw3D(window, map);
}

void playerInputs(int speed, int* map, float delta) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        rot += 0.05 * speed * delta;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        rot -= 0.05 * speed * delta;
    }

    // Make sure rotation is between 0 and 2Pi
    if (rot < 0) {
        rot += TAU;
    } else if (rot >= TAU) {
        rot -= TAU;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        int yOff, xOff;
        if (rot >= 0 && rot < PI) {
            yOff = -1;
        } else {
            yOff = 1;
        }

        if (rot >= PI2 && rot < PI32) {
            xOff = -1;
        } else {
            xOff = 1;
        }

        for (int i = 0; i < 100; i++) {
            xPos += speed / 100. * cos(rot) * delta;
            if (map[(int)((yPos + 5 * yOff) / (SCREENY / mapY)) * mapX + (int)((xPos + 5 * xOff) / (SCREENY / mapY))] == 1) {
                xPos -= speed / 100. * cos(rot) * delta;
            }
            yPos -= speed / 100. * sin(rot) * delta;
            if (map[(int)((yPos + 5 * yOff) / (SCREENY / mapY)) * mapX + (int)((xPos + 5 * xOff) / (SCREENY / mapY))] == 1) {
                yPos += speed / 100. * sin(rot) * delta;
            }
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        int yOff, xOff;
        if (rot >= 0 && rot < PI) {
            yOff = 1;
        }
        else {
            yOff = -1;
        }

        if (rot >= PI2 && rot < PI32) {
            xOff = 1;
        }
        else {
            xOff = -1;
        }

        for (int i = 0; i < 100; i++) {
            xPos -= speed / 100. * cos(rot) * delta;
            if (map[(int)((yPos + 5 * yOff) / (SCREENY / mapY)) * mapX + (int)((xPos + 5 * xOff) / (SCREENY / mapY))] == 1) {
                xPos += speed / 100. * cos(rot) * delta;
            }
            yPos += speed / 100. * sin(rot) * delta;
            if (map[(int)((yPos + 5 * yOff) / (SCREENY / mapY)) * mapX + (int)((xPos + 5 * xOff) / (SCREENY / mapY))] == 1) {
                yPos -= speed / 100. * sin(rot) * delta;
            }
        }
    }
}

void raycast(float angle, int* map, float* distance, int* wall, int xPos, int yPos) {
    float x = xPos, y = yPos, vX = INFINITY, vY = INFINITY, hX = INFINITY, hY = INFINITY, step = (SCREENY / mapY);
    *distance = INFINITY;
    *wall = 0;

    if (map[(int)(y / (SCREENY / mapY)) * mapX + (int)(x / (SCREENY / mapY))] == 1) {
        *distance = 0;
        return;
    }

    // Detect vertical wall
    {
        if (angle != PI2 && angle != PI32) {
            x = floor(x / (SCREENY / mapY)) * (SCREENY / mapY);

            if (angle <= PI2 || angle >= PI32) {
                x += step;
            }
            else {
                step = -(SCREENY / mapY);
            }
        }

        float dy = -((SCREENY / mapY) / cos(angle)) * sin(angle);
        y = y + dy * (x - xPos) / (SCREENY / mapY);

        for (int i = 0; i < 1000; i++) {
            float sneakyX = x;
            if (angle > PI2 && angle < PI32) {
                sneakyX -= (SCREENY / mapY);
            }
            int pos = (int)(y / (SCREENY / mapY)) * mapX + (int)(sneakyX / (SCREENY / mapY));
            if (pos < mapX * mapY && pos >= 0) {
                if (map[(int)(y / (SCREENY / mapY)) * mapX + (int)(sneakyX / (SCREENY / mapY))] == 1) {
                    vX = x;
                    vY = y;
                    break;
                }
            }

            x += step;
            if (angle > PI2 && angle < PI32) {
                y -= dy;
            }
            else {
                y += dy;
            }
        }
    }

    // Detect horizontal wall
    {
        x = xPos, y = yPos, step = (SCREENY / mapY);
        if (angle != 0 && angle != PI) {
            y = floor(y / (SCREENY / mapY)) * (SCREENY / mapY);
            if (angle > PI) {
                y += step;
            } else {
                step = -(SCREENY / mapY);
            }

            float dx = -((SCREENY / mapY) / sin(angle)) * cos(angle);
            x = x + dx * (y - yPos) / (SCREENY / mapY);

            for (int i = 0; i < 1000; i++) {
                float sneakyY = y;
                if (angle < PI) {
                    sneakyY -= (SCREENY / mapY);
                }
                int pos = (int)(sneakyY / (SCREENY / mapY)) * mapX + (int)(x / (SCREENY / mapY));
                if (pos < mapX * mapY && pos >= 0) {
                    if (map[(int)(sneakyY / (SCREENY / mapY)) * mapX + (int)(x / (SCREENY / mapY))] == 1) {
                        hX = x;
                        hY = y;
                        break;
                    }
                }

                y += step;
                if (angle > PI) {
                    x += dx;
                } else {
                    x -= dx;
                }
            }
        }
    }

    if (getDistance(xPos, yPos, hX, hY) < getDistance(xPos, yPos, vX, vY)) {
        *distance = getDistance(xPos, yPos, hX, hY);
        *wall = 1;
    }
    else {
        *distance = getDistance(xPos, yPos, vX, vY);
    }
}

float getDistance(float x1, float y1, float x2, float y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

void draw3D(sf::RenderWindow* window, int* map) {
    // Draw floor
    sf::VertexArray floor(sf::Quads, 4);
    floor[0].position = sf::Vector2f(0, SCREENY / 2);
    floor[1].position = sf::Vector2f(SCREENX, SCREENY / 2);
    floor[2].position = sf::Vector2f(SCREENX, SCREENY);
    floor[3].position = sf::Vector2f(0, SCREENY);
    floor[0].color = sf::Color(120, 135, 120);
    floor[1].color = sf::Color(120, 135, 120);
    floor[2].color = sf::Color(120, 135, 120);
    floor[3].color = sf::Color(120, 135, 120);
    (*window).draw(floor);

    // Draw ray
    int* pos;
    pos = (int*)calloc(SCREENX, sizeof(int));

    float* dists, * pixels;
    pixels = (float*)calloc(SCREENX, sizeof(float));
    dists = (float*)calloc(SCREENX, sizeof(float));

    float distance, x, y;
    int wall;
    float pixel;
    for (int i = 0; i < SCREENX; i++) {
        // Find data for each vertical slice
        float angle = rot + (i * 60. / SCREENX - 30) * PI / 180;
        if (angle < 0) {
            angle += TAU;
        }
        else if (angle >= TAU) {
            angle -= TAU;
        }
        raycast(angle, map, &distance, &wall, xPos, yPos);
        distance *= cos(angle - rot);

        x = xPos + distance * cos(angle);
        y = yPos - distance * sin(angle);

        // Find part of texture to draw
        if (wall) {
            // Map with x
            pixel = (x - (int)(x / (SCREENY / mapY)) * (SCREENY / mapY)) / (SCREENY / mapY) * 32.;
        }
        else {
            // Map with y
            pixel = (y - (int)(y / (SCREENY / mapY)) * (SCREENY / mapY)) / (SCREENY / mapY) * 32.;
        }

        dists[i] = distance;
        pos[i] = i;
        pixels[i] = pixel;
    }

    for (int i = 0; i < SCREENX; i++) {
        // Draw 3D view
        sf::VertexArray line(sf::Lines, 2);
        line[0].position = sf::Vector2f(SCREENX - 1 - pos[i], SCREENY / 2 - 200 / dists[i] * 70);
        line[1].position = sf::Vector2f(SCREENX - 1 - pos[i], SCREENY / 2 + 200 / dists[i] * 70);
        line[0].color = sf::Color(255, 255, 255);
        line[1].color = sf::Color(255, 255, 255);
        line[0].texCoords = sf::Vector2f(pixels[i], 0.0);
        line[1].texCoords = sf::Vector2f(pixels[i], 32.0);
        (*window).draw(line, &wallTex);

        // Draw fog
        float fogDist = dists[i];
        if (fogDist > 500) {
            fogDist = 500;
        }
        line[0].position = sf::Vector2f(SCREENX - 1 - pos[i], SCREENY / 2 - 200 / dists[i] * 70);
        line[1].position = sf::Vector2f(SCREENX - 1 - pos[i], SCREENY / 2 + 200 / dists[i] * 70);
        line[0].color = sf::Color(77, 77, 77, fogDist / 500. * 255.);
        line[1].color = sf::Color(77, 77, 77, fogDist / 500. * 255.);
        (*window).draw(line);
    }

    free(dists);
    free(pos);
    free(pixels);
}

void bubble(int* pos, float* dists, float* pixels) {
    float switchF;
    int switchI;
    for (int i = 0; i < SCREENX; i++) {
        for (int j = 0; j < SCREENX - 1; j++) {
            if (dists[j] < dists[i]) {
                // Switch variables
                switchI = pos[j];
                pos[j] = pos[j + 1];
                pos[j + 1] = switchI;

                switchF = dists[j];
                dists[j] = dists[j + 1];
                dists[j + 1] = switchF;

                switchF = pixels[j];
                pixels[j] = pixels[j + 1];
                pixels[j + 1] = switchF;
            }
        }
    }
}