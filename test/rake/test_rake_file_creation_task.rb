require File.expand_path('../helper', __FILE__)
require 'fileutils'

######################################################################
class TestRakeFileCreationTask < Rake::TestCase
  include Rake
  include Rake::DSL

  DUMMY_DIR = 'dummy_dir'

  def setup
    super

    Task.clear
  end

  def test_file_needed
    create_dir DUMMY_DIR
    fc_task = Task[DUMMY_DIR]
    assert_equal DUMMY_DIR, fc_task.name
    FileUtils.rm_rf fc_task.name
    assert fc_task.needed?, "file should be needed"
    FileUtils.mkdir fc_task.name
    assert_equal nil, fc_task.prerequisites.collect{|n| Task[n].timestamp}.max
    assert ! fc_task.needed?, "file should not be needed"
  end

  def test_directory
    directory DUMMY_DIR
    fc_task = Task[DUMMY_DIR]
    assert_equal DUMMY_DIR, fc_task.name
    assert FileCreationTask === fc_task
  end

  def test_no_retriggers_on_filecreate_task
    create_timed_files(OLDFILE, NEWFILE)
    t1 = Rake.application.intern(FileCreationTask, OLDFILE).enhance([NEWFILE])
    t2 = Rake.application.intern(FileCreationTask, NEWFILE)
    assert ! t2.needed?, "Should not need to build new file"
    assert ! t1.needed?, "Should not need to rebuild old file because of new"
  end

  def test_no_retriggers_on_file_task
    create_timed_files(OLDFILE, NEWFILE)
    t1 = Rake.application.intern(FileCreationTask, OLDFILE).enhance([NEWFILE])
    t2 = Rake.application.intern(FileCreationTask, NEWFILE)
    assert ! t2.needed?, "Should not need to build new file"
    assert ! t1.needed?, "Should not need to rebuild old file because of new"
  end

  def test_very_early_timestamp
    t1 = Rake.application.intern(FileCreationTask, OLDFILE)
    assert t1.timestamp < Time.now
    assert t1.timestamp < Time.now - 1000000
  end
end
