require File.expand_path('../helper', __FILE__)

class TestRakeTaskWithArguments < Rake::TestCase
  include Rake

  def setup
    super

    Task.clear
    Rake::TaskManager.record_task_metadata = true
  end

  def teardown
    Rake::TaskManager.record_task_metadata = false

    super
  end

  def test_no_args_given
    t = task :t
    assert_equal [], t.arg_names
  end

  def test_args_given
    t = task :t, :a, :b
    assert_equal [:a, :b], t.arg_names
  end

  def test_name_and_needs
    t = task(:t => [:pre])
    assert_equal "t", t.name
    assert_equal [], t.arg_names
    assert_equal ["pre"], t.prerequisites
  end

  def test_name_args_and_explicit_needs
    ignore_deprecations do
      t = task(:t, :x, :y, :needs => [:pre])
      assert_equal "t", t.name
      assert_equal [:x, :y], t.arg_names
      assert_equal ["pre"], t.prerequisites
    end
  end

  def test_illegal_keys_in_task_name_hash
    ignore_deprecations do
      assert_raises RuntimeError do
        t = task(:t, :x, :y => 1, :needs => [:pre])
      end
    end
  end

  def test_arg_list_is_empty_if_no_args_given
    t = task(:t) { |tt, args| assert_equal({}, args.to_hash) }
    t.invoke(1, 2, 3)
  end

  def test_tasks_can_access_arguments_as_hash
    t = task :t, :a, :b, :c do |tt, args|
      assert_equal({:a => 1, :b => 2, :c => 3}, args.to_hash)
      assert_equal 1, args[:a]
      assert_equal 2, args[:b]
      assert_equal 3, args[:c]
      assert_equal 1, args.a
      assert_equal 2, args.b
      assert_equal 3, args.c
    end
    t.invoke(1, 2, 3)
  end

  def test_actions_of_various_arity_are_ok_with_args
    notes = []
    t = task(:t, :x) do
      notes << :a
    end
    t.enhance do | |
      notes << :b
    end
    t.enhance do |task|
      notes << :c
      assert_kind_of Task, task
    end
    t.enhance do |t2, args|
      notes << :d
      assert_equal t, t2
      assert_equal({:x => 1}, args.to_hash)
    end
    t.invoke(1)
    assert_equal [:a, :b, :c, :d], notes
  end

  def test_arguments_are_passed_to_block
    t = task(:t, :a, :b) { |tt, args|
      assert_equal( { :a => 1, :b => 2 }, args.to_hash )
    }
    t.invoke(1, 2)
  end

  def test_extra_parameters_are_ignored
    t = task(:t, :a) { |tt, args|
      assert_equal 1, args.a
      assert_nil args.b
    }
    t.invoke(1, 2)
  end

  def test_arguments_are_passed_to_all_blocks
    counter = 0
    t = task :t, :a
    task :t do |tt, args|
      assert_equal 1, args.a
      counter += 1
    end
    task :t do |tt, args|
      assert_equal 1, args.a
      counter += 1
    end
    t.invoke(1)
    assert_equal 2, counter
  end

  def test_block_with_no_parameters_is_ok
    t = task(:t) { }
    t.invoke(1, 2)
  end

  def test_name_with_args
    desc "T"
    t = task(:tt, :a, :b)
    assert_equal "tt", t.name
    assert_equal "T", t.comment
    assert_equal "[a,b]", t.arg_description
    assert_equal "tt[a,b]", t.name_with_args
    assert_equal [:a, :b],t.arg_names
  end

  def test_named_args_are_passed_to_prereqs
    value = nil
    pre = task(:pre, :rev) { |t, args| value = args.rev }
    t = task(:t, [:name, :rev] => [:pre])
    t.invoke("bill", "1.2")
    assert_equal "1.2", value
  end

  def test_args_not_passed_if_no_prereq_names
    pre = task(:pre) { |t, args|
      assert_equal({}, args.to_hash)
      assert_equal "bill", args.name
    }
    t = task(:t, [:name, :rev] => [:pre])
    t.invoke("bill", "1.2")
  end

  def test_args_not_passed_if_no_arg_names
    pre = task(:pre, :rev) { |t, args|
      assert_equal({}, args.to_hash)
    }
    t = task(:t  => [:pre])
    t.invoke("bill", "1.2")
  end

  def test_values_at
    t = task(:pre, [:a, :b, :c]) { |task, args|
      a, b, c = args.values_at(:a, :b, :c)
      assert_equal %w[1 2 3], [a, b, c]
    }

    t.invoke(*%w[1 2 3])

    # HACK no assertions
  end
end

