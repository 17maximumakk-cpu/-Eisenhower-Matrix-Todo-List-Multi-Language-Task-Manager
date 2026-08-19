// Eisenhower.java
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.SerializationFeature;
import com.fasterxml.jackson.core.type.TypeReference;

import java.io.File;
import java.io.IOException;
import java.time.Instant;
import java.util.*;
import java.util.stream.Collectors;

class Task {
    public String id;
    public String title;
    public String description;
    public int quadrant;
    public String created;
    public boolean completed;

    public Task() {}
    public Task(String title, String description, int quadrant) {
        this.id = UUID.randomUUID().toString().substring(0,8);
        this.title = title;
        this.description = description;
        this.quadrant = quadrant;
        this.created = Instant.now().toString();
        this.completed = false;
    }
}

public class Eisenhower {
    private List<Task> tasks = new ArrayList<>();
    private final File dataFile = new File("tasks.json");
    private final ObjectMapper mapper = new ObjectMapper()
            .enable(SerializationFeature.INDENT_OUTPUT);

    public Eisenhower() {
        load();
    }

    private void load() {
        if (!dataFile.exists()) return;
        try {
            tasks = mapper.readValue(dataFile, new TypeReference<List<Task>>(){});
        } catch (IOException e) {
            System.err.println("Error loading tasks: " + e.getMessage());
        }
    }

    private void save() {
        try {
            mapper.writeValue(dataFile, tasks);
        } catch (IOException e) {
            System.err.println("Error saving tasks: " + e.getMessage());
        }
    }

    public void addTask(String title, String desc, int quadrant) {
        Task t = new Task(title, desc, quadrant);
        tasks.add(t);
        save();
        System.out.printf("Task added: %s (ID: %s)%n", t.title, t.id);
    }

    public void deleteTask(String id) {
        boolean removed = tasks.removeIf(t -> t.id.equals(id));
        if (removed) {
            save();
            System.out.printf("Task %s deleted.%n", id);
        } else {
            System.out.printf("Task %s not found.%n", id);
        }
    }

    public void moveTask(String id, int newQuadrant) {
        for (Task t : tasks) {
            if (t.id.equals(id)) {
                t.quadrant = newQuadrant;
                save();
                System.out.printf("Task %s moved to quadrant %d.%n", id, newQuadrant);
                return;
            }
        }
        System.out.printf("Task %s not found.%n", id);
    }

    public void listTasks(Integer quadrant) {
        List<Task> filtered = tasks.stream()
                .filter(t -> quadrant == null || t.quadrant == quadrant)
                .collect(Collectors.toList());
        if (filtered.isEmpty()) {
            System.out.println("No tasks found.");
            return;
        }
        String[] labels = {"Urgent & Important", "Not Urgent but Important",
                "Urgent but Not Important", "Not Urgent & Not Important"};
        String[] colors = {"\033[31m", "\033[32m", "\033[33m", "\033[37m"};
        System.out.println("\nEisenhower Matrix");
        System.out.println("=".repeat(60));
        for (int q = 1; q <= 4; q++) {
            System.out.println(colors[q-1] + labels[q-1] + "\033[0m");
            List<Task> qTasks = filtered.stream().filter(t -> t.quadrant == q).collect(Collectors.toList());
            if (qTasks.isEmpty()) {
                System.out.println("  (empty)");
            } else {
                for (Task t : qTasks) {
                    System.out.printf("  [%s] %s - %s%n", t.id, t.title,
                            t.description.isEmpty() ? "No description" : t.description);
                }
            }
            if (q < 4) System.out.println("-".repeat(40));
        }
        System.out.println("=".repeat(60));
    }

    public void stats() {
        int[] counts = new int[5];
        for (Task t : tasks) counts[t.quadrant]++;
        String[] labels = {"Urgent & Important", "Not Urgent but Important",
                "Urgent but Not Important", "Not Urgent & Not Important"};
        System.out.println("Eisenhower Statistics");
        for (int q = 1; q <= 4; q++) {
            System.out.printf("%s: %d tasks%n", labels[q-1], counts[q]);
        }
    }

    public static void main(String[] args) {
        if (args.length < 1) {
            System.out.println("Usage: java Eisenhower <command> [options]");
            return;
        }
        Eisenhower app = new Eisenhower();
        String cmd = args[0];
        switch (cmd) {
            case "add": {
                String title = "", desc = "";
                int quadrant = 1;
                for (int i = 1; i < args.length; i++) {
                    if (args[i].equals("-t") && i+1 < args.length) title = args[++i];
                    if (args[i].equals("-d") && i+1 < args.length) desc = args[++i];
                    if (args[i].equals("-q") && i+1 < args.length) quadrant = Integer.parseInt(args[++i]);
                }
                app.addTask(title, desc, quadrant);
                break;
            }
            case "list": {
                Integer quadrant = null;
                for (int i = 1; i < args.length; i++) {
                    if (args[i].equals("-q") && i+1 < args.length) quadrant = Integer.parseInt(args[++i]);
                }
                app.listTasks(quadrant);
                break;
            }
            case "move": {
                String id = "";
                int q = 0;
                for (int i = 1; i < args.length; i++) {
                    if (args[i].equals("--id") && i+1 < args.length) id = args[++i];
                    if (args[i].equals("-q") && i+1 < args.length) q = Integer.parseInt(args[++i]);
                }
                app.moveTask(id, q);
                break;
            }
            case "delete": {
                String id = "";
                for (int i = 1; i < args.length; i++) {
                    if (args[i].equals("--id") && i+1 < args.length) id = args[++i];
                }
                app.deleteTask(id);
                break;
            }
            case "stats":
                app.stats();
                break;
            default:
                System.out.println("Unknown command.");
        }
    }
}
