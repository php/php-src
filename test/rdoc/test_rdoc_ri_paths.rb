require 'rubygems'
require 'minitest/autorun'
require 'tmpdir'
require 'fileutils'
require 'rdoc/ri/paths'

class TestRDocRIPaths < MiniTest::Unit::TestCase

  def setup
    RDoc::RI::Paths.instance_variable_set :@gemdirs, %w[/nonexistent/gemdir]
  end

  def teardown
    RDoc::RI::Paths.instance_variable_set :@gemdirs, nil
  end

  def test_class_path_nonexistent
    path = RDoc::RI::Paths.path true, true, true, true, '/nonexistent'

    refute_includes path, '/nonexistent'
  end

  def test_class_raw_path
    path = RDoc::RI::Paths.raw_path true, true, true, true

    assert_equal RDoc::RI::Paths::SYSDIR,  path.shift
    assert_equal RDoc::RI::Paths::SITEDIR, path.shift
    assert_equal RDoc::RI::Paths::HOMEDIR, path.shift
    assert_equal '/nonexistent/gemdir',    path.shift
  end

  def test_class_raw_path_extra_dirs
    path = RDoc::RI::Paths.raw_path true, true, true, true, '/nonexistent'

    assert_equal '/nonexistent',           path.shift
    assert_equal RDoc::RI::Paths::SYSDIR,  path.shift
    assert_equal RDoc::RI::Paths::SITEDIR, path.shift
    assert_equal RDoc::RI::Paths::HOMEDIR, path.shift
    assert_equal '/nonexistent/gemdir',    path.shift
  end

end

