# oxygenui

### [ русский ](README_RU.md)
a simple and intuitive GUI library that brings the layout experience of CSS and Roblox Studio into C++ and SFML development.

<img src="readme/scr1.png" alt="Screenshot 1" width="50%">

_ __
## about
oxygenui is a lightweight wrapper between SFML and the developer, allowing you to create beautiful and modern GUIs (Graphical User Interfaces) in a matter of minutes, even without deep knowledge of C++.

### example
> a small piece of library-specific code
<table>
  <tr>
    <td width="40%" valign="middle">
      <img src="readme/scr2.png" alt="Screenshot 2" width="100%">
    </td>
    <td width="60%" valign="top">

<pre><code class="language-cpp">auto thing = uiobject::create();
thing->pos = { 0, 0, 0, 0 };
thing->size = { 1, 0, 1, 0 };
thing->roundness = 0.1f;
thing->padding = 0.1f;

auto white = std::make_shared<color>();
white->content = sf::Color(255, 255, 255);
white->position = 0.0f;

auto gray = std::make_shared<color>();
gray->content = sf::Color(70, 70, 70);
gray->position = 1.0f;

thing->background_color.content.push_back(white);
thing->background_color.content.push_back(gray);

finalize_ui_sort();</code></pre>
  </tr>
</table>

_ __

## key features
<br/> oxygenui combines the best of CSS and Roblox Studio UI:
<br>
 - **css flexbox**
 - **Roblox-like adaptive  positioning**
 - **Roblox and SFML like event system**
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
## how it works
for deep dive into the depths of the library, go to [Wiki](readme/wiki.md)
_ __
## how 2 install

### 1. install SFML (STRICTLY 2.6.1)
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
