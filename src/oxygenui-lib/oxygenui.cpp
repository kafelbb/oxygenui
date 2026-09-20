#include "oxygenui.h"
#include <cmath>
#include <algorithm>


namespace oxyui {

    float ease_linear(float t) { return t; }

    float ease_quad(float t, ease_type type) {
        if (type == ease_type::in) return t * t;
        if (type == ease_type::out) return t * (2.0f - t);
        return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
    }

    float ease_cubic(float t, ease_type type) {
        if (type == ease_type::in) return t * t * t;
        if (type == ease_type::out) { t--; return t * t * t + 1.0f; }
        return t < 0.5f ? 4.0f * t * t * t : (t - 1.0f) * (2.0f * t - 2.0f) * (2.0f * t - 2.0f) + 1.0f;
    }

    float ease_quart(float t, ease_type type) {
        if (type == ease_type::in) return t * t * t * t;
        if (type == ease_type::out) { t--; return 1.0f - t * t * t * t; }
        return t < 0.5f ? 8.0f * t * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 4.0f) / 2.0f;
    }

    float tween::get_eased_value(float alpha) {
        alpha = std::max(0.0f, std::min(1.0f, alpha));
        switch (description.easing_style) {
        case ease_style::quad:  return ease_quad(alpha, description.easing_type);
        case ease_style::cubic: return ease_cubic(alpha, description.easing_type);
        case ease_style::quart: return ease_quart(alpha, description.easing_type);
        case ease_style::step:  return alpha >= 1.0f ? 1.0f : 0.0f;
        case ease_style::linear:
        default:                return ease_linear(alpha);
        }
    }

    // --- СИСТЕМА ДИНАМИЧЕСКИХ СВОЙСТВ ---
    float tween::get_current_property_value(const std::string& prop) {
        if (!target) return 0.0f;
        if (prop == "pos.x_scale")  return target->pos.x_scale;
        if (prop == "pos.x_offset") return target->pos.x_offset;
        if (prop == "pos.y_scale")  return target->pos.y_scale;
        if (prop == "pos.y_offset") return target->pos.y_offset;
        if (prop == "size.x_scale") return target->size.x_scale;
        if (prop == "size.x_offset") return target->size.x_offset;
        if (prop == "size.y_scale") return target->size.y_scale;
        if (prop == "size.y_offset") return target->size.y_offset;
        if (prop == "visibility")   return target->visibility;
        if (prop == "roundness")    return target->roundness;
        if (prop == "border_size")  return target->border_size;
        if (prop == "rotation")     return static_cast<float>(target->rotation);
        if (prop == "shadow_size")  return target->shadow_size;
        return 0.0f;
    }

    void tween::apply_property(const std::string& prop, float value) {
        if (!target) return;
        if (prop == "pos.x_scale")  target->pos.x_scale = value;
        else if (prop == "pos.x_offset") target->pos.x_offset = value;
        else if (prop == "pos.y_scale")  target->pos.y_scale = value;
        else if (prop == "pos.y_offset") target->pos.y_offset = value;
        else if (prop == "size.x_scale") target->size.x_scale = value;
        else if (prop == "size.x_offset") target->size.x_offset = value;
        else if (prop == "size.y_scale")  target->size.y_scale = value;
        else if (prop == "size.y_offset") target->size.y_offset = value;
        else if (prop == "visibility")   target->visibility = value;
        else if (prop == "roundness")    target->roundness = value;
        else if (prop == "border_size")  target->border_size = value;
        else if (prop == "rotation")     target->rotation = static_cast<int>(std::round(value));
        else if (prop == "shadow_size")  target->shadow_size = value;
    }

    // --- РЕАЛИЗАЦИЯ МЕТОДОВ КЛАССА TWEEN ---
    std::shared_ptr<tween> tween::create(uisystem& sys, uiobject* target, tween_info info, std::map<std::string, float> target_properties) {
        auto t = std::make_shared<tween>();
        t->target = target;
        t->description = info;
        t->goal_properties = target_properties;

        sys.tweens.push_back(t); // Регистрируем в глобальной системе
        return t;
    }

    void tween::play() {
        if (!target || description.target_time <= 0.0f) return;

        // Захватываем начальные состояния ТОЛЬКО для тех свойств, которые изменяем
        initial_properties.clear();
        for (const auto& pair : goal_properties) {
            initial_properties[pair.first] = get_current_property_value(pair.first);
        }

        current_time = 0.0f;
        playing = true;
        finished = false;
    }

    void tween::pause() {
        playing = !playing; // Переключатель пауза/продолжить
    }

    void tween::stop() {
        playing = false;
        current_time = 0.0f;
    }

    void tween::update(float dt) {
        if (!playing || finished) return;

        current_time += dt;
        float alpha = current_time / description.target_time;

        if (alpha >= 1.0f) {
            alpha = 1.0f;
            playing = false;
            finished = true;
        }

        float eased_alpha = get_eased_value(alpha);

        // Линейная интерполяция (LERP) для каждого зарегистрированного свойства
        for (const auto& pair : goal_properties) {
            const std::string& prop = pair.first;
            float start_val = initial_properties[prop];
            float end_val = pair.second;

            float current_val = start_val + (end_val - start_val) * eased_alpha;
            apply_property(prop, current_val);
        }
    }

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
        flex_padding(0.f, 0.f),
        parent(nullptr)
    {
    }

    uiobject* uisystem::get_by_name(const std::string& target_name) {
        for (const auto& obj : objects) {
            if (obj && obj->name == target_name) {
                return obj.get();
            }
        }
        return nullptr; // Если ничего не нашли
    }

    uiobject* uisystem::get_by_id(int target_id) {
        for (const auto& obj : objects) {
            if (obj && obj->id == target_id) {
                return obj.get();
            }
        }
        return nullptr;
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
        std::sort(this->children.begin(), this->children.end(), [](const auto& a, const auto& b) {
            return a->z_index < b->z_index;
            });
    }

    void apply_aspect_ratio(uiobject* o, float& real_w, float& real_h) {
        if (o->aspect_ratio <= 0.0f) return;

        if (o->dominant_axis == axis::width) {
            real_h = real_w / o->aspect_ratio;
        }
        else if (o->dominant_axis == axis::height) {
            real_w = real_h * o->aspect_ratio;
        }
    }

    vec4 get_dimensions_and_pos(uiobject* o, float win_w, float win_h, const uisystem& ui_sys) {
        float parent_w = o->parent ? o->parent->real_size.x : win_w;
        float parent_h = o->parent ? o->parent->real_size.y : win_h;

        float padding_basis = (o->dominant_padding_axis == axis::width) ? ui_sys.window_size.x : ui_sys.window_size.y;

        float pad_l = o->padding.left * padding_basis;
        float pad_r = o->padding.right * padding_basis;
        float pad_t = o->padding.top * padding_basis;
        float pad_b = o->padding.bottom * padding_basis;

        float w = (parent_w * o->size.x_scale + o->size.x_offset) - (pad_l + pad_r);
        float h = (parent_h * o->size.y_scale + o->size.y_offset) - (pad_t + pad_b);

        // --- ИСПРАВЛЕНИЕ: Применяем aspect_ratio СТРОГО с учетом вычтенных паддингов ---
        if (o->aspect_ratio > 0.0f) {
            if (o->dominant_axis == axis::width) {
                h = w / o->aspect_ratio;
            }
            else if (o->dominant_axis == axis::height) {
                w = h * o->aspect_ratio;
            }
        }

        if (w < o->min_w) w = o->min_w;
        if (h < o->min_h) h = o->min_h;

        float x = (parent_w * o->pos.x_scale + o->pos.x_offset) + pad_l;
        float y = (parent_h * o->pos.y_scale + o->pos.y_offset) + pad_t;

        return { x, y, w, h };
    }

    vec4 get_dimensions_and_pos_by_flex(uiobject* o, float win_w, float win_h, const uisystem& ui_sys) {
        if (!o || !o->parent) {
            return { 0.0f, 0.0f, 0.0f, 0.0f };
        }

        int children_count = o->parent->children.size();
        if (children_count == 0) return { 0.0f, 0.0f, 0.0f, 0.0f };

        auto& p_ch = o->parent->children;
        auto it = std::find(p_ch.begin(), p_ch.end(), o);
        if (it == p_ch.end()) return { 0.0f, 0.0f, 0.0f, 0.0f };

        float w = o->parent->real_size.x;
        float h = o->parent->real_size.y;

        float fx = 0.0f; float fy = 0.0f; float fw = 0.0f; float fh = 0.0f;

        float padding_basis_parent = (o->parent->dominant_padding_axis == axis::width) ? ui_sys.window_size.x : ui_sys.window_size.y;

        float p_pad_l = o->parent->padding.left * padding_basis_parent;
        float p_pad_r = o->parent->padding.right * padding_basis_parent;
        float p_pad_t = o->parent->padding.top * padding_basis_parent;
        float p_pad_b = o->parent->padding.bottom * padding_basis_parent;

        float inner_w = w - (p_pad_l + p_pad_r);
        float inner_h = h - (p_pad_t + p_pad_b);

        float total_scale_x = 0.0f;
        float total_scale_y = 0.0f;

        for (int i = 0; i < children_count; ++i) {
            total_scale_x += std::max(0.01f, p_ch[i]->size.x_scale);
            total_scale_y += std::max(0.01f, p_ch[i]->size.y_scale);
        }

        if (o->parent->layout_type == layout::flex_x) {
            // --- ИСПРАВЛЕНИЕ: Горизонтальный зазор зависит СТРОГО от ширины окна ---
            float opora = 0;
            if (o->dominant_flex_padding_axis == axis::width) {
                opora = ui_sys.window_size.x;
            }
            else {
                opora = ui_sys.window_size.y;
            }
            float gap_px = o->parent->flex_padding.x * opora;

            float total_gaps_w = (children_count > 1) ? static_cast<float>(children_count - 1) * gap_px : 0.0f;
            float available_for_items = inner_w - total_gaps_w;

            float current_fx = p_pad_l;
            float unit_w = (total_scale_x > 0.0f) ? available_for_items / total_scale_x : 0.0f;

            for (int i = 0; i < children_count; ++i) {
                float child_w = unit_w * p_ch[i]->size.x_scale;
                float child_h = inner_h * p_ch[i]->size.y_scale;

                if (p_ch[i]->aspect_ratio > 0.0f && p_ch[i]->dominant_axis == axis::height) {
                    child_w = child_h * p_ch[i]->aspect_ratio;
                }
                else if (p_ch[i]->aspect_ratio > 0.0f && p_ch[i]->dominant_axis == axis::width) {
                    child_h = child_w / p_ch[i]->aspect_ratio;
                }

                if (child_w < p_ch[i]->min_w) child_w = p_ch[i]->min_w;
                if (child_h < p_ch[i]->min_h) child_h = p_ch[i]->min_h;

                if (p_ch[i] == o) {
                    fw = child_w;
                    fh = child_h;
                    fx = current_fx;
                    fy = p_pad_t;
                    break;
                }
                current_fx += child_w + gap_px;
            }
        }
        else if (o->parent->layout_type == layout::flex_y) {
            // --- ИСПРАВЛЕНИЕ: Вертикальный зазор зависит СТРОГО от высоты окна ---
            float gap_px = o->parent->flex_padding.y * ui_sys.window_size.y;

            float total_gaps_h = (children_count > 1) ? static_cast<float>(children_count - 1) * gap_px : 0.0f;
            float available_for_items = inner_h - total_gaps_h;

            float current_fy = p_pad_t;
            float unit_h = (total_scale_y > 0.0f) ? available_for_items / total_scale_y : 0.0f;

            float fixed_height_allocated = 0.0f;
            float flexible_scale_y = total_scale_y;

            for (int i = 0; i < children_count; ++i) {
                float naive_h = unit_h * p_ch[i]->size.y_scale;
                if (naive_h < p_ch[i]->min_h) {
                    fixed_height_allocated += p_ch[i]->min_h;
                    flexible_scale_y -= p_ch[i]->size.y_scale;
                }
            }

            if (flexible_scale_y > 0.0f) {
                unit_h = (available_for_items - fixed_height_allocated) / flexible_scale_y;
            }

            for (int i = 0; i < children_count; ++i) {
                float child_w = inner_w * p_ch[i]->size.x_scale;
                float child_h = 0.0f;

                float naive_h = (total_scale_y > 0.0f) ? (available_for_items / total_scale_y) * p_ch[i]->size.y_scale : 0.0f;
                if (naive_h < p_ch[i]->min_h) {
                    child_h = p_ch[i]->min_h;
                }
                else {
                    child_h = unit_h * p_ch[i]->size.y_scale;
                }

                if (p_ch[i]->aspect_ratio > 0.0f && p_ch[i]->dominant_axis == axis::width) {
                    child_h = child_w / p_ch[i]->aspect_ratio;
                }
                else if (p_ch[i]->aspect_ratio > 0.0f && p_ch[i]->dominant_axis == axis::height) {
                    child_w = child_h * p_ch[i]->aspect_ratio;
                }

                if (child_w < p_ch[i]->min_w) child_w = p_ch[i]->min_w;
                if (child_h < p_ch[i]->min_h) child_h = p_ch[i]->min_h;

                if (p_ch[i] == o) {
                    fw = child_w;
                    fh = child_h;
                    fx = p_pad_l;
                    fy = current_fy;
                    break;
                }
                current_fy += child_h + gap_px;
            }
        }
        else if (o->parent->layout_type == layout::none) {
            return get_dimensions_and_pos(o, win_w, win_h, ui_sys);
        }

        // Внутренние паддинги
        float child_pad_basis = (o->dominant_padding_axis == axis::width) ? ui_sys.window_size.x : ui_sys.window_size.y;
        float c_pad_l = o->padding.left * child_pad_basis;
        float c_pad_t = o->padding.top * child_pad_basis;

        if (o->aspect_ratio > 0.0f) {
            float c_pad_r = o->padding.right * child_pad_basis;
            float c_pad_b = o->padding.bottom * child_pad_basis;
            fw -= (c_pad_l + c_pad_r + 1.0f);
            fh -= (c_pad_t + c_pad_b + 1.0f);

            if (o->dominant_axis == axis::height) {
                fw = fh * o->aspect_ratio;
            }
            else if (o->dominant_axis == axis::width) {
                fh = fw / o->aspect_ratio;
            }
            fx += c_pad_l;
            fy += c_pad_t;
        }
        else {
            if (fw > 1.0f) fw -= 1.0f;
            if (fh > 1.0f) fh -= 1.0f;
        }

        if (fw < o->min_w) fw = o->min_w;
        if (fh < o->min_h) fh = o->min_h;

        return { fx, fy, fw, fh };
    }


    sf::Color get_gradient_color(const color_composition& comp, float targetPos, float alphaMultiplier) {
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
        // ИСПРАВЛЕНО: Копируем конкретные вершины по индексам, а не весь объект va целиком
        va[index++] = va[0];
        va[index++] = va[1];
        win.draw(va);
    }
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

                sf::Vector2f innerLocal = centers[q] + dir * radius;
                sf::Vector2f innerGlobal = innerLocal - origin + shadowPos;

                sf::Vector2f outerLocal = centers[q] + dir * (radius + shadowSizeInPixels);
                sf::Vector2f outerGlobal = outerLocal - origin + shadowPos;

                vaOuter[index++] = sf::Vertex(innerGlobal, innerColor);
                vaOuter[index++] = sf::Vertex(outerGlobal, outerColor);
            }
        }
        // ИСПРАВЛЕНО
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
        // ИСПРАВЛЕНО
        vaInner[innerIndex] = vaInner[1];

        win.draw(vaInner);
        win.draw(vaOuter);
    }


    sf::Shader* get_ui_shader() {
        static sf::Shader ui_shader;
        static bool ui_shader_loaded = false;
        if (!ui_shader_loaded) {
            const std::string fragmentShader = R"(
            uniform vec2 size;
            uniform float alpha;
            uniform sampler2D texture;
            uniform float normalized_roundness;
            uniform float global_scale;
            uniform sampler2D gradient_texture;
            uniform bool has_blur;
            uniform bool is_repeated;
            uniform vec2 image_offset;
            uniform vec2 image_scale;
            uniform sampler2D img_texture;
            uniform bool has_image;
            void main() {
                vec2 uv = vec2(gl_TexCoord[0].x, gl_TexCoord[0].y);
                vec2 p = uv * size;
                vec2 half_size = size * 0.5;

                float min_side = min(size.x, size.y);
                float radius = global_scale * 0.5 * normalized_roundness;
                radius = min(radius, min_side * 0.5);

                vec2 d = abs(p - half_size) - half_size + vec2(radius);
                float dist = length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - radius;
                float edge_alpha = smoothstep(0.5, -0.5, dist);

                if (edge_alpha <= 0.0) {
                    discard;
                }

                vec4 base_color = texture2D(gradient_texture, uv);
                if (has_blur == true) {
                    vec4 blur_color = texture2D(texture, uv);
                    base_color = mix(blur_color, base_color, base_color.a);
                }

                vec4 final_pixel = base_color;
                if (has_image == true) {
                    if (is_repeated == true) {
                        vec2 scale = max(vec2(0.001), image_scale);
                        vec2 scale_factor = size / (global_scale * scale);
                        vec2 repeated_uv = fract((uv * scale_factor) + (image_offset / (global_scale * scale)));
                        vec4 img_color = texture2D(img_texture, repeated_uv);
                        final_pixel = vec4(base_color.rgb * img_color.rgb, base_color.a * img_color.a);
                    } else {
                        vec4 img_color = texture2D(img_texture, uv);
                        float current_img_alpha = img_color.a * alpha;
                        float out_a = current_img_alpha + base_color.a * (1.0 - current_img_alpha);
                        vec3 out_rgb = (img_color.rgb * current_img_alpha + base_color.rgb * base_color.a * (1.0 - current_img_alpha)) / max(0.001, out_a);
                        final_pixel = vec4(out_rgb, out_a);
                    }
                }
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


    std::shared_ptr<uiobject> uiobject::create(uisystem& sys) {
        auto obj = std::make_shared<uiobject>();
        sys.objects.push_back(obj);
        return obj;
    }

    void uiobject::update(sf::RenderTarget& win, const uisystem& ui_sys) { // <-- переименовали в ui_sys
        sf::Vector2u win_dim = win.getSize();
        float win_w = static_cast<float>(win_dim.x);
        float win_h = static_cast<float>(win_dim.y);
        float real_w, real_h, real_x, real_y;

        if (parent == nullptr) {
            vec4 ass = get_dimensions_and_pos(this, win_w, win_h, ui_sys); // <-- ui_sys
            real_x = ass.x; real_y = ass.y; real_w = ass.z; real_h = ass.w;
        }
        else {
            vec4 ass = get_dimensions_and_pos_by_flex(this, win_w, win_h, ui_sys); // <-- ui_sys
            real_x = ass.x; real_y = ass.y; real_w = ass.z; real_h = ass.w;
        }
        if (real_w <= 0.f || real_h <= 0.f) return;

        unsigned int tex_w = static_cast<unsigned int>(std::round(real_w));
        unsigned int tex_h = static_cast<unsigned int>(std::round(real_h));

        real_size = { static_cast<float>(tex_w), static_cast<float>(tex_h) };
        real_pos = { std::round(real_x), std::round(real_y) };

        if (parent == nullptr) {
            absolute_pos = real_pos;
        }
        else {
            absolute_pos = parent->absolute_pos + real_pos;
        }

        if (render_tex.getSize() != sf::Vector2u(tex_w, tex_h)) {
            render_tex.create(tex_w, tex_h);
            render_tex.setSmooth(true);
        }

        sf::RenderStates bg_states;
        bg_states.blendMode = sf::BlendNone;
        render_tex.clear(sf::Color::Transparent);

        if (background_color.content.size() >= 2) {
            sf::VertexArray gradient(sf::PrimitiveType::TriangleStrip, 4);
            gradient[0].position = { 0, 0 };
            gradient[1].position = { real_size.x, 0 };
            gradient[2].position = { 0, real_size.y };
            gradient[3].position = { real_size.x, real_size.y };

            gradient[0].color = background_color.content[0]->content;
            gradient[1].color = background_color.content[0]->content;
            gradient[2].color = background_color.content[1]->content;
            gradient[3].color = background_color.content[1]->content;
            render_tex.draw(gradient, bg_states);
        }
        else if (!background_color.content.empty()) {
            sf::RectangleShape r(real_size);
            r.setFillColor(background_color.content[0]->content);
            render_tex.draw(r, bg_states);
        }
        else {
            sf::RectangleShape r(real_size);
            r.setFillColor(sf::Color::White);
            render_tex.draw(r, bg_states);
        }

        float padding_basis = (dominant_padding_axis == axis::width) ? ui_sys.window_size.x : ui_sys.window_size.y;
        float border_px = border_size;
        if (border_px >= 1.0f) border_px = (border_size / 512.f) * padding_basis;
        else border_px = border_size * padding_basis;


        if (border_px > 0.0f && !border_color.content.empty()) {
            float r_px = ui_sys.window_size.x * 0.5f * roundness; // <-- ui_sys
            sf::Vector2f inner_border_size = { real_size.x - border_px * 2.0f + 0.5f, real_size.y - border_px * 2.0f + 0.5f };
            sf::Vector2f inner_border_pos = { border_px - 0.25f, border_px - 0.25f };
            float inner_radius = std::max(0.0f, r_px - border_px);
            draw_rounded_border(render_tex, inner_border_size, inner_border_pos, { 0, 0 }, inner_radius, border_px, border_color, 1.0f);
        }

        for (const auto& child : children) {
            child->update(render_tex, ui_sys); // <-- ui_sys
        }

        // --- Вставьте этот блок в самый конец метода uiobject::update ---
        if (!text_content.empty()) {
            // Загружаем шрифт СТРОГО один раз
            if (!font_loaded || font_path != font_path) {
                if (font.loadFromFile(font_path)) {
                    font_loaded = true;
                }
            }

            if (font_loaded) {
                text.setFont(font);
                text.setStyle(text_style);
                text.setString(text_content);

                // Считаем размер шрифта от window_size
                float window_basis = (dominant_axis == axis::width) ? ui_sys.window_size.x : ui_sys.window_size.y;
                float targetPixelSize = window_basis * text_size;

                if (targetPixelSize > 0.f) {
                    unsigned int calculatedSize = static_cast<unsigned int>(std::round(targetPixelSize));
                    text.setCharacterSize(calculatedSize);
                    if (dominant_axis == axis::height) {
                        if (sf::Font* f = const_cast<sf::Font*>(text.getFont())) {
                            float lineSpacing = f->getLineSpacing(calculatedSize);
                            float scaleCorrection = targetPixelSize / lineSpacing;
                            unsigned int correctedSize = static_cast<unsigned int>(std::round(calculatedSize * scaleCorrection));
                            text.setCharacterSize(std::max(1U, correctedSize));
                        }
                    }
                }

                // Перенос строк делаем ТОЖЕ только при обновлении геометрии
                if (text_wrap) {
                    wrapped_str = wrap_text(text_content, real_size.x, text);
                    text.setString(wrapped_str);
                }
                else {
                    text.setString(text_content);
                }

                sf::FloatRect textBounds = text.getLocalBounds();
                text.setOrigin({ textBounds.left, textBounds.top });

                float posX = absolute_pos.x;
                if (align_x == text_align_x::center) posX += (real_size.x - textBounds.width) / 2.0f;
                else if (align_x == text_align_x::right) posX += real_size.x - textBounds.width;

                float posY = absolute_pos.y;
                if (align_y == text_align_y::top) posY += real_size.y - textBounds.height;
                else if (align_y == text_align_y::center) posY += (real_size.y - textBounds.height) / 2.0f;

                text.setPosition({ std::round(posX), std::round(posY) });
            }
        }
        // -----------------------------------------------------------------


        render_tex.display();
        sprite.setTexture(render_tex.getTexture(), true);
    }

    void uiobject::draw(sf::RenderTarget& win, const uisystem& ui_sys) { // <-- Принимаем ui_sys
        if (visibility <= 0.0f) return;
        sf::Vector2u win_dim = win.getSize();

        float win_w = static_cast<float>(win_dim.x);
        float win_h = static_cast<float>(win_dim.y);

        float a = 1.f;
        if (parent != nullptr) a = parent->visibility;
        float alpha_mul = visibility * a;

        float sprite_center_x = absolute_pos.x + real_size.x / 2.0f;
        float sprite_center_y = absolute_pos.y + real_size.y / 2.0f;

        if (shadow_color.a > 0 && shadow_size > 0.0f) {
            float r_px = win_w * 0.5f * roundness;
            float shadow_px = shadow_size * win_w;
            float offset_y = shadow_offset.y * win_h;

            if (parent != nullptr) offset_y = -offset_y;

            draw_rounded_shadow(win, real_size, absolute_pos, { 0, 0 }, r_px, shadow_px, { shadow_offset.x * win_w, offset_y }, shadow_color, alpha_mul);
        }

        sprite.setPosition({ sprite_center_x, sprite_center_y });
        sprite.setOrigin({ sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2 });
        sprite.setRotation(rotation);
        sprite.setScale(1.f, 1.f);

        sf::Shader* ui_shader = get_ui_shader();
        if (ui_shader != nullptr) {
            ui_shader->setUniform("size", real_size);
            ui_shader->setUniform("normalized_roundness", roundness);
            ui_shader->setUniform("alpha", alpha_mul);
            ui_shader->setUniform("global_scale", win_w);

            if (background_image.getSize().x > 0 && background_image.getSize().y > 0) {
                ui_shader->setUniform("has_image", true);
                ui_shader->setUniform("is_repeated", repeated);
                ui_shader->setUniform("image_offset", image_offset);
                ui_shader->setUniform("image_scale", image_scale);
                ui_shader->setUniform("img_texture", background_image);
            }
            else {
                ui_shader->setUniform("has_image", false);
            }
            ui_shader->setUniform("gradient_texture", render_tex.getTexture());

            sf::RenderStates states;
            states.shader = ui_shader;
            states.blendMode = sf::BlendAlpha;
            win.draw(sprite, states);
        }
        else {
            sf::RenderStates states;
            states.blendMode = sf::BlendAlpha;
            win.draw(sprite, states);
        }

        // Отрисовка текста
    // --- Внутри uiobject::draw заменяем весь старый блок текста на этот: ---
        if (!text_content.empty() && font_loaded) {
            sf::Vector2u win_dim = win.getSize();
            float win_w = static_cast<float>(win_dim.x);

            float a = 1.f;
            if (parent != nullptr) a = parent->visibility;
            float alpha_mul = visibility * a;

            // Быстро обновляем только прозрачность и цвет контура, если они изменились
            sf::Color dynamic_text_color = text_color;
            dynamic_text_color.a = static_cast<sf::Uint8>(dynamic_text_color.a * alpha_mul);
            text.setFillColor(dynamic_text_color);

            sf::Color dynamic_border_color = text_border_color;
            dynamic_border_color.a = static_cast<sf::Uint8>(dynamic_border_color.a * alpha_mul);
            text.setOutlineColor(dynamic_border_color);
            text.setOutlineThickness(text_border_size * win_w);

            // Мгновенная пиксельная отрисовка
            win.draw(text);
        }
        // ----------------------------------------------------------------------


        // Рекурсивно передаем ui_sys дальше по иерархии детей
        for (const auto& child : children) {
            child->draw(win, ui_sys); // <-- ИСПРАВЛЕНИЕ
        }
    }

    void uisystem::update_uiobjects(sf::RenderTarget& win) {
        sf::Vector2u size = win.getSize();
        window_size = { static_cast<float>(size.x), static_cast<float>(size.y) };

        for (const auto& o : objects) {
            if (o->parent == nullptr) {
                o->update(win, *this);
            }
        }
    }

    void uisystem::update_tweens() {
        static sf::Clock delta_clock;
        float dt = delta_clock.restart().asSeconds();

        // 1. Апдейтим и очищаем завершенные твины
        for (auto it = tweens.begin(); it != tweens.end();) {
            if (*it) {
                (*it)->update(dt);
            }

            // Очищаем уничтоженные/ненужные твины, если shared_ptr больше нигде не удерживается
            if (it->use_count() == 1 && (*it)->is_finished()) {
                it = tweens.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    void uisystem::draw(sf::RenderTarget& win) {
        for (const auto& o : objects) {
            if (o->parent == nullptr) {
                o->draw(win, *this);
            }
        }
    }

    void calculate_gzindex(uiobject* obj, int& current_global_index) {
        if (!obj) return;

        // Сначала присваиваем глобальный индекс самому родителю
        obj->global_zindex = current_global_index++;

        // Сортируем детей этого объекта по их локальному z_index перед обходом
        if (!obj->children.empty()) {
            std::sort(obj->children.begin(), obj->children.end(), [](const uiobject* a, const uiobject* b) {
                return a->z_index < b->z_index;
                });

            // Рекурсивно идем вглубь по отсортированным детям
            for (uiobject* child : obj->children) {
                calculate_gzindex(child, current_global_index);
            }
        }
    }

    void uisystem::sort() {
        if (objects.empty()) return;

        // 1. Собираем все корневые элементы (у которых parent == nullptr)
        std::vector<uiobject*> roots;
        for (const auto& obj : objects) {
            if (obj->parent == nullptr) {
                roots.push_back(obj.get());
            }
        }

        // 2. Сортируем корневые элементы между собой по их локальному z_index
        std::sort(roots.begin(), roots.end(), [](const uiobject* a, const uiobject* b) {
            return a->z_index < b->z_index;
            });

        // 3. Запускаем рекурсивный сквозной подсчет глобального z-индекса по всему дереву
        int current_global_index = 0;
        for (uiobject* root : roots) {
            calculate_gzindex(root, current_global_index);
        }

        // 4. Теперь сортируем НАШ ПЛОСКИЙ ВЕКТОР СИСТЕМЫ по сгенерированному global_zindex.
        // Элементы, которые рисуются позже (имеют больший global_zindex), окажутся в конце вектора.
        std::sort(objects.begin(), objects.end(), [](const std::shared_ptr<uiobject>& a, const std::shared_ptr<uiobject>& b) {
            return a->global_zindex < b->global_zindex;
            });
    }

    event uisystem::check_events(sf::Event& ev, sf::RenderWindow& win) {
        sf::Vector2f point = win.mapPixelToCoords(sf::Mouse::getPosition(win));

        // Так как плоский вектор objects теперь ИДЕАЛЬНО отсортирован по возрастанию global_zindex,
        // мы можем просто пройтись по нему В ОБРАТНОМ НАПРАВЛЕНИИ (от самых верхних объектов на экране к нижним).
        for (auto it = objects.rbegin(); it != objects.rend(); ++it) {
            uiobject* o = it->get();

            // Проверяем видимость и флаг кликабельности (clickable)
            if (o->visibility <= 0.0f || !o->clickable) continue;

            // Проверка попадания в рамки absolute_pos за O(1)
            if (point.x >= o->absolute_pos.x && point.x <= o->absolute_pos.x + o->real_size.x &&
                point.y >= o->absolute_pos.y && point.y <= o->absolute_pos.y + o->real_size.y) {

                event e;
                e.object = o;
                e.mouse_pos = point;
                e.type = event_t::hover;
                e.button = mousebutton::none;

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
                else if (ev.type == sf::Event::MouseWheelScrolled && ev.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                    e.type = (ev.mouseWheelScroll.delta > 0.f) ? event_t::scrollup : event_t::scrolldown;
                    e.button = mousebutton::none;
                    ev.type = sf::Event::Count;
                }
                return e;
            }
        }

        event empty_e;
        empty_e.type = event_t::none;
        empty_e.button = mousebutton::none;
        return empty_e;
    }
}