import tkinter as tk
from tkinter import simpledialog, messagebox
import time
import math
import random
import winsound
import argparse
import sys

class CompactStopwatch:
    def __init__(self, selected_theme=None):
        self.root = tk.Tk()
        self.root.title("Floating Stopwatch/Timer")
        self.root.overrideredirect(True)
        self.root.attributes('-topmost', True)
        
        # --- THEME ENGINE PRESETS ---
        self.THEMES = {
            "Windows 11": {
                "bg": "#ffffff", "text": "#000000", "dim": "#888888", 
                "accent": "#1c6ea4", "grad_s": "#d8efab", "grad_e": "#3498db"
            },
            "Cyberpunk": {
                "bg": "#0d0d0d", "text": "#00ff41", "dim": "#008f11", 
                "accent": "#00ff41", "grad_s": "#ff00ff", "grad_e": "#00ffff"
            },
            "Midnight": {
                "bg": "#1a1a2e", "text": "#e94560", "dim": "#4e4e6a", 
                "accent": "#0f3460", "grad_s": "#e94560", "grad_e": "#16213e"
            },
            "Rose Gold": {
                "bg": "#fff5f5", "text": "#b76e79", "dim": "#d4a5a5", 
                "accent": "#e5b3bb", "grad_s": "#ffc0cb", "grad_e": "#b76e79"
            },
            "Nord": {
                "bg": "#2e3440", "text": "#eceff4", "dim": "#4c566a", 
                "accent": "#88c0d0", "grad_s": "#81a1c1", "grad_e": "#88c0d0"
            },
            "Forest": {
                "bg": "#1b2e1b", "text": "#dcfce7", "dim": "#3f6212", 
                "accent": "#22c55e", "grad_s": "#166534", "grad_e": "#86efac"
            },
            "Dracula": {
                "bg": "#282a36", "text": "#f8f8f2", "dim": "#6272a4", 
                "accent": "#bd93f9", "grad_s": "#ff79c6", "grad_e": "#bd93f9"
            },
            "Sunset": {
                "bg": "#2d1b2d", "text": "#ff9e7d", "dim": "#634b63", 
                "accent": "#ff5f6d", "grad_s": "#ff5f6d", "grad_e": "#ffc371"
            }
        }
        self.MOTIVATIONS = [
            "KEEP PUSHING!", "YOU'VE GOT THIS!", "STAY FOCUSED!", "ALMOST THERE!",
            "CONSISTENCY IS KEY!", "ONE STEP AT A TIME!", "PROGRESS IS PROGRESS!",
            "STAY HUNGRY!", "DON'T STOP NOW!", "KEEP THE MOMENTUM!", "YOU ARE UNSTOPPABLE!",
            "MAKE IT COUNT!", "PROVE THEM WRONG!", "FOCUS ON THE GOAL!", "DO IT FOR YOU!"
        ]
        self.motivation_index = 0
        self.rainbow_colors = ["#ff0000", "#ff7f00", "#ffff00", "#00ff00", "#0000ff", "#4b0082", "#9400d3"]
        self.motivation_active = False
        
        # --- ADJUSTABLE PARAMETERS (Professional Finish) ---
        # [THEME LIST]: 1.Windows 11, 2.Cyberpunk, 3.Midnight, 4.Rose Gold, 5.Nord, 6.Forest, 7.Dracula, 8.Sunset
        self.active_theme = selected_theme if selected_theme else "Forest" 
        self.enable_intensity = True   # Enable border color shifts based on work time (Calm -> Focused -> Intense)
        self.lava_lamp_mode = True     # Enable fluid, melting border animation (OFF for classic rotation)
        self.show_pet = True           # Toggle the tiny cat companion at the bottom-left
        self.enable_motivation = True  # Toggle rainbow motivational quotes every minute
        self.enable_sounds = True      # Toggle satisfying "pop" sounds for button clicks
        
        self.width = 190               # Width of the floating widget
        self.height = 60               # Height of the floating widget (60 is optimized for Pet + Quotes)
        self.corner_radius = 12        # Roundness of the widget corners
        
        self.idle_opacity = 0.85       # Opacity (0.0 to 1.0) when the mouse is NOT over the widget
        self.hover_opacity = 1.0       # Opacity (0.0 to 1.0) when the mouse is over the widget
        
        # Internal State (Do not change)
        self.pet_emojis = {"idle": "🐱", "work": "😺", "intense": "🙀", "sleep": "😴"}
        self.pet_bounce = 0
        self.motivation_active = False
        
        self.show_labels = False       # Toggle extra text labels (minimalist look uses False)
        self.show_task = False         # Toggle task name display at the top
        self.show_start_info = False   # Toggle timer start duration info
        self.time_alignment = "center" # Vertical alignment of the time text
        
        self.stopwatch_minute_blink = True  # Flash the time text every new minute
        self.stopwatch_blink_count = 3       # Number of times to flash
        self.stopwatch_blink_color = "#ff4757" # Color of the flash
        
        self.timer_warn_color = "#ff4757"  # Warning color for last 2 minutes of timer
        self.rotation_speed = 30           # Speed of the border animation (lower is faster)
        
        # --- COMPACT MODES (Dynamic Layouts) ---
        self.compact_level = 0  # 0: Standard, 1: Slim, 2: Mini, 3: Nano
        self.MODES = [
            {"width": 190, "height": 60, "time_font": 18, "ms_font": 10, "show_pet": True, "show_task": True},
            {"width": 155, "height": 50, "time_font": 15, "ms_font": 9, "show_pet": True, "show_task": False},
            {"width": 125, "height": 42, "time_font": 13, "ms_font": 8, "show_pet": False, "show_task": False},
            {"width": 95, "height": 34, "time_font": 11, "ms_font": 0, "show_pet": False, "show_task": False}
        ]
        # ----------------------------------------------------
        # ----------------------------------------------------

        # Load Theme Initial Values
        if isinstance(self.active_theme, dict):
            theme = self.active_theme
        else:
            theme = self.THEMES.get(self.active_theme, self.THEMES["Windows 11"])
            
        self.bg_color = theme["bg"]
        self.text_main_color = theme["text"]
        self.text_dim_color = theme["dim"]
        self.task_text_color = theme["accent"]
        self.gradient_start = theme["grad_s"]
        self.gradient_end = theme["grad_e"]
        self.border_width = 2
        
        self.mode = "stopwatch"     
        self.task_name = "CURRENT TASK"
        self.rotation_index = 0
        self.border_segments = []
        self.labels = [] 
        self.blink_state = False
        self.last_minute = 0        
        self.is_blinking = False    
        self.intensity_level = 0    # 0: Calm, 1: Focused, 2: Intense
        
        self.palette = self.generate_palette(self.gradient_start, self.gradient_end, 45)
        
        sw = self.root.winfo_screenwidth()
        self.root.geometry(f"{self.width}x{self.height}+{sw - self.width - 40}+40")
        
        self.root.attributes("-alpha", self.idle_opacity)
        self.transparent_color = "#abcdef" 
        self.root.config(bg=self.transparent_color)
        self.root.attributes("-transparentcolor", self.transparent_color)

        self.running = False
        self.start_time = 0
        self.elapsed_time = 0
        self.timer_duration = 0     
        self.original_duration = 0  

        self.setup_ui()
        
        self.canvas.bind("<ButtonPress-1>", self.start_move)
        self.canvas.bind("<B1-Motion>", self.do_move)
        self.root.bind("<Enter>", self.on_enter)
        self.root.bind("<Leave>", self.on_leave)
        self.root.bind("<Button-3>", self.cycle_compact_mode) # Right click for quick toggle

    def apply_theme(self, theme_name):
        if isinstance(theme_name, dict):
            theme = theme_name
            self.active_theme = "Generated"
        else:
            if theme_name not in self.THEMES: return
            self.active_theme = theme_name
            theme = self.THEMES[theme_name]
            
        self.bg_color = theme["bg"]
        self.text_main_color = theme["text"]
        self.text_dim_color = theme["dim"]
        self.task_text_color = theme["accent"]
        self.gradient_start = theme["grad_s"]
        self.gradient_end = theme["grad_e"]
        
        self.intensity_level = 0 
        self.palette = self.generate_palette(self.gradient_start, self.gradient_end, 45)
        
        self.pet_emojis = {"idle": "🐱", "work": "😺", "intense": "🙀", "sleep": "😴"}
        self.pet_bounce = 0
        self.motivation_active = False
        
        # Redraw everything
        self.apply_layout()
        self.controls_frame.config(bg=self.bg_color)
        for widget in self.controls_frame.winfo_children():
            widget.config(bg=self.bg_color)

    def generate_palette(self, c1, c2, n):
        r1, g1, b1 = self.hex_to_rgb(c1)
        r2, g2, b2 = self.hex_to_rgb(c2)
        palette = []
        for i in range(n):
            t = i / (n - 1)
            r = int(r1 + (r2 - r1) * t)
            g = int(g1 + (g2 - g1) * t)
            b = int(b1 + (b2 - b1) * t)
            palette.append(f'#{r:02x}{g:02x}{b:02x}')
        return palette + palette[::-1]

    def hex_to_rgb(self, hex_color):
        hex_color = hex_color.lstrip('#')
        return tuple(int(hex_color[i:i+2], 16) for i in (0, 2, 4))

    def setup_ui(self):
        self.canvas = tk.Canvas(self.root, width=self.width, height=self.height, 
                               bg=self.transparent_color, highlightthickness=0)
        self.canvas.pack(fill="both", expand=True)

        self.draw_smooth_rounded_rect(0, 0, self.width, self.height, self.corner_radius + 1, 
                                     fill="#000000", outline="", stipple="gray25")
        self.draw_smooth_rounded_rect(1, 1, self.width-1, self.height-1, self.corner_radius, 
                                     fill=self.bg_color, outline="")

        self.create_gradient_border()
        self.draw_smooth_rounded_rect(self.border_width + 1, self.border_width + 1, 
                                     self.width-self.border_width-1, self.height-self.border_width-1, 
                                     self.corner_radius - 2, fill="", outline="#f5f5f5", width=1)

        self.left_info = self.canvas.create_text(10, 8, text="", anchor="nw",
                                                font=('Segoe UI Semibold', 6), fill=self.text_dim_color)
        if self.show_task:
            self.canvas.itemconfig(self.left_info, text=self.task_name)
            self.canvas.tag_bind(self.left_info, "<Button-1>", self.change_task)

        self.mode_indicator = self.canvas.create_text(self.width - 10, 8, text="", anchor="ne",
                                                    font=('Segoe UI', 6), fill=self.text_dim_color)

        time_x = self.width / 2 - 10
        if self.time_alignment == "top": time_y = self.height / 2 - 2
        elif self.time_alignment == "bottom": time_y = self.height / 2 + 4
        else: time_y = self.height / 2 + 1 

        self.time_text = self.canvas.create_text(time_x, time_y - 4, text="00:00:00", 
                                               font=('Segoe UI Variable Display', 18, 'bold'), 
                                               fill=self.text_main_color, tags="main_ui")
        self.ms_text = self.canvas.create_text(time_x + 60, time_y - 1, text=".00", 
                                             font=('Segoe UI Variable Display', 10), 
                                             fill=self.task_text_color, tags="main_ui")
        
        # Focus Pet with "Color Look" Glow
        self.pet_glow = self.canvas.create_oval(15, self.height-28, 35, self.height-8, 
                                               fill=self.task_text_color, outline="", 
                                               state='hidden', tags="main_ui")
        self.pet_id = self.canvas.create_text(25, self.height - 18, text="🐱", 
                                             font=('Segoe UI Emoji', 14), tags="main_ui")

        # Motivation Text Placeholder
        self.motivation_text = self.canvas.create_text(self.width/2 + 10, self.height - 15, 
                                                      text="", font=('Segoe UI Semibold', 7), 
                                                      fill=self.bg_color, state='hidden', tags="main_ui")

        self.controls_visible = False
        self.controls_frame = tk.Frame(self.root, bg=self.bg_color)
        
        # Base button style
        btn_base = {"bg": self.bg_color, "font": ('Segoe UI', 9), "width": 2, "cursor": "hand2"}
        
        self.play_btn = tk.Label(self.controls_frame, text="", fg=self.text_main_color, **btn_base) 
        self.play_btn.pack(side='left', padx=1)
        self.play_btn.bind("<Button-1>", self.toggle)
        
        self.add_btn = tk.Label(self.controls_frame, text="+", fg=self.timer_add_color, **btn_base)
        self.add_btn.pack(side='left', padx=1)
        self.add_btn.bind("<Button-1>", self.add_minute)
        
        self.theme_btn = tk.Label(self.controls_frame, text="", fg=self.text_main_color, **btn_base)
        self.theme_btn.pack(side='left', padx=1)
        self.theme_btn.bind("<Button-1>", self.next_theme)
        
        self.mode_btn = tk.Label(self.controls_frame, text="", fg=self.text_main_color, **btn_base) 
        self.mode_btn.pack(side='left', padx=1)
        self.mode_btn.bind("<Button-1>", self.switch_mode)

        self.task_edit_btn = tk.Label(self.controls_frame, text="", fg=self.text_main_color, **btn_base)
        self.task_edit_btn.pack(side='left', padx=1)
        self.task_edit_btn.bind("<Button-1>", self.change_task)

        self.task_toggle_btn = tk.Label(self.controls_frame, text="", fg=self.text_main_color, **btn_base)
        self.task_toggle_btn.pack(side='left', padx=1)
        self.task_toggle_btn.bind("<Button-1>", self.toggle_task_visibility)

        self.compact_btn = tk.Label(self.controls_frame, text="", fg=self.task_text_color, **btn_base)
        self.compact_btn.pack(side='left', padx=1)
        self.compact_btn.bind("<Button-1>", self.cycle_compact_mode)
        
        self.reset_btn = tk.Label(self.controls_frame, text="", fg=self.text_main_color, **btn_base)
        self.reset_btn.pack(side='left', padx=1)
        self.reset_btn.bind("<Button-1>", self.reset)
        
        self.close_btn = tk.Label(self.controls_frame, text="", fg="#c42b1c", **btn_base)
        self.close_btn.pack(side='left', padx=1)
        self.close_btn.bind("<Button-1>", lambda e: self.root.destroy())

        # Initial layout apply
        self.apply_layout()

    def cycle_compact_mode(self, event=None):
        self.play_pop_sound(700)
        self.compact_level = (self.compact_level + 1) % len(self.MODES)
        self.apply_layout()

    def apply_layout(self):
        config = self.MODES[self.compact_level]
        self.width = config["width"]
        self.height = config["height"]
        self.show_pet = config["show_pet"]
        self.show_task = config["show_task"]
        
        # Update Window Size
        curr_x = self.root.winfo_x()
        curr_y = self.root.winfo_y()
        self.root.geometry(f"{self.width}x{self.height}+{curr_x}+{curr_y}")
        self.canvas.config(width=self.width, height=self.height)
        
        # Redraw Canvas Elements
        self.canvas.delete("all")
        self.border_segments = []
        
        # Re-Setup UI elements that were on canvas
        self.draw_smooth_rounded_rect(0, 0, self.width, self.height, self.corner_radius + 1, 
                                     fill="#000000", outline="", stipple="gray25")
        self.draw_smooth_rounded_rect(1, 1, self.width-1, self.height-1, self.corner_radius, 
                                     fill=self.bg_color, outline="")

        self.create_gradient_border()
        self.draw_smooth_rounded_rect(self.border_width + 1, self.border_width + 1, 
                                     self.width-self.border_width-1, self.height-self.border_width-1, 
                                     self.corner_radius - 2, fill="", outline="#f5f5f5", width=1)

        self.left_info = self.canvas.create_text(10, 8, text="", anchor="nw",
                                                font=('Segoe UI Semibold', 6), fill=self.text_dim_color)
        if self.show_task:
            self.canvas.itemconfig(self.left_info, text=self.task_name)
            self.canvas.tag_bind(self.left_info, "<Button-1>", self.change_task)
        else:
            self.canvas.itemconfig(self.left_info, state='hidden')

        self.mode_indicator = self.canvas.create_text(self.width - 10, 8, text="" if self.mode=="stopwatch" else "", 
                                                     anchor="ne", font=('Segoe UI', 6), fill=self.text_dim_color)

        time_x = self.width / 2 - (10 if config["ms_font"] > 0 else 0)
        time_y = self.height / 2 + 1 

        self.time_text = self.canvas.create_text(time_x, time_y - 4, text="00:00:00", 
                                               font=('Segoe UI Variable Display', config["time_font"], 'bold'), 
                                               fill=self.text_main_color, tags="main_ui")
        
        if config["ms_font"] > 0:
            self.ms_text = self.canvas.create_text(time_x + (30 + config["time_font"]*1.5), time_y - 1, text=".00", 
                                                 font=('Segoe UI Variable Display', config["ms_font"]), 
                                                 fill=self.task_text_color, tags="main_ui")
        else:
            self.ms_text = self.canvas.create_text(0, 0, text="", state='hidden', tags="main_ui")
        
        # Focus Pet
        self.pet_glow = self.canvas.create_oval(15, self.height-28, 35, self.height-8, 
                                               fill=self.task_text_color, outline="", 
                                               state='hidden', tags="main_ui")
        self.pet_id = self.canvas.create_text(25, self.height - 18, text="🐱", 
                                             font=('Segoe UI Emoji', 12 if self.compact_level > 0 else 14), tags="main_ui")
        
        if not self.show_pet:
            self.canvas.itemconfig(self.pet_id, state='hidden')

        # Motivation Text Placeholder
        self.motivation_text = self.canvas.create_text(self.width/2 + 10, self.height - 15, 
                                                      text="", font=('Segoe UI Semibold', 6), 
                                                      fill=self.bg_color, state='hidden', tags="main_ui")

        # Repack buttons in correct order
        for slave in self.controls_frame.pack_slaves():
            slave.pack_forget()
            
        self.play_btn.pack(side='left', padx=1)
        if self.compact_level < 2:
            if self.mode == "timer":
                self.add_btn.pack(side='left', padx=1)
            self.task_edit_btn.pack(side='left', padx=1)
            self.task_toggle_btn.pack(side='left', padx=1)
            self.theme_btn.pack(side='left', padx=1)
            self.mode_btn.pack(side='left', padx=1)
        
        self.compact_btn.pack(side='left', padx=1)
        self.reset_btn.pack(side='left', padx=1)
        self.close_btn.pack(side='left', padx=1)

        self.update_display(self.elapsed_time)
        self.controls_visible = False

    def next_theme(self, event=None):
        themes = list(self.THEMES.keys())
        idx = themes.index(self.active_theme)
        next_idx = (idx + 1) % len(themes)
        self.apply_theme(themes[next_idx])

    def switch_mode(self, event=None):
        if self.running: return
        if self.mode == "stopwatch":
            self.mode = "timer"
            dur_str = simpledialog.askstring("Timer", "Enter minutes:", parent=self.root)
            try:
                mins = float(dur_str) if dur_str else 0
                self.timer_duration = int(mins * 60)
                self.original_duration = self.timer_duration
                self.elapsed_time = self.timer_duration
                self.update_display(self.elapsed_time)
            except:
                self.mode = "stopwatch" # Revert
                return
        else:
            self.mode = "stopwatch"
            self.elapsed_time = 0
            self.last_minute = 0
            self.intensity_level = 0
            self.update_display(0)
        
        self.apply_layout()

    def add_minute(self, event=None):
        if self.mode == "timer":
            self.elapsed_time += 60
            if self.running:
                self.start_time += 60
            self.update_display(self.elapsed_time)
            self.canvas.itemconfig(self.time_text, fill=self.timer_add_color)
            self.canvas.itemconfig(self.ms_text, fill=self.timer_add_color)
            self.root.after(300, lambda: self.reset_text_colors())

    def reset_text_colors(self):
        if not self.is_blinking:
            self.canvas.itemconfig(self.time_text, fill=self.text_main_color)
            self.canvas.itemconfig(self.ms_text, fill=self.task_text_color)

    def create_gradient_border(self):
        # Already handled segments cleanup in apply_layout by canvas.delete("all")
        x1, y1, x2, y2 = 1, 1, self.width-1, self.height-1
        r = self.corner_radius
        def get_arc_points(cx, cy, r, start, end):
            return [(cx + r * math.cos(math.radians(a)), cy + r * math.sin(math.radians(a))) 
                    for a in range(start, end + 1, 2)]
        path = []
        path.extend(get_arc_points(x2-r, y1+r, r, 270, 360))
        path.append((x2, y2-r))
        path.extend(get_arc_points(x2-r, y2-r, r, 0, 90))
        path.append((x1+r, y2))
        path.extend(get_arc_points(x1+r, y2-r, r, 90, 180))
        path.append((x1, y1+r))
        path.extend(get_arc_points(x1+r, y1+r, r, 180, 270))
        path.append(path[0])
        for i in range(len(path) - 1):
            seg = self.canvas.create_line(path[i][0], path[i][1], path[i+1][0], path[i+1][1], 
                                          width=self.border_width, fill="#e0e0e0", capstyle="round")
            self.border_segments.append(seg)

    def draw_smooth_rounded_rect(self, x1, y1, x2, y2, r, **kwargs):
        def get_arc(cx, cy, r, start, end):
            pts = []
            for i in range(start, end + 1, 5):
                a = math.radians(i)
                pts.extend([cx + r * math.cos(a), cy + r * math.sin(a)])
            return pts
        points = []
        points.extend(get_arc(x2-r, y1+r, r, 270, 360))
        points.extend(get_arc(x2-r, y2-r, r, 0, 90))
        points.extend(get_arc(x1+r, y2-r, r, 90, 180))
        points.extend(get_arc(x1+r, y1+r, r, 180, 270))
        return self.canvas.create_polygon(points, **kwargs)

    def change_task(self, event=None):
        self.play_pop_sound(800)
        new_task = simpledialog.askstring("Task", "Change Task Name:", parent=self.root)
        if new_task:
            self.task_name = new_task.upper()
            self.show_task = True
            self.canvas.itemconfig(self.left_info, text=self.task_name, state='normal')

    def toggle_task_visibility(self, event=None):
        self.play_pop_sound(600)
        self.show_task = not self.show_task
        if self.show_task:
            self.canvas.itemconfig(self.left_info, text=self.task_name, state='normal')
        else:
            self.canvas.itemconfig(self.left_info, state='hidden')

    def rotate_border(self):
        if self.running:
            self.rotation_index = (self.rotation_index + 1) % len(self.palette)
            
            # Lava Lamp Effect: Sine-wave based color shifting
            if self.lava_lamp_mode:
                t = time.time() * 2
                for i, seg in enumerate(self.border_segments):
                    # Fluid movement based on time and segment position
                    shift = math.sin(t + i * 0.2) * 0.5 + 0.5
                    color_idx = int((i + self.rotation_index * shift) % len(self.palette))
                    self.canvas.itemconfig(seg, fill=self.palette[color_idx])
            else:
                for i, seg in enumerate(self.border_segments):
                    color_idx = (i + self.rotation_index) % len(self.palette)
                    self.canvas.itemconfig(seg, fill=self.palette[color_idx])
            
            self.update_pet_animation()
            self.root.after(self.rotation_speed, self.rotate_border)
        else:
            self.canvas.itemconfig(self.pet_id, text=self.pet_emojis["sleep"])
            if self.mode == "timer" and self.elapsed_time <= 0:
                current_fill = self.canvas.itemcget(self.border_segments[0], "fill")
                new_fill = self.timer_alert_color if current_fill != self.timer_alert_color else "#e0e0e0"
                for seg in self.border_segments: self.canvas.itemconfig(seg, fill=new_fill)
                self.root.after(500, self.rotate_border)
            else:
                for seg in self.border_segments: self.canvas.itemconfig(seg, fill="#e0e0e0")

    def on_enter(self, event):
        self.root.attributes("-alpha", self.hover_opacity)
        if not self.controls_visible:
            # Blur/Dim Effect for Background
            self.canvas.create_rectangle(0, 0, self.width, self.height, 
                                        fill="#000000", stipple="gray50", tags="blur_dim")
            
            if self.left_info: self.canvas.itemconfig(self.left_info, state='hidden')
            self.canvas.itemconfig(self.mode_indicator, state='hidden')
            
            # Position controls dynamically
            self.controls_window = self.canvas.create_window(self.width/2, self.height/2 + 2, 
                                                            window=self.controls_frame, tags="controls")
            self.controls_visible = True

    def on_leave(self, event):
        x, y = self.root.winfo_pointerxy()
        wx = self.root.winfo_rootx()
        wy = self.root.winfo_rooty()
        if not (wx <= x <= wx + self.width and wy <= y <= wy + self.height):
            self.root.attributes("-alpha", self.idle_opacity)
            self.canvas.delete("controls")
            self.canvas.delete("blur_dim")
            if self.left_info: self.canvas.itemconfig(self.left_info, state='normal')
            self.canvas.itemconfig(self.mode_indicator, state='normal')
            self.controls_visible = False

    def start_move(self, event):
        self.x = event.x
        self.y = event.y

    def do_move(self, event):
        # Smoother dragging using pointer coordinates to prevent "jumping"
        x = self.root.winfo_pointerx() - self.x
        y = self.root.winfo_pointery() - self.y
        self.root.geometry(f"+{x}+{y}")
        self.root.update_idletasks()

    def toggle(self, event=None):
        self.play_pop_sound()
        if self.running:
            self.running = False
            self.play_btn.config(text="")
            self.reset_text_colors()
            self.canvas.itemconfig(self.pet_id, text=self.pet_emojis["sleep"])
        else:
            self.running = True
            if self.mode == "stopwatch":
                self.start_time = time.time() - self.elapsed_time
                self.last_minute = int(self.elapsed_time // 60)
            else:
                self.start_time = time.time() + self.elapsed_time
                self.last_minute = int(abs(self.elapsed_time) // 60)
            self.play_btn.config(text="")
            self.canvas.itemconfig(self.pet_id, text=self.pet_emojis["work"])
            self.update_timer()
            self.rotate_border()

    def update_timer(self):
        if self.running:
            if self.mode == "stopwatch":
                self.elapsed_time = time.time() - self.start_time
                mins_worked = self.elapsed_time / 60
                curr_min = int(self.elapsed_time // 60)
                
                self.update_intensity(mins_worked)
                
                if curr_min > self.last_minute:
                    self.last_minute = curr_min
                    self.on_minute_passed()
                
                self.update_display(self.elapsed_time)
                self.root.after(20, self.update_timer)
            else:
                remaining = self.start_time - time.time()
                self.elapsed_time = remaining
                
                # Intensity based on total time worked in this session
                if remaining > 0:
                    mins_worked = (self.original_duration - remaining) / 60
                else:
                    mins_worked = (self.original_duration + abs(remaining)) / 60
                
                self.update_intensity(mins_worked)
                
                curr_min = int(abs(remaining) // 60)
                if curr_min != self.last_minute:
                    self.last_minute = curr_min
                    self.on_minute_passed()
                
                self.update_display(remaining)
                
                if remaining < 0:
                    # Overtime Mode: Pulse between Red and Black
                    if int(remaining * 2) % 2 == 0:
                        self.canvas.itemconfig(self.time_text, fill="#ff0000") # Red
                    else:
                        self.canvas.itemconfig(self.time_text, fill="#000000") # Black
                elif remaining < 120:
                    if int(remaining * 2) % 2 == 0:
                        self.canvas.itemconfig(self.time_text, fill=self.timer_warn_color)
                    else:
                        self.canvas.itemconfig(self.time_text, fill=self.text_main_color)
                else:
                    self.reset_text_colors()
                
                self.root.after(20, self.update_timer)

    def update_intensity(self, mins):
        if not self.enable_intensity: return
        new_level = 0
        if mins >= 50: new_level = 2   # Intense (Red/Orange)
        elif mins >= 25: new_level = 1 # Focused (Yellow/Green)
        
        if new_level != self.intensity_level:
            self.intensity_level = new_level
            if new_level == 1:
                self.palette = self.generate_palette("#f1c40f", "#27ae60", 45)
            elif new_level == 2:
                self.palette = self.generate_palette("#e67e22", "#e74c3c", 45)
            else:
                self.palette = self.generate_palette(self.gradient_start, self.gradient_end, 45)

    def on_minute_passed(self):
        if self.stopwatch_minute_blink:
            self.run_blink_sequence(self.stopwatch_blink_count * 2)
        if self.enable_motivation:
            self.trigger_motivation()
        self.play_pop_sound(800)

    def play_pop_sound(self, freq=600):
        if not self.enable_sounds: return
        try:
            winsound.Beep(freq, 50)
        except:
            pass

    def update_pet_animation(self):
        if not self.show_pet:
            self.canvas.itemconfig(self.pet_id, state='hidden')
            self.canvas.itemconfig(self.pet_glow, state='hidden')
            return
            
        self.canvas.itemconfig(self.pet_id, state='normal')
        if self.running:
            # Determine emoji based on intensity
            emoji = self.pet_emojis["work"]
            glow_color = self.gradient_start
            if self.intensity_level == 1: 
                emoji = self.pet_emojis["intense"]
                glow_color = "#ff9f43"
            elif self.intensity_level == 2: 
                emoji = "🔥"
                glow_color = "#ff4757"
            
            self.canvas.itemconfig(self.pet_glow, state='normal', fill=glow_color)
            
            # Simple bounce animation
            self.pet_bounce = (self.pet_bounce + 1) % 10
            y_offset = -2 if self.pet_bounce > 5 else 0
            self.canvas.coords(self.pet_id, 25, self.height - 18 + y_offset)
            self.canvas.coords(self.pet_glow, 15, self.height-28+y_offset, 35, self.height-8+y_offset)
            self.canvas.itemconfig(self.pet_id, text=emoji)
        else:
            self.canvas.itemconfig(self.pet_id, text=self.pet_emojis["sleep"])
            self.canvas.itemconfig(self.pet_glow, state='hidden')

    def trigger_motivation(self):
        if self.motivation_active: return
        self.motivation_active = True
        quote = self.MOTIVATIONS[self.motivation_index]
        self.motivation_index = (self.motivation_index + 1) % len(self.MOTIVATIONS)
        
        self.canvas.itemconfig(self.motivation_text, text=quote, state='normal')
        self.animate_motivation(0, 0)

    def animate_motivation(self, color_idx, step):
        # step: 0 to 100 (percentage of fade)
        if not self.running and self.mode == "stopwatch": 
            self.canvas.itemconfig(self.motivation_text, state='hidden')
            self.motivation_active = False
            return
            
        if step <= 100:
            # Rainbow cycling
            current_rainbow = self.rainbow_colors[color_idx % len(self.rainbow_colors)]
            
            # Fade calculation (towards bg_color)
            fade_factor = step / 100.0
            display_color = self.interpolate_color(current_rainbow, self.bg_color, fade_factor)
            
            self.canvas.itemconfig(self.motivation_text, fill=display_color)
            
            # Slow down the fade, speed up the rainbow
            self.root.after(50, lambda: self.animate_motivation(color_idx + 1, step + 1))
        else:
            self.canvas.itemconfig(self.motivation_text, state='hidden')
            self.motivation_active = False

    def interpolate_color(self, c1, c2, t):
        r1, g1, b1 = self.hex_to_rgb(c1)
        r2, g2, b2 = self.hex_to_rgb(c2)
        r = int(r1 + (r2 - r1) * t)
        g = int(g1 + (g2 - g1) * t)
        b = int(b1 + (b2 - b1) * t)
        return f'#{r:02x}{g:02x}{b:02x}'

    def run_blink_sequence(self, steps):
        if steps > 0:
            self.is_blinking = True
            color = self.stopwatch_blink_color if steps % 2 != 0 else self.text_main_color
            self.canvas.itemconfig(self.time_text, fill=color)
            self.root.after(300, lambda: self.run_blink_sequence(steps - 1))
        else:
            self.is_blinking = False
            self.reset_text_colors()

    def update_display(self, total_seconds):
        is_negative = total_seconds < 0
        abs_seconds = abs(total_seconds)
        hours = int(abs_seconds // 3600)
        minutes = int((abs_seconds % 3600) // 60)
        seconds = int(abs_seconds % 60)
        ms = int((abs_seconds % 1) * 100)
        
        prefix = "-" if is_negative else ""
        self.canvas.itemconfig(self.time_text, text=f"{prefix}{hours:02}:{minutes:02}:{seconds:02}")
        self.canvas.itemconfig(self.ms_text, text=f".{ms:02}")

    def reset(self, event=None):
        self.running = False
        if self.mode == "stopwatch":
            self.elapsed_time = 0
            self.last_minute = 0
            self.intensity_level = 0
            self.palette = self.generate_palette(self.gradient_start, self.gradient_end, 45)
        else:
            self.elapsed_time = self.timer_duration
            self.last_minute = int(abs(self.timer_duration) // 60)
        self.update_display(self.elapsed_time)
        self.play_btn.config(text="")
        self.reset_text_colors()
        self.rotate_border()

    def run(self):
        self.root.mainloop()

if __name__ == "__main__":
    THEME_MAP = {
        "1": "Windows 11", "2": "Cyberpunk", "3": "Midnight", "4": "Rose Gold",
        "5": "Nord", "6": "Forest", "7": "Dracula", "8": "Sunset"
    }
    
    parser = argparse.ArgumentParser(description='Stopwatch Pro')
    parser.add_argument('--theme', type=str, help='Theme name or number (1-8)')
    parser.add_argument('-r', '--random', action='store_true', help='Pick a random theme from presets')
    parser.add_argument('-tg', '--generate', action='store_true', help='Generate a unique random theme')
    args, unknown = parser.parse_known_args()
    
    theme = None
    if args.generate:
        def r_val(low=0, high=255): return random.randint(low, high)
        # 1. Generate Background
        br, bg, bb = r_val(20, 235), r_val(20, 235), r_val(20, 235)
        bg_hex = f"#{br:02x}{bg:02x}{bb:02x}"
        
        # 2. Calculate Luminance (Contrast Check)
        lum = (br * 299 + bg * 587 + bb * 114) / 1000
        is_dark = lum < 128
        
        # 3. Generate Contrasting Text/Accents
        if is_dark:
            tr, tg, tb = r_val(200, 255), r_val(200, 255), r_val(200, 255) # Light Main Text
            dr, dg, db = r_val(100, 150), r_val(100, 150), r_val(100, 150) # Dimmed Text
            ar, ag, ab = r_val(180, 255), r_val(150, 220), r_val(150, 220) # Bright Accent
        else:
            tr, tg, tb = r_val(0, 50), r_val(0, 50), r_val(0, 50)       # Dark Main Text
            dr, dg, db = r_val(60, 100), r_val(60, 100), r_val(60, 100) # Dimmed Text
            ar, ag, ab = r_val(0, 100), r_val(0, 100), r_val(0, 100)    # Deep Accent
            
        theme = {
            "bg": bg_hex, 
            "text": f"#{tr:02x}{tg:02x}{tb:02x}", 
            "dim": f"#{dr:02x}{dg:02x}{db:02x}", 
            "accent": f"#{ar:02x}{ag:02x}{ab:02x}", 
            "grad_s": f"#{r_val(50, 255):02x}{r_val(50, 255):02x}{r_val(50, 255):02x}", 
            "grad_e": f"#{r_val(50, 255):02x}{r_val(50, 255):02x}{r_val(50, 255):02x}"
        }
    elif args.random:
        theme = random.choice(list(THEME_MAP.values()))
    elif args.theme:
        theme = THEME_MAP.get(args.theme, args.theme)
        
    app = CompactStopwatch(selected_theme=theme)
    app.run()
