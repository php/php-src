require File.expand_path('../helper', __FILE__)
require 'fileutils'

class TestRakeFileTask < Rake::TestCase
  include Rake

  def setup
    super

    Task.clear
    @runs = Array.new
    FileUtils.rm_f NEWFILE
    FileUtils.rm_f OLDFILE
  end

  def test_file_need
    name = "dummy"
    file name

    ftask = Task[name]

    assert_equal name.to_s, ftask.name
    File.delete(ftask.name) rescue nil

    assert ftask.needed?, "file should be needed"

    open(ftask.name, "w") { |f| f.puts "HI" }

    assert_equal nil, ftask.prerequisites.collect{|n| Task[n].timestamp}.max
    assert ! ftask.needed?, "file should not be needed"
  ensure
    File.delete(ftask.name) rescue nil
  end

  def test_file_times_new_depends_on_old
    create_timed_files(OLDFILE, NEWFILE)

    t1 = Rake.application.intern(FileTask, NEWFILE).enhance([OLDFILE])
    t2 = Rake.application.intern(FileTask, OLDFILE)
    assert ! t2.needed?, "Should not need to build old file"
    assert ! t1.needed?, "Should not need to rebuild new file because of old"
  end

  def test_file_times_old_depends_on_new
    create_timed_files(OLDFILE, NEWFILE)

    t1 = Rake.application.intern(FileTask,OLDFILE).enhance([NEWFILE])
    t2 = Rake.application.intern(FileTask, NEWFILE)
    assert ! t2.needed?, "Should not need to build new file"
    preq_stamp = t1.prerequisites.collect{|t| Task[t].timestamp}.max
    assert_equal t2.timestamp, preq_stamp
    assert t1.timestamp < preq_stamp, "T1 should be older"
    assert t1.needed?, "Should need to rebuild old file because of new"
  end

  def test_file_depends_on_task_depend_on_file
    create_timed_files(OLDFILE, NEWFILE)

    file NEWFILE => [:obj] do |t| @runs << t.name end
    task :obj => [OLDFILE] do |t| @runs << t.name end
    file OLDFILE           do |t| @runs << t.name end

    Task[:obj].invoke
    Task[NEWFILE].invoke
    assert ! @runs.include?(NEWFILE)
  end

  def test_existing_file_depends_on_non_existing_file
    @ran = false

    create_file(OLDFILE)
    delete_file(NEWFILE)
    file NEWFILE do
      @ran = true
    end

    file OLDFILE => NEWFILE

    Task[OLDFILE].invoke

    assert @ran
  end

  # I have currently disabled this test.  I'm not convinced that
  # deleting the file target on failure is always the proper thing to
  # do.  I'm willing to hear input on this topic.
  def ztest_file_deletes_on_failure
    task :obj
    file NEWFILE => [:obj] do |t|
      FileUtils.touch NEWFILE
      fail "Ooops"
    end
    assert Task[NEWFILE]
    begin
      Task[NEWFILE].invoke
    rescue Exception
    end
    assert( ! File.exist?(NEWFILE), "NEWFILE should be deleted")
  end

end

