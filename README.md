# ⏱️ Stopwatch Pro: Productivity Companion

A high-performance, aesthetically pleasing floating stopwatch and timer designed for deep work and solo productivity.

![Stopwatch Pro Banner](https://raw.githubusercontent.com/AlimamHu/Flying-stop-watch/main/Asset/banner.png)

## 🚀 Key Features

- **🛡️ Always-on-Top**: Stays visible over any application for seamless tracking.
- **🎨 Dynamic Theme System**: 8 curated visual styles + unique theme generation.
- **💻 Command Line Power**: Launch with precise themes or random shuffles.
- **📊 Intensity Tracking**: Border colors evolve based on your work duration.
- **✨ Dopamine System**: Rainbow quotes, Focus Pet with Aura, and satisfying sound effects.
- **🎬 Professional HUD**: Hover-dimming background to highlight controls.

---

## ⌨️ Command Line Usage
Launch the application with these flags to customize your startup experience:

| Command | Description | Example |
| :--- | :--- | :--- |
| `--theme [Name]` | Launch with a specific theme name. | `python stopwatch_pro.py --theme Cyberpunk` |
| `--theme [Number]`| Launch with a specific theme index (1-8). | `python stopwatch_pro.py --theme 4` |
| `-r`, `--random` | Start with a **Random Preset** from the library. | `python stopwatch_pro.py -r` |
| `-tg`, `--generate`| Generate a **Completely Unique** color shuffle. | `python stopwatch_pro.py -tg` |

---

## 🎨 Theme Library Reference
Use these names or numbers with the `--theme` command:

1. **Windows 11** - Clean, modern, and translucent.
2. **Cyberpunk** - Neon pink and blue high-contrast vibe.
3. **Midnight** - Deep blacks and professional grays.
4. **Rose Gold** - Elegant, soft, and premium.
5. **Nord** - Arctic blue frost, calm and focused.
6. **Forest** - Natural greens for deep concentration.
7. **Dracula** - The classic developer dark mode.
8. **Sunset** - Warm oranges and purples.

---

## 🛠️ Adjustable Parameters
Customize these values in the `stopwatch_pro.py` file under the `ADJUSTABLE PARAMETERS` section:

### Visual & Behavior
| Parameter | Default | Description |
| :--- | :--- | :--- |
| `active_theme` | `"Forest"` | The theme applied at startup. |
| `enable_intensity` | `True` | Changes border colors based on work time (Calm/Focused/Intense). |
| `lava_lamp_mode` | `True` | Enables fluid, liquid-style border animations. |
| `show_pet` | `True` | Toggles the tiny cat companion and its dynamic aura. |
| `enable_motivation` | `True` | Toggles the rainbow motivational quotes every minute. |
| `enable_sounds` | `True` | Toggles the "pop" sound effects for interactions. |

### Layout & Transparency
| Parameter | Default | Description |
| :--- | :--- | :--- |
| `width` | `190` | Width of the floating widget in pixels. |
| `height` | `60` | Height of the floating widget (Optimized for pet). |
| `idle_opacity` | `0.85` | Transparency level when the mouse is NOT over the widget. |
| `hover_opacity` | `1.0` | Transparency level when hovering over the widget. |
| `corner_radius` | `12` | Roundness of the widget corners. |

---

## 📸 Screenshots

| Standard View | Hover Controls (Dimmed) |
| :---: | :---: |
| ![Idle State](Asset/1.png) | ![Hover State](Asset/2.png) |

## 📦 Installation & Run

1. **Clone the repository**:
   ```bash
   git clone https://github.com/AlimamHu/Flying-stop-watch.git
   ```
2. **Run the App**:
   ```bash
   python stopwatch_pro.py
   ```

## 📜 License
MIT License - Feel free to use and modify for your own productivity!
