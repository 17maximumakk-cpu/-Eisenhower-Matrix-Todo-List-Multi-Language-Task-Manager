✅ Eisenhower Matrix Todo List — Multi‑Language Task Manager
8 languages, one powerful task manager – organize your tasks by urgency and importance using the Eisenhower Matrix, right from your terminal.

✨ Features
📋 Add tasks with title, description (optional), and priority (urgent/important)

🔄 Move tasks between quadrants:

Q1: Urgent & Important (Do First)

Q2: Not Urgent but Important (Schedule)

Q3: Urgent but Not Important (Delegate)

Q4: Not Urgent & Not Important (Eliminate)

🗑️ Delete tasks by ID

📊 View all tasks with color‑coded quadrants (where supported)

💾 Persistent storage – tasks saved to a local file (tasks.json or similar)

🔍 Filter by quadrant or status

📈 Statistics – count per quadrant

🧰 Supported Languages
Language	File	Dependencies
Python	eisenhower.py	colorama (optional)
Go	eisenhower.go	none (standard library)
JavaScript (Node)	eisenhower.js	chalk, commander (or yargs)
Ruby	eisenhower.rb	colorize (optional)
PHP	eisenhower.php	none (extensions)
Java	Eisenhower.java	com.fasterxml.jackson (or built‑in JSON)
C#	Eisenhower.cs	System.Text.Json
C++	eisenhower.cpp	nlohmann/json
🚀 Quick Start
All implementations share a similar CLI interface:

bash
# Add a task
<program> add -t "Buy groceries" -d "Milk, eggs" -q 1
# q: 1=Urgent&Important, 2=Not Urgent&Important, 3=Urgent&Not Important, 4=Not Urgent&Not Important

# List all tasks
<program> list

# List tasks in a specific quadrant
<program> list -q 1

# Move a task to another quadrant
<program> move -id 3 -q 2

# Delete a task
<program> delete -id 5

# Show statistics
<program> stats
Options:

add – add a new task

list – show tasks (optionally filtered by quadrant)

move – change quadrant of a task

delete– remove a task

stats – display counts per quadrant

📸 Example Output
text
Eisenhower Matrix
╔═══════════════════════════════════════════════════════════════╗
║  URGENT & IMPORTANT          │  NOT URGENT BUT IMPORTANT     ║
║  [1] Buy groceries (milk)    │  [2] Plan vacation            ║
║  [3] Pay bills               │  [4] Exercise                 ║
║──────────────────────────────┼───────────────────────────────║
║  URGENT BUT NOT IMPORTANT    │  NOT URGENT & NOT IMPORTANT   ║
║  [5] Answer spam emails      │  [6] Watch TV                 ║
╚═══════════════════════════════════════════════════════════════╝
Colors: Q1 in red, Q2 in green, Q3 in yellow, Q4 in grey.

📁 Repository Structure
text
.
├── README.md
├── python/
│   └── eisenhower.py
├── go/
│   └── eisenhower.go
├── javascript/
│   └── eisenhower.js
├── ruby/
│   └── eisenhower.rb
├── php/
│   └── eisenhower.php
├── java/
│   └── Eisenhower.java
├── csharp/
│   └── Eisenhower.cs
└── cpp/
    └── eisenhower.cpp
💻 Source Code – 8 Languages
Below are the complete implementations. Copy each file into the corresponding folder.
