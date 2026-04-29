import customtkinter as ctk
import time
import math
import random
import winsound
import argparse
import sys
from tkinter import simpledialog, messagebox

# Set appearance mode and color theme
ctk.set_appearance_mode("Dark")
ctk.set_default_color_theme("blue")

class CompactStopwatchCTK:
    def __init__(self, selected_theme=None):
        self.root = ctk.CTk()
        self.root.title("Floating Stopwatch/Timer (CustomTkinter)")
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
        
        # --- ADJUSTABLE PARAMETERS ---
        self.active_theme = selected_theme if selected_theme else "Forest" 
        self.enable_intensity = True   
        self.lava_lamp_mode = True     
        self.show_pet = True           
        self.enable_motivation = True  
        self.enable_sounds = True      
        
        self.width = 200               
        self.height = 65               
        self.corner_radius = 15        
        
        self.idle_opacity = 0.85       
        self.hover_opacity = 1.0       
        
        # Internal State
        self.pet_emojis = {"idle": "🐱", "work": "😺", "intense": "🙀", "sleep": "😴"}
        self.pet_bounce = 0
        self.motivation_active = False
        
        self.show_labels = False       
        self.show_task = False         
        self.show_start_info = False   
        self.time_alignment = "center" 
        
        self.stopwatch_minute_blink = True  
        self.stopwatch_blink_count = 3       
        self.stopwatch_blink_color = "#ff4757" 
        
        self.timer_alert_color = "#ff4757" 
        self.timer_add_color = "#27ae60"   
        self.timer_warn_color = "#ff4757"  
        self.rotation_speed = 30           
        
        # Theme Init
        self.update_theme_vars()
        
        self.mode = "stopwatch"     
        self.task_name = "CURRENT TASK"
        self.rotation_index = 0
        self.border_segments = []
        self.is_blinking = False    
        self.intensity_level = 0    
        
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
        
        self.root.bind("<ButtonPress-1>", self.start_move)
        self.root.bind("<B1-Motion>", self.do_move)
        self.root.bind("<Enter>", self.on_enter)
        self.root.bind("<Leave>", self.on_leave)

    def update_theme_vars(self):
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

    def generate_palette(self, c1, c2, n):
        r1, g1, b1 = self.hex_to_rgb(c1)
        r2, g2, b2 = self.hex_to_rgb(c2)
        palette = []
        for i in range(n):
            t = i / (n - 1)
            r = int(r1 + (r2 - r1) * t)
            r = max(0, min(255, r))
            g = int(g1 + (g2 - g1) * t)
            g = max(0, min(255, g))
            b = int(b1 + (b2 - b1) * t)
            b = max(0, min(255, b))
            palette.append(f'#{r:02x}{g:02x}{b:02x}')
        return palette + palette[::-1]

    def hex_to_rgb(self, hex_color):
        hex_color = hex_color.lstrip('#')
        return tuple(int(hex_color[i:i+2], 16) for i in (0, 2, 4))

    def setup_ui(self):
        # Main Frame using CTK for rounded corners
        self.main_frame = ctk.CTkFrame(self.root, width=self.width, height=self.height, 
                                     corner_radius=self.corner_radius, fg_color=self.bg_color,
                                     border_width=0)
        self.main_frame.pack(fill="both", expand=True)

        # Canvas for the animated gradient border
        self.canvas = ctk.CTkCanvas(self.main_frame, width=self.width, height=self.height, 
                                   bg=self.bg_color, highlightthickness=0)
        self.canvas.place(x=0, y=0)

        self.create_gradient_border()

        self.left_info = self.canvas.create_text(12, 10, text="", anchor="nw",
                                                font=('Segoe UI Semibold', 7), fill=self.text_dim_color)
        if self.show_task:
            self.canvas.itemconfig(self.left_info, text=self.task_name)
            self.canvas.tag_bind(self.left_info, "<Button-1>", self.change_task)

        self.mode_indicator = self.canvas.create_text(self.width - 12, 10, text="", anchor="ne",
                                                    font=('Segoe UI', 7), fill=self.text_dim_color)

        time_x = self.width / 2 - 10
        time_y = self.height / 2 

        self.time_text = self.canvas.create_text(time_x, time_y - 2, text="00:00:00", 
                                               font=('Segoe UI Variable Display', 20, 'bold'), 
                                               fill=self.text_main_color, tags="main_ui")
        self.ms_text = self.canvas.create_text(time_x + 65, time_y + 1, text=".00", 
                                             font=('Segoe UI Variable Display', 11), 
                                             fill=self.task_text_color, tags="main_ui")
        
        self.pet_glow = self.canvas.create_oval(15, self.height-32, 35, self.height-12, 
                                               fill=self.task_text_color, outline="", 
                                               state='hidden', tags="main_ui")
        self.pet_id = self.canvas.create_text(25, self.height - 22, text="🐱", 
                                             font=('Segoe UI Emoji', 16), tags="main_ui")

        self.motivation_text = self.canvas.create_text(self.width/2 + 10, self.height - 18, 
                                                      text="", font=('Segoe UI Semibold', 8), 
                                                      fill=self.bg_color, state='hidden', tags="main_ui")

        # Controls UI
        self.controls_visible = False
        self.controls_frame = ctk.CTkFrame(self.main_frame, fg_color=self.bg_color, corner_radius=10)
        
        btn_font = ('Segoe UI', 12)
        btn_params = {"fg_color": "transparent", 
                     "hover_color": self.text_dim_color, "width": 30, "height": 30, 
                     "font": btn_font, "corner_radius": 6}

        self.play_btn = ctk.CTkButton(self.controls_frame, text="", command=self.toggle, 
                                     text_color=self.text_main_color, **btn_params) 
        self.play_btn.pack(side='left', padx=2)
        
        self.add_btn = ctk.CTkButton(self.controls_frame, text="+1", command=self.add_minute, 
                                    text_color=self.timer_add_color, **btn_params)
        
        self.theme_btn = ctk.CTkButton(self.controls_frame, text="", command=self.next_theme, 
                                      text_color=self.text_main_color, **btn_params)
        self.theme_btn.pack(side='left', padx=2)
        
        self.mode_btn = ctk.CTkButton(self.controls_frame, text="", command=self.switch_mode, 
                                     text_color=self.text_main_color, **btn_params) 
        self.mode_btn.pack(side='left', padx=2)
        
        self.reset_btn = ctk.CTkButton(self.controls_frame, text="", command=self.reset, 
                                      text_color=self.text_main_color, **btn_params)
        self.reset_btn.pack(side='left', padx=2)
        
        self.close_btn = ctk.CTkButton(self.controls_frame, text="", command=self.root.destroy, 
                                      text_color="#c42b1c", **btn_params)
        self.close_btn.pack(side='left', padx=2)

    def create_gradient_border(self):
        x1, y1, x2, y2 = 2, 2, self.width-2, self.height-2
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
        
        self.border_segments = []
        for i in range(len(path) - 1):
            seg = self.canvas.create_line(path[i][0], path[i][1], path[i+1][0], path[i+1][1], 
                                          width=self.border_width, fill="#e0e0e0", capstyle="round")
            self.border_segments.append(seg)

    def apply_theme(self, theme_name):
        if isinstance(theme_name, dict):
            self.active_theme = "Generated"
            theme = theme_name
        else:
            if theme_name not in self.THEMES: return
            self.active_theme = theme_name
            theme = self.THEMES[theme_name]
            
        self.update_theme_vars()
        self.intensity_level = 0 
        self.palette = self.generate_palette(self.gradient_start, self.gradient_end, 45)
        
        self.main_frame.configure(fg_color=self.bg_color)
        self.canvas.configure(bg=self.bg_color)
        self.canvas.itemconfig(self.left_info, fill=self.text_dim_color)
        self.canvas.itemconfig(self.mode_indicator, fill=self.text_dim_color)
        self.canvas.itemconfig(self.time_text, fill=self.text_main_color)
        self.canvas.itemconfig(self.ms_text, fill=self.task_text_color)
        
        self.controls_frame.configure(fg_color=self.bg_color)
        for child in self.controls_frame.winfo_children():
            if isinstance(child, ctk.CTkButton):
                child.configure(fg_color="transparent", text_color=self.text_main_color if child != self.close_btn and child != self.add_btn else child.cget("text_color"))

    def next_theme(self):
        themes = list(self.THEMES.keys())
        idx = themes.index(self.active_theme) if self.active_theme in themes else 0
        next_idx = (idx + 1) % len(themes)
        self.apply_theme(themes[next_idx])

    def switch_mode(self):
        if self.running: return
        if self.mode == "stopwatch":
            self.mode = "timer"
            self.mode_btn.configure(text="")
            self.canvas.itemconfig(self.mode_indicator, text="")
            self.add_btn.pack(side='left', padx=2, before=self.theme_btn)
            dur_str = simpledialog.askstring("Timer", "Enter minutes:", parent=self.root)
            try:
                mins = float(dur_str) if dur_str else 0
                self.timer_duration = int(mins * 60)
                self.original_duration = self.timer_duration
                self.elapsed_time = self.timer_duration
                self.update_display(self.elapsed_time)
            except:
                self.switch_mode()
        else:
            self.mode = "stopwatch"
            self.mode_btn.configure(text="")
            self.canvas.itemconfig(self.mode_indicator, text="")
            self.add_btn.pack_forget()
            self.elapsed_time = 0
            self.last_minute = 0
            self.intensity_level = 0
            self.update_display(0)

    def add_minute(self):
        if self.mode == "timer":
            self.elapsed_time += 60
            if self.running: self.start_time += 60
            self.update_display(self.elapsed_time)
            self.canvas.itemconfig(self.time_text, fill=self.timer_add_color)
            self.root.after(300, self.reset_text_colors)

    def reset_text_colors(self):
        if not self.is_blinking:
            self.canvas.itemconfig(self.time_text, fill=self.text_main_color)
            self.canvas.itemconfig(self.ms_text, fill=self.task_text_color)

    def rotate_border(self):
        if self.running:
            self.rotation_index = (self.rotation_index + 1) % len(self.palette)
            if self.lava_lamp_mode:
                t = time.time() * 2
                for i, seg in enumerate(self.border_segments):
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
            self.canvas.create_rectangle(0, 0, self.width, self.height, 
                                        fill="#000000", stipple="gray50", tags="blur_dim")
            self.canvas.itemconfig(self.left_info, state='hidden')
            self.canvas.itemconfig(self.mode_indicator, state='hidden')
            self.controls_window = self.canvas.create_window(self.width/2, self.height/2, 
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
            self.canvas.itemconfig(self.left_info, state='normal')
            self.canvas.itemconfig(self.mode_indicator, state='normal')
            self.controls_visible = False

    def start_move(self, event):
        self.x = event.x
        self.y = event.y

    def do_move(self, event):
        x = self.root.winfo_pointerx() - self.x
        y = self.root.winfo_pointery() - self.y
        self.root.geometry(f"+{x}+{y}")

    def toggle(self):
        self.play_pop_sound()
        if self.running:
            self.running = False
            self.play_btn.configure(text="")
            self.canvas.itemconfig(self.pet_id, text=self.pet_emojis["sleep"])
        else:
            self.running = True
            if self.mode == "stopwatch":
                self.start_time = time.time() - self.elapsed_time
            else:
                self.start_time = time.time() + self.elapsed_time
            self.play_btn.configure(text="")
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
            else:
                remaining = self.start_time - time.time()
                self.elapsed_time = remaining
                mins_worked = (self.original_duration - remaining) / 60 if remaining > 0 else (self.original_duration + abs(remaining)) / 60
                self.update_intensity(mins_worked)
                curr_min = int(abs(remaining) // 60)
                if curr_min != self.last_minute:
                    self.last_minute = curr_min
                    self.on_minute_passed()
                self.update_display(remaining)
                if remaining < 0:
                    self.canvas.itemconfig(self.time_text, fill="#ff0000" if int(remaining * 2) % 2 == 0 else "#000000")
                elif remaining < 120:
                    self.canvas.itemconfig(self.time_text, fill=self.timer_warn_color if int(remaining * 2) % 2 == 0 else self.text_main_color)
                else:
                    self.reset_text_colors()
            self.root.after(20, self.update_timer)

    def update_intensity(self, mins):
        if not self.enable_intensity: return
        new_level = 2 if mins >= 50 else (1 if mins >= 25 else 0)
        if new_level != self.intensity_level:
            self.intensity_level = new_level
            if new_level == 1: self.palette = self.generate_palette("#f1c40f", "#27ae60", 45)
            elif new_level == 2: self.palette = self.generate_palette("#e67e22", "#e74c3c", 45)
            else: self.palette = self.generate_palette(self.gradient_start, self.gradient_end, 45)

    def on_minute_passed(self):
        if self.stopwatch_minute_blink: self.run_blink_sequence(self.stopwatch_blink_count * 2)
        if self.enable_motivation: self.trigger_motivation()
        self.play_pop_sound(800)

    def play_pop_sound(self, freq=600):
        if self.enable_sounds:
            try: winsound.Beep(freq, 50)
            except: pass

    def update_pet_animation(self):
        if not self.show_pet:
            self.canvas.itemconfig(self.pet_id, state='hidden')
            self.canvas.itemconfig(self.pet_glow, state='hidden')
            return
        self.canvas.itemconfig(self.pet_id, state='normal')
        if self.running:
            emoji = self.pet_emojis["work"]
            glow_color = self.gradient_start
            if self.intensity_level == 1: emoji, glow_color = self.pet_emojis["intense"], "#ff9f43"
            elif self.intensity_level == 2: emoji, glow_color = "🔥", "#ff4757"
            self.canvas.itemconfig(self.pet_glow, state='normal', fill=glow_color)
            self.pet_bounce = (self.pet_bounce + 1) % 10
            y_offset = -2 if self.pet_bounce > 5 else 0
            self.canvas.coords(self.pet_id, 25, self.height - 22 + y_offset)
            self.canvas.coords(self.pet_glow, 15, self.height-32+y_offset, 35, self.height-12+y_offset)
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
        if not self.running and self.mode == "stopwatch": 
            self.canvas.itemconfig(self.motivation_text, state='hidden')
            self.motivation_active = False
            return
        if step <= 100:
            current_rainbow = self.rainbow_colors[color_idx % len(self.rainbow_colors)]
            display_color = self.interpolate_color(current_rainbow, self.bg_color, step / 100.0)
            self.canvas.itemconfig(self.motivation_text, fill=display_color)
            self.root.after(50, lambda: self.animate_motivation(color_idx + 1, step + 1))
        else:
            self.canvas.itemconfig(self.motivation_text, state='hidden')
            self.motivation_active = False

    def interpolate_color(self, c1, c2, t):
        r1, g1, b1 = self.hex_to_rgb(c1)
        r2, g2, b2 = self.hex_to_rgb(c2)
        return f'#{int(r1+(r2-r1)*t):02x}{int(g1+(g2-g1)*t):02x}{int(b1+(b2-b1)*t):02x}'

    def run_blink_sequence(self, steps):
        if steps > 0:
            self.is_blinking = True
            self.canvas.itemconfig(self.time_text, fill=self.stopwatch_blink_color if steps % 2 != 0 else self.text_main_color)
            self.root.after(300, lambda: self.run_blink_sequence(steps - 1))
        else:
            self.is_blinking = False
            self.reset_text_colors()

    def update_display(self, total_seconds):
        is_neg = total_seconds < 0
        s = abs(total_seconds)
        h, m, sec, ms = int(s//3600), int((s%3600)//60), int(s%60), int((s%1)*100)
        self.canvas.itemconfig(self.time_text, text=f"{'-' if is_neg else ''}{h:02}:{m:02}:{sec:02}")
        self.canvas.itemconfig(self.ms_text, text=f".{ms:02}")

    def reset(self):
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
        self.play_btn.configure(text="")
        self.reset_text_colors()
        self.rotate_border()

    def change_task(self, event=None):
        new_task = simpledialog.askstring("Task", "Change Task Name:", parent=self.root)
        if new_task:
            self.task_name = new_task.upper()
            self.canvas.itemconfig(self.left_info, text=self.task_name)

    def run(self):
        self.root.mainloop()

if __name__ == "__main__":
    THEME_MAP = {"1":"Windows 11", "2":"Cyberpunk", "3":"Midnight", "4":"Rose Gold", "5":"Nord", "6":"Forest", "7":"Dracula", "8":"Sunset"}
    parser = argparse.ArgumentParser()
    parser.add_argument('--theme', type=str)
    parser.add_argument('-r', '--random', action='store_true')
    args, _ = parser.parse_known_args()
    theme = None
    if args.random: theme = random.choice(list(THEME_MAP.values()))
    elif args.theme: theme = THEME_MAP.get(args.theme, args.theme)
    app = CompactStopwatchCTK(selected_theme=theme)
    app.run()
