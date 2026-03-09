import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.animation import FuncAnimation, PillowWriter
import numpy as np

ROWS = 10
COLS = 10

# -----------------------------
# Utilities
# -----------------------------
def generate_zigzag_path(rows, cols, start_from_left=True):
    path = []
    for r in range(rows):
        if (r % 2 == 0 and start_from_left) or (r % 2 == 1 and not start_from_left):
            for c in range(cols):
                path.append((r, c))
        else:
            for c in reversed(range(cols)):
                path.append((r, c))
    return path


def clamp_index(i, n):
    return max(0, min(i, n - 1))


# -----------------------------
# Paths
# -----------------------------
# Zigzag traversal for teams
path_f1 = generate_zigzag_path(ROWS, COLS, start_from_left=True)
path_f2 = generate_zigzag_path(ROWS, COLS, start_from_left=False)
path_g = generate_zigzag_path(ROWS, COLS, start_from_left=True)

# DragonLord path (short custom movement)
path_dl = [
    (5, 5), (5, 6), (5, 6), (5, 6), (5, 6),
    (5, 5), (5, 4), (5, 4), (5, 4), (5, 4)
]

TOTAL_TURNS = 12

# Pick spaced frames from zigzag paths
indices_f1 = np.linspace(0, len(path_f1) - 1, TOTAL_TURNS, dtype=int)
indices_f2 = np.linspace(5, len(path_f2) - 1, TOTAL_TURNS, dtype=int)
indices_g = np.linspace(15, len(path_g) - 1, TOTAL_TURNS, dtype=int)

# -----------------------------
# Event schedule
# -----------------------------
event_schedule = {
    2: {"spawn": (3, 6), "event": "DragonLord spawns SmartDragon"},
    4: {"battle": True, "event": "FlyTeam1 engages SmartDragon"},
    6: {"trap": 3, "event": "GroundTeam traps DragonLord"},
    8: {"spawn": (7, 3), "event": "A new SmartDragon appears"},
    9: {"battle": True, "event": "FlyTeam2 engages SmartDragon"},
}

# -----------------------------
# Simulation state per turn
# -----------------------------
steps = []
hp_state = {"F1": 300, "F2": 300, "G": 420, "DL": 900}
active_dragons = []
spawn_counter = 0
trap_remaining = 0

for t in range(TOTAL_TURNS):
    f1 = path_f1[indices_f1[t]]
    f2 = path_f2[indices_f2[t]]
    g = path_g[indices_g[t]]
    dl = path_dl[clamp_index(t, len(path_dl))]

    event = "Traversal phase"
    spawn_pos = None
    battle = None
    trap = False
    flash = False

    if t in event_schedule:
        info = event_schedule[t]
        event = info["event"]

        if "spawn" in info:
            spawn_pos = info["spawn"]
            active_dragons.append(spawn_pos)
            spawn_counter += 1

        if "battle" in info and active_dragons:
            target = active_dragons[0]
            if t % 2 == 0:
                battle = (f1, target)
                hp_state["F1"] -= 20
            else:
                battle = (f2, target)
                hp_state["F2"] -= 15

            hp_state["DL"] -= 40
            flash = True
            active_dragons.pop(0)

        if "trap" in info:
            trap_remaining = info["trap"]
            trap = True

    if trap_remaining > 0:
        trap = True
        trap_remaining -= 1

    entities = {
        "F1": f1,
        "F2": f2,
        "G": g,
        "DL": dl,
    }

    # Only show one active SmartDragon visually for simplicity
    if active_dragons:
        entities["SD"] = active_dragons[0]

    steps.append(
        {
            "turn": t + 1,
            "entities": entities,
            "event": event,
            "battle": battle,
            "spawn": spawn_pos,
            "trap": trap,
            "flash": flash,
            "hp": hp_state.copy(),
            "spawn_counter": spawn_counter,
            "progress": (t + 1) / TOTAL_TURNS,
        }
    )

# -----------------------------
# Visual config
# -----------------------------
colors = {
    "F1": "#2563eb",
    "F2": "#06b6d4",
    "G": "#16a34a",
    "DL": "#dc2626",
    "SD": "#f59e0b",
}

labels = {
    "F1": "F1",
    "F2": "F2",
    "G": "G",
    "DL": "DL",
    "SD": "SD",
}

# Heatmap weights for cells
heatmap = np.zeros((ROWS, COLS))
for pos in path_f1[:40]:
    heatmap[pos[0], pos[1]] += 1
for pos in path_f2[:40]:
    heatmap[pos[0], pos[1]] += 1
for pos in path_g[:40]:
    heatmap[pos[0], pos[1]] += 1

max_heat = heatmap.max() if heatmap.max() > 0 else 1

fig = plt.figure(figsize=(13, 8))
ax = fig.add_axes([0.05, 0.10, 0.55, 0.80])       # main map
mini_ax = fig.add_axes([0.78, 0.60, 0.16, 0.22])

# -----------------------------
# Draw helpers
# -----------------------------
def draw_grid(ax):
    ax.set_xlim(0, COLS)
    ax.set_ylim(0, ROWS)
    ax.set_aspect("equal")
    ax.invert_yaxis()
    ax.set_xticks(range(COLS + 1))
    ax.set_yticks(range(ROWS + 1))
    ax.grid(True, linewidth=1)
    ax.set_xticklabels([])
    ax.set_yticklabels([])
    ax.tick_params(length=0)


def draw_heat_cells(ax, current_entities):
    for r in range(ROWS):
        for c in range(COLS):
            intensity = heatmap[r, c] / max_heat
            face = (0.92 - 0.22 * intensity, 0.96 - 0.18 * intensity, 1.0 - 0.05 * intensity)
            rect = patches.Rectangle((c, r), 1, 1, edgecolor="#cbd5e1", facecolor=face, linewidth=0.8)
            ax.add_patch(rect)

    # highlight current entity cells
    for _, pos in current_entities.items():
        r, c = pos
        rect = patches.Rectangle((c, r), 1, 1, fill=False, edgecolor="#111827", linewidth=2)
        ax.add_patch(rect)


def draw_entity(ax, name, pos):
    r, c = pos
    circle = patches.Circle((c + 0.5, r + 0.5), 0.33, facecolor=colors[name], edgecolor="black", linewidth=1.5)
    ax.add_patch(circle)
    ax.text(
        c + 0.5, r + 0.5, labels[name],
        color="white", ha="center", va="center",
        fontsize=10, fontweight="bold"
    )


def draw_spawn_effect(ax, pos):
    r, c = pos
    ring = patches.Circle((c + 0.5, r + 0.5), 0.48, fill=False, edgecolor="#f59e0b", linewidth=3, linestyle="--")
    ax.add_patch(ring)
    ax.text(
        c + 0.5, r + 0.12, "SPAWN",
        color="#92400e", ha="center", va="center",
        fontsize=9, fontweight="bold"
    )


def draw_battle_effect(ax, p1, p2, flash=False):
    r1, c1 = p1
    r2, c2 = p2
    color = "#ef4444" if flash else "#7f1d1d"
    ax.plot([c1 + 0.5, c2 + 0.5], [r1 + 0.5, r2 + 0.5], linewidth=3, linestyle="--", color=color)
    ax.text(
        (c1 + c2) / 2 + 0.5,
        (r1 + r2) / 2 + 0.25,
        "BATTLE",
        color=color,
        fontsize=10,
        fontweight="bold",
        ha="center",
        bbox=dict(boxstyle="round", facecolor="white", alpha=0.9)
    )


def draw_trap_effect(ax, dl_pos):
    r, c = dl_pos
    rect = patches.Rectangle((c + 0.1, r + 0.1), 0.8, 0.8, fill=False, edgecolor="#7c3aed", linewidth=3)
    ax.add_patch(rect)
    ax.text(
        c + 0.5, r + 0.10, "TRAP",
        color="#6d28d9",
        ha="center", va="center",
        fontsize=9, fontweight="bold"
    )


def draw_mini_heatmap(mini_ax, current_entities):
    mini_ax.clear()
    mini_ax.imshow(heatmap, cmap="Blues", origin="upper")
    mini_ax.set_title("Mini Heatmap", fontsize=10, pad=6)
    mini_ax.set_xticks([])
    mini_ax.set_yticks([])

    for name, pos in current_entities.items():
        r, c = pos
        mini_ax.scatter(c, r, s=40, c=colors.get(name, "black"), edgecolors="black")


def draw_progress_bar(fig, progress):
    x = 0.64
    y = 0.88
    width = 0.30
    height = 0.02

    bg = patches.Rectangle(
        (x, y), width, height,
        transform=fig.transFigure,
        facecolor="#e5e7eb",
        edgecolor="none"
    )
    fg = patches.Rectangle(
        (x, y), width * progress, height,
        transform=fig.transFigure,
        facecolor="#2563eb",
        edgecolor="none"
    )

    fig.add_artist(bg)
    fig.add_artist(fg)
    fig.text(x, y + 0.03, f"Mission Progress: {int(progress * 100)}%", fontsize=10, fontweight="bold")

def draw_team_health(fig, hp):

    text = (
        "TEAM HEALTH\n"
        f"F1 : {hp['F1']}\n"
        f"F2 : {hp['F2']}\n"
        f"G  : {hp['G']}\n"
        f"DL : {hp['DL']}"
    )

    fig.text(
        0.64,
        0.78,
        text,
        fontsize=11,
        va="top",
        bbox=dict(boxstyle="round", facecolor="white", alpha=0.95)
    )


def draw_info_boxes(fig, step):
    turn = step["turn"]
    event = step["event"]
    spawn_counter = step["spawn_counter"]
    trap_text = "Active" if step["trap"] else "Inactive"

    event_text = (
        f"TURN INFO\n"
        f"Turn: {turn}/{TOTAL_TURNS}\n"
        f"Spawn Counter: {spawn_counter}\n"
        f"Trap Status: {trap_text}\n"
        f"Traversal: Zigzag\n"
        f"Event: {event}"
    )

    legend_text = (
        "LEGEND\n"
        "F1 = FlyTeam1\n"
        "F2 = FlyTeam2\n"
        "G  = GroundTeam\n"
        "DL = DragonLord\n"
        "SD = SmartDragon"
    )

    fig.text(
        0.64, 0.48, event_text,
        fontsize=10, va="top",
        bbox=dict(boxstyle="round", facecolor="white", alpha=0.95)
    )

    fig.text(
        0.64, 0.27, legend_text,
        fontsize=10, va="top",
        bbox=dict(boxstyle="round", facecolor="white", alpha=0.95)
    )


# -----------------------------
# Animation update
# -----------------------------
def update(frame):
    fig.clear()

    ax = fig.add_axes([0.05, 0.10, 0.55, 0.80])
    mini_ax = fig.add_axes([0.78, 0.60, 0.16, 0.22])

    step = steps[frame]
    entities = step["entities"]

    draw_grid(ax)
    draw_heat_cells(ax, entities)

    if frame > 0:
        f1_positions = [steps[i]["entities"]["F1"] for i in range(frame + 1)]
        f2_positions = [steps[i]["entities"]["F2"] for i in range(frame + 1)]
        g_positions = [steps[i]["entities"]["G"] for i in range(frame + 1)]

        for positions, color in [
            (f1_positions, "#2563eb"),
            (f2_positions, "#06b6d4"),
            (g_positions, "#16a34a")
        ]:
            xs = [p[1] + 0.5 for p in positions]
            ys = [p[0] + 0.5 for p in positions]
            ax.plot(xs, ys, linewidth=2, alpha=0.5, color=color)

    for name, pos in entities.items():
        draw_entity(ax, name, pos)

    if step["spawn"] is not None:
        draw_spawn_effect(ax, step["spawn"])

    if step["battle"] is not None:
        draw_battle_effect(ax, step["battle"][0], step["battle"][1], flash=step["flash"])

    if step["trap"] and "DL" in entities:
        draw_trap_effect(ax, entities["DL"])

    ax.set_title(
        f"Dragon Warriors Simulation\nTurn {step['turn']}/{TOTAL_TURNS}",
        fontsize=14,
        fontweight="bold"
    )

    draw_mini_heatmap(mini_ax, entities)
    draw_progress_bar(fig, step["progress"])
    draw_team_health(fig, step["hp"])
    draw_info_boxes(fig, step)

anim = FuncAnimation(
    fig,
    update,
    frames=len(steps),
    interval=1200,
    repeat=False
)

anim.save("dragon_warriors_simulation.gif", writer=PillowWriter(fps=1))
plt.close(fig)

print("dragon_warriors_simulation.gif generated")