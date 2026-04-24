# ⏱️ Stopwatch Pro: Productivity Companion

A high-performance, aesthetically pleasing floating stopwatch and timer designed for deep work and solo productivity.

![Stopwatch Pro Banner](https://raw.githubusercontent.com/AlimamHu/Flying-stop-watch/main/Asset/banner.png)

## 🚀 Key Features

- **🛡️ Always-on-Top**: Stays visible over any application for seamless tracking.
- **🎨 Dynamic Theme System**: Switch between curated themes like *Rose Gold*, *Cyberpunk*, *Midnight*, and *Nord*.
- **📊 Work Intensity Tracking**: The gradient border evolves from **Calm** 🟢 to **Focused** 🟡 to **Intense** 🔥 based on your session duration.
- **✨ Dopamine Motivation System**:
    - **🌈 Rainbow Quotes**: Displays motivational phrases in cycling rainbow colors every minute.
    - **🐱 Focus Pet + Aura**: A tiny companion with a **Dynamic Glow/Aura** that changes color based on your work intensity. It bounces while you work and sleeps when you're idle.
    - **🌊 Lava Lamp Border**: A fluid, liquid-style animation for a premium visual feel.
    - **🔊 Satisfying Sounds**: Auditory "pop" feedback for interactions and minute transitions.
- **🎬 Professional Controls**: Hover over the widget to reveal smooth controls with a **Background Dimming (Blur)** effect for better focus.

## 🛠️ Configuration
You can customize your experience directly in `stopwatch_pro.py` under the **ADJUSTABLE PARAMETERS** section:

```python
# --- ADJUSTABLE PARAMETERS ---
self.active_theme = "Rose Gold"    # Choose your aesthetic
self.enable_intensity = True      # Border color shifts based on effort
self.lava_lamp_mode = True        # Fluid liquid animation
self.show_pet = True              # Toggle your cat companion + Aura
self.enable_motivation = True     # Toggle minute-by-minute quotes
self.enable_sounds = True         # Toggle interaction sounds
```

## 📸 Screenshots

| Standard View | Hover Controls (Dimmed) |
| :---: | :---: |
| ![Idle State](Asset/1.png) | ![Hover State](Asset/2.png) |

## 📦 Installation

1. **Clone the repository**:
   ```bash
   git clone https://github.com/AlimamHu/Flying-stop-watch.git
   ```
2. **Install Dependencies**:
   This app uses the Python Standard Library (`tkinter`). No external packages are required!
3. **Run the App**:
   ```bash
   python stopwatch_pro.py
   ```

## 📜 License
MIT License - Feel free to use and modify for your own productivity!
