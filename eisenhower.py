# eisenhower.py
import sys, os, json, argparse, uuid
from datetime import datetime
try:
    from colorama import init, Fore, Style
    init()
    COLORS = True
except ImportError:
    COLORS = False
    Fore = Style = type('', (), {'RESET_ALL':'', 'RED':'', 'GREEN':'', 'YELLOW':'', 'CYAN':'', 'WHITE':'', 'BLACK':''})()

DATA_FILE = "tasks.json"

class Task:
    def __init__(self, title, description="", quadrant=1, task_id=None):
        self.id = task_id or str(uuid.uuid4())[:8]
        self.title = title
        self.description = description
        self.quadrant = quadrant  # 1-4
        self.created = datetime.now().isoformat()
        self.completed = False

    def to_dict(self):
        return {
            "id": self.id,
            "title": self.title,
            "description": self.description,
            "quadrant": self.quadrant,
            "created": self.created,
            "completed": self.completed
        }

    @classmethod
    def from_dict(cls, data):
        task = cls(data["title"], data.get("description", ""), data["quadrant"], data["id"])
        task.created = data.get("created", datetime.now().isoformat())
        task.completed = data.get("completed", False)
        return task

class EisenhowerApp:
    def __init__(self):
        self.tasks = []
        self.load()

    def load(self):
        if os.path.exists(DATA_FILE):
            with open(DATA_FILE, "r") as f:
                data = json.load(f)
                self.tasks = [Task.from_dict(t) for t in data]

    def save(self):
        with open(DATA_FILE, "w") as f:
            json.dump([t.to_dict() for t in self.tasks], f, indent=2)

    def add_task(self, title, desc="", quadrant=1):
        task = Task(title, desc, quadrant)
        self.tasks.append(task)
        self.save()
        print(f"Task added: {task.title} (ID: {task.id})")

    def delete_task(self, task_id):
        for i, t in enumerate(self.tasks):
            if t.id == task_id:
                del self.tasks[i]
                self.save()
                print(f"Task {task_id} deleted.")
                return
        print(f"Task {task_id} not found.")

    def move_task(self, task_id, new_quadrant):
        for t in self.tasks:
            if t.id == task_id:
                t.quadrant = new_quadrant
                self.save()
                print(f"Task {task_id} moved to quadrant {new_quadrant}.")
                return
        print(f"Task {task_id} not found.")

    def list_tasks(self, quadrant=None):
        filtered = [t for t in self.tasks if quadrant is None or t.quadrant == quadrant]
        if not filtered:
            print("No tasks found.")
            return
        quadrants = {
            1: ("Urgent & Important", Fore.RED),
            2: ("Not Urgent but Important", Fore.GREEN),
            3: ("Urgent but Not Important", Fore.YELLOW),
            4: ("Not Urgent & Not Important", Fore.WHITE)
        }
        print("\nEisenhower Matrix")
        print("="*60)
        for q in range(1,5):
            q_tasks = [t for t in filtered if t.quadrant == q]
            label, color = quadrants[q]
            if COLORS:
                print(f"{color}{label}{Style.RESET_ALL}")
            else:
                print(label)
            if q_tasks:
                for t in q_tasks:
                    print(f"  [{t.id}] {t.title} - {t.description or 'No description'}")
            else:
                print("  (empty)")
            if q < 4:
                print("-"*40)
        print("="*60)

    def stats(self):
        counts = {1:0,2:0,3:0,4:0}
        for t in self.tasks:
            counts[t.quadrant] += 1
        print("Eisenhower Statistics")
        for q, label in [(1,"Urgent & Important"), (2,"Not Urgent but Important"),
                         (3,"Urgent but Not Important"), (4,"Not Urgent & Not Important")]:
            print(f"{label}: {counts[q]} tasks")

def main():
    app = EisenhowerApp()
    parser = argparse.ArgumentParser(description="Eisenhower Matrix Todo List")
    subparsers = parser.add_subparsers(dest="command", required=True)

    add_parser = subparsers.add_parser("add")
    add_parser.add_argument("-t", "--title", required=True)
    add_parser.add_argument("-d", "--desc", default="")
    add_parser.add_argument("-q", "--quadrant", type=int, choices=[1,2,3,4], default=1)

    list_parser = subparsers.add_parser("list")
    list_parser.add_argument("-q", "--quadrant", type=int, choices=[1,2,3,4])

    move_parser = subparsers.add_parser("move")
    move_parser.add_argument("--id", required=True)
    move_parser.add_argument("-q", "--quadrant", type=int, choices=[1,2,3,4], required=True)

    delete_parser = subparsers.add_parser("delete")
    delete_parser.add_argument("--id", required=True)

    stats_parser = subparsers.add_parser("stats")

    args = parser.parse_args()
    if args.command == "add":
        app.add_task(args.title, args.desc, args.quadrant)
    elif args.command == "list":
        app.list_tasks(args.quadrant)
    elif args.command == "move":
        app.move_task(args.id, args.quadrant)
    elif args.command == "delete":
        app.delete_task(args.id)
    elif args.command == "stats":
        app.stats()

if __name__ == "__main__":
    main()
