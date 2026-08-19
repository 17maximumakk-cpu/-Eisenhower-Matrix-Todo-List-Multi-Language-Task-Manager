# eisenhower.rb
require 'json'
require 'securerandom'
require 'colorize'
require 'optparse'

DATA_FILE = 'tasks.json'

class Task
  attr_accessor :id, :title, :description, :quadrant, :created, :completed

  def initialize(title, description = '', quadrant = 1)
    @id = SecureRandom.hex(4)
    @title = title
    @description = description
    @quadrant = quadrant
    @created = Time.now.iso8601
    @completed = false
  end

  def to_hash
    { id: @id, title: @title, description: @description, quadrant: @quadrant,
      created: @created, completed: @completed }
  end

  def self.from_hash(h)
    t = new(h['title'], h['description'], h['quadrant'])
    t.id = h['id']
    t.created = h['created']
    t.completed = h['completed']
    t
  end
end

class EisenhowerApp
  attr_reader :tasks

  def initialize
    @tasks = []
    load
  end

  def load
    return unless File.exist?(DATA_FILE)
    data = JSON.parse(File.read(DATA_FILE))
    @tasks = data.map { |h| Task.from_hash(h) }
  end

  def save
    File.write(DATA_FILE, JSON.pretty_generate(@tasks.map(&:to_hash)))
  end

  def add_task(title, desc, quadrant)
    task = Task.new(title, desc, quadrant)
    @tasks << task
    save
    puts "Task added: #{task.title} (ID: #{task.id})"
  end

  def delete_task(id)
    task = @tasks.find { |t| t.id == id }
    if task
      @tasks.delete(task)
      save
      puts "Task #{id} deleted."
    else
      puts "Task #{id} not found."
    end
  end

  def move_task(id, new_quadrant)
    task = @tasks.find { |t| t.id == id }
    if task
      task.quadrant = new_quadrant
      save
      puts "Task #{id} moved to quadrant #{new_quadrant}."
    else
      puts "Task #{id} not found."
    end
  end

  def list_tasks(quadrant = nil)
    filtered = quadrant ? @tasks.select { |t| t.quadrant == quadrant } : @tasks
    if filtered.empty?
      puts "No tasks found."
      return
    end
    labels = {
      1 => 'Urgent & Important',
      2 => 'Not Urgent but Important',
      3 => 'Urgent but Not Important',
      4 => 'Not Urgent & Not Important'
    }
    colors = [:red, :green, :yellow, :white]
    puts "\nEisenhower Matrix"
    puts "=" * 60
    (1..4).each do |q|
      puts labels[q].colorize(colors[q-1])
      q_tasks = filtered.select { |t| t.quadrant == q }
      if q_tasks.empty?
        puts "  (empty)"
      else
        q_tasks.each do |t|
          puts "  [#{t.id}] #{t.title} - #{t.description.empty? ? 'No description' : t.description}"
        end
      end
      puts "-" * 40 if q < 4
    end
    puts "=" * 60
  end

  def stats
    counts = {1=>0,2=>0,3=>0,4=>0}
    @tasks.each { |t| counts[t.quadrant] += 1 }
    labels = {
      1 => 'Urgent & Important',
      2 => 'Not Urgent but Important',
      3 => 'Urgent but Not Important',
      4 => 'Not Urgent & Not Important'
    }
    puts "Eisenhower Statistics"
    labels.each do |q, label|
      puts "#{label}: #{counts[q]} tasks"
    end
  end
end

options = {}
OptionParser.new do |opts|
  opts.banner = "Usage: eisenhower.rb <command> [options]"
  opts.on("add", "Add a new task") do
    options[:command] = :add
  end
  opts.on("list", "List tasks") do
    options[:command] = :list
  end
  opts.on("move", "Move a task") do
    options[:command] = :move
  end
  opts.on("delete", "Delete a task") do
    options[:command] = :delete
  end
  opts.on("stats", "Show statistics") do
    options[:command] = :stats
  end
  opts.on("-t TITLE", "--title TITLE", "Task title") { |v| options[:title] = v }
  opts.on("-d DESC", "--desc DESC", "Task description") { |v| options[:desc] = v }
  opts.on("-q QUADRANT", Integer, "Quadrant (1-4)") { |v| options[:quadrant] = v }
  opts.on("--id ID", "Task ID") { |v| options[:id] = v }
end.parse!

app = EisenhowerApp.new
case options[:command]
when :add
  app.add_task(options[:title] || "Untitled", options[:desc] || "", options[:quadrant] || 1)
when :list
  app.list_tasks(options[:quadrant])
when :move
  app.move_task(options[:id], options[:quadrant])
when :delete
  app.delete_task(options[:id])
when :stats
  app.stats
else
  puts "Unknown command. Use add, list, move, delete, stats."
end
