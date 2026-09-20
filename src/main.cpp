#include <iostream>
#include <thread>
#include <chrono>

#include "oxygenui-lib/oxygenui.h"
#include "uithing.h"

std::shared_ptr<oxyui::tween> git_tween = nullptr;
std::shared_ptr<oxyui::tween> wiki_tween = nullptr;
std::shared_ptr<oxyui::tween> demo_tween = nullptr;

oxyui::uiobject* gitbtn;
oxyui::uiobject* wikibtn;
oxyui::uiobject* demobtn;

oxyui::tween_info info;

void bulge(std::string mode) {
    info.target_time = 0.15f;
    info.easing_style = oxyui::ease_style::quad;
    info.easing_type = oxyui::ease_type::out;

    float git_factor = 1.f;
    float wiki_factor = 1.f;
    float demo_factor = 1.f;

    if (mode == "git") {
        git_factor = 1.25f;
        wiki_factor = 1.f;
        demo_factor = 1.f;
    }
    if (mode == "wiki") {
        git_factor = 1.f;
        wiki_factor = 1.25f;
        demo_factor = 1.f;
    }
    if (mode == "demo") {
        git_factor = 1.f;
        wiki_factor = 1.f;
        demo_factor = 1.25f;
    }
    if (mode == "nothing") {
        git_factor = 1.f;
        wiki_factor = 1.f;
        demo_factor = 1.f;
    }

    wiki_tween = oxyui::tween::create(sys, wikibtn, info, { {"size.y_scale", wiki_factor} });
    wiki_tween->play();
    git_tween = oxyui::tween::create(sys, gitbtn, info, { {"size.y_scale", git_factor} });
    git_tween->play();
    demo_tween = oxyui::tween::create(sys, demobtn, info, { {"size.y_scale", demo_factor} });
    demo_tween->play();
}

void display(sf::RenderWindow& win) {
    win.clear({ 1,0,0 });
    sys.draw(win);
    win.display();
    sys.update_tweens();
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

    sys.update_uiobjects(win);
    display(win);
    sys.update_uiobjects(win);

    while (win.isOpen()) {
        dt = clock.restart().asSeconds();
        while (win.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) {
                win.close();
            }
            if (ev.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0.f, 0.f, static_cast<float>(ev.size.width), static_cast<float>(ev.size.height));
                win.setView(sf::View(visibleArea));
                sys.update_uiobjects(win);
            }

            oxyui::event e = sys.check_events(ev, win);

            if (e.object) {
                if (e.object->name == "git" || e.object->name == "wiki" || e.object->name == "demo") {
                    if (e.type == oxyui::event_t::hover) {
                        win.setMouseCursor(cursor_hand);

                        if (e.object->name == "git") {
                            bulge("git");
                        }
                        else if (e.object->name == "wiki") {
                            bulge("wiki");
                        }
                        else if (e.object->name == "demo") {
                            bulge("demo");
                        }
                        else {
                            bulge("nothing");
                        }
                    }
                }
                else {
                    win.setMouseCursor(cursor_arrow);
                    bulge("nothing");
                }
            }
        }

        oxyui::uiobject* main_center = sys.objects[2].get(); // Или найдите его по имени center через get_by_name

        if ((git_tween && git_tween->is_playing()) ||
            (wiki_tween && wiki_tween->is_playing()) ||
            (demo_tween && demo_tween->is_playing())) {

            main_center->update(win, sys);
        }


        if (dt > 0.16f) dt = 0.f;

        x -= 16 * dt;
        y += 16 * dt;
        sys.objects[0]->image_offset = { x,y };

        display(win);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    sys.objects.clear();
}


int main() {
    sf::ContextSettings st;
    st.antialiasingLevel = 8;
	sf::RenderWindow win;
	win.create(sf::VideoMode(1280,720), "oxygenui demo", sf::Style::Default, st);

    sf::Image icon;
    if (icon.loadFromFile("res/icon.png")) {
        win.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    }

    win.setFramerateLimit(100);

	init_ui();

    gitbtn = sys.get_by_name("git");
    wikibtn = sys.get_by_name("wiki");
    demobtn = sys.get_by_name("demo");

    start(win);

	return 0;
}