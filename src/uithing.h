#include "oxygenui-lib/oxygenui.h"
#include "resources.h"

oxyui::uisystem sys;

std::string build_number;

void init_ui() {
	build_number = "22092026\\1800";

	auto tr = std::make_shared<oxyui::color>();
	tr->content = sf::Color::Transparent;
	tr->position = 0.0f;

	//bg
	auto bg = oxyui::uiobject::create(sys);
	bg->pos = { -0.5,0,-0.5,0 };
	bg->size = { 2,0,2,0 };
	bg->id = 0;
	bg->set_image_from_mem(a_png_dat, a_png_len, false);
	bg->rotation = 12;
	bg->repeated = true;
	bg->image_scale = { 0.75f, 0.75f };

	auto wh = std::make_shared<oxyui::color>();
	wh->content = sf::Color(255, 255, 255);
	wh->position = 0.0f;

	auto g = std::make_shared<oxyui::color>();
	g->content = sf::Color(25, 25, 25);
	g->position = 1.0f;

	bg->background_color.content.push_back(wh);
	bg->background_color.content.push_back(g);

	//black
	auto black = oxyui::uiobject::create(sys);
	black->pos = { 0,0,0,0 };
	black->size = { 1,0,1,0 };

	auto g2 = std::make_shared<oxyui::color>();
	g2->content = sf::Color(35, 35, 35, 200);
	g2->position = 0.0f;

	auto b = std::make_shared<oxyui::color>();
	b->content = sf::Color(0,0,0, 180);
	b->position = 1.f;

	black->background_color.content.push_back(g2);
	black->background_color.content.push_back(b);


	//center
	auto center = oxyui::uiobject::create(sys);
	center->padding = { 0.05, 0.05, 0.05, 0.05};
	center->size = { 1,0,1,0 };
	center->layout_type = oxyui::layout::flex_y;
	center->flex_padding = { 0.05, 0.05 };
	center->background_color.content.push_back(tr);
	center->name = "center";
	
	//bar
	auto bar = oxyui::uiobject::create(sys);
	bar->size = { 1,0,0.145,0 };
	bar->roundness = 0.03;
	bar->shadow_color = sf::Color(0, 0, 0, 25);
	bar->shadow_size = 0.025f;
	bar->shadow_offset = { 0,0 };
	bar->layout_type = oxyui::layout::flex_x;
	bar->name = "bar";
	//bar->flex_padding = { 0.05, 0.05 };
	
	auto grey = std::make_shared<oxyui::color>();
	grey->content = sf::Color(175, 175, 175);
	grey->position = 1.f;

	bar->background_color.content.push_back(wh);
	bar->background_color.content.push_back(grey);

	//horizontal
	auto hor = oxyui::uiobject::create(sys);
	hor->size = { 1,0,0.85,0 };
	hor->layout_type = oxyui::layout::flex_x;
	hor->flex_padding = { 0.025,0.05 };
	hor->background_color.content.push_back(tr);

	//grey windows
	auto grey1 = oxyui::uiobject::create(sys);
	grey1->size = { 1,0,0.6,0 };
	grey1->layout_type = oxyui::layout::flex_y;
	grey1->flex_padding = { 0.05,0.05 };
	grey1->roundness = 0.03;
	grey1->shadow_color = sf::Color(0, 0, 0, 25);
	grey1->shadow_size = 0.025f;
	grey1->shadow_offset = { 0,0 };
	
	auto gr1 = std::make_shared<oxyui::color>();
	gr1->content = sf::Color(150,150,150);
	gr1->position = 0.f;

	auto gr2 = std::make_shared<oxyui::color>();
	gr2->content = sf::Color(100, 100, 100);
	gr2->position = 1.f;

	grey1->background_color.content.push_back(gr1);
	grey1->background_color.content.push_back(gr2);

	grey1->name = "text";

	auto grey2 = oxyui::uiobject::create(sys);
	grey2->size = { 0.75,0,0.85,0 };
	grey2->layout_type = oxyui::layout::flex_y;
	grey2->flex_padding = { 0.05,0.05 };
	grey2->roundness = 0.03;
	grey2->shadow_color = sf::Color(0, 0, 0, 25);
	grey2->shadow_size = 0.025f;
	grey2->shadow_offset = { 0,0 };

	grey2->background_color.content.push_back(gr1);
	grey2->background_color.content.push_back(gr2);

	grey2->name = "links";

	
	//blacks

	auto bl = std::make_shared<oxyui::color>();
	bl->content = sf::Color(0, 0, 0, 153);;
	bl->position = 0.f;

	auto black2 = oxyui::uiobject::create(sys);
	black2->size = { 1,0,1,0 };
	black2->roundness = 0.03;
	black2->padding = { 0.015, 0.015, 0.015, 0.015 };
	black2->flex_padding = { 0.025,0.025 };

	black2->background_color.content.push_back(bl);

	auto black1 = oxyui::uiobject::create(sys);
	black1->size = { 1,0,1,0 };
	black1->roundness = 0.03;
	black1->padding = { 0.015, 0.015, 0.015, 0.015 };
	black1->flex_padding = { 0.025,0.025 };

	black1->background_color.content.push_back(bl);


	//titles
	auto title_about = oxyui::uiobject::create(sys);
	title_about->size = { 1,0,0.4,0 };
	title_about->background_color.content.push_back(tr);
	title_about->set_font_from_mem(fonts_mont_sb_ttf_dat, fonts_mont_sb_ttf_len);
	title_about->text_content = "about";
	title_about->text_color = sf::Color::White;
	title_about->text_size = 0.03f;
	title_about->padding = {0.01, 0.01, 0.01, 0.01};
	title_about->dominant_axis = oxyui::axis::height;
	title_about->align_x = oxyui::text_align_x::left;
	title_about->align_y = oxyui::text_align_y::center;
	title_about->text_border_color = sf::Color::Transparent;
	title_about->text_style = sf::Text::Regular;

	auto title_links = oxyui::uiobject::create(sys);
	title_links->size = { 1,0,0.3,0 };
	title_links->background_color.content.push_back(tr);
	title_links->set_font_from_mem(fonts_mont_sb_ttf_dat, fonts_mont_sb_ttf_len);
	title_links->text_content = "links";
	title_links->text_color = sf::Color::White;
	title_links->text_size = 0.03f;
	title_links->padding = { 0.01, 0.01, 0.01, 0.01 };
	title_links->dominant_axis = oxyui::axis::height;
	title_links->align_x = oxyui::text_align_x::left;
	title_links->align_y = oxyui::text_align_y::center;
	title_links->text_border_color = sf::Color::Transparent;
	title_links->text_style = sf::Text::Regular;

	auto title_main = oxyui::uiobject::create(sys);
	title_main->size = { 1,0,1,0 };
	title_main->background_color.content.push_back(tr);
	title_main->set_font_from_mem(fonts_mont_sb_ttf_dat, fonts_mont_sb_ttf_len);
	title_main->text_content = "oxygenui";
	title_main->text_color = sf::Color::Black;
	title_main->text_size = 0.03f;
	title_main->dominant_axis = oxyui::axis::height;
	title_main->align_x = oxyui::text_align_x::left;
	title_main->align_y = oxyui::text_align_y::center;
	title_main->text_border_color = sf::Color::Transparent;
	title_main->text_style = sf::Text::Regular;

	//pos
	auto mid1 = oxyui::uiobject::create(sys);
	mid1->size = { 1,0,1,0 };
	mid1->background_color.content.push_back(tr);
	mid1->layout_type = oxyui::layout::flex_y;
	mid1->padding = {0.015, 0.015, 0, 0.015};

	auto icon = oxyui::uiobject::create(sys);
	icon->background_color.content.push_back(tr);
	icon->size = { 1,0,1,0 };
	icon->padding = { 0.0175, 0.0175, 0.0175, 0.0175 };
	icon->set_image_from_mem(icon_png_dat, icon_png_len, true);
	icon->aspect_ratio = 1;
	icon->dominant_axis = oxyui::axis::height;
	icon->dominant_padding_axis = oxyui::axis::height;

	auto mid2 = oxyui::uiobject::create(sys);
	mid2->size = { 1,0,1,0 };
	mid2->background_color.content.push_back(tr);
	mid2->layout_type = oxyui::layout::flex_y;
	mid2->padding = { 0.015, 0.015, 0, 0.015 };


	auto about_text = oxyui::uiobject::create(sys);
	about_text->size = { 1,0,1,0 };
	about_text->padding = { 0.01, 0.01, 0.01, 0.01 };
	about_text->background_color.content.push_back(tr);
	about_text->text_color = sf::Color::White;
	about_text->text_size = 0.025;
	about_text->text_content = "oxygenui is a crossplatform, simple and intuitive GUI library that brings UI development experience of CSS and Roblox Studio into C++.\nno strict, boring af standard window - forms (like in GTK / Qt), only pure creative freedom!";
	about_text->text_wrap = true;
	about_text->dominant_axis = oxyui::axis::height;
	about_text->set_font_from_mem(fonts_mont_sb_ttf_dat, fonts_mont_sb_ttf_len);
	black1->add(about_text.get());
	about_text->name = "about_text";


	auto info_text = oxyui::uiobject::create(sys);
	info_text->size = { 1,0,0.25,0 };
	info_text->pos = { 0,0,0.75,0 };
	info_text->background_color.content.push_back(tr);
	info_text->text_color = sf::Color(255,255,255, 50);
	info_text->text_size = 0.025;
	info_text->text_content = "demo build number: "+build_number+"\noxygenui release string: "+oxyui::release_string;
	info_text->text_wrap = true;
	info_text->dominant_axis = oxyui::axis::height;
	info_text->set_font_from_mem(fonts_mont_sb_ttf_dat, fonts_mont_sb_ttf_len);
	info_text->z_index = 67;


	//btns
	auto prpl1 = std::make_shared<oxyui::color>();
	prpl1->content = sf::Color(101, 84, 255, 255);
	auto prpl2 = std::make_shared<oxyui::color>();
	prpl2->content = sf::Color(66, 0, 181, 255);

	auto gitbtn = oxyui::uiobject::create(sys);
	gitbtn->size = { 1,0,1,0 };
	gitbtn->background_color.content.push_back(prpl1);
	gitbtn->background_color.content.push_back(prpl2);
	gitbtn->roundness = 0.0175;
	gitbtn->shadow_color = sf::Color(0, 0, 0, 50);
	gitbtn->shadow_size = 0.025f;
	gitbtn->shadow_offset = { 0,0 };
	gitbtn->name = "git";

	auto wikibtn = oxyui::uiobject::create(sys);
	wikibtn->size = { 1,0,1,0 };
	wikibtn->background_color.content.push_back(wh);
	wikibtn->background_color.content.push_back(grey);
	wikibtn->roundness = 0.0175;
	wikibtn->shadow_color = sf::Color(0, 0, 0, 50);
	wikibtn->shadow_size = 0.025f;
	wikibtn->shadow_offset = { 0,0 };
	wikibtn->name = "wiki";

	auto demobtn = oxyui::uiobject::create(sys);
	demobtn->size = { 1,0,1,0 };
	demobtn->background_color.content.push_back(wh);
	demobtn->background_color.content.push_back(grey);
	demobtn->roundness = 0.0175;
	demobtn->shadow_color = sf::Color(0, 0, 0, 50);
	demobtn->shadow_size = 0.025f;
	demobtn->shadow_offset = { 0,0 };
	demobtn->name = "demo";

	auto btnfrm = oxyui::uiobject::create(sys);
	btnfrm->size = { 1,0,1,0 };
	btnfrm->padding = { 0.005,0.005,0.005,0.005 };
	btnfrm->background_color.content.push_back(tr);
	btnfrm->layout_type = oxyui::layout::flex_y;
	btnfrm->flex_padding = { 0.02, 0.02 };

	auto text_github = oxyui::uiobject::create(sys);
	text_github->size = { 1,0,1,0 };
	text_github->background_color.content.push_back(tr);
	text_github->set_font_from_mem(fonts_mont_sb_ttf_dat, fonts_mont_sb_ttf_len);
	text_github->text_content = "github";
	text_github->text_color = sf::Color::White;
	text_github->text_size = 0.025f;
	text_github->padding = { 0.01, 0.01, 0.01, 0.01 };
	text_github->dominant_axis = oxyui::axis::height;
	text_github->align_x = oxyui::text_align_x::center;
	text_github->align_y = oxyui::text_align_y::center;
	text_github->text_border_color = sf::Color::Transparent;
	text_github->text_style = sf::Text::Regular;
	text_github->clickable = false;

	auto text_wiki = oxyui::uiobject::create(sys);
	text_wiki->size = { 1,0,1,0 };
	text_wiki->background_color.content.push_back(tr);
	text_wiki->set_font_from_mem(fonts_mont_sb_ttf_dat, fonts_mont_sb_ttf_len);
	text_wiki->text_content = "wiki";
	text_wiki->text_color = sf::Color::Black;
	text_wiki->text_size = 0.025f;
	text_wiki->padding = { 0.01, 0.01, 0.01, 0.01 };
	text_wiki->dominant_axis = oxyui::axis::height;
	text_wiki->align_x = oxyui::text_align_x::center;
	text_wiki->align_y = oxyui::text_align_y::center;
	text_wiki->text_border_color = sf::Color::Transparent;
	text_wiki->text_style = sf::Text::Regular;
	text_wiki->clickable = false;

	auto text_demo = oxyui::uiobject::create(sys);
	text_demo->size = { 1,0,1,0 };
	text_demo->background_color.content.push_back(tr);
	text_demo->set_font_from_mem(fonts_mont_sb_ttf_dat, fonts_mont_sb_ttf_len);
	text_demo->text_content = "demo";
	text_demo->text_color = sf::Color::Black;
	text_demo->text_size = 0.025f;
	text_demo->padding = { 0.01, 0.01, 0.01, 0.01 };
	text_demo->dominant_axis = oxyui::axis::height;
	text_demo->align_x = oxyui::text_align_x::center;
	text_demo->align_y = oxyui::text_align_y::center;
	text_demo->text_border_color = sf::Color::Transparent;
	text_demo->text_style = sf::Text::Regular;
	text_demo->clickable = false;

	gitbtn->add(text_github.get());
	wikibtn->add(text_wiki.get());
	demobtn->add(text_demo.get());

	btnfrm->add(gitbtn.get());
	btnfrm->add(wikibtn.get());
	btnfrm->add(demobtn.get());
	black2->add(btnfrm.get());

	bar->add(icon.get());
	bar->add(title_main.get());

	mid1->add(title_about.get());
	mid1->add(black1.get());
	grey1->add(mid1.get());

	mid2->add(title_links.get());
	mid2->add(black2.get());
	grey2->add(mid2.get());

	hor->add(grey1.get());
	hor->add(grey2.get());

	center->add(bar.get());
	center->add(hor.get());

	black->add(center.get());

	sys.sort();
}