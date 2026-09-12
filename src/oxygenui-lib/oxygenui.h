#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <queue>

#include "SFML/Graphics.hpp"

//

const float pi = 3.14159265f;

extern bool updating;

enum class mousebutton {
	left,
	right,
	middle,
	none
};

enum class layout {
	none,
	flex_y,
	flex_x
};

enum class axis {
	width,
	height,
};

enum class text_align_x {
	left,
	center,
	right
};

enum class text_align_y {
	top,
	center,
	bottom
};

struct color {
	sf::Color content;
	float position;
};

struct color_composition {
	std::vector<std::shared_ptr<color>> content;
};

struct vector4 {
	float x_scale;
	float x_offset;
	float y_scale;
	float y_offset;
};

struct vec4 {
	float x;
	float y;
	float z;
	float w;
};

class uiobject {
public:
	uiobject();

	static std::shared_ptr<uiobject> create();
	void add(uiobject* child);
	void sort();
	void set_image(std::string s, bool smooth = true);

	bool clickable = true;

	vector4 size;
	vector4 pos;

	float visibility;
	float roundness;

	color_composition background_color;

	color_composition border_color;
	float border_size;
	float padding;

	int rotation;

	int id;
	int z_index = 0;
	int global_z_index = 0;
	std::string name;

	sf::Vector2f real_size;
	sf::Vector2f real_pos;
	sf::Vector2f absolute_pos;

	float shadow_size;
	sf::Vector2f shadow_offset;
	sf::Color shadow_color;

	layout layout_type = layout::none;
	sf::Vector2f flex_padding;

	sf::Image img;
	sf::Sprite sprite;
	sf::RenderTexture render_tex;

	sf::Texture background_image;
	std::string image_path;
	sf::Sprite image_sprite;
	bool repeated;
	sf::Vector2f image_offset{ 0.f, 0.f };
	sf::Vector2f image_scale{ 1.f, 1.f };

	float aspect_ratio = 0.0f;
	axis dominant_axis = axis::width;

	float min_w = 0.0f;
	float min_h = 0.0f; 
	axis dominant_padding_axis = axis::width;

	std::string text_content = "";
	sf::Color text_color;
	sf::Color text_border_color;
	float text_border_size = 0;
	sf::Text::Style text_style = sf::Text::Style::Regular;
	std::string font_path = "";
	sf::Font font;
	float text_size = 1.f;

	text_align_x align_x = text_align_x::center;
	text_align_y align_y = text_align_y::center;
	bool text_wrap = false;

	uiobject* parent;
	std::vector<uiobject*> children;
};

enum class event_t {
	mousedown,
	mouseup,
	scrollup,
	scrolldown,
	hover,
	none
};

struct event {
	event_t type = event_t::none;
	mousebutton button = mousebutton::none;
	sf::Vector2f mouse_pos{ 0.f, 0.f };
	uiobject* object = nullptr;
};

extern std::vector<std::shared_ptr<uiobject>> ui_objects;

void draw(sf::RenderTarget& win);
void sort();

event check_events(sf::Event& ev, sf::RenderWindow& win);