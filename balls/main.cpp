#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Mouse.hpp>
#include <stdint.h>
#include <random>
#include <iostream>
#include <string>

#define KEY_PRESS(key) Keyboard::isKeyPressed(Keyboard::Key::key)
#define WINDOW_H 800
#define WINDOW_W 800
#define PADDLE_H 10
#define PADDLE_W 50

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

using namespace sf;

i32 rand_range(i32 min, i32 max) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<i32> dist(min, max);
    return dist(rng);
}

std::array<Drawable*, 64> draw_list;
i32 draw_idx = 0;

struct Ball {
    i32 idx;
    i32 d_idx;
    i32 speed;
    f32 y_velocity;
    CircleShape sprite;

    Ball() {
        this->y_velocity = 0;
        this->sprite = CircleShape(10);
        this->sprite.setOrigin(this->sprite.getGeometricCenter());
        this->sprite.setPosition({static_cast<float>(rand_range(0, 20)), static_cast<float>(rand_range(0, 100))});
        this->sprite.setFillColor(Color(rand_range(0, 255), rand_range(0, 255), rand_range(0, 127)));
        this->speed = rand_range(10, 75);
    }
};

std::array<Ball*, 64> ball_list;
i32 ball_n = 0;

i32 score = 0;
i32 lives = 10;
bool ball_on_screen = false;

i32 pushdl(Drawable* d) {
    for (i32 i = 0; i < 64; i++) {
        if (!draw_list[i]) {
            draw_list[i] = d;
            return i;
        }
    }

    std::cerr << "FUUUUUCKJ YOU FUCKED UP GOD FUCKING DAMIN IT\n";
    exit(-1);
}

void spawn_ball() {
    i32 slot = -1;
    for (i32 i = 0; i < 64; i++) {
        if (!ball_list[i]) {
            slot = i;
            break;
        }
    }
    if (slot == -1) {
        std::cerr << "FUUUUUCKJ YOU FUCKED UP GOD FUCKING DAMIN IT\n";
        exit(-1);
    }

    Ball* new_ball = new Ball;
    new_ball->idx = slot;

    ball_list[slot] = new_ball;
    ball_n++;

    new_ball->d_idx = new_ball->d_idx = pushdl(&new_ball->sprite);
}

void rmdl(i32 idx) {
    draw_list[idx] = nullptr;
}

void remove_ball(i32 b_idx, i32 d_idx) {
    if (ball_list[b_idx]) {
        delete ball_list[b_idx];
        ball_list[b_idx] = nullptr;
    }
    if (d_idx >= 0 && d_idx < 64) {
        draw_list[d_idx] = nullptr;
    }

    ball_on_screen = false;
    ball_n--;
}

Vector2f vector2_cmp(Vector2f v1, Vector2f v2) {
    return Vector2f(fabs(v1.x - v2.x), fabs(v1.y - v2.y));
}

int main() {
    for (auto& d : draw_list) d = nullptr;
    for (auto& b : ball_list) b = nullptr;
    RenderWindow game_window(VideoMode({WINDOW_W, WINDOW_H}), "Balls!");

    RectangleShape paddle({PADDLE_W, PADDLE_H}); pushdl(&paddle);
    paddle.setOrigin(paddle.getSize() / 2.0f);
    paddle.setPosition({0, WINDOW_H - 150});

    Font debug_font("debugfont.ttf");
    Text debug_text(debug_font); pushdl(&debug_text);
    debug_text.setPosition({WINDOW_W - 200, 0});

    Clock cooldown;
    Clock delta_clock;

    std::array<RectangleShape, 10> hearts;

    bool ai_mode = false;
    bool am_locked_on_target = false;

    for (int i = 0; i < lives; i++) {
        hearts[i].setSize({25, 25});
        hearts[i].setPosition({20 + 30.0f * i, 20});
        hearts[i].setFillColor(Color::Red);
        pushdl(&hearts[i]);
    }

    while (game_window.isOpen()) {
        if (KEY_PRESS(Q)) game_window.close();
        if (KEY_PRESS(G)) ai_mode = !ai_mode;

        Time delta_time = delta_clock.restart();
        f32 dt = delta_time.asSeconds();

        Vector2i mouse_pos = Mouse::getPosition(game_window);
        if (mouse_pos.x >= 0 && mouse_pos.x <= WINDOW_W && !ai_mode) {
            paddle.setPosition({static_cast<float>(mouse_pos.x), paddle.getPosition().y});
        }

        if (ai_mode) {
            i32 r = rand_range(0, 63);
            if (ball_list[r]) {
                    paddle.setPosition({ball_list[r]->sprite.getPosition().x, paddle.getPosition().y});
            }
        }

        if (cooldown.getElapsedTime().asSeconds() > 1 && !ball_on_screen) {
            if (rand_range(0, 5) == 5) spawn_ball();
            if (rand_range(0, 10) == 10) spawn_ball();

            if (score > 25) {
                if (rand_range(0, 10) == 10) spawn_ball();
                if (rand_range(0, 10) == 10) spawn_ball();
            }

            if (score > 50) {
                if (rand_range(0, 10) == 10) spawn_ball();
                if (rand_range(0, 10) == 10) spawn_ball();
            }

            spawn_ball();
            ball_on_screen = true;
            cooldown.restart();
        }

        for (i32 i = 0; i < 64; i++) {
            if (ball_list[i]) {
                CircleShape* spr = &ball_list[i]->sprite;
                ball_list[i]->y_velocity += dt * 0.05f;
                spr->move({100 * dt * ball_list[i]->speed / 20.0f, ball_list[i]->y_velocity});

                if (spr->getPosition().x > WINDOW_W) {
                    score++;
                    remove_ball(ball_list[i]->idx, ball_list[i]->d_idx);
                    continue;
                }

                if (spr->getPosition().y > WINDOW_H) {
                    lives--;
                    rmdl(2 + lives);
                    remove_ball(ball_list[i]->idx, ball_list[i]->d_idx);
                    continue;
                }

                if (spr->getGlobalBounds().findIntersection(paddle.getGlobalBounds())) {
                    ball_list[i]->y_velocity = -fabs(ball_list[i]->y_velocity) * 0.8f;
                    ball_list[i]->speed *= 1.8;
                }
            }
        }

        debug_text.setString("score: " + std::to_string(score));

        if (lives == 0) {
            std::cout << "You fucking suck at this" << std::endl;
            exit(0);
        }


        game_window.clear(Color::Blue);

        for (i32 i = 0; i < 64; i++) 
            if (draw_list[i]) game_window.draw(*draw_list[i]);

        game_window.display();
    }

    return 0;
} 
