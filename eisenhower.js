// eisenhower.js
#!/usr/bin/env node
const fs = require('fs');
const path = require('path');
const { program } = require('commander');
const chalk = require('chalk');
const { v4: uuidv4 } = require('uuid');

const DATA_FILE = path.join(__dirname, 'tasks.json');

class Task {
    constructor(title, description = '', quadrant = 1) {
        this.id = uuidv4().slice(0, 8);
        this.title = title;
        this.description = description;
        this.quadrant = quadrant;
        this.created = new Date().toISOString();
        this.completed = false;
    }
}

class EisenhowerApp {
    constructor() {
        this.tasks = [];
        this.load();
    }

    load() {
        if (fs.existsSync(DATA_FILE)) {
            const data = fs.readFileSync(DATA_FILE, 'utf8');
            this.tasks = JSON.parse(data);
        }
    }

    save() {
        fs.writeFileSync(DATA_FILE, JSON.stringify(this.tasks, null, 2));
    }

    addTask(title, desc, quadrant) {
        const task = new Task(title, desc, quadrant);
        this.tasks.push(task);
        this.save();
        console.log(`Task added: ${task.title} (ID: ${task.id})`);
    }

    deleteTask(id) {
        const index = this.tasks.findIndex(t => t.id === id);
        if (index === -1) {
            console.log(`Task ${id} not found.`);
            return;
        }
        this.tasks.splice(index, 1);
        this.save();
        console.log(`Task ${id} deleted.`);
    }

    moveTask(id, newQuadrant) {
        const task = this.tasks.find(t => t.id === id);
        if (!task) {
            console.log(`Task ${id} not found.`);
            return;
        }
        task.quadrant = newQuadrant;
        this.save();
        console.log(`Task ${id} moved to quadrant ${newQuadrant}.`);
    }

    listTasks(quadrant) {
        let filtered = this.tasks;
        if (quadrant !== undefined) {
            filtered = filtered.filter(t => t.quadrant === quadrant);
        }
        if (filtered.length === 0) {
            console.log('No tasks found.');
            return;
        }
        const labels = {
            1: 'Urgent & Important',
            2: 'Not Urgent but Important',
            3: 'Urgent but Not Important',
            4: 'Not Urgent & Not Important'
        };
        const colors = [chalk.red, chalk.green, chalk.yellow, chalk.gray];
        console.log('\nEisenhower Matrix');
        console.log('='.repeat(60));
        for (let q = 1; q <= 4; q++) {
            console.log(colors[q-1](labels[q]));
            const qTasks = filtered.filter(t => t.quadrant === q);
            if (qTasks.length === 0) {
                console.log('  (empty)');
            } else {
                qTasks.forEach(t => {
                    console.log(`  [${t.id}] ${t.title} - ${t.description || 'No description'}`);
                });
            }
            if (q < 4) console.log('-'.repeat(40));
        }
        console.log('='.repeat(60));
    }

    stats() {
        const counts = {1:0,2:0,3:0,4:0};
        this.tasks.forEach(t => counts[t.quadrant]++);
        console.log('Eisenhower Statistics');
        const labels = {
            1: 'Urgent & Important',
            2: 'Not Urgent but Important',
            3: 'Urgent but Not Important',
            4: 'Not Urgent & Not Important'
        };
        for (let q = 1; q <= 4; q++) {
            console.log(`${labels[q]}: ${counts[q]} tasks`);
        }
    }
}

program
    .command('add')
    .option('-t, --title <title>', 'Task title')
    .option('-d, --desc <description>', 'Task description')
    .option('-q, --quadrant <quadrant>', 'Quadrant (1-4)', parseInt, 1)
    .action((options) => {
        const app = new EisenhowerApp();
        app.addTask(options.title, options.desc, options.quadrant);
    });

program
    .command('list')
    .option('-q, --quadrant <quadrant>', 'Filter by quadrant', parseInt)
    .action((options) => {
        const app = new EisenhowerApp();
        app.listTasks(options.quadrant);
    });

program
    .command('move')
    .option('--id <id>', 'Task ID')
    .option('-q, --quadrant <quadrant>', 'New quadrant', parseInt)
    .action((options) => {
        const app = new EisenhowerApp();
        app.moveTask(options.id, options.quadrant);
    });

program
    .command('delete')
    .option('--id <id>', 'Task ID')
    .action((options) => {
        const app = new EisenhowerApp();
        app.deleteTask(options.id);
    });

program
    .command('stats')
    .action(() => {
        const app = new EisenhowerApp();
        app.stats();
    });

program.parse(process.argv);
