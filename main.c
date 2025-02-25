#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Here you can change the backgroun and particle color.
const Color BACKGROUND_COLOR = {40, 44, 51, 100};
const Color PARTICLE_COLOR = {152, 195, 121, 100};

const int MIN_PARTICLE_DURATION = 60 * 2;
const int MAX_PARTICLE_DURATION = 60 * 8;
const int MINFORCE = 1;
const int MAXFORCE = 1;
const int barHeight = 20;

#define UPDATE_SPEED 60

// Declarations.

typedef struct Particle {
  Vector2 pos;
  Vector2 vel;
  Color color;
} Particle_t;

// Essentially a particle with extra params.
typedef struct Attractor {
  Vector2 pos;
  Vector2 vel;
  int duration;
  float force;
} Attractor_t;

void initialize_attractor(Attractor_t *attractor, int screenWidth,
                          int screenHeight);
void updateAttractor(Attractor_t *attractor, int frames);
void moveAttractor(Attractor_t *attractor, int screenWidth, int screenHeight);
void initialize_particles(Particle_t *particle, int screenWidth,
                                   int screenHeight);
void attract(Particle_t *particle, Vector2 attractorPos, float force);
void doFriction(Particle_t *particle, float friction);
void move(Particle_t *particle, int screenWidth, int screenHeight);
float getDist(Vector2 pos, Vector2 otherPos);
Vector2 getDirectionVector(Vector2 pos, Vector2 otherPos);
double keepOpenTime = 60.0;

int main(int argc, char *argv[]) {

  InitWindow(0, 0, "");
  printf("Monitor w: %d\n", GetScreenWidth());
  const int screenWidth = (int)GetScreenWidth() * 0.8;
  const int screenHeight = (int)GetScreenHeight() * 0.8;
  CloseWindow();

  SetRandomSeed((unsigned int)time(NULL));

  const int particleCount = 100000;
  Particle_t particles[particleCount];
  Attractor_t attractor;

  for (long int i = 0; i < particleCount; i++) {
    initialize_particles(&particles[i], screenWidth, screenHeight);
  }
  initialize_attractor(&attractor, screenWidth, screenHeight);

  InitWindow(screenWidth, screenHeight, "Particle screen saver");

  SetTargetFPS(UPDATE_SPEED);

  int frames = 0;




  if (argc >= 2) {
    keepOpenTime = atof(argv[1]) * 60.0;
  }
  float timeLeft = keepOpenTime;

  while (!WindowShouldClose()) {

    frames += 1;
    // Update attractor
    updateAttractor(&attractor, frames);
    moveAttractor(&attractor, screenWidth, screenHeight);

    // Update particles
    for (int i = 0; i < particleCount; i++) {
      attract(&particles[i], attractor.pos, 1.);
      doFriction(&particles[i], 0.99);
      move(&particles[i], screenWidth, screenHeight);
    }

    BeginDrawing();

    ClearBackground(BACKGROUND_COLOR);

    for (int i = 0; i < particleCount; i++) {
      DrawPixelV(particles[i].pos, particles[i].color);
    }

    // How much time do we have left? Get the progress.
    timeLeft -= GetFrameTime();
    float progress = timeLeft / keepOpenTime;
    // Width of bar.
    float progressWidth = progress * screenWidth;
    progressWidth = Lerp(progressWidth, (int)progressWidth, 1);
    

    DrawRectangle(0, screenHeight - barHeight, screenWidth, barHeight,
                  (Color){106, 136, 84, 255});

    DrawRectangle(0, screenHeight - barHeight, (int)progressWidth, barHeight,
                  (Color){152, 195, 121, 255});

    EndDrawing();

    // Break when no time left.
    if (timeLeft <= 0){
      break;
    }
  }

  return 0;
}

void initialize_attractor(Attractor_t *attractor, int screenWidth,
                          int screenHeight) {

  attractor->pos.x = GetRandomValue(0, screenWidth);
  attractor->pos.y = GetRandomValue(0, screenHeight);

  attractor->vel.x = GetRandomValue(-200, 200) / 100.f;
  attractor->vel.y = GetRandomValue(-200, 200) / 100.f;

  attractor->duration =
      GetRandomValue(MIN_PARTICLE_DURATION, MAX_PARTICLE_DURATION);
  attractor->force = GetRandomValue(MINFORCE, MAXFORCE);
}

void updateAttractor(Attractor_t *attractor, int frames) {
  if (frames % attractor->duration == 0) {

    attractor->duration =
        frames + GetRandomValue(MIN_PARTICLE_DURATION, MAX_PARTICLE_DURATION);

    attractor->force = GetRandomValue(MINFORCE, MAXFORCE);

    attractor->vel.x = GetRandomValue(-200, 200) / 100.f;
    attractor->vel.y = GetRandomValue(-200, 200) / 100.f;
  }
}

void moveAttractor(Attractor_t *attractor, int screenWidth, int screenHeight) {
  attractor->pos.x += attractor->vel.x;
  attractor->pos.y += attractor->vel.y;

  if (attractor->pos.x < 0)
    attractor->pos.x += screenWidth;
  if (attractor->pos.x >= screenWidth)
    attractor->pos.x -= screenWidth;
  if (attractor->pos.y < 0)
    attractor->pos.y += screenHeight;
  if (attractor->pos.y >= screenHeight)
    attractor->pos.y -= screenHeight;
}

void initialize_particles(Particle_t *particle, int screenWidth,
                                   int screenHeight) {

  particle->pos.x = GetRandomValue(0, screenWidth - 1);
  particle->pos.y = GetRandomValue(0, screenHeight - 1);
  particle->vel.x = GetRandomValue(-100, 100) / 100.f;
  particle->vel.y = GetRandomValue(-100, 100) / 100.f;
  particle->color = PARTICLE_COLOR;
}

void attract(Particle_t *particle, Vector2 attractorPos, float force) {

  float dist = fmax(getDist(particle->pos, attractorPos), force);
  Vector2 normal = getDirectionVector(particle->pos, attractorPos);

  particle->vel.x -= normal.x / dist;
  particle->vel.y -= normal.y / dist;
}
void doFriction(Particle_t *particle, float friction) {
  particle->vel.x *= friction;
  particle->vel.y *= friction;
}
void move(Particle_t *particle, int screenWidth, int screenHeight) {
  particle->pos.x += particle->vel.x;
  particle->pos.y += particle->vel.y;

  if (particle->pos.x < 0)
    particle->pos.x += screenWidth;
  if (particle->pos.x >= screenWidth)
    particle->pos.x -= screenWidth;
  if (particle->pos.y < 0)
    particle->pos.y += screenHeight;
  if (particle->pos.y >= screenHeight)
    particle->pos.y -= screenHeight;
}

float getDist(Vector2 pos, Vector2 otherPos) {
  const float dx = pos.x - otherPos.x;
  const float dy = pos.y - otherPos.y;
  return sqrt((dx * dx) + (dy * dy));
}

Vector2 getDirectionVector(Vector2 pos, Vector2 otherPos) {
  float dist = getDist(pos, otherPos);
  if (dist == 0.0f)
    dist = 1;
  const float dx = pos.x - otherPos.x;
  const float dy = pos.y - otherPos.y;
  Vector2 normal = (Vector2){dx * (1 / dist), dy * (1 / dist)};
  return normal;
}
