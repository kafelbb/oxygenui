#include "oxygenui.h"
#include <cmath>
#include <algorithm>


std::vector<std::shared_ptr<uiobject>> ui_objects;

uiobject::uiobject()
    : size{ 0, 100, 0, 100 },
    pos{ 0, 0, 0, 0 },
    visibility(1.0f),
    roundness(0.0f),
    border_size(0.0f),
    rotation(0),
    padding(0.0f),
    id(0),
    repeated(false),
    shadow_size(0.0f),
    shadow_offset(0.0f, 0.0f),
    shadow_color(sf::Color::Transparent),
    flex_padding(0.f,0.f),
    parent(nullptr)
{
}

std::shared_ptr<uiobject> uiobject::create() {
    auto obj = std::make_shared<uiobject>();
    ui_objects.push_back(obj);
    return obj;
}

void uiobject::set_image(std::string s, bool smooth) {
    image_path = s;
    if (img.loadFromFile(s)) {
        img.flipVertically();
        background_image.loadFromImage(img);
        background_image.setSmooth(smooth);
        image_sprite.setTexture(background_image);
    }
    else {
        std::cerr << "can't load " << s << " as a background_image" << std::endl;
    }
}

void uiobject::add(uiobject* child) {
    if (!child) return;

    if (child->parent != nullptr) {
        auto& old_children = child->parent->children;
        old_children.erase(std::remove(old_children.begin(), old_children.end(), child), old_children.end());
    }

    child->parent = this;
    this->children.push_back(child);
}

void uiobject::sort() {
    if (this->children.empty()) return;
    for (const auto& o : ui_objects) {
        std::sort(o->children.begin(), o->children.end(), [](const auto& a, const auto& b) {
            return a->z_index < b->z_index;
            });
    }
}


bool updating = true;

void apply_aspect_ratio(uiobject* o, float& real_w, float& real_h) {
    if (o->aspect_ratio <= 0.0f) return;

    if (o->dominant_axis == axis::width) {
        real_h = real_w / o->aspect_ratio;
    }
    else if (o->dominant_axis == axis::height) {
        real_w = real_h * o->aspect_ratio;
    }
}

vec4 get_dimensions_and_pos(uiobject* o) {
    float parent_w = o->parent->real_size.x;
    float parent_h = o->parent->real_size.y;
    float parent_x = 0.0f;
    float parent_y = 0.0f;

    // Вычисляем размер паддинга на основе выбранной доминантной оси
    float padding_basis = (o->padding * ((o->dominant_padding_axis == axis::width) ? parent_w : parent_h));

    float w = (parent_w * o->size.x_scale + o->size.x_offset) - padding_basis;
    float h = (parent_h * o->size.y_scale + o->size.y_offset) - padding_basis;

    // Применяем ограничения min_w и min_h
    if (w < o->min_w) w = o->min_w;
    if (h < o->min_h) h = o->min_h;

    float x = (parent_x + parent_w * o->pos.x_scale + o->pos.x_offset) + padding_basis / 2.0f;
    float y = (parent_y + parent_h * o->pos.y_scale + o->pos.y_offset) + padding_basis / 2.0f;

    return { x, y, w, h };
}


//vibecoded
vec4 get_dimensions_and_pos_by_flex(uiobject* o) {
    if (!o || !o->parent) {
        return { 0.0f, 0.0f, 0.0f, 0.0f };
    }

    int children_count = o->parent->children.size();
    if (children_count == 0) return { 0.0f, 0.0f, 0.0f, 0.0f };

    auto& p_ch = o->parent->children;
    auto it = std::find(p_ch.begin(), p_ch.end(), o);
    if (it == p_ch.end()) return { 0.0f, 0.0f, 0.0f, 0.0f };

    int index = static_cast<int>(std::distance(p_ch.begin(), it));

    float w = o->parent->real_size.x;
    float h = o->parent->real_size.y;

    float fx = 0.0f; float fy = 0.0f; float fw = 0.0f; float fh = 0.0f;

    // Считаем паддинг родителя на основе его собственной настройки dominant_padding_axis
    float parent_padding_basis = (o->dominant_padding_axis == axis::width) ? w : h;
    float parent_padding_px = (o->padding * parent_padding_basis);

    float inner_w = w - (parent_padding_px * 2.0f);
    float inner_h = h - (parent_padding_px * 2.0f);

    float total_scale_x = 0.0f;
    float total_scale_y = 0.0f;

    for (int i = 0; i < children_count; ++i) {
        total_scale_x += std::max(0.01f, p_ch[i]->size.x_scale);
        total_scale_y += std::max(0.01f, p_ch[i]->size.y_scale);
    }

    if (o->parent->layout_type == layout::flex_x) {
        float gap_px = o->parent->flex_padding.x * w;
        float total_gaps_w = static_cast<float>(children_count - 1) * gap_px;
        float available_for_items = inner_w - total_gaps_w;

        float current_fx = parent_padding_px;
        float unit_w = available_for_items / total_scale_x;

        for (int i = 0; i < children_count; ++i) {
            float child_w = unit_w * p_ch[i]->size.x_scale;
            float child_h = inner_h * p_ch[i]->size.y_scale;

            if (p_ch[i]->aspect_ratio > 0.0f && p_ch[i]->dominant_axis == axis::height) {
                child_w = child_h * p_ch[i]->aspect_ratio;
            }

            // Применяем min_w и min_h ограничение
            if (child_w < p_ch[i]->min_w) child_w = p_ch[i]->min_w;
            if (child_h < p_ch[i]->min_h) child_h = p_ch[i]->min_h;

            if (p_ch[i] == o) {
                fw = child_w;
                fh = child_h;
                fx = current_fx;
                fy = parent_padding_px;
                break;
            }
            current_fx += child_w + gap_px;
        }
    }
    else if (o->parent->layout_type == layout::flex_y) {
        float gap_px = o->parent->flex_padding.y * w;
        float total_gaps_h = static_cast<float>(children_count - 1) * gap_px;
        float available_for_items = inner_h - total_gaps_h;

        // --- ИСПРАВЛЕНИЕ ВЫЛЕЗАНИЯ КОНТЕНТА ВО FLEX_Y ---
        // Считаем, сколько высоты займут элементы, если распределять их «наивно»
        float current_fy = parent_padding_px;
        float unit_h = available_for_items / total_scale_y;

        // Первый проход: определяем элементы, которые упираются в min_h,
        // и корректируем доступное пространство под оставшиеся элементы.
        float fixed_height_allocated = 0.0f;
        float flexible_scale_y = total_scale_y;

        for (int i = 0; i < children_count; ++i) {
            float naive_h = unit_h * p_ch[i]->size.y_scale;
            if (naive_h < p_ch[i]->min_h) {
                fixed_height_allocated += p_ch[i]->min_h;
                flexible_scale_y -= p_ch[i]->size.y_scale;
            }
        }

        // Пересчитываем единичную высоту для адаптивных элементов, которым хватает места
        if (flexible_scale_y > 0.0f) {
            unit_h = (available_for_items - fixed_height_allocated) / flexible_scale_y;
        }

        // Второй проход: финальная сборка координат цепочки элементов
        for (int i = 0; i < children_count; ++i) {
            float child_w = inner_w * p_ch[i]->size.x_scale;
            float child_h = 0.0f;

            // Если элемент уперся в минимум — отдаем жесткий минимум, иначе — адаптивную долю
            float naive_h = (available_for_items / total_scale_y) * p_ch[i]->size.y_scale;
            if (naive_h < p_ch[i]->min_h) {
                child_h = p_ch[i]->min_h;
            }
            else {
                child_h = unit_h * p_ch[i]->size.y_scale;
            }

            if (p_ch[i]->aspect_ratio > 0.0f && p_ch[i]->dominant_axis == axis::width) {
                child_h = child_w / p_ch[i]->aspect_ratio;
            }

            if (child_w < p_ch[i]->min_w) child_w = p_ch[i]->min_w;
            if (child_h < p_ch[i]->min_h) child_h = p_ch[i]->min_h;

            if (p_ch[i] == o) {
                fw = child_w;
                fh = child_h;
                fx = parent_padding_px;
                fy = current_fy;
                break;
            }
            current_fy += child_h + gap_px;
        }
    }
    else if (o->parent->layout_type == layout::none) {
        return get_dimensions_and_pos(o);
    }

    apply_aspect_ratio(o, fw, fh);

    // Финальная проверка лимитов на случай модификации через aspect_ratio
    if (fw < o->min_w) fw = o->min_w;
    if (fh < o->min_h) fh = o->min_h;

    return { fx, fy, fw, fh };
}


sf::Texture& get_white_placeholder_texture() {
    static sf::Texture texture;
    static bool initialized = false;
    if (!initialized) {
        sf::Image image;
        image.create(1, 1, sf::Color::White);
        texture.loadFromImage(image);
        initialized = true;
    }
    return texture;
}

sf::Color get_gradient_color(const color_composition& comp, float targetPos, float alphaMultiplier) { //vibecoded
    if (comp.content.empty()) {
        return sf::Color(255, 255, 255, static_cast<sf::Uint8>(255 * alphaMultiplier));
    }

    if (comp.content.size() == 1) {
        sf::Color c = comp.content[0]->content;
        c.a = static_cast<sf::Uint8>(c.a * alphaMultiplier);
        return c;
    }

    auto sortedContent = comp.content;
    std::sort(sortedContent.begin(), sortedContent.end(), [](const auto& a, const auto& b) {
        return a->position < b->position;
        });

    targetPos = std::max(0.0f, std::min(1.0f, targetPos));

    if (targetPos <= sortedContent.front()->position) {
        sf::Color c = sortedContent.front()->content;
        c.a = static_cast<sf::Uint8>(c.a * alphaMultiplier);
        return c;
    }
    if (targetPos >= sortedContent.back()->position) {
        sf::Color c = sortedContent.back()->content;
        c.a = static_cast<sf::Uint8>(c.a * alphaMultiplier);
        return c;
    }

    for (size_t i = 0; i < sortedContent.size() - 1; ++i) {
        auto left = sortedContent[i];
        auto right = sortedContent[i + 1];

        if (targetPos >= left->position && targetPos <= right->position) {
            float t = (targetPos - left->position) / (right->position - left->position);

            sf::Uint8 r = static_cast<sf::Uint8>(left->content.r + t * (right->content.r - left->content.r));
            sf::Uint8 g = static_cast<sf::Uint8>(left->content.g + t * (right->content.g - left->content.g));
            sf::Uint8 b = static_cast<sf::Uint8>(left->content.b + t * (right->content.b - left->content.b));
            sf::Uint8 a = static_cast<sf::Uint8>(left->content.a + t * (right->content.a - left->content.a));

            return sf::Color(r, g, b, static_cast<sf::Uint8>(a * alphaMultiplier));
        }
    }

    return sf::Color::White;
}

//vibecoded
void draw_rounded_border(sf::RenderTarget& win, sf::Vector2f size, sf::Vector2f position, sf::Vector2f origin, float radius, float borderSize, const color_composition& borderComp, float alphaMultiplier) {
    if (borderSize <= 0.0f || borderComp.content.empty()) return;

    float maxRadius = std::min(size.x, size.y) / 2.0f;
    if (radius > maxRadius) radius = maxRadius;

    if (radius <= 0.0f) {
        sf::RectangleShape borderRect(size);
        borderRect.setOrigin(origin);
        borderRect.setPosition(position);
        borderRect.setFillColor(sf::Color::Transparent);
        borderRect.setOutlineThickness(borderSize);

        borderRect.setOutlineColor(get_gradient_color(borderComp, 0.0f, alphaMultiplier));
        win.draw(borderRect);
        return;
    }

    const unsigned int pointsPerCorner = 8;

    unsigned int totalPoints = 4 * (pointsPerCorner + 1);
    sf::VertexArray va(sf::TriangleStrip, totalPoints * 2 + 2);

    sf::Vector2f centers[4] = {
        { size.x - radius, radius },
        { size.x - radius, size.y - radius },
        { radius, size.y - radius },
        { radius, radius }
    };

    float startAngles[4] = { -pi / 2.0f, 0.0f, pi / 2.0f, pi };
    unsigned int index = 0;

    for (int q = 0; q < 4; ++q) {
        float startAngle = startAngles[q];

        for (unsigned int j = 0; j <= pointsPerCorner; ++j) {
            float angle = startAngle + (static_cast<float>(j) / static_cast<float>(pointsPerCorner)) * (pi / 2.0f);

            sf::Vector2f dir(std::cos(angle), std::sin(angle));

            sf::Vector2f innerLocal = centers[q] + dir * radius;
            sf::Vector2f innerGlobal = innerLocal - origin + position;

            sf::Vector2f outerLocal = centers[q] + dir * (radius + borderSize);
            sf::Vector2f outerGlobal = outerLocal - origin + position;

            float relativeY = innerLocal.y / size.y;
            sf::Color vertexColor = get_gradient_color(borderComp, relativeY, alphaMultiplier);

            va[index++] = sf::Vertex(innerGlobal, vertexColor);
            va[index++] = sf::Vertex(outerGlobal, vertexColor);
        }
    }

    va[index++] = va[0];
    va[index++] = va[1];

    win.draw(va);
}

//vibecoded
void draw_rounded_shadow(sf::RenderTarget& win, sf::Vector2f size, sf::Vector2f position, sf::Vector2f origin, float radius, float shadowSizeInPixels, sf::Vector2f offset, sf::Color shadowColor, float alphaMultiplier) {
    if (shadowSizeInPixels <= 0.0f || shadowColor.a == 0) return;

    sf::Vector2f shadowPos = position + offset;

    float maxRadius = std::min(size.x, size.y) / 2.0f;
    if (radius > maxRadius) radius = maxRadius;

    sf::Color innerColor = shadowColor;
    innerColor.a = static_cast<sf::Uint8>(innerColor.a * alphaMultiplier);

    sf::Color outerColor = innerColor;
    outerColor.a = 0;

    const unsigned int pointsPerCorner = 8;
    unsigned int totalPoints = 4 * (pointsPerCorner + 1);

    sf::VertexArray vaOuter(sf::TriangleStrip, totalPoints * 2 + 2);

    sf::Vector2f centers[4] = {
        { size.x - radius, radius },
        { size.x - radius, size.y - radius },
        { radius, size.y - radius },
        { radius, radius }
    };

    float startAngles[4] = { -pi / 2.0f, 0.0f, pi / 2.0f, pi };
    unsigned int index = 0;

    for (int q = 0; q < 4; ++q) {
        float startAngle = startAngles[q];

        for (unsigned int j = 0; j <= pointsPerCorner; ++j) {
            float angle = startAngle + (static_cast<float>(j) / static_cast<float>(pointsPerCorner)) * (pi / 2.0f);
            sf::Vector2f dir(std::cos(angle), std::sin(angle));

            // Внутренняя точка тени (под телом объекта)
            sf::Vector2f innerLocal = centers[q] + dir * radius;
            sf::Vector2f innerGlobal = innerLocal - origin + shadowPos;

            // Внешняя точка тени (размытие уходит вовне на shadowSizeInPixels)
            sf::Vector2f outerLocal = centers[q] + dir * (radius + shadowSizeInPixels);
            sf::Vector2f outerGlobal = outerLocal - origin + shadowPos;

            vaOuter[index++] = sf::Vertex(innerGlobal, innerColor);
            vaOuter[index++] = sf::Vertex(outerGlobal, outerColor);
        }
    }
    vaOuter[index++] = vaOuter[0];
    vaOuter[index++] = vaOuter[1];

    sf::VertexArray vaInner(sf::TriangleFan, totalPoints + 2);
    unsigned int innerIndex = 0;

    sf::Vector2f centerPoint(size.x / 2.0f - origin.x + shadowPos.x, size.y / 2.0f - origin.y + shadowPos.y);
    vaInner[innerIndex++] = sf::Vertex(centerPoint, innerColor);

    for (int q = 0; q < 4; ++q) {
        float startAngle = startAngles[q];
        for (unsigned int j = 0; j <= pointsPerCorner; ++j) {
            float angle = startAngle + (static_cast<float>(j) / static_cast<float>(pointsPerCorner)) * (pi / 2.0f);
            sf::Vector2f localPoint(centers[q].x + std::cos(angle) * radius, centers[q].y + std::sin(angle) * radius);
            sf::Vector2f globalPoint = localPoint - origin + shadowPos;
            vaInner[innerIndex++] = sf::Vertex(globalPoint, innerColor);
        }
    }
    vaInner[innerIndex] = vaInner[1];

    win.draw(vaInner);
    win.draw(vaOuter);
}

sf::Texture& get_global_screen_texture() {
    static sf::Texture texture;
    return texture;
}

//vibecoded
sf::Shader* get_ui_shader() {
    static sf::Shader ui_shader;
    static bool ui_shader_loaded = false;

    if (!ui_shader_loaded) {
        const std::string fragmentShader = R"(
            uniform vec2 size;
            uniform float alpha;
            uniform sampler2D texture; // Сюда автоматически прилетит размытый буфер экрана!
            uniform float normalized_roundness;
            uniform float global_scale;

            uniform sampler2D gradient_texture; // Наша RenderTexture с градиентом и обводкой
            uniform bool has_blur;

            // Бесконечный фон
            uniform bool is_repeated;
            uniform vec2 image_offset;
            uniform vec2 image_scale;
            uniform sampler2D img_texture; 
            uniform bool has_image;

            void main() {
                // ИСПРАВЛЕНО НАВСЕГДА: Явно указываем индекс массива [0] перед вызовом .x и .y,
                // чтобы у строгого компилятора больше физически не было шансов выдать ошибку!
                vec2 uv = vec2(gl_TexCoord[0].x, gl_TexCoord[0].y); 
                
                vec2 p = uv * size;
                vec2 half_size = size * 0.5;
                
                // Скругление углов
                float min_side = min(size.x, size.y);
                float radius = global_scale * 0.5 * normalized_roundness;
                radius = min(radius, min_side * 0.5);

                vec2 d = abs(p - half_size) - half_size + vec2(radius);
                float dist = length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - radius;

                float edge_alpha = smoothstep(0.5, -0.5, dist);
                if (edge_alpha <= 0.0) {
                    discard;
                }
                
                // Считываем наш готовый геометрический градиент фона/обводки
                vec4 base_color = texture2D(gradient_texture, uv);

                // Если включен блюр — плавно подмешиваем размытые пиксели 
                // из текстуры подложки (texture) под прозрачность нашего градиента (base_color)
                if (has_blur == true) {
                    vec4 blur_color = texture2D(texture, uv);
                    base_color = mix(blur_color, base_color, base_color.a);
                }

                vec4 final_pixel = base_color;

                // ... внутри fragmentShader ищите ветку has_image == true:
                // ... внутри fragmentShader ищите ветку has_image == true:
                if (has_image == true) {
                    if (is_repeated == true) {
                        vec2 scale = max(vec2(0.001), image_scale);
                        vec2 scale_factor = size / (global_scale * scale);
                        vec2 repeated_uv = fract((uv * scale_factor) + (image_offset / (global_scale * scale)));
        
                        vec4 img_color = texture2D(img_texture, repeated_uv);
                        final_pixel = vec4(base_color.rgb * img_color.rgb, base_color.a * img_color.a);
                    } else {
                        // Считываем пиксель статической картинки
                        vec4 img_color = texture2D(img_texture, uv);
        
                        // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Принудительно умножаем альфу картинки на юниформ alpha!
                        // Благодаря этому свойство o->visibility начнет напрямую влиять на изображение.
                        float current_img_alpha = img_color.a * alpha;
        
                        // Пересчитываем Alpha Blending с учетом динамической прозрачности картинки
                        float out_a = current_img_alpha + base_color.a * (1.0 - current_img_alpha);
                        vec3 out_rgb = (img_color.rgb * current_img_alpha + base_color.rgb * base_color.a * (1.0 - current_img_alpha)) / max(0.001, out_a);
        
                        final_pixel = vec4(out_rgb, out_a);
                    }
                }

                // Финальный вывод (здесь оставляем как есть)
                gl_FragColor = vec4(final_pixel.rgb, final_pixel.a * edge_alpha);
            }
        )";

        if (ui_shader.loadFromMemory(fragmentShader, sf::Shader::Type::Fragment)) {
            ui_shader_loaded = true;
        }
    }
    return ui_shader_loaded ? &ui_shader : nullptr;
}

std::string wrap_text(const std::string& str, float max_width, sf::Text& text_obj) {
    if (str.empty() || max_width <= 0.f) return str;

    std::string result = "";
    std::string current_line = "";
    std::string word = "";

    auto flush_word = [&]() {
        if (word.empty()) return;

        // Проверяем, как будет выглядеть строка, если добавить слово
        std::string test_line = current_line.empty() ? word : current_line + " " + word;
        text_obj.setString(test_line);

        // Если строка превысила максимальную ширину (с учетом отступов)
        if (text_obj.getLocalBounds().width > max_width) {
            if (!current_line.empty()) {
                result += current_line + "\n";
                current_line = word; // Слово уходит на новую строку
            }
            else {
                // Если даже ОДНО слово шире, чем max_width, принудительно оставляем его
                result += word + "\n";
                current_line = "";
            }
        }
        else {
            current_line = test_line;
        }
        word = "";
        };

    for (char c : str) {
        if (c == ' ') {
            flush_word();
        }
        else if (c == '\n') {
            flush_word();
            result += current_line + "\n";
            current_line = "";
        }
        else {
            word += c;
        }
    }
    flush_word();

    if (!current_line.empty()) {
        result += current_line;
    }

    // Убираем возможный лишний '\n' на конце
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }

    return result;
}


//semi-vibecoded
void draw_obj(uiobject* o, sf::RenderTarget& win) {
    if (o->visibility <= 0.0f) return;

    sf::Vector2u win_dim = win.getSize();
    float win_w = static_cast<float>(win_dim.x);
    float win_h = static_cast<float>(win_dim.y);

    // Флаг: нужно ли этому объекту пересчитывать геометрию и перерисовывать буфер?
    bool need_redraw = updating;

    // Если текстура еще не создана — принудительно обновляем
    if (o->render_tex.getSize() != sf::Vector2u(o->real_size.x, o->real_size.y)) {
        need_redraw = true;
    }

    float a = 1.f;
    if (o->parent != nullptr) a = o->parent->visibility;
    float alpha_mul = o->visibility * a;

    // --- ЭТАП 1: РАСЧЕТ ГЕОМЕТРИИ И ОБНОВЛЕНИЕ ТЕКСТУРЫ (Только при изменениях!) ---
    if (need_redraw) {
        float real_w, real_h, real_x, real_y;
        if (o->parent == nullptr) {
            float padding_basis = (o->padding * ((o->dominant_padding_axis == axis::width) ? win_w : win_h));
            real_w = win_w * o->size.x_scale + o->size.x_offset - padding_basis;
            real_h = win_h * o->size.y_scale + o->size.y_offset - padding_basis;

            if (real_w < o->min_w) real_w = o->min_w;
            if (real_h < o->min_h) real_h = o->min_h;

            real_x = win_w * o->pos.x_scale + o->pos.x_offset + padding_basis / 2.0f;
            real_y = win_h * o->pos.y_scale + o->pos.y_offset + padding_basis / 2.0f;
            apply_aspect_ratio(o, real_w, real_h);
        }
        else {
            // Тяжелый расчет флексов происходит ТОЛЬКО ТУТ
            vec4 ass = get_dimensions_and_pos_by_flex(o);
            real_x = ass.x; real_y = ass.y; real_w = ass.z; real_h = ass.w;
            apply_aspect_ratio(o, real_w, real_h);
        }

        if (real_w <= 0.f || real_h <= 0.f) return;

        // Округляем до целых пикселей
        unsigned int tex_w = static_cast<unsigned int>(std::round(real_w));
        unsigned int tex_h = static_cast<unsigned int>(std::round(real_h));

        o->real_size = { static_cast<float>(tex_w), static_cast<float>(tex_h) };
        o->real_pos = { std::round(real_x), std::round(real_y) };

        if (o->parent == nullptr) o->absolute_pos = o->real_pos;
        else o->absolute_pos = o->parent->absolute_pos + o->real_pos;

        // Проверяем/пересоздаем RenderTexture
        if (o->render_tex.getSize() != sf::Vector2u(tex_w, tex_h)) {
            o->render_tex.create(tex_w, tex_h);
            o->render_tex.setSmooth(true);
        }

        // Очищаем и заливаем фон с жестким сбросом блендинга (убирает потемнение)
        sf::RenderStates bg_states;
        bg_states.blendMode = sf::BlendNone;
        o->render_tex.clear(sf::Color::Transparent);

        if (o->background_color.content.size() >= 2) {
            sf::VertexArray gradient(sf::PrimitiveType::TriangleStrip, 4);
            gradient[0].position = { 0, 0 }; gradient[1].position = { o->real_size.x, 0 };
            gradient[2].position = { 0, o->real_size.y }; gradient[3].position = { o->real_size.x, o->real_size.y };
            gradient[0].color = o->background_color.content[0]->content; gradient[1].color = o->background_color.content[0]->content;
            gradient[2].color = o->background_color.content[1]->content; gradient[3].color = o->background_color.content[1]->content;
            o->render_tex.draw(gradient, bg_states);
        }
        else if (!o->background_color.content.empty()) {
            sf::RectangleShape r(o->real_size);
            r.setFillColor(o->background_color.content[0]->content);
            o->render_tex.draw(r, bg_states);
        }
        else {
            sf::RectangleShape r(o->real_size);
            r.setFillColor(sf::Color::White);
            o->render_tex.draw(r, bg_states);
        }

        // Отрисовка бордеров
        float border_px = o->border_size;
        if (border_px >= 1.0f) border_px = (o->border_size / 512.f) * win_w;
        else border_px = o->border_size * win_w;

        if (border_px > 0.0f && !o->border_color.content.empty()) {
            float r_px = win_w * 0.5f * o->roundness;
            sf::Vector2f inner_border_size = { o->real_size.x - border_px * 2.0f + 0.5f, o->real_size.y - border_px * 2.0f + 0.5f };
            sf::Vector2f inner_border_pos = { border_px - 0.25f, border_px - 0.25f };
            float inner_radius = std::max(0.0f, r_px - border_px);
            draw_rounded_border(o->render_tex, inner_border_size, inner_border_pos, { 0, 0 }, inner_radius, border_px, o->border_color, 1.0f);
        }

        // Отрисовка текста (внутри RenderTexture)
        if (!o->text_content.empty()) {
            o->font.loadFromFile(o->font_path);
            sf::Text t;
            sf::Color dynamic_text_color = o->text_color;
            // Умножаем родную альфу текста на текущую видимость объекта и его родителя
            dynamic_text_color.a = static_cast<sf::Uint8>(dynamic_text_color.a * alpha_mul);
            t.setFillColor(dynamic_text_color);

            sf::Color dynamic_border_color = o->text_border_color;
            dynamic_border_color.a = static_cast<sf::Uint8>(dynamic_border_color.a * alpha_mul);
            t.setOutlineColor(dynamic_border_color);
            t.setOutlineThickness(o->text_border_size * win_w);
            t.setFont(o->font);
            t.setStyle(o->text_style);

            float dominantBounds = (o->dominant_axis == axis::width) ? o->real_size.x : o->real_size.y;
            float targetPixelSize = dominantBounds * o->text_size;

            if (targetPixelSize > 0.f) {
                unsigned int calculatedSize = static_cast<unsigned int>(std::round(targetPixelSize));
                t.setCharacterSize(calculatedSize);

                if (o->dominant_axis == axis::height) {
                    if (sf::Font* font = const_cast<sf::Font*>(t.getFont())) {
                        float lineSpacing = font->getLineSpacing(calculatedSize);
                        float scaleCorrection = targetPixelSize / lineSpacing;
                        unsigned int correctedSize = static_cast<unsigned int>(std::round(calculatedSize * scaleCorrection));
                        t.setCharacterSize(std::max(1U, correctedSize));
                    }
                }
            }

            if (o->text_wrap) {
                std::string wrapped = wrap_text(o->text_content, o->real_size.x, t);
                t.setString(wrapped);
            }
            else {
                t.setString(o->text_content);
            }

            sf::FloatRect textBounds = t.getLocalBounds();
            t.setOrigin({ textBounds.left, textBounds.top });

            float posX = 0.f;
            if (o->align_x == text_align_x::center) posX = (o->real_size.x - textBounds.width) / 2.0f;
            else if (o->align_x == text_align_x::right) posX = o->real_size.x - textBounds.width;

            float posY = 0.f;
            if (o->align_y == text_align_y::top) posY = o->real_size.y - textBounds.height;
            else if (o->align_y == text_align_y::center) posY = (o->real_size.y - textBounds.height) / 2.0f;

            t.setPosition({ posX, posY });
            o->render_tex.draw(t);
        }

        // Рекурсивно заставляем детей обновить свои RenderTexture ИМЕННО внутри буфера
        for (const auto& o2 : o->children) {
            draw_obj(o2, o->render_tex);
        }

        o->render_tex.display();
        o->sprite.setTexture(o->render_tex.getTexture(), true);
    }

    // --- ЭТАП 2: БЫСТРЫЙ ВЫВОД ГОТОВОГО СПРАЙТА НА ЭКРАН (Выполняется каждый кадр, O(1)) ---
    float sprite_center_x = o->real_pos.x + o->real_size.x / 2.0f;
    float sprite_center_y = o->real_pos.y + o->real_size.y / 2.0f;

    // Отрисовка теней самого объекта на целевую win
    if (o->shadow_color.a > 0 && o->shadow_size > 0.0f) {
        float r_px = win_w * 0.5f * o->roundness;
        float shadow_px = o->shadow_size * win_w;
        float offset_y = o->shadow_offset.y * win_h;
        if (o->parent != nullptr) offset_y = -offset_y;

        draw_rounded_shadow(win, o->real_size, o->real_pos, { 0, 0 }, r_px, shadow_px, { o->shadow_offset.x * win_w, offset_y }, o->shadow_color, alpha_mul);
    }

    o->sprite.setPosition({ sprite_center_x, sprite_center_y });
    o->sprite.setOrigin({ o->sprite.getLocalBounds().width / 2, o->sprite.getLocalBounds().height / 2 });
    o->sprite.setRotation(o->rotation);
    o->sprite.setScale(1.f, 1.f);

    sf::Shader* ui_shader = get_ui_shader();
    if (ui_shader != nullptr) {
        ui_shader->setUniform("size", o->real_size);
        ui_shader->setUniform("normalized_roundness", o->roundness);
        ui_shader->setUniform("alpha", alpha_mul);
        ui_shader->setUniform("global_scale", win_w);

        if (o->background_image.getSize().x > 0 && o->background_image.getSize().y > 0) {
            ui_shader->setUniform("has_image", true);
            ui_shader->setUniform("is_repeated", o->repeated);
            ui_shader->setUniform("image_offset", o->image_offset);
            ui_shader->setUniform("image_scale", o->image_scale);
            ui_shader->setUniform("img_texture", o->background_image);
        }
        else {
            ui_shader->setUniform("has_image", false);
        }

        ui_shader->setUniform("gradient_texture", o->render_tex.getTexture());

        sf::RenderStates states;
        states.shader = ui_shader;
        states.blendMode = sf::BlendAlpha;
        win.draw(o->sprite, states);
    }
    else {
        sf::RenderStates states;
        states.blendMode = sf::BlendAlpha;
        win.draw(o->sprite, states);
    }
}


void draw(sf::RenderTarget& win) {
    for (const auto& o : ui_objects) {
        if (o->parent == nullptr) {
            draw_obj(o.get(), win);
        }
    }
}

void sort() {
    std::sort(ui_objects.begin(), ui_objects.end(), [](const auto& a, const auto& b) {
        return a->global_z_index < b->global_z_index;
        });
}

event check_events(sf::Event& ev, sf::RenderWindow& win) {
    // 1. Получаем координаты мыши в реальном времени
    sf::Vector2f point = win.mapPixelToCoords(sf::Mouse::getPosition(win));

    // Обходим плоский список UI-объектов с конца (от верхних к нижним по global_z_index)
    for (auto it = ui_objects.rbegin(); it != ui_objects.rend(); ++it) {
        uiobject* o = it->get();

        // Пропускаем невидимые элементы
        if (o->visibility <= 0.0f) continue;

        // 2. Проверяем, находится ли курсор над текущим UI-объектом
        if (point.x >= o->absolute_pos.x && point.x <= o->absolute_pos.x + o->real_size.x &&
            point.y >= o->absolute_pos.y && point.y <= o->absolute_pos.y + o->real_size.y) {

            event e;
            e.object = o;
            e.mouse_pos = point;
            e.type = event_t::hover; // <-- По умолчанию выставляем HOVER
            e.button = mousebutton::none;

            // 3. Если в этот момент зажата какая-либо кнопка — переопределяем на mousedown
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                e.type = event_t::mousedown;
                e.button = mousebutton::left;
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
                e.type = event_t::mousedown;
                e.button = mousebutton::right;
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle)) {
                e.type = event_t::mousedown;
                e.button = mousebutton::middle;
            }
            // 4. Обрабатываем скролл, если он прилетел в этом кадре
            else if (ev.type == sf::Event::MouseWheelScrolled && ev.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                e.type = (ev.mouseWheelScroll.delta > 0.f) ? event_t::scrollup : event_t::scrolldown;
                e.button = mousebutton::none;

                // Сбрасываем тип события SFML, чтобы другие элементы его не перехватили
                ev.type = sf::Event::Count;
            }

            return e; // Возвращаем UI-событие для самого верхнего элемента под мышью
        }
    }

    // Если мышь ни над чем не находится
    event empty_e;
    empty_e.type = event_t::none;
    empty_e.button = mousebutton::none;
    return empty_e;
}
