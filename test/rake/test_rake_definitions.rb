require File.expand_path('../helper', __FILE__)
require 'fileutils'

class TestRakeDefinitions < Rake::TestCase
  include Rake

  EXISTINGFILE = "existing"

  def setup
    super

    Task.clear
  end

  def test_task
    done = false
    task :one => [:two] do done = true end
    task :two
    task :three => [:one, :two]
    check_tasks(:one, :two, :three)
    assert done, "Should be done"
  end

  def test_file_task
    done = false
    file "one" => "two" do done = true end
    file "two"
    file "three" => ["one", "two"]
    check_tasks("one", "two", "three")
    assert done, "Should be done"
  end

  def check_tasks(n1, n2, n3)
    t = Task[n1]
    assert Task === t, "Should be a Task"
    assert_equal n1.to_s, t.name
    assert_equal [n2.to_s], t.prerequisites.collect{|n| n.to_s}
    t.invoke
    t2 = Task[n2]
    assert_equal FileList[], t2.prerequisites
    t3 = Task[n3]
    assert_equal [n1.to_s, n2.to_s], t3.prerequisites.collect{|n|n.to_s}
  end

  def test_incremental_definitions
    runs = []
    task :t1 => [:t2] do runs << "A"; 4321 end
    task :t1 => [:t3] do runs << "B"; 1234 end
    task :t1 => [:t3]
    task :t2
    task :t3
    Task[:t1].invoke
    assert_equal ["A", "B"], runs
    assert_equal ["t2", "t3"], Task[:t1].prerequisites
  end

  def test_missing_dependencies
    task :x => ["missing"]
    assert_raises(RuntimeError) { Task[:x].invoke }
  end

  def test_implicit_file_dependencies
    runs = []
    create_existing_file
    task :y => [EXISTINGFILE] do |t| runs << t.name end
    Task[:y].invoke
    assert_equal runs, ['y']
  end

  private # ----------------------------------------------------------

  def create_existing_file
    Dir.mkdir File.dirname(EXISTINGFILE) unless
      File.exist?(File.dirname(EXISTINGFILE))
    open(EXISTINGFILE, "w") do |f| f.puts "HI" end unless
      File.exist?(EXISTINGFILE)
  end

end

