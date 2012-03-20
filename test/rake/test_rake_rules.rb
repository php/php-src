require File.expand_path('../helper', __FILE__)
require 'fileutils'

class TestRakeRules < Rake::TestCase
  include Rake

  SRCFILE    = "abc.c"
  SRCFILE2   = "xyz.c"
  FTNFILE    = "abc.f"
  OBJFILE    = "abc.o"
  FOOFILE    = "foo"
  DOTFOOFILE = ".foo"

  def setup
    super

    Task.clear
    @runs = []
  end

  def test_multiple_rules1
    create_file(FTNFILE)
    delete_file(SRCFILE)
    delete_file(OBJFILE)
    rule(/\.o$/ => ['.c']) do @runs << :C end
    rule(/\.o$/ => ['.f']) do @runs << :F end
    t = Task[OBJFILE]
    t.invoke
    Task[OBJFILE].invoke
    assert_equal [:F], @runs
  end

  def test_multiple_rules2
    create_file(FTNFILE)
    delete_file(SRCFILE)
    delete_file(OBJFILE)
    rule(/\.o$/ => ['.f']) do @runs << :F end
    rule(/\.o$/ => ['.c']) do @runs << :C end
    Task[OBJFILE].invoke
    assert_equal [:F], @runs
  end

  def test_create_with_source
    create_file(SRCFILE)
    rule(/\.o$/ => ['.c']) do |t|
      @runs << t.name
      assert_equal OBJFILE, t.name
      assert_equal SRCFILE, t.source
    end
    Task[OBJFILE].invoke
    assert_equal [OBJFILE], @runs
  end

  def test_single_dependent
    create_file(SRCFILE)
    rule(/\.o$/ => '.c') do |t|
      @runs << t.name
    end
    Task[OBJFILE].invoke
    assert_equal [OBJFILE], @runs
  end

  def test_rule_can_be_created_by_string
    create_file(SRCFILE)
    rule '.o' => ['.c'] do |t|
      @runs << t.name
    end
    Task[OBJFILE].invoke
    assert_equal [OBJFILE], @runs
  end

  def test_rule_prereqs_can_be_created_by_string
    create_file(SRCFILE)
    rule '.o' => '.c' do |t|
      @runs << t.name
    end
    Task[OBJFILE].invoke
    assert_equal [OBJFILE], @runs
  end

  def test_plain_strings_as_dependents_refer_to_files
    create_file(SRCFILE)
    rule '.o' => SRCFILE do |t|
      @runs << t.name
    end
    Task[OBJFILE].invoke
    assert_equal [OBJFILE], @runs
  end

  def test_file_names_beginning_with_dot_can_be_tricked_into_referring_to_file
    verbose(false) do
      create_file('.foo')
      rule '.o' => "./.foo" do |t|
        @runs << t.name
      end
      Task[OBJFILE].invoke
      assert_equal [OBJFILE], @runs
    end
  end

  def test_file_names_beginning_with_dot_can_be_wrapped_in_lambda
    verbose(false) do

      create_file(".foo")
      rule '.o' => lambda{".foo"} do |t|
        @runs << "#{t.name} - #{t.source}"
      end
      Task[OBJFILE].invoke
      assert_equal ["#{OBJFILE} - .foo"], @runs
    end
  end

  def test_file_names_containing_percent_can_be_wrapped_in_lambda
    verbose(false) do
      create_file("foo%x")
      rule '.o' => lambda{"foo%x"} do |t|
        @runs << "#{t.name} - #{t.source}"
      end
      Task[OBJFILE].invoke
      assert_equal ["#{OBJFILE} - foo%x"], @runs
    end
  end

  def test_non_extension_rule_name_refers_to_file
    verbose(false) do
      create_file("abc.c")
      rule "abc" => '.c' do |t|
        @runs << t.name
      end
      Task["abc"].invoke
      assert_equal ["abc"], @runs
    end
  end

  def test_pathmap_automatically_applies_to_name
    verbose(false) do
      create_file("zzabc.c")
      rule ".o" => 'zz%{x,a}n.c' do |t|
        @runs << "#{t.name} - #{t.source}"
      end
      Task["xbc.o"].invoke
      assert_equal ["xbc.o - zzabc.c"], @runs
    end
  end

  def test_plain_strings_are_just_filenames
    verbose(false) do
      create_file("plainname")
      rule ".o" => 'plainname' do |t|
        @runs << "#{t.name} - #{t.source}"
      end
      Task["xbc.o"].invoke
      assert_equal ["xbc.o - plainname"], @runs
    end
  end

  def test_rule_runs_when_explicit_task_has_no_actions
    create_file(SRCFILE)
    create_file(SRCFILE2)
    delete_file(OBJFILE)
    rule '.o' => '.c' do |t|
      @runs << t.source
    end
    file OBJFILE => [SRCFILE2]
    Task[OBJFILE].invoke
    assert_equal [SRCFILE], @runs
  end

  def test_close_matches_on_name_do_not_trigger_rule
    create_file("x.c")
    rule '.o' => ['.c'] do |t|
      @runs << t.name
    end
    assert_raises(RuntimeError) { Task['x.obj'].invoke }
    assert_raises(RuntimeError) { Task['x.xyo'].invoke }
  end

  def test_rule_rebuilds_obj_when_source_is_newer
    create_timed_files(OBJFILE, SRCFILE)
    rule(/\.o$/ => ['.c']) do
      @runs << :RULE
    end
    Task[OBJFILE].invoke
    assert_equal [:RULE], @runs
  end

  def test_rule_with_two_sources_runs_if_both_sources_are_present
    create_timed_files(OBJFILE, SRCFILE, SRCFILE2)
    rule OBJFILE => [lambda{SRCFILE}, lambda{SRCFILE2}] do
      @runs << :RULE
    end
    Task[OBJFILE].invoke
    assert_equal [:RULE], @runs
  end

  def test_rule_with_two_sources_but_one_missing_does_not_run
    create_timed_files(OBJFILE, SRCFILE)
    delete_file(SRCFILE2)
    rule OBJFILE => [lambda{SRCFILE}, lambda{SRCFILE2}] do
      @runs << :RULE
    end
    Task[OBJFILE].invoke
    assert_equal [], @runs
  end

  def test_rule_with_two_sources_builds_both_sources
    task 'x.aa'
    task 'x.bb'
    rule '.a' => '.aa' do
      @runs << "A"
    end
    rule '.b' => '.bb' do
      @runs << "B"
    end
    rule ".c" => ['.a', '.b'] do
      @runs << "C"
    end
    Task["x.c"].invoke
    assert_equal ["A", "B", "C"], @runs.sort
  end

  def test_second_rule_runs_when_first_rule_doesnt
    create_timed_files(OBJFILE, SRCFILE)
    delete_file(SRCFILE2)
    rule OBJFILE => [lambda{SRCFILE}, lambda{SRCFILE2}] do
      @runs << :RULE1
    end
    rule OBJFILE => [lambda{SRCFILE}] do
      @runs << :RULE2
    end
    Task[OBJFILE].invoke
    assert_equal [:RULE2], @runs
  end

  def test_second_rule_doest_run_if_first_triggers
    create_timed_files(OBJFILE, SRCFILE, SRCFILE2)
    rule OBJFILE => [lambda{SRCFILE}, lambda{SRCFILE2}] do
      @runs << :RULE1
    end
    rule OBJFILE => [lambda{SRCFILE}] do
      @runs << :RULE2
    end
    Task[OBJFILE].invoke
    assert_equal [:RULE1], @runs
  end

  def test_second_rule_doest_run_if_first_triggers_with_reversed_rules
    create_timed_files(OBJFILE, SRCFILE, SRCFILE2)
    rule OBJFILE => [lambda{SRCFILE}] do
      @runs << :RULE1
    end
    rule OBJFILE => [lambda{SRCFILE}, lambda{SRCFILE2}] do
      @runs << :RULE2
    end
    Task[OBJFILE].invoke
    assert_equal [:RULE1], @runs
  end

  def test_rule_with_proc_dependent_will_trigger
    mkdir_p("src/jw")
    create_file("src/jw/X.java")
    rule %r(classes/.*\.class) => [
      proc { |fn| fn.pathmap("%{classes,src}d/%n.java") }
    ] do |task|
      assert_equal task.name, 'classes/jw/X.class'
      assert_equal task.source, 'src/jw/X.java'
      @runs << :RULE
    end
    Task['classes/jw/X.class'].invoke
    assert_equal [:RULE], @runs
  ensure
    rm_r("src", :verbose=>false) rescue nil
  end

  def test_proc_returning_lists_are_flattened_into_prereqs
    ran = false
    mkdir_p("flatten")
    create_file("flatten/a.txt")
    task 'flatten/b.data' do |t|
      ran = true
      touch t.name, :verbose => false
    end
    rule '.html' =>
      proc { |fn|
      [
        fn.ext("txt"),
        "flatten/b.data"
      ]
    } do |task|
    end
    Task['flatten/a.html'].invoke
    assert ran, "Should have triggered flattened dependency"
  ensure
    rm_r("flatten", :verbose=>false) rescue nil
  end

  def test_recursive_rules_will_work_as_long_as_they_terminate
    actions = []
    create_file("abc.xml")
    rule '.y' => '.xml' do actions << 'y' end
    rule '.c' => '.y' do actions << 'c'end
    rule '.o' => '.c' do actions << 'o'end
    rule '.exe' => '.o' do actions << 'exe'end
    Task["abc.exe"].invoke
    assert_equal ['y', 'c', 'o', 'exe'], actions
  end

  def test_recursive_rules_that_dont_terminate_will_overflow
    create_file("a.a")
    prev = 'a'
    ('b'..'z').each do |letter|
      rule ".#{letter}" => ".#{prev}" do |t| puts "#{t.name}" end
      prev = letter
    end
    ex = assert_raises(Rake::RuleRecursionOverflowError) {
      Task["a.z"].invoke
    }
    assert_match(/a\.z => a.y/, ex.message)
  end

  def test_rules_with_bad_dependents_will_fail
    rule "a" => [ 1 ] do |t| puts t.name end
    assert_raises(RuntimeError) do Task['a'].invoke end
  end

end

