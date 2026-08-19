// Eisenhower.cs
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.Json;
using System.Text.Json.Serialization;

class Task
{
    [JsonPropertyName("id")]
    public string Id { get; set; }
    [JsonPropertyName("title")]
    public string Title { get; set; }
    [JsonPropertyName("description")]
    public string Description { get; set; }
    [JsonPropertyName("quadrant")]
    public int Quadrant { get; set; }
    [JsonPropertyName("created")]
    public string Created { get; set; }
    [JsonPropertyName("completed")]
    public bool Completed { get; set; }

    public Task() { }
    public Task(string title, string description, int quadrant)
    {
        Id = Guid.NewGuid().ToString().Substring(0, 8);
        Title = title;
        Description = description;
        Quadrant = quadrant;
        Created = DateTime.UtcNow.ToString("o");
        Completed = false;
    }
}

class EisenhowerApp
{
    private List<Task> tasks = new List<Task>();
    private readonly string dataFile = "tasks.json";
    private readonly JsonSerializerOptions options = new JsonSerializerOptions { WriteIndented = true };

    public EisenhowerApp()
    {
        Load();
    }

    private void Load()
    {
        if (!File.Exists(dataFile)) return;
        string json = File.ReadAllText(dataFile);
        tasks = JsonSerializer.Deserialize<List<Task>>(json) ?? new List<Task>();
    }

    private void Save()
    {
        string json = JsonSerializer.Serialize(tasks, options);
        File.WriteAllText(dataFile, json);
    }

    public void AddTask(string title, string desc, int quadrant)
    {
        var t = new Task(title, desc, quadrant);
        tasks.Add(t);
        Save();
        Console.WriteLine($"Task added: {t.Title} (ID: {t.Id})");
    }

    public void DeleteTask(string id)
    {
        var task = tasks.FirstOrDefault(t => t.Id == id);
        if (task != null)
        {
            tasks.Remove(task);
            Save();
            Console.WriteLine($"Task {id} deleted.");
        }
        else
        {
            Console.WriteLine($"Task {id} not found.");
        }
    }

    public void MoveTask(string id, int newQuadrant)
    {
        var task = tasks.FirstOrDefault(t => t.Id == id);
        if (task != null)
        {
            task.Quadrant = newQuadrant;
            Save();
            Console.WriteLine($"Task {id} moved to quadrant {newQuadrant}.");
        }
        else
        {
            Console.WriteLine($"Task {id} not found.");
        }
    }

    public void ListTasks(int? quadrant)
    {
        var filtered = tasks.Where(t => quadrant == null || t.Quadrant == quadrant).ToList();
        if (!filtered.Any())
        {
            Console.WriteLine("No tasks found.");
            return;
        }
        string[] labels = { "Urgent & Important", "Not Urgent but Important",
                            "Urgent but Not Important", "Not Urgent & Not Important" };
        string[] colors = { "\x1b[31m", "\x1b[32m", "\x1b[33m", "\x1b[37m" };
        Console.WriteLine("\nEisenhower Matrix");
        Console.WriteLine(new string('=', 60));
        for (int q = 1; q <= 4; q++)
        {
            Console.WriteLine($"{colors[q-1]}{labels[q-1]}\x1b[0m");
            var qTasks = filtered.Where(t => t.Quadrant == q).ToList();
            if (!qTasks.Any())
                Console.WriteLine("  (empty)");
            else
                foreach (var t in qTasks)
                    Console.WriteLine($"  [{t.Id}] {t.Title} - {(string.IsNullOrEmpty(t.Description) ? "No description" : t.Description)}");
            if (q < 4) Console.WriteLine(new string('-', 40));
        }
        Console.WriteLine(new string('=', 60));
    }

    public void Stats()
    {
        int[] counts = new int[5];
        foreach (var t in tasks) counts[t.Quadrant]++;
        string[] labels = { "Urgent & Important", "Not Urgent but Important",
                            "Urgent but Not Important", "Not Urgent & Not Important" };
        Console.WriteLine("Eisenhower Statistics");
        for (int q = 1; q <= 4; q++)
            Console.WriteLine($"{labels[q-1]}: {counts[q]} tasks");
    }

    static void Main(string[] args)
    {
        if (args.Length == 0)
        {
            Console.WriteLine("Usage: Eisenhower <command> [options]");
            return;
        }
        var app = new EisenhowerApp();
        string cmd = args[0];
        switch (cmd)
        {
            case "add":
                string title = "", desc = "";
                int quadrant = 1;
                for (int i = 1; i < args.Length; i++)
                {
                    if (args[i] == "-t" && i + 1 < args.Length) title = args[++i];
                    if (args[i] == "-d" && i + 1 < args.Length) desc = args[++i];
                    if (args[i] == "-q" && i + 1 < args.Length) quadrant = int.Parse(args[++i]);
                }
                app.AddTask(title, desc, quadrant);
                break;
            case "list":
                int? qFilter = null;
                for (int i = 1; i < args.Length; i++)
                    if (args[i] == "-q" && i + 1 < args.Length) qFilter = int.Parse(args[++i]);
                app.ListTasks(qFilter);
                break;
            case "move":
                string id = "";
                int qMove = 0;
                for (int i = 1; i < args.Length; i++)
                {
                    if (args[i] == "--id" && i + 1 < args.Length) id = args[++i];
                    if (args[i] == "-q" && i + 1 < args.Length) qMove = int.Parse(args[++i]);
                }
                app.MoveTask(id, qMove);
                break;
            case "delete":
                string delId = "";
                for (int i = 1; i < args.Length; i++)
                    if (args[i] == "--id" && i + 1 < args.Length) delId = args[++i];
                app.DeleteTask(delId);
                break;
            case "stats":
                app.Stats();
                break;
            default:
                Console.WriteLine("Unknown command.");
                break;
        }
    }
}
