#include "oxygenui-lib/oxygenui.h"

void init_ui() {
	//bg
	auto bg = uiobject::create();
	bg->pos = { -0.5,0,-0.5,0 };
	bg->size = { 2,0,2,0 };
	bg->id = 0;
	bg->set_image("res/a.png", false);
	bg->rotation = 12;
	bg->repeated = true;
	bg->image_scale = { 0.5f, 0.5f };

	auto wh = std::make_shared<color>();
	wh->content = sf::Color(255, 255, 255);
	wh->position = 0.0f;

	auto g = std::make_shared<color>();
	g->content = sf::Color(25, 25, 25);
	g->position = 1.0f;

	bg->background_color.content.push_back(wh);
	bg->background_color.content.push_back(g);

	//black
	auto black = uiobject::create();
	black->pos = { 0,0,0,0 };
	black->size = { 1,0,1,0 };
	black->roundness = 0.05;
	black->padding = 0.1;
	black->id = 1;

	black->shadow_size = 0.1f;
	black->shadow_offset = sf::Vector2f(0.f, 0.f);
	black->shadow_color = sf::Color(0, 0, 0, 25);

	auto g2 = std::make_shared<color>();
	g2->content = sf::Color(35, 35, 35, 225);
	g2->position = 0.0f;

	auto b = std::make_shared<color>();
	b->content = sf::Color(0,0,0, 205);
	b->position = 1.f;

	black->layout_type = layout::flex_y;
	black->flex_padding = { 0.05, 0.04 };

	black->background_color.content.push_back(g2);
	black->background_color.content.push_back(b);

	//dark grey
	auto darkg = uiobject::create();
	darkg->pos = { 0,0,0,0 };
	darkg->size = { 1, 0, 1, 0 };
	darkg->roundness = 0.025;
	darkg->padding = 0.035;
	darkg->id = 2;

	auto wg = std::make_shared<color>();
	wg->content = sf::Color(150, 150, 150);
	wg->position = 0.0f;

	auto mg = std::make_shared<color>();
	mg->content = sf::Color(100, 100, 100);
	mg->position = 1.f;


	darkg->background_color.content.push_back(wg);
	darkg->background_color.content.push_back(mg);

	/*auto border_color_point = std::make_shared<color>();
	border_color_point->content = sf::Color::Black;
	border_color_point->position = 0.0f;
	ass2->border_size = 0.01f;
	ass2->border_color.content.push_back(border_color_point);*/

	darkg->shadow_size = 0.01f;
	darkg->shadow_offset = sf::Vector2f(0.f, 0.f);
	darkg->shadow_color = sf::Color(0, 0, 0, 25);

	//bar
	auto bar = uiobject::create();
	bar->pos = { 0,0,0,0 };
	bar->size = { 1, 0, 0.125, 0 };
	bar->roundness = 0.025;
	bar->padding = 0.035;
	bar->id = 3;

	bar->background_color.content.push_back(wh);
	bar->background_color.content.push_back(wg);

	bar->shadow_size = 0.01f;
	bar->shadow_offset = sf::Vector2f(0.f, 0.f);
	bar->shadow_color = sf::Color(0, 0, 0, 25);
	bar->layout_type = layout::flex_x;
	bar->flex_padding = { 0.005f, 0.01f };

	bar->min_h = 48.f;

	auto cz = std::make_shared<color>();
	cz->content = sf::Color::Transparent;
	cz->position = 1.f;

	//title
	auto title = uiobject::create();
	title->size = { 1, 0, 1, 0 };
	title->pos = { 0.015,0,0,0 };
	title->id = 67;
	title->dominant_axis = axis::height;
	title->background_color.content.push_back(cz);
	title->text_content = "oxygenui demo";
	title->text_color = sf::Color::Black;
	title->font_path = "res/fonts/Montserrat-SemiBold.ttf";
	title->align_x = text_align_x::left;
	title->align_y = text_align_y::center;
	title->text_wrap = true;
	title->text_size = 0.4f;

	//icon
	auto icon = uiobject::create();
	icon->size = { 0,0,1,0 };
	icon->background_color.content.push_back(cz);
	icon->pos = { 0,0,0,0 };
	icon->set_image("res/icon.png");
	icon->dominant_axis = axis::height;
	icon->aspect_ratio = 1;
	icon->padding = 0.2f;
	icon->dominant_padding_axis = axis::height;

	//middle
	auto mid = uiobject::create();
	mid->background_color.content.push_back(cz);
	mid->size = { 1,0,1,0 };
	mid->pos = { 0,0,0,0 };
	mid->roundness = 0.025;
	mid->padding = 0.05;
	mid->set_image("res/catbunny.jpg");
	mid->name = "mid";

	mid->shadow_size = 0.0075f;
	mid->shadow_offset = sf::Vector2f(0.f, 0.f);
	mid->shadow_color = sf::Color(0, 0, 0, 20);

	/*auto border_color_point = std::make_shared<color>();
	border_color_point->content = sf::Color(154, 87, 255);
	border_color_point->position = 0.0f;

	auto border_color_point2 = std::make_shared<color>();
	border_color_point2->content = sf::Color(89, 0, 222);
	border_color_point2->position = 1.0f;

	mid->border_size = 0.01f;
	mid->border_color.content.push_back(border_color_point);
	mid->border_color.content.push_back(border_color_point2);*/

	//tree
	darkg->add(mid.get());
	darkg->sort();

	bar->add(icon.get());
	bar->add(title.get());
	bar->sort();

	black->add(bar.get());
	black->add(darkg.get());
	black->sort();

	sort();
}