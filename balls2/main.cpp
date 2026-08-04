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
#define PADDLE_H 20
#define PADDLE_W 10000

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

using namespace sf;

i32 rand_range(i32 min, i32 max) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<i32> dist(min, max);
    return dist(rng);
}

std::array<Drawable*, UINT16_MAX> draw_list;
i32 draw_idx = 0;

struct Ball {
    i32 idx;
    i32 d_idx;
    f32 x_velocity;
    f32 y_velocity;
    CircleShape sprite;

    Ball() {
        this->y_velocity = 0;
        this->sprite = CircleShape(rand_range(10, 20));
        this->sprite.setOrigin(this->sprite.getGeometricCenter());
        this->sprite.setPosition({static_cast<f32>(rand_range(0, WINDOW_W)), static_cast<f32>(rand_range(0, WINDOW_H - 200))});
        this->sprite.setFillColor(Color(rand_range(0, 255), rand_range(0, 255), rand_range(0, 127)));
        this->x_velocity = rand_range(-50, 50);
    }
};

std::array<Ball*, UINT16_MAX> ball_list;
i32 ball_n = 0;

i32 score = 0;
i32 lives = 0;
bool ball_on_screen = false;

i32 pushdl(Drawable* d) {
    for (i32 i = 0; i < draw_list.size(); i++) {
        if (!draw_list[i]) {
            draw_list[i] = d;
            return i;
        }
    }

    std::cerr << "FUUUUUCKJ YOU FUCKED UP GOD FUCKING DAMIN IT\n";
    exit(-1);
}

void spawn_ball(Vector2f pos) {
    i32 slot = -1;
    for (i32 i = 0; i < ball_list.size(); i++) {
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
    new_ball->sprite.setPosition(pos);

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
    debug_text.setPosition({WINDOW_W - 800, 0});

    Clock cooldown;
    Clock delta_clock;

    std::array<RectangleShape, 0> hearts;

    bool ai_mode = false;
    bool am_locked_on_target = false;

    for (int i = 0; i < lives; i++) {
        hearts[i].setSize({25, 25});
        hearts[i].setPosition({20 + 30.0f * i, 20});
        hearts[i].setFillColor(Color::Red);
        pushdl(&hearts[i]);
    }
    
    spawn_ball({400, 400});

    while (game_window.isOpen()) {
        if (KEY_PRESS(Q)) game_window.close();
        if (KEY_PRESS(G)) ai_mode = !ai_mode;

        Time delta_time = delta_clock.restart();
        f32 dt = delta_time.asSeconds();

        Vector2i mouse_pos = Mouse::getPosition(game_window);
        if (mouse_pos.x >= 0 && mouse_pos.x <= WINDOW_W && !ai_mode) {
            paddle.setPosition({static_cast<float>(mouse_pos.x), paddle.getPosition().y});
        }


        //if (cooldown.getElapsedTime().asSeconds() > 0.2f) {
        //    if (rand_range(0, 2) == 2) spawn_ball();
        //    if (rand_range(0, 5) == 5) spawn_ball();
        //    if (rand_range(0, 10) == 10) spawn_ball();
        //    if (rand_range(0, 15) == 15) spawn_ball();
        //    if (rand_range(0, 20) == 20) spawn_ball();
        //    if (rand_range(0, 25) == 25) spawn_ball();
        //    if (rand_range(0, 30) == 30) spawn_ball();
        //
        //    spawn_ball();
        //    ball_on_screen = true;
        //    cooldown.restart();
        //}

        for (i32 i = 0; i < ball_list.size(); i++) {
            if (ball_list[i]) {
                CircleShape* spr = &ball_list[i]->sprite;
                if (ai_mode) paddle.setPosition({spr->getPosition().x, paddle.getPosition().y});
                ball_list[i]->y_velocity += dt * 0.5f;
                spr->move({100 * dt * ball_list[i]->x_velocity / 20.0f, ball_list[i]->y_velocity});

                if (spr->getPosition().x > WINDOW_W) {
                    score++;
                    ball_list[i]->x_velocity = -fabs(ball_list[i]->x_velocity) * 1.025f;
                    continue;
                }

                if (spr->getPosition().x < 0) {
                    ball_list[i]->x_velocity = fabs(ball_list[i]->x_velocity) * 1.025f;
                    continue;
                }

                if (spr->getPosition().y > WINDOW_H) {
                    score--;
                    remove_ball(ball_list[i]->idx, ball_list[i]->d_idx);

                    continue;
                }

                if (spr->getPosition().y < 0) {
                    ball_list[i]->y_velocity = fabs(ball_list[i]->y_velocity) * 1.2f;
                    continue;
                }

                if (spr->getGlobalBounds().findIntersection(paddle.getGlobalBounds())) {
                    score++;
                    ball_list[i]->y_velocity = -fabs(ball_list[i]->y_velocity) * 2;
                    spr->move({0, -PADDLE_H});
                    spawn_ball(spr->getPosition());
                }
            }
        }

        debug_text.setString("balls: " + std::to_string(ball_n));



        game_window.clear(Color::Blue);

        for (i32 i = 0; i < draw_list.size(); i++) 
            if (draw_list[i]) game_window.draw(*draw_list[i]);

        game_window.display();
    }

    return 0;
} 
