#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Mouse.hpp>
#include <stdint.h>
#include <random>
#include <iostream>
#include <string>
#define KEY_PRESS(key) Keyboard::isKeyPressed(Keyboard::Key::key)

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

using namespace sf;

int rand_range(int min, int max) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

std::array<Drawable*, 64> draw_list;
i32 draw_idx = 0;

struct Ball {
    i32 idx;
    i32 d_idx;
    i32 speed;
    CircleShape sprite;

    Ball() {
        this->sprite = CircleShape(10);
        this->sprite.setPosition({static_cast<float>(rand_range(0, 20)), static_cast<float>(rand_range(0, 100))});
        this->sprite.setFillColor(Color(rand_range(0, 255), rand_range(0, 255), rand_range(0, 127)));
        this->speed = rand_range(1, 5);
    }
};

std::array<Ball*, 64> ball_list;
i32 ball_n = 0;

i32 score = 0;
i32 lives = 5;

void pushdl(Drawable* d) {
    if (draw_idx > 63) {
        std::cerr << "FUCK\n";
        exit(-1);
    }

    draw_list[draw_idx] = d;
    draw_idx++;
}

void spawn_ball() {
    if (ball_n > 63) {
        std::cerr << "FUCK\n";
        exit(-1);
    }

    Ball* new_ball = new Ball;
    new_ball->idx = ball_n;

    ball_list[ball_n] = new_ball;
    ball_n++;

    new_ball->d_idx = draw_idx;
    pushdl(&new_ball->sprite);
}

void rmdl(i32 idx) {
    draw_list[idx] = nullptr;
}

void remove_ball(i32 b_idx, i32 d_idx) {
    delete ball_list[b_idx];
    ball_list[b_idx] = nullptr;
    rmdl(d_idx);
}

int main() {
    for (auto& d : draw_list) d = nullptr;
    for (auto& b : ball_list) b = nullptr;
    RenderWindow game_window(VideoMode({800, 800}), "Balls!");

    RectangleShape paddle({50, 10}); pushdl(&paddle);
    paddle.setOrigin(paddle.getSize() / 2.0f);
    paddle.setPosition({0, 800});

    Font debug_font("debugfont.ttf");
    Text debug_text(debug_font); pushdl(&debug_text);

    Clock cooldown;
    Clock delta_clock;

    std::array<RectangleShape, 5> hearts;

    for (int i = 0; i < lives; i++) {
        RectangleShape heart = hearts[i];
        heart.setPosition({20 + 40.0f * i, 20});
        heart.setFillColor(Color::Red);
        pushdl(&hearts[i]);
    }

    while (game_window.isOpen()) {
        if (KEY_PRESS(Q)) game_window.close();

        Time delta_time = delta_clock.restart();
        f32 dt = delta_time.asSeconds();

        Vector2i mouse_pos = Mouse::getPosition(game_window);
        if (mouse_pos.x >= 0 && mouse_pos.x <= 800) {
            paddle.setPosition({static_cast<float>(mouse_pos.x), paddle.getPosition().y});
        }

        if (cooldown.getElapsedTime().asSeconds() > 0.001) {
            spawn_ball();
            cooldown.restart();
        }

        for (i32 i = 0; i < ball_n; i++) {
            if (ball_list[i]) {
                CircleShape* spr = &ball_list[i]->sprite;
                spr->move({100 * dt * ball_list[i]->speed, 3 * spr->getPosition().y * dt});

                if (spr->getPosition().x > 800) {
                    score++;
                    remove_ball(ball_list[i]->idx, ball_list[i]->d_idx);
                }

                if (spr->getPosition().y > 800) {
                    lives--;
                    remove_ball(ball_list[i]->idx, ball_list[i]->d_idx);
                }
            }
        }

        debug_text.setString(std::to_string(ball_n));

        game_window.clear(Color::Blue);

        for (i32 i = 0; i < draw_idx; i++) 
            if (draw_list[i]) game_window.draw(*draw_list[i]);

        game_window.display();
    }

    return 0;
}
