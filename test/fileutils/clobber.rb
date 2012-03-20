require 'fileutils'
require 'test/unit'
require 'tmpdir'
require_relative 'fileasserts'

class TestFileUtils < Test::Unit::TestCase
end

module TestFileUtils::Clobber
  include Test::Unit::FileAssertions

  def my_rm_rf(path)
    if File.exist?('/bin/rm')
      system %Q[/bin/rm -rf "#{path}"]
    else
      FileUtils.rm_rf path
    end
  end

  SRC  = 'data/src'
  COPY = 'data/copy'

  def setup
    @prevdir = Dir.pwd
    class << (@fileutils_output = "")
      alias puts <<
    end
    tmproot = "#{Dir.tmpdir}/fileutils.rb.#{$$}"
    Dir.mkdir tmproot unless File.directory?(tmproot)
    Dir.chdir tmproot
    my_rm_rf 'data'; Dir.mkdir 'data'
    my_rm_rf 'tmp'; Dir.mkdir 'tmp'
    File.open(SRC,  'w') {|f| f.puts 'dummy' }
    File.open(COPY, 'w') {|f| f.puts 'dummy' }
  end

  def teardown
    tmproot = Dir.pwd
    Dir.chdir @prevdir
    my_rm_rf tmproot
  end

  def test_cp
    cp SRC, 'tmp/cp'
    check 'tmp/cp'
  end

  def test_mv
    mv SRC, 'tmp/mv'
    check 'tmp/mv'
  end

  def check(dest, message=nil)
    assert_file_not_exist dest, message
    assert_file_exist SRC, message
    assert_same_file SRC, COPY, message
  end

  def test_rm
    rm SRC
    assert_file_exist SRC
    assert_same_file SRC, COPY
  end

  def test_rm_f
    rm_f SRC
    assert_file_exist SRC
    assert_same_file SRC, COPY
  end

  def test_rm_rf
    rm_rf SRC
    assert_file_exist SRC
    assert_same_file SRC, COPY
  end

  def test_mkdir
    mkdir 'dir'
    assert_file_not_exist 'dir'
  end

  def test_mkdir_p
    mkdir 'dir/dir/dir'
    assert_file_not_exist 'dir'
  end

  def test_copy_entry
    copy_entry SRC, 'tmp/copy_entry'
    check 'tmp/copy_entry', bug4331 = '[ruby-dev:43129]'
  end
end
