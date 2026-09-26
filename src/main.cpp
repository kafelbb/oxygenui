#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <cstdlib>

#include "oxygenui-lib/oxygenui.h"
#include "uithing.h"

std::shared_ptr<oxyui::tween> git_tween = nullptr;
std::shared_ptr<oxyui::tween> wiki_tween = nullptr;
std::shared_ptr<oxyui::tween> demo_tween = nullptr;

oxyui::uiobject* gitbtn;
oxyui::uiobject* wikibtn;
oxyui::uiobject* demobtn;

oxyui::uiobject* links;
oxyui::uiobject* text;
oxyui::uiobject* about;
oxyui::uiobject* bar;
oxyui::uiobject* center;

oxyui::tween_info info;

void open_link(const std::string& url) {
#if defined(_WIN32) || defined(_WIN64)
    std::string command = "start " + url;
    std::system(command.c_str());
#elif defined(__APPLE__)
    std::string command = "open " + url;
    std::system(command.c_str());
#elif defined(__LINUX__) || defined(__linux__)
    std::string command = "xdg-open " + url;
    std::system(command.c_str());
#else
#error "unsupported platform"
#endif
}

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

void apply_adaptive_bullshit(float aspect_ratio) {
    std::cout << aspect_ratio << std::endl;
    if (aspect_ratio < 1.6f && aspect_ratio > 1.3f) { //4/3
        links->size = { 0.75,0,0.75,0 };
        text->size = { 1,0,0.55,1 };
        about->text_size = 0.025;
        bar->size = { 1,0,0.12,0 };
        center->padding = { 0.05, 0.05, 0.05, 0.05 };
    }
    if (aspect_ratio < 1.83f && aspect_ratio > 1.6f) { //16/9
        links->size = { 0.75,0,0.85,0 };
        text->size = { 1,0,0.6,1 };
        about->text_size = 0.025;
        bar->size = { 1,0,0.145,0 };
        center->padding = { 0.05, 0.05, 0.05, 0.05 };
    }
    if (aspect_ratio > 1.83f) { //uw
        center->padding = { 0.075, 0.075, 0.03, 0.03 };
    }
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

                float aspect_ratio = static_cast<float>(ev.size.width) / static_cast<float>(ev.size.height);
                apply_adaptive_bullshit(aspect_ratio);

                sys.update_uiobjects(win);
            }

            oxyui::event e = sys.check_events(ev, win);

            if (e.object) {
                if (e.object->name == "git" || e.object->name == "wiki" || e.object->name == "demo") {
                    if (e.object->name == "git") {
                        if (e.type == oxyui::event_t::hover) {
                            win.setMouseCursor(cursor_hand);
                            bulge("git");
                        }
                        else if (e.type == oxyui::event_t::mouseup) {
                            open_link("https://github.com/kafelbb/oxygenui");
                        }
                    }
                    else if (e.object->name == "wiki") {
                        if (e.type == oxyui::event_t::hover) {
                            win.setMouseCursor(cursor_hand);
                            bulge("wiki");
                        }
                        if (e.type == oxyui::event_t::mouseup) {
                            open_link("https://github.com/kafelbb/oxygenui/blob/main/readme/wiki.md");
                        }
                    }
                    else if (e.object->name == "demo") {
                        if (e.type == oxyui::event_t::hover) {
                            win.setMouseCursor(cursor_hand);
                            bulge("demo");
                        }
                        if (e.type == oxyui::event_t::mouseup) {
                            open_link("file:///C:/Users/ass/source/repos/oxygenui/page/images/o.mp4");
                        }
                    }
                    else {
                        if (e.type == oxyui::event_t::hover) {
                            win.setMouseCursor(cursor_hand);
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

    links = sys.get_by_name("links");
    text = sys.get_by_name("text");
    about = sys.get_by_name("about_text");
    bar = sys.get_by_name("bar");
    center = sys.get_by_name("center");

    start(win);

	return 0;
}