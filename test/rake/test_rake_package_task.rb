require File.expand_path('../helper', __FILE__)
require 'rake/packagetask'

class TestRakePackageTask < Rake::TestCase

  def test_initialize
    touch 'install.rb'
    touch 'a.c'
    touch 'b.c'
    mkdir 'CVS'
    touch 'a.rb~'

    pkg = Rake::PackageTask.new("pkgr", "1.2.3") { |p|
      p.package_files << "install.rb"
      p.package_files.include '*.c'
      p.package_files.exclude(/\bCVS\b/)
      p.package_files.exclude(/~$/)
      p.package_dir = 'pkg'
      p.need_tar = true
      p.need_tar_gz = true
      p.need_tar_bz2 = true
      p.need_zip = true
    }

    assert_equal "pkg", pkg.package_dir

    assert_includes pkg.package_files, 'a.c'

    assert_equal 'pkgr', pkg.name
    assert_equal '1.2.3', pkg.version
    assert Rake::Task[:package]
    assert Rake::Task['pkg/pkgr-1.2.3.tgz']
    assert Rake::Task['pkg/pkgr-1.2.3.tar.gz']
    assert Rake::Task['pkg/pkgr-1.2.3.tar.bz2']
    assert Rake::Task['pkg/pkgr-1.2.3.zip']
    assert Rake::Task['pkg/pkgr-1.2.3']
    assert Rake::Task[:clobber_package]
    assert Rake::Task[:repackage]
  end

  def test_initialize_no_version
    e = assert_raises RuntimeError do
      Rake::PackageTask.new 'pkgr'
    end

    assert_equal 'Version required (or :noversion)', e.message
  end

  def test_initialize_noversion
    pkg = Rake::PackageTask.new 'pkgr', :noversion

    assert_equal 'pkg',  pkg.package_dir
    assert_equal 'pkgr', pkg.name
    assert_equal nil,    pkg.version
  end

  def test_clone
    pkg = Rake::PackageTask.new("x", :noversion)
    p2 = pkg.clone
    pkg.package_files << "y"
    p2.package_files << "x"
    assert_equal ["y"], pkg.package_files
    assert_equal ["x"], p2.package_files
  end

  def test_package_name
    pkg = Rake::PackageTask.new 'a', '1'

    assert_equal 'a-1', pkg.package_name
  end

  def test_package_name_noversion
    pkg = Rake::PackageTask.new 'a', :noversion

    assert_equal 'a', pkg.package_name
  end

end

