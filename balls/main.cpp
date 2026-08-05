#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include <random>
#include <stdint.h>
#include <string>

#define KEY_PRESS(key) Keyboard::isKeyPressed(Keyboard::Key::key)
#define WINDOW_H 800
#define WINDOW_W 1200
#define VALID_AREA WINDOW_W - 200
#define DRAW_LIST_SIZE UINT16_MAX
#define BALL_LIST_SIZE UINT16_MAX

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

std::array<Drawable *, DRAW_LIST_SIZE> draw_list;
i32 draw_idx = 0;

struct Ball {
    i32 idx;
    i32 d_idx;
    f32 x_velocity;
    f32 y_velocity;
    f32 gravity_mult;
    CircleShape sprite;

    Ball() {
        this->y_velocity = 0;
        this->gravity_mult = rand_range(30, 50) / 20.0f;
        this->sprite = CircleShape(rand_range(5, 30));
        this->sprite.setOrigin(this->sprite.getGeometricCenter());
        this->sprite.setPosition({static_cast<f32>(rand_range(0, 10)),
                                  static_cast<f32>(rand_range(-15, 25))});
        this->sprite.setFillColor(
            Color(rand_range(0, 255), rand_range(0, 255), rand_range(0, 127)));
        this->x_velocity = rand_range(20, 100);
    }
};

std::array<Ball *, BALL_LIST_SIZE> ball_list;
i32 ball_n = 0;

f32 paddle_h = 20;
f32 paddle_w = 80;

i32 score = 0;
i32 lives = 16;
i32 evilfuckingcounter = 0;
bool ball_on_screen = false;
bool paused = false;

i32 pushdl(Drawable *d) {
    for (i32 i = 0; i < DRAW_LIST_SIZE; i++) {
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
    for (i32 i = 0; i < BALL_LIST_SIZE; i++) {
        if (!ball_list[i]) {
            slot = i;
            break;
        }
    }
    if (slot == -1) {
        std::cerr << "FUUUUUCKJ YOU FUCKED UP GOD FUCKING DAMIN IT\n";
        exit(-1);
    }

    Ball *new_ball = new Ball;
    new_ball->idx = slot;

    ball_list[slot] = new_ball;
    ball_n++;

    new_ball->d_idx = new_ball->d_idx = pushdl(&new_ball->sprite);
}

void rmdl(i32 idx) { draw_list[idx] = nullptr; }

void remove_ball(i32 b_idx, i32 d_idx) {
    if (ball_list[b_idx]) {
        delete ball_list[b_idx];
        ball_list[b_idx] = nullptr;
    }
    if (d_idx >= 0 && d_idx < BALL_LIST_SIZE) {
        draw_list[d_idx] = nullptr;
    }

    ball_on_screen = false;
    ball_n--;
}

Vector2f vector2_cmp(Vector2f v1, Vector2f v2) {
    return Vector2f(fabs(v1.x - v2.x), fabs(v1.y - v2.y));
}

int main() {
    for (auto &d : draw_list)
        d = nullptr;
    for (auto &b : ball_list)
        b = nullptr;
    RenderWindow game_window(VideoMode({WINDOW_W, WINDOW_H}), "Balls!");

    RectangleShape paddle({paddle_w, paddle_h});
    pushdl(&paddle);
    paddle.setOrigin(paddle.getSize() / 2.0f);
    paddle.setPosition({0, WINDOW_H - 150});

    Font debug_font("debugfont.ttf");
    debug_font.setSmooth(true);
    Text debug_text(debug_font);
    pushdl(&debug_text);
    debug_text.setPosition({WINDOW_W - 175, 10});

    Clock cooldown;
    Clock delta_clock;

    std::array<RectangleShape, 32> hearts;
    for (int i = 0; i < hearts.size(); i++) {
        hearts[i].setSize({25, 25});
        hearts[i].setPosition({20 + 30.0f * i, 20});
        hearts[i].setFillColor(Color::Red);
        game_window.draw(hearts[i]);
    }

    bool ai_mode = false;
    bool ishowspeed = false;

    RectangleShape delim({VALID_AREA, WINDOW_H});
    pushdl(&delim);
    delim.setOutlineColor(Color::Red);
    delim.setOutlineThickness(5);
    delim.setFillColor(Color::Transparent);
    delim.setPosition({VALID_AREA, 0});

    while (game_window.isOpen()) {
        while (const std::optional event = game_window.pollEvent()) {
            if (event->is<Event::Closed>())
                game_window.close();

            if (event->is<Event::KeyPressed>()) {
                if (const auto *key_pressed =
                        event->getIf<Event::KeyPressed>()) {
                    if (key_pressed->code == Keyboard::Key::Q)
                        game_window.close();
                    if (key_pressed->code == Keyboard::Key::A)
                        ai_mode = !ai_mode;
                    if (key_pressed->code == Keyboard::Key::P)
                        paused = !paused;
                    if (key_pressed->code == Keyboard::Key::S)
                        ishowspeed = !ishowspeed;
                }
            }
        }

        Time delta_time = delta_clock.restart();
        f32 dt = delta_time.asSeconds();

        if (ishowspeed)
            dt *= 10;

        if (!paused) {
            Vector2i mouse_pos = Mouse::getPosition(game_window);
            if (mouse_pos.x >= 0 && mouse_pos.x <= WINDOW_W && !ai_mode) {
                paddle.setPosition(
                    {static_cast<float>(mouse_pos.x), paddle.getPosition().y});
            }

            if (cooldown.getElapsedTime().asSeconds() >
                    rand_range(5, 20) / 10.0f &&
                !ball_on_screen) {
                if (rand_range(0, 5) == 5)
                    spawn_ball();
                if (rand_range(0, 10) == 10)
                    spawn_ball();

                for (int i = 0; i < evilfuckingcounter; i++) {
                    if (rand_range(0, 15) == 15)
                        spawn_ball();
                }

                spawn_ball();
                ball_on_screen = true;
                cooldown.restart();
            }

            for (i32 i = 0; i < BALL_LIST_SIZE; i++) {
                if (ball_list[i]) {
                    CircleShape *spr = &ball_list[i]->sprite;
                    if (ai_mode)
                        paddle.setPosition(
                            {spr->getPosition().x, paddle.getPosition().y});

                    ball_list[i]->y_velocity +=
                        dt * 0.1f * ball_list[i]->gravity_mult;
                    spr->move({100 * dt * ball_list[i]->x_velocity / 20.0f,
                               ball_list[i]->y_velocity});

                    if (spr->getPosition().x > VALID_AREA) {
                        score++;
                        if (lives < 20)
                            lives++;
                        remove_ball(ball_list[i]->idx, ball_list[i]->d_idx);
                        continue;
                    }

                    if (spr->getPosition().y > WINDOW_H &&
                        spr->getPosition().x < VALID_AREA) {
                        lives--;
                        remove_ball(ball_list[i]->idx, ball_list[i]->d_idx);
                        continue;
                    } else if (spr->getPosition().y > WINDOW_H &&
                               spr->getPosition().x < VALID_AREA) {
                        if (lives < 20)
                            lives++;

                        score++;
                        remove_ball(ball_list[i]->idx, ball_list[i]->d_idx);
                        continue;
                    }

                    if (spr->getPosition().y < 0 && spr->getPosition().x > 50) {
                        ball_list[i]->y_velocity =
                            fabs(ball_list[i]->y_velocity) * 1.025f;
                    }

                    if (spr->getGlobalBounds().findIntersection(
                            paddle.getGlobalBounds())) {

                        paddle_w += 0.5;

                        paddle.setSize({paddle_w, paddle_h});
                        paddle.setOrigin(paddle.getSize() / 2.0f);
                        spr->move({0, -2 * paddle_h});
                        ball_list[i]->y_velocity =
                            -fabs(ball_list[i]->y_velocity) * 0.9f;
                        ball_list[i]->x_velocity += 10;
                    }
                }
            }

            debug_text.setString("score: " + std::to_string(score));
            debug_text.setPosition({0.0f + Mouse::getPosition(game_window).x,
                                    0.0f + Mouse::getPosition(game_window).y});
        }

        if (lives == 0) {
            std::cout << "nooooooo" << std::endl;
            exit(0);
        }

        if (score % 15 == 0) {
            evilfuckingcounter++;
            score++;
        }

        for (RectangleShape &r : hearts) {
            r.setFillColor(Color::Transparent);
        }

        game_window.clear(Color::Blue);

        for (i32 i = 0; i < std::min(lives, static_cast<i32>(hearts.size()));
             i++) {
            hearts[i].setFillColor(Color::Red);
            game_window.draw(hearts[i]);
        }

        for (i32 i = 0; i < DRAW_LIST_SIZE; i++)
            if (draw_list[i])
                game_window.draw(*draw_list[i]);

        game_window.display();
    }

    return 0;
}
