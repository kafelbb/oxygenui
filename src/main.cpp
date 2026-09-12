#include <iostream>
#include <thread>
#include <chrono>

#include "oxygenui-lib/oxygenui.h"
#include "uithing.h"

void update(sf::RenderWindow& win) {
    win.clear({ 1,0,0 });
    draw(win);
    win.display();
}

void start(sf::RenderWindow& win) {
    sf::Event ev;

    sf::Clock clock;
    float dt = clock.restart().asSeconds();

    float x = 0;
    float y = 0;

    static sf::Cursor cursor_arrow;
    static sf::Cursor cursor_hand;
    bool debounce = false;

    cursor_arrow.loadFromSystem(sf::Cursor::Arrow);
    cursor_hand.loadFromSystem(sf::Cursor::Hand);

    update(win);
    update(win);

    while (win.isOpen()) {
        dt = clock.restart().asSeconds();
        updating = false;
        while (win.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) {
                win.close();
            }
            if (ev.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0.f, 0.f, static_cast<float>(ev.size.width), static_cast<float>(ev.size.height));
                win.setView(sf::View(visibleArea));
                updating = true;
            }

            event e = check_events(ev, win);

            if (e.object) {
                if (e.object->name == "mid") {
                    if (e.type == event_t::hover) {
                        win.setMouseCursor(cursor_hand);
                    }

                    if (e.type == event_t::mousedown) {
                        e.object->visibility -= 0.15f;
                        updating = true;
                    }
                }
                else {
                    win.setMouseCursor(cursor_arrow);
                }
            }
        }

        if (dt > 0.16f) dt = 0.f;

        x -= 16 * dt;
        y += 16 * dt;
        ui_objects[0]->image_offset = { x,y };

        update(win);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    ui_objects.clear();
}


int main() {
    sf::ContextSettings st;
    st.antialiasingLevel = 8;
	sf::RenderWindow win;
	win.create(sf::VideoMode(512, 512), "oxygenui demo", sf::Style::Default, st);

    sf::Image icon;
    if (icon.loadFromFile("res/icon.png")) {
        win.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    }

    win.setFramerateLimit(100);

	init_ui();
    start(win);

	return 0;
}