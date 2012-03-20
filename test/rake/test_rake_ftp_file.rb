require File.expand_path('../helper', __FILE__)
require 'date'
require 'time'
require 'rake/contrib/ftptools'

class FakeDate
  def self.today
    Date.new(2003,10,3)
  end
  def self.now
    Time.local(2003,10,3,12,00,00)
  end
end

class TestRakeFtpFile < Rake::TestCase

  def setup
    super

    Rake::FtpFile.class_eval { @date_class = FakeDate; @time_class = FakeDate }
  end

  def test_general
    file = Rake::FtpFile.new("here", "-rw-r--r--   1 a279376  develop   121770 Mar  6 14:50 wiki.pl")
    assert_equal "wiki.pl", file.name
    assert_equal "here/wiki.pl", file.path
    assert_equal "a279376", file.owner
    assert_equal "develop", file.group
    assert_equal 0644, file.mode
    assert_equal 121770, file.size
    assert_equal Time.mktime(2003,3,6,14,50,0,0), file.time
    assert ! file.directory?
    assert ! file.symlink?
  end

  def test_far_date
    file = Rake::FtpFile.new(".", "drwxr-xr-x   3 a279376  develop     4096 Nov 26  2001 vss")
    assert_equal Time.mktime(2001,11,26,0,0,0,0), file.time
  end

  def test_close_date
    file = Rake::FtpFile.new(".", "drwxr-xr-x   3 a279376  develop     4096 Nov 26 15:35 vss")
    assert_equal Time.mktime(2002,11,26,15,35,0,0), file.time
  end

  def test_directory
    file = Rake::FtpFile.new(".", "drwxrwxr-x   9 a279376  develop     4096 Mar 13 14:32 working")
    assert file.directory?
    assert !file.symlink?
  end

  def test_symlink
    file = Rake::FtpFile.new(".", "lrwxrwxrwx   1 a279376  develop       64 Mar 26  2002 xtrac -> /home/a279376/working/ics/development/java/com/fmr/fwp/ics/xtrac")
    assert_equal 'xtrac', file.name
    assert file.symlink?
    assert !file.directory?
  end
end

