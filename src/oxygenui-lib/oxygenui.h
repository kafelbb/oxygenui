#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <queue>

#include "SFML/Graphics.hpp"


namespace oxyui {

	const float pi = 3.14159265f;

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

	struct padding_t {
		float left = 0;
		float right = 0;
		float top = 0;
		float bottom = 0;
	};

	struct vec4 {
		float x;
		float y;
		float z;
		float w;
	};

	struct uisystem;

	class uiobject {
	public:
		uiobject();

		static std::shared_ptr<uiobject> create(uisystem& sys);
		void add(uiobject* child);
		void sort();
		void set_image(std::string s, bool smooth = true);

		void update(sf::RenderTarget& win, const uisystem& ui_sys);
		void draw(sf::RenderTarget& win, const uisystem& ui_sys);

	public:
		bool clickable = true;

		vector4 size;
		vector4 pos;

		float visibility;
		float roundness;

		color_composition background_color;
		color_composition border_color;
		float border_size;
		padding_t padding{ 0.0f, 0.0f, 0.0f, 0.0f };

		int rotation;

		int id;
		int z_index = 0;
		int global_zindex = 0;
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
		axis dominant_flex_padding_axis = axis::width;

		std::string text_content = "";
		sf::Color text_color;
		sf::Color text_border_color = sf::Color::Transparent;
		float text_border_size = 0;
		sf::Text::Style text_style = sf::Text::Style::Regular;
		std::string font_path = "";
		sf::Font font;
		float text_size = 1.f;
		bool font_loaded = false;
		std::string wrapped_str;
		sf::Text text;

		text_align_x align_x = text_align_x::center;
		text_align_y align_y = text_align_y::center;
		bool text_wrap = false;

		uiobject* parent;
		std::vector<uiobject*> children;
	};

	class tween;

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

	struct uisystem {
		std::vector<std::shared_ptr<uiobject>> objects;
		std::vector<std::shared_ptr<tween>> tweens;

		void draw(sf::RenderTarget& win);
		void sort();
		void update_uiobjects(sf::RenderTarget& win);
		void update_tweens();

		uiobject* get_by_name(const std::string& target_name);
		uiobject* get_by_id(int target_id);

		event check_events(sf::Event& ev, sf::RenderWindow& win);

		sf::Vector2f window_size;
	};

	enum class ease_type {
		in,
		out,
		inout
	};

	enum class ease_style {
		linear,
		quad,
		quart,
		cubic,
		step
	};

	// --- Внутри namespace oxyui ---

	struct tween_info {
		float target_time = 1.0f;                        // Длительность в секундах
		ease_style easing_style = ease_style::linear;
		ease_type easing_type = ease_type::out;
	};

	class tween {
	public:
		static std::shared_ptr<tween> create(uisystem& sys, uiobject* target, tween_info info, std::map<std::string, float> target_properties);

		void play();
		void pause();
		void stop();

		void update(float dt);

		bool is_playing() const { return playing; }
		bool is_finished() const { return finished; }

	private:
		bool playing = false;
		bool finished = false;
		float current_time = 0.0f;

		uiobject* target = nullptr;
		tween_info description;

		std::map<std::string, float> goal_properties;
		std::map<std::string, float> initial_properties;

		float get_eased_value(float alpha);
		void apply_property(const std::string& prop, float value);
		float get_current_property_value(const std::string& prop);
	};

}