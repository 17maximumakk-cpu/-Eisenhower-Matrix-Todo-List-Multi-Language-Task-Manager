// eisenhower.go
package main

import (
	"bufio"
	"encoding/json"
	"fmt"
	"os"
	"strconv"
	"strings"
	"time"
	"github.com/google/uuid"
)

type Task struct {
	ID          string    `json:"id"`
	Title       string    `json:"title"`
	Description string    `json:"description"`
	Quadrant    int       `json:"quadrant"`
	Created     time.Time `json:"created"`
	Completed   bool      `json:"completed"`
}

type App struct {
	Tasks []Task
	file  string
}

func NewApp(file string) *App {
	app := &App{file: file}
	app.load()
	return app
}

func (a *App) load() {
	data, err := os.ReadFile(a.file)
	if err != nil {
		return
	}
	json.Unmarshal(data, &a.Tasks)
}

func (a *App) save() {
	data, _ := json.MarshalIndent(a.Tasks, "", "  ")
	os.WriteFile(a.file, data, 0644)
}

func (a *App) AddTask(title, desc string, quadrant int) {
	task := Task{
		ID:          uuid.New().String()[:8],
		Title:       title,
		Description: desc,
		Quadrant:    quadrant,
		Created:     time.Now(),
	}
	a.Tasks = append(a.Tasks, task)
	a.save()
	fmt.Printf("Task added: %s (ID: %s)\n", task.Title, task.ID)
}

func (a *App) DeleteTask(id string) {
	for i, t := range a.Tasks {
		if t.ID == id {
			a.Tasks = append(a.Tasks[:i], a.Tasks[i+1:]...)
			a.save()
			fmt.Printf("Task %s deleted.\n", id)
			return
		}
	}
	fmt.Printf("Task %s not found.\n", id)
}

func (a *App) MoveTask(id string, newQuadrant int) {
	for i, t := range a.Tasks {
		if t.ID == id {
			a.Tasks[i].Quadrant = newQuadrant
			a.save()
			fmt.Printf("Task %s moved to quadrant %d.\n", id, newQuadrant)
			return
		}
	}
	fmt.Printf("Task %s not found.\n", id)
}

func (a *App) ListTasks(quadrant *int) {
	filtered := []Task{}
	for _, t := range a.Tasks {
		if quadrant == nil || t.Quadrant == *quadrant {
			filtered = append(filtered, t)
		}
	}
	if len(filtered) == 0 {
		fmt.Println("No tasks found.")
		return
	}
	labels := map[int]string{
		1: "Urgent & Important",
		2: "Not Urgent but Important",
		3: "Urgent but Not Important",
		4: "Not Urgent & Not Important",
	}
	fmt.Println("\nEisenhower Matrix")
	fmt.Println(strings.Repeat("=", 60))
	for q := 1; q <= 4; q++ {
		fmt.Printf("\033[%dm%s\033[0m\n", 31+q-1, labels[q])
		has := false
		for _, t := range filtered {
			if t.Quadrant == q {
				fmt.Printf("  [%s] %s - %s\n", t.ID, t.Title, t.Description)
				has = true
			}
		}
		if !has {
			fmt.Println("  (empty)")
		}
		if q < 4 {
			fmt.Println(strings.Repeat("-", 40))
		}
	}
	fmt.Println(strings.Repeat("=", 60))
}

func (a *App) Stats() {
	counts := map[int]int{1:0,2:0,3:0,4:0}
	for _, t := range a.Tasks {
		counts[t.Quadrant]++
	}
	labels := []string{"Urgent & Important", "Not Urgent but Important", "Urgent but Not Important", "Not Urgent & Not Important"}
	fmt.Println("Eisenhower Statistics")
	for i, label := range labels {
		q := i + 1
		fmt.Printf("%s: %d tasks\n", label, counts[q])
	}
}

func main() {
	if len(os.Args) < 2 {
		fmt.Println("Usage: eisenhower <command> [options]")
		return
	}
	app := NewApp("tasks.json")
	cmd := os.Args[1]
	switch cmd {
	case "add":
		if len(os.Args) < 4 {
			fmt.Println("add requires -t title [-d desc] [-q quadrant]")
			return
		}
		title, desc := "", ""
		quadrant := 1
		for i := 2; i < len(os.Args); i++ {
			switch os.Args[i] {
			case "-t": title = os.Args[i+1]; i++
			case "-d": desc = os.Args[i+1]; i++
			case "-q": quadrant, _ = strconv.Atoi(os.Args[i+1]); i++
			}
		}
		app.AddTask(title, desc, quadrant)
	case "list":
		quadrant := -1
		for i := 2; i < len(os.Args); i++ {
			if os.Args[i] == "-q" {
				quadrant, _ = strconv.Atoi(os.Args[i+1]); i++
			}
		}
		var qPtr *int
		if quadrant != -1 {
			qPtr = &quadrant
		}
		app.ListTasks(qPtr)
	case "move":
		id, q := "", 0
		for i := 2; i < len(os.Args); i++ {
			if os.Args[i] == "--id" { id = os.Args[i+1]; i++ }
			if os.Args[i] == "-q" { q, _ = strconv.Atoi(os.Args[i+1]); i++ }
		}
		app.MoveTask(id, q)
	case "delete":
		id := ""
		for i := 2; i < len(os.Args); i++ {
			if os.Args[i] == "--id" { id = os.Args[i+1]; i++ }
		}
		app.DeleteTask(id)
	case "stats":
		app.Stats()
	default:
		fmt.Println("Unknown command")
	}
}
