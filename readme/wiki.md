## Wiki
### intro
I highly recommend getting familliar with SFML 2.6.1 basics first, before we dive deeply into the material 

------------------------------
## data types
oxygenui introduces several custom data types:

- **vector4** (and its close sibling vec4)
- **color** 
- **color_composition**
- axis
- layout
- text_align_x (and its counterpart text_align_y)
- **mousebutton**
- **event_t**
- **event** 


------------------------------
vector4 — an UDim2 analouge

used in size and pos properties.

it's a mathematical vector that stores:

* float x_scale — normalized relative X coordinate
* float x_offset — absolute X coordinate
* float y_scale — normalized relative Y coordinate
* float y_offset — absolute Y coordinate

by "normalized coordinate," I mean a float num from 0 to 1 (and > 1, basically percentages divided by 100).

------------------------------
vec4 — a **vector4** analouge

it is a mathematical vector that stores:

* float x — normalized relative X coordinate
* float y — normalized relative Y coordinate
* float z — normalized relative Z coordinate
* float w — normalized relative W coordinate

------------------------------
color — a data type describing a specific color and its position in a linear gradient

it stores:

* sf::Color content — a 3 or 4-component mathematical vector describing the color via RGB(A).
* float position — a float number indicating the color's position along the linear gradient.

------------------------------
color_composition — an array of color data that defines the overall color of an object

it stores:

* std::vector<std::shared_ptr> content — an array of color instances

------------------------------
axis — an enum class specifying the axis used for calculations

it can be:

* width (default)
* height

------------------------------
layout — an enum class specifying how an object's children are positioned and sized.

it can be:

* none (default) — children are positioned independently of each other.
* flex_x — children fill the available space along the X axis.
* flex_y — children fill the available space along the Y axis.

------------------------------
text_align_x — an enum class specifying text justification

it can be:

* center (default)
* right
* left

------------------------------
text_align_y —an enum class specifying text alignment

it can be:

* center (default)
* top
* bottom

------------------------------
mousebutton — an enum class representing mouse buttons

it can be:

* left
* right
* middle
* none

------------------------------
event_t — an enum class representing the type of event that occurred with an object.

it can be:

* none — nothing happened
* mouseup — mouse button released on object
* mousedown — mouse button pressed on object
* hover — hovering the cursor over the object

------------------------------
event — a struct that represents the event itself

it stores:

* event_t type — type of event
* mousebutton button — pressed button
* sf::Vector2f mouse_pos — mouse position
* uiobject* object — a pointer to the object that event happend with

------------------------------
## architecture
### graphics pipeline
graphics pipeline vars:

* const float pi — pi constant used for rounding corners
* bool updating — a flag telling the pipeline to recalculate the absolute sizes of objects, recreate their render_texture buffers, and redraw them
* std::vector<std::shared_ptr> ui_objects — a global array that stores all uiobjects

graphics pipeline funcs:

* void draw(sf::RenderTarget& win) — begins drawing the root objects and triggers recursive rendering for their children
* void sort() — sorts objects from ``ui_objects`` based on their global_z_index
* event check_events(sf::Event& ev, sf::RenderWindow& win) — checks for events that happened with all ``uiobjects``

## uiobject
``uiobject`` is a class representing the abstract UI element used to construct your GUI. 
instead of dealing with a massive hierarchy of classes like **Frame**, **ImageLabel**, and others from Roblox Studio, or a bunch of HTML tags like **div**, **img**, or other, we have one single class that serves as a representation for all of them.

(i am also too lazy to make some sophisticated system like in rb studio)

every object has a ``sprite`` and a ``rendertexture``. When ``draw_obj()`` is called, 
**object** draws itself onto the ``rendertexture``, calls `draw_obj()` on his children, then it loads
its ``rendertexture`` into the ``sprite``, and finaly draws its ``sprite`` on its parent ``rendertexture``.
<br>This way oxygenui achieves some optimization "points", bc library doesn't need to do heavy calcualtions and OpenGL things every single frame. It only has to do it when ``updating`` is set to ``true``.

in short, ``uiobject`` is a container holding an already-rendered version of itself, its children (and holding data about its description, aka properties)

#### properties of uiobject:

* **core properties**
   * bool **clickable** (default: ``true``) — determines if the object is tracked during input handling within check_events().
   * vector4 **size** — the size of the object relative to its parent.
   * vector4 **pos** — the position of the object relative to its parent.
   * float **visibility** (default: ``1.0f``) — the opacity of the object (from 0.0 to 1.0), evaluated relative to its parent's visibility.
   * float **roundness** — the corner rounding coefficient.
   * color_composition **background_color** — the filling color configuration of the object.
   * color_composition **border_color** — the outline color configuration.
   * float **border_size** — the thickness of the object's outline.
   * float **padding** — the offset of the object from its parent's boundaries.
   * int **rotation** — the rotation angle of the object in degrees.
   * int **id** — a unique numeric ID for the object.
   * int **z_index** (default: ``0``) — the local rendering priority relative to its parent.
   * int **global_z_index** (default: ``0``) — the global layer index (to be deprecated).
   * std::string **name** — the custom string name of the object.
* **internal properties**
    * sf::Vector2f **real_size** — the absolute size of the object in pixels.
   * sf::Vector2f **real_pos** — the absolute position of the object in pixels relative to its parent.
   * sf::Vector2f **absolute_pos** — the absolute position of the object relative to the application window.
   * sf::Image **img** — raw pixel data of the background_image.
   * sf::Sprite **sprite** — the main sprite of the object drawn into the parent's render_tex during draw_obj().
   * sf::RenderTexture **render_tex** — the buffer canvas where the object and its children are drawn.
   * sf::Sprite **image_sprite** — the sprite component for the background_image.
   * sf::Font **font** — the SFML font asset object.
* **shadows**
    * float **shadow_size** — the blur radius of the shadow.
   * sf::Vector2f **shadow_offset** — the shadow offset along the X and Y axes.
   * sf::Color **shadow_color** — the color of the shadow.
* **layout**
    * layout **layout_type** (default: ``layout::none``) — specifies how the object's children are positioned and sized.
   * sf::Vector2f **flex_padding** — the gap between the object's children (ignored when layout_type is none).
* **texturing**
    * sf::Texture **background_image** — the texture asset of the loaded image.
   * std::string **image_path** — the disk path to the background_image file.
   * bool **repeated** — a flag enabling texture wrapping/repeating.
   * sf::Vector2f **image_offset** — the rendering offset of the image texture.
   * sf::Vector2f **image_scale** — the scale multiplier of the texture image.
* **constraints**
    * float **aspect_ratio** (default: ``0.0f``) — forces a fixed aspect ratio constraint on the object.
   * axis **dominant_axis** (default: ``axis::width``) — the axis prioritized when calculating size with an active aspect_ratio.
   * float **min_w** / float **min_h** (default: ``0.0f``) — absolute lower bounds for width and height in pixels.
   * axis **dominant_padding_axis** (default: ``axis::width``) — the axis from which padding pixel offsets are evaluated.
* **text**
    * std::string **text_content** (default: ``empty``) — the text string rendered into the object's render_texture.
   * sf::Color **text_color** — the fill color of the text.
   * sf::Color **text_border_color** — the outline color of the text.
   * float **text_border_size** — the thickness of the text outline.
   * sf::Text::Style **text_style** — the text font style formatting (Bold, Regular, etc.).
   * std::string **font_path** — the disk path to the font asset file.
   * float **text_size** (default: ``1.0f``) — the text font size evaluated relative to the dominant_axis.
   * text_align_x **align_x** / text_align_y **align_y** — the alignment positioning of the text inside the object.
   * bool **text_wrap** (default: ``false``) — a flag enabling automatic word wrapping.
* **hierarchy**
    * uiobject **parent** — a pointer to the parent UI container.
   * std::vector<uiobject*> **children** — an array of pointers to the object's child elements.

uiobject methods:

* static std::shared_ptr **create()** — creates new `uiobject` and pushes it to `ui_objects`
* void **add(uiobject child)** — adds specified element to `children` of an element
* void **sort()** — sorts child objects by their zindex
* void **set_image(std::string s, bool smooth = true)** — loads image to object

------------------------------
## usage examples

### example 1
le simpliest example
#### uithing.h

```cpp
#include "oxygenui-lib/oxygenui.h"

void init_ui() {
	auto thing = uiobject::create(); //creating our "thing" object
	thing->pos = { 0, 0, 0, 0 };
	thing->size = { 1, 0, 1, 0 }; //making sure that thing width is equal to 100% of parent width (and same for height)
	thing->roundness = 0.1; //making corners of the thing rounded by 10%
	thing->padding = 0.1; //10% padding from parent bounds

	auto white = std::make_shared<color>(); //creating first color point
	white->content = sf::Color(255, 255, 255);
	white->position = 0.0f;

	auto gray = std::make_shared<color>(); //creating second color point
	gray->content = sf::Color(70, 70, 70);
	gray->position = 1.0f; //making sure that gray color is in the end of the linear gradient

	thing->background_color.content.push_back(white); //appending color points to the background_color(s) array
	thing->background_color.content.push_back(gray);

	sort(); //sorting elements by their global z-index (not entirely necessary here)
}
```

## main.cpp

```cpp
#include <iostream>
#include <chrono>
#include "oxygenui-lib/oxygenui.h"
#include "uithing.h"

void update(sf::RenderWindow& win) {
    win.clear({ 1,0,0 });  //idk why but when you clear with 0,0,0 nothing is drawn at all
    draw(win);             //so use only 1,0,0
    win.display();
}

void start(sf::RenderWindow& win) {
    sf::Event ev;

    update(win); //idk why but for first frame you need to call draw() twice
    update(win);

    while (win.isOpen()) {
        //if you need to constantly update something like infinite background,
        //then use while (win.pollEvent(ev))

        if (win.waitEvent(ev)) { //wait for SFML events (not oxygenui ones)
            if (ev.type == sf::Event::Closed) {
                win.close();
            }
            if (ev.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0.f, 0.f, static_cast<float>(ev.size.width), static_cast<float>(ev.size.height));
                win.setView(sf::View(visibleArea));
                update(win);
            }
        }
    }

    ui_objects.clear(); //clear ui_object vector so opengl doesn't freak out on uninitialized contexts
}

int main() {
    sf::ContextSettings st;
    st.antialiasingLevel = 8;
    sf::RenderWindow win;

    win.create(sf::VideoMode(512, 512), "example1", sf::Style::Default, st); //init SFML window

    win.setVerticalSyncEnabled(true); //turn on vsync

    init_ui(); //add elements from uithing.h
    start(win);

    return 0;
}
```

#### result

<img src="scr2.png" width="50%">

### example 2
this example is too big for wiki.md, so I left it as a full blown vs project in `src` folder.
<br>[main.cpp](../src/main.cpp)
<br>[uithing.h](../src/uithing.h)