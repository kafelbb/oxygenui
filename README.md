# oxygenui

### [ русский ](README_RU.md)
a crossplatform, simple and intuitive GUI library that brings UI development experience of CSS and Roblox Studio into C++.

<img src="readme/scr1.png" alt="Screenshot 1" width="50%">

_ __
## about
**oxygenui** is a simple and intuitive SFML wrapper, that allows for pretty fast and straightforward 
UI development. 
<br><br>It does not use any standard windows-forms (like in GTK/Qt).
Instead it relies on it's own Roblox and CSS-like ideology.
<br><br>You only have one UI class that does all the work you need - **uiobject**.
<br><br>**uiobject** has both CSS-like properties and Roblox-like positioning and scaling system. 
It is a **Text Label**, an **ImageButton**, **Text Button** and a **Frame** combined.

### example
> a small piece of example code
<table>
  <tr>
    <td width="40%" valign="middle">
      <img src="readme/scr2.png" alt="Screenshot 2" width="100%">
    </td>
    <td width="60%" valign="top">

<pre><code class="language-cpp">auto thing = oxyui::uiobject::create(sys);
thing->pos = { 0, 0, 0, 0 };
thing->size = { 1, 0, 1, 0 };
thing->roundness = 0.1f;
thing->padding = 0.1f;

auto white = std::make_shared<oxyui::color>();
white->content = sf::Color(255, 255, 255);
white->position = 0.0f;

auto grey = std::make_shared<oxyui::color>();
grey->content = sf::Color(70, 70, 70);
grey->position = 1.0f;

thing->background_color.content.push_back(white);
thing->background_color.content.push_back(grey);

sys.sort();</code></pre>
  </tr>
</table>

<br>see? This is how simple **oxygenui** system is.
_ __

## key features
<br/> oxygenui combines the best of CSS and Roblox Studio UI:
<br>
 - **CSS-like flexbox system**
 - **Roblox-like adaptive positioning**
 - **Roblox and SFML-like event system**
 - **huge customization** 
 - **object-oriented architecture**

oxygenui also has some other convenient features, as:
<br>
- **relative unit paradigm** (most properties of the UI elements rely on **relative** calculations and units, instead of absolute ones)
- **infinite scrolling background functionality**
- **GPU acceleration**
- **full crossplatform**
- **total creative freedom** (no strict standard window-forms like in GTK / Qt. full freedom on how you make your layout)

 _ __
## how it works and how 2 use it
for deep dive into the depths of the library, go to [library Wiki](readme/wiki.md)
_ __
## how 2 install

### 1. install SFML (STRICTLY 2.6.1!!11!!)
since **oxygenui** is just a silly little SFML wrapper, it needs the SFML to work (captain obvious right here)

* **michaelsoft binbows 96 (vcpkg)**
  ```json
    {
        "name": "test",
        "version-string": "0.1.0",
        "dependencies": [
            "sfml"
        ],
        "overrides": [
            {
                "name": "sfml",
                "version": "2.6.1"
            }
        ],
        "builtin-baseline": "d015e31e90838a4c9dfa3eed45979bc70d9357fc"
    }
  ```

* **debian based**
  ```bash
  sudo apt-get update
  sudo apt-get install libsfml-dev=2.6.1*
  ```

* **arch based**
<br>no arch is not based

* **fedora \ rhel**
  ```bash
  sudo dnf install SFML-devel-2.6.1
  ```

### 2. install **oxygenui**
1. download the latest library release from the [Releases](github.com) (`oxygenui-lib.7z` / `.zip`).
2. extract the archive into your project directory (e.g., somewhere under `src/libs/oxygenui-lib`).

that's all
_ __
## license

this project is licensed under the permissive **MIT License** — you are free to use, modify, distribute, and sell this software in any project, including commercial games and applications.
The only requirement is to preserve the original copyright notice.
See the [LICENSE](../LICENSE) file for more details.

### third-party licenses
**oxygenui** utilizes the following third-party libraries:
* **SFML** — distributed under the [zlib/libpng license](https://github.com/SFML/SFML).

all third-party dependencies are permissive and **fully compatible** with the MIT license.
