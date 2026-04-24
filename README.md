# 🚀 Stopwatch Pro: Ultimate Floating Productivity Tool

![Stopwatch Pro Overview](Asset/Image%20.png)

Stopwatch Pro is a sleek, minimalist, and high-performance floating stopwatch and timer designed to help you stay focused and track your productivity without cluttering your workspace. Built with a modern aesthetic, it mimics a premium desktop widget while providing powerful work-tracking features.

## ✨ Key Features

- **🛸 Always-on-Top Floating Window**: Keep your time visible above all other applications.
- **🔄 Dual Mode Functionality**: Seamlessly switch between a traditional **Stopwatch** and a customizable **Timer**.
- **⏳ Overtime Support**: The timer no longer stops at zero; it continues with a negative sign and pulses in **Red & Black** to alert you.
- **🎨 Dynamic Theme Engine**: Choose from curated presets.
  - **Windows 11**: Clean, light, and professional.
  - **Cyberpunk**: Vibrant neon accents for high-energy sessions.
  - **Midnight**: Deep blues and reds for late-night focus.
  - **Rose Gold**: Soft and elegant aesthetic.
  - **Nord**: Arctic frost blue-gray for a clean look.
  - **Forest**: Deep emerald and earth tones for nature lovers.
  - **Dracula**: The classic dark mode with purple/pink accents.
  - **Sunset**: Vibrant oranges and warm purples.
- **🔥 Work Intensity Tracking**: The gradient border evolves as you work (in both **Stopwatch** and **Timer** modes):
  - **Calm (0-25m)**: Default theme colors.
  - **Focused (25-50m)**: Yellow/Green gradients.
  - **Intense (50m+)**: Orange/Red gradients to signal deep work state.
- **🌈 Rotating Gradient Border**: A beautiful animated border that pulses and rotates while the clock is running.
- **✨ Dopamine Motivation System**: 
  - **Rainbow Quotes**: Displays motivational phrases in cycling rainbow colors every minute.
  - **Fade-Out Animation**: Quotes smoothly appear and fade away to maintain focus.
  - **Solo-Work Boost**: Designed specifically to keep you going when working alone.
- **🔔 Smart Visual Notifications**:
  - **Minute Pulse**: Flashes the time text every minute.
  - **Timer Alerts**: Flashes red when the timer is nearly finished or in overtime.
- **🖱️ Intuitive Interactions**:
  - **Draggable**: Move the widget anywhere on your screen with optimized, smooth motion.
  - **Hover Reveal**: Controls and info are hidden until you hover over the window to minimize distraction.
  - **Transparency**: Adjusts opacity automatically when idle to blend into your background.

## 🛠️ Installation

### Prerequisites
- **Python 3.x**
- **Tkinter** (usually included with Python)

### Setup
1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/stopwatch-pro.git
   cd stopwatch-pro
   ```
2. Run the application:
   ```bash
   python stopwatch_pro.py
   ```

## 🎮 How to Use

- **Start/Pause**: Click the  (Play) or  (Pause) icon.
- **Switch Mode**: Click the  (Stopwatch) or  (Timer) icon.
- **Change Theme**: Click the  (Gear/Theme) icon to cycle through aesthetics.
- **Add Time (Timer Mode)**: Click the **+1** icon to instantly add a minute.
- **Reset**: Click the  (Reset) icon to start over.
- **Move**: Click and drag anywhere on the widget to reposition it.
- **Close**: Click the  (X) icon to exit.

## 📜 Configuration

You can customize the default behavior directly in `stopwatch_pro.py` under the `ADJUSTABLE PARAMETERS` section:

```python
self.active_theme = "Rose Gold"  # Default theme
self.enable_intensity = True    # Enable color shifts based on work time
self.width = 190                # Widget width
self.height = 50                # Widget height (optimized for motivation text)
self.idle_opacity = 0.85        # Opacity when not hovered
```

## 🤝 Contributing

Contributions are welcome! Whether it's adding new themes, refining animations, or improving performance, feel free to open a Pull Request.

## 📄 License

This project is licensed under the MIT License.

## 📸 Screenshots

| Active Timer | Hover Controls |
| :---: | :---: |
| ![Timer Running](Asset/Image%20%20(2).png) | ![Controls Reveal](Asset/Image%20%20(1).png) |

---
*Built for focused minds.* 🧠✨
