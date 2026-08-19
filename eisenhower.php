# eisenhower.php
<?php
$dataFile = 'tasks.json';

class Task {
    public $id, $title, $description, $quadrant, $created, $completed;
    function __construct($title, $description='', $quadrant=1) {
        $this->id = substr(bin2hex(random_bytes(4)), 0, 8);
        $this->title = $title;
        $this->description = $description;
        $this->quadrant = $quadrant;
        $this->created = date('c');
        $this->completed = false;
    }
}

class EisenhowerApp {
    private $tasks = [];
    private $file;

    function __construct($file) {
        $this->file = $file;
        $this->load();
    }

    function load() {
        if (file_exists($this->file)) {
            $data = json_decode(file_get_contents($this->file), true);
            foreach ($data as $item) {
                $task = new Task($item['title'], $item['description'], $item['quadrant']);
                $task->id = $item['id'];
                $task->created = $item['created'];
                $task->completed = $item['completed'];
                $this->tasks[] = $task;
            }
        }
    }

    function save() {
        $data = [];
        foreach ($this->tasks as $t) {
            $data[] = [
                'id' => $t->id,
                'title' => $t->title,
                'description' => $t->description,
                'quadrant' => $t->quadrant,
                'created' => $t->created,
                'completed' => $t->completed
            ];
        }
        file_put_contents($this->file, json_encode($data, JSON_PRETTY_PRINT));
    }

    function addTask($title, $desc, $quadrant) {
        $task = new Task($title, $desc, $quadrant);
        $this->tasks[] = $task;
        $this->save();
        echo "Task added: {$task->title} (ID: {$task->id})\n";
    }

    function deleteTask($id) {
        foreach ($this->tasks as $i => $t) {
            if ($t->id == $id) {
                array_splice($this->tasks, $i, 1);
                $this->save();
                echo "Task $id deleted.\n";
                return;
            }
        }
        echo "Task $id not found.\n";
    }

    function moveTask($id, $newQuadrant) {
        foreach ($this->tasks as $t) {
            if ($t->id == $id) {
                $t->quadrant = $newQuadrant;
                $this->save();
                echo "Task $id moved to quadrant $newQuadrant.\n";
                return;
            }
        }
        echo "Task $id not found.\n";
    }

    function listTasks($quadrant = null) {
        $filtered = $this->tasks;
        if ($quadrant !== null) {
            $filtered = array_filter($filtered, function($t) use ($quadrant) {
                return $t->quadrant == $quadrant;
            });
        }
        if (empty($filtered)) {
            echo "No tasks found.\n";
            return;
        }
        $labels = [
            1 => 'Urgent & Important',
            2 => 'Not Urgent but Important',
            3 => 'Urgent but Not Important',
            4 => 'Not Urgent & Not Important'
        ];
        $colors = ["\033[31m", "\033[32m", "\033[33m", "\033[37m"];
        echo "\nEisenhower Matrix\n";
        echo str_repeat("=", 60) . "\n";
        for ($q = 1; $q <= 4; $q++) {
            echo $colors[$q-1] . $labels[$q] . "\033[0m\n";
            $qTasks = array_filter($filtered, function($t) use ($q) {
                return $t->quadrant == $q;
            });
            if (empty($qTasks)) {
                echo "  (empty)\n";
            } else {
                foreach ($qTasks as $t) {
                    echo "  [{$t->id}] {$t->title} - " . ($t->description ?: 'No description') . "\n";
                }
            }
            if ($q < 4) echo str_repeat("-", 40) . "\n";
        }
        echo str_repeat("=", 60) . "\n";
    }

    function stats() {
        $counts = [1=>0,2=>0,3=>0,4=>0];
        foreach ($this->tasks as $t) {
            $counts[$t->quadrant]++;
        }
        $labels = [
            1 => 'Urgent & Important',
            2 => 'Not Urgent but Important',
            3 => 'Urgent but Not Important',
            4 => 'Not Urgent & Not Important'
        ];
        echo "Eisenhower Statistics\n";
        foreach ($labels as $q => $label) {
            echo "$label: {$counts[$q]} tasks\n";
        }
    }
}

// Parse command line
if ($argc < 2) {
    die("Usage: php eisenhower.php <command> [options]\n");
}
$app = new EisenhowerApp($dataFile);
$cmd = $argv[1];
switch ($cmd) {
    case 'add':
        $title = $desc = '';
        $quadrant = 1;
        for ($i = 2; $i < $argc; $i++) {
            if ($argv[$i] == '-t' && isset($argv[$i+1])) { $title = $argv[++$i]; }
            if ($argv[$i] == '-d' && isset($argv[$i+1])) { $desc = $argv[++$i]; }
            if ($argv[$i] == '-q' && isset($argv[$i+1])) { $quadrant = (int)$argv[++$i]; }
        }
        $app->addTask($title, $desc, $quadrant);
        break;
    case 'list':
        $quadrant = null;
        for ($i = 2; $i < $argc; $i++) {
            if ($argv[$i] == '-q' && isset($argv[$i+1])) { $quadrant = (int)$argv[++$i]; }
        }
        $app->listTasks($quadrant);
        break;
    case 'move':
        $id = ''; $q = 0;
        for ($i = 2; $i < $argc; $i++) {
            if ($argv[$i] == '--id' && isset($argv[$i+1])) { $id = $argv[++$i]; }
            if ($argv[$i] == '-q' && isset($argv[$i+1])) { $q = (int)$argv[++$i]; }
        }
        $app->moveTask($id, $q);
        break;
    case 'delete':
        $id = '';
        for ($i = 2; $i < $argc; $i++) {
            if ($argv[$i] == '--id' && isset($argv[$i+1])) { $id = $argv[++$i]; }
        }
        $app->deleteTask($id);
        break;
    case 'stats':
        $app->stats();
        break;
    default:
        echo "Unknown command.\n";
}
?>
