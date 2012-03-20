require File.expand_path('../helper', __FILE__)
require 'rake/loaders/makefile'

class TestRakeMakefileLoader < Rake::TestCase
  include Rake

  def test_parse
    Dir.chdir @tempdir

    open 'sample.mf', 'w' do |io|
      io << <<-'SAMPLE_MF'
# Comments
a: a1 a2 a3 a4
b: b1 b2 b3 \
   b4 b5 b6\
# Mid: Comment
b7

 a : a5 a6 a7
c: c1
d: d1 d2 \

e f : e1 f1

g\ 0: g1 g\ 2 g\ 3 g4
      SAMPLE_MF
    end

    Task.clear
    loader = Rake::MakefileLoader.new
    loader.load 'sample.mf'
    %w(a b c d).each do |t|
      assert Task.task_defined?(t), "#{t} should be a defined task"
    end
    assert_equal %w(a1 a2 a3 a4 a5 a6 a7).sort, Task['a'].prerequisites.sort
    assert_equal %w(b1 b2 b3 b4 b5 b6 b7).sort, Task['b'].prerequisites.sort
    assert_equal %w(c1).sort, Task['c'].prerequisites.sort
    assert_equal %w(d1 d2).sort, Task['d'].prerequisites.sort
    assert_equal %w(e1 f1).sort, Task['e'].prerequisites.sort
    assert_equal %w(e1 f1).sort, Task['f'].prerequisites.sort
    assert_equal ["g1", "g 2", "g 3", "g4"].sort, Task['g 0'].prerequisites.sort
    assert_equal 7, Task.tasks.size
  end
end
