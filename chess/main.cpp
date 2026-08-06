#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <array>
#include <iostream>
#include <stdint.h>

#define WINDOW_SIZE 1024
#define TILE_SIZE 128
#define DRAW_QUEUE_SIZE 128
#define KEY_PRESSED(k) key_pressed->code == Keyboard::Key::k

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

using namespace sf;


std::array<Drawable *, DRAW_QUEUE_SIZE> draw_queue;

i32 dq_add(Drawable *d) {
    for (i32 i = 0; i < DRAW_QUEUE_SIZE; i++)
        if (!draw_queue[i]) {
            draw_queue[i] = d;
            return i;
        }

    std::cerr << "draw queue full you fucker\n";
    exit(1);
}

void dq_remove(i32 index) { draw_queue[index] = nullptr; }

bool bound_check(Vector2f pos) {
    return pos.x >= 0 && pos.x < WINDOW_SIZE && pos.y >= 0 &&
           pos.y < WINDOW_SIZE;
}

struct Piece {
    RectangleShape sprite;
    Piece() {
        sprite.setFillColor(Color::Red);
        sprite.setSize({100, 100});
        sprite.setPosition(Vector2f(Mouse::getPosition()));
    }
};

int main() {
    for (auto &d : draw_queue)
        d = nullptr;

    RenderWindow game_window(sf::VideoMode({WINDOW_SIZE, WINDOW_SIZE}), "SFML");
    std::array<RectangleShape, 64> grid;
    const i8 magicfuckingnumber = 85;
    Piece p;

    // evil fucking bit manipulation loop for the grid i have no idea why i
    // did this bullshit instead of like. A normal loop and just setting the
    // rectangles but i was in too deep anyways and had to finish it so
    // like #Whatever no one gaf
    for (i32 i = 0; i < grid.size(); i++) {
        i32 col = i % 8;
        i32 row = i / 8;

        grid[i].setSize({TILE_SIZE, TILE_SIZE});
        grid[i].setPosition(Vector2f(col * TILE_SIZE, row * TILE_SIZE));

        i32 bit_idx = 7 - (col % 8);
        i8 bit = (magicfuckingnumber >> bit_idx) & 1;

        bit ^= (row & 1);

        if (bit == 0)
            grid[i].setFillColor(Color::White);
        else
            grid[i].setFillColor(Color::Black);

        dq_add(&grid[i]);
    }

    dq_add(&p.sprite);

    RectangleShape cursor({TILE_SIZE, TILE_SIZE});
    cursor.setFillColor(Color::Transparent);
    cursor.setOutlineColor(Color::Red);
    cursor.setOutlineThickness(5);
    dq_add(&cursor);

    bool selected = false;

    while (game_window.isOpen()) {
        Vector2f cursor_pos = cursor.getPosition();
        Vector2f cursor_movement_vec({0, 0});

        while (const std::optional event = game_window.pollEvent()) {
            if (event->is<Event::Closed>())
                game_window.close();
            if (event->is<Event::KeyPressed>()) {
                if (const auto *key_pressed =
                        event->getIf<Event::KeyPressed>()) {
                    if (KEY_PRESSED(Q))
                        game_window.close();

                    switch (key_pressed->code) {
                    case Keyboard::Key::Q:
                        game_window.close();
                        break;

                    case Keyboard::Key::Z:
                        selected = !selected;
                        if (selected)
                            cursor.setOutlineColor(Color::Blue);
                        else
                            cursor.setOutlineColor(Color::Red);
                        break;

                    case Keyboard::Key::Up:
                        cursor_movement_vec = {0, -TILE_SIZE};
                        break;

                    case Keyboard::Key::Down:
                        cursor_movement_vec = {0, TILE_SIZE};
                        break;

                    case Keyboard::Key::Left:
                        cursor_movement_vec = {-TILE_SIZE, 0};
                        break;

                    case Keyboard::Key::Right:
                        cursor_movement_vec = {TILE_SIZE, 0};
                        break;

                    default:
                        break;
                    }
                }
            }
        }

        Vector2f new_pos = cursor_pos + cursor_movement_vec;
        if (bound_check(new_pos))
            cursor.setPosition(new_pos);

        p.sprite.setPosition(Vector2f(Mouse::getPosition(game_window)));
        game_window.clear(Color::Black);

        for (i32 i = 0; i < DRAW_QUEUE_SIZE; i++)
            if (draw_queue[i])
                game_window.draw(*draw_queue[i]);

        game_window.display();
    }
}
