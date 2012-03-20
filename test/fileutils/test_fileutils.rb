# $Id$

require 'fileutils'
require_relative 'fileasserts'
require 'pathname'
require 'tmpdir'
require 'test/unit'

class TestFileUtils < Test::Unit::TestCase
  TMPROOT = "#{Dir.tmpdir}/fileutils.rb.#{$$}"
  include Test::Unit::FileAssertions
end

prevdir = Dir.pwd
tmproot = TestFileUtils::TMPROOT
Dir.mkdir tmproot unless File.directory?(tmproot)
Dir.chdir tmproot

def have_drive_letter?
  /mswin(?!ce)|mingw|bcc|emx/ =~ RUBY_PLATFORM
end

def have_file_perm?
  /mswin|mingw|bcc|emx/ !~ RUBY_PLATFORM
end

$fileutils_rb_have_symlink = nil

def have_symlink?
  if $fileutils_rb_have_symlink == nil
    $fileutils_rb_have_symlink = check_have_symlink?
  end
  $fileutils_rb_have_symlink
end

def check_have_symlink?
  File.symlink nil, nil
rescue NotImplementedError
  return false
rescue
  return true
end

$fileutils_rb_have_hardlink = nil

def have_hardlink?
  if $fileutils_rb_have_hardlink == nil
    $fileutils_rb_have_hardlink = check_have_hardlink?
  end
  $fileutils_rb_have_hardlink
end

def check_have_hardlink?
  File.link nil, nil
rescue NotImplementedError
  return false
rescue
  return true
end

begin
  Dir.mkdir("\n")
  Dir.rmdir("\n")
  def lf_in_path_allowed?
    true
  end
rescue
  def lf_in_path_allowed?
    false
  end
end

Dir.chdir prevdir
Dir.rmdir tmproot

class TestFileUtils

  include FileUtils

  def check_singleton(name)
    assert_respond_to ::FileUtils, name
  end

  def my_rm_rf(path)
    if File.exist?('/bin/rm')
      system %Q[/bin/rm -rf "#{path}"]
    else
      FileUtils.rm_rf path
    end
  end

  def mymkdir(path)
    Dir.mkdir path
    File.chown nil, Process.gid, path if have_file_perm?
  end

  def setup
    @prevdir = Dir.pwd
    tmproot = TMPROOT
    mymkdir tmproot unless File.directory?(tmproot)
    Dir.chdir tmproot
    my_rm_rf 'data'; mymkdir 'data'
    my_rm_rf 'tmp';  mymkdir 'tmp'
    prepare_data_file
  end

  def teardown
    tmproot = Dir.pwd
    Dir.chdir @prevdir
    my_rm_rf tmproot
  end


  TARGETS = %w( data/a data/all data/random data/zero )

  def prepare_data_file
    File.open('data/a', 'w') {|f|
      32.times do
        f.puts 'a' * 50
      end
    }

    all_chars = (0..255).map {|n| n.chr }.join('')
    File.open('data/all', 'w') {|f|
      32.times do
        f.puts all_chars
      end
    }

    random_chars = (0...50).map { rand(256).chr }.join('')
    File.open('data/random', 'w') {|f|
      32.times do
        f.puts random_chars
      end
    }

    File.open('data/zero', 'w') {|f|
      ;
    }
  end

  BIGFILE = 'data/big'

  def prepare_big_file
    File.open('data/big', 'w') {|f|
      (4 * 1024 * 1024 / 256).times do   # 4MB
        f.print "aaaa aaaa aaaa aaaa aaaa aaaa aaaa aaaa aaaa aaaa\n"
      end
    }
  end

  def prepare_time_data
    File.open('data/old',    'w') {|f| f.puts 'dummy' }
    File.open('data/newer',  'w') {|f| f.puts 'dummy' }
    File.open('data/newest', 'w') {|f| f.puts 'dummy' }
    t = Time.now
    File.utime t-8, t-8, 'data/old'
    File.utime t-4, t-4, 'data/newer'
  end

  def each_srcdest
    TARGETS.each do |path|
      yield path, "tmp/#{File.basename(path)}"
    end
  end

  #
  # Test Cases
  #

  def test_pwd
    check_singleton :pwd

    assert_equal Dir.pwd, pwd()

    cwd = Dir.pwd
    root = have_drive_letter? ? 'C:/' : '/'
    cd(root) {
      assert_equal root, pwd()
    }
    assert_equal cwd, pwd()
  end

  def test_cmp
    check_singleton :cmp

    TARGETS.each do |fname|
      assert cmp(fname, fname), 'not same?'
    end
    assert_raise(ArgumentError) {
      cmp TARGETS[0], TARGETS[0], :undefinedoption => true
    }

    # pathname
    touch 'tmp/cmptmp'
    assert_nothing_raised {
      cmp Pathname.new('tmp/cmptmp'), 'tmp/cmptmp'
      cmp 'tmp/cmptmp', Pathname.new('tmp/cmptmp')
      cmp Pathname.new('tmp/cmptmp'), Pathname.new('tmp/cmptmp')
    }
  end

  def test_cp
    check_singleton :cp

    each_srcdest do |srcpath, destpath|
      cp srcpath, destpath
      assert_same_file srcpath, destpath

      cp srcpath, File.dirname(destpath)
      assert_same_file srcpath, destpath

      cp srcpath, File.dirname(destpath) + '/'
      assert_same_file srcpath, destpath

      cp srcpath, destpath, :preserve => true
      assert_same_file srcpath, destpath
      assert_same_entry srcpath, destpath
    end

    assert_raise(Errno::ENOENT) {
      cp 'tmp/cptmp', 'tmp/cptmp_new'
    }
    assert_file_not_exist('tmp/cptmp_new')

    # src==dest (1) same path
    touch 'tmp/cptmp'
    assert_raise(ArgumentError) {
      cp 'tmp/cptmp', 'tmp/cptmp'
    }
  end

  def test_cp_preserve_permissions
    bug4507 = '[ruby-core:35518]'
    touch 'tmp/cptmp'
    chmod 0755, 'tmp/cptmp'
    cp 'tmp/cptmp', 'tmp/cptmp2'
    assert_equal(File.stat('tmp/cptmp').mode,
                 File.stat('tmp/cptmp2').mode,
                 bug4507)
  end

  def test_cp_symlink
    touch 'tmp/cptmp'
    # src==dest (2) symlink and its target
    File.symlink 'cptmp', 'tmp/cptmp_symlink'
    assert_raise(ArgumentError) {
      cp 'tmp/cptmp', 'tmp/cptmp_symlink'
    }
    assert_raise(ArgumentError) {
      cp 'tmp/cptmp_symlink', 'tmp/cptmp'
    }
    # src==dest (3) looped symlink
    File.symlink 'symlink', 'tmp/symlink'
    assert_raise(Errno::ELOOP) {
      cp 'tmp/symlink', 'tmp/symlink'
    }
  end if have_symlink?

  def test_cp_pathname
    # pathname
    touch 'tmp/cptmp'
    assert_nothing_raised {
      cp 'tmp/cptmp', Pathname.new('tmp/tmpdest')
      cp Pathname.new('tmp/cptmp'), 'tmp/tmpdest'
      cp Pathname.new('tmp/cptmp'), Pathname.new('tmp/tmpdest')
      mkdir 'tmp/tmpdir'
      cp ['tmp/cptmp', 'tmp/tmpdest'], Pathname.new('tmp/tmpdir')
    }
  end

  def test_cp_r
    check_singleton :cp_r

    cp_r 'data', 'tmp'
    TARGETS.each do |fname|
      assert_same_file fname, "tmp/#{fname}"
    end

    cp_r 'data', 'tmp2', :preserve => true
    TARGETS.each do |fname|
      assert_same_entry fname, "tmp2/#{File.basename(fname)}"
      assert_same_file fname, "tmp2/#{File.basename(fname)}"
    end

    # a/* -> b/*
    mkdir 'tmp/cpr_src'
    mkdir 'tmp/cpr_dest'
    File.open('tmp/cpr_src/a', 'w') {|f| f.puts 'a' }
    File.open('tmp/cpr_src/b', 'w') {|f| f.puts 'b' }
    File.open('tmp/cpr_src/c', 'w') {|f| f.puts 'c' }
    mkdir 'tmp/cpr_src/d'
    cp_r 'tmp/cpr_src/.', 'tmp/cpr_dest'
    assert_same_file 'tmp/cpr_src/a', 'tmp/cpr_dest/a'
    assert_same_file 'tmp/cpr_src/b', 'tmp/cpr_dest/b'
    assert_same_file 'tmp/cpr_src/c', 'tmp/cpr_dest/c'
    assert_directory 'tmp/cpr_dest/d'
    my_rm_rf 'tmp/cpr_src'
    my_rm_rf 'tmp/cpr_dest'

    bug3588 = '[ruby-core:31360]'
    assert_nothing_raised(ArgumentError, bug3588) do
      cp_r 'tmp', 'tmp2'
    end
    assert_directory 'tmp2/tmp'
    assert_raise(ArgumentError, bug3588) do
      cp_r 'tmp2', 'tmp2/new_tmp2'
    end
  end

  def test_cp_r_symlink
    # symlink in a directory
    mkdir 'tmp/cpr_src'
    ln_s 'SLdest', 'tmp/cpr_src/symlink'
    cp_r 'tmp/cpr_src', 'tmp/cpr_dest'
    assert_symlink 'tmp/cpr_dest/symlink'
    assert_equal 'SLdest', File.readlink('tmp/cpr_dest/symlink')

    # root is a symlink
    ln_s 'cpr_src', 'tmp/cpr_src2'
    cp_r 'tmp/cpr_src2', 'tmp/cpr_dest2'
    assert_directory 'tmp/cpr_dest2'
    assert_not_symlink 'tmp/cpr_dest2'
    assert_symlink 'tmp/cpr_dest2/symlink'
    assert_equal 'SLdest', File.readlink('tmp/cpr_dest2/symlink')
  end if have_symlink?

  def test_cp_r_pathname
    # pathname
    touch 'tmp/cprtmp'
    assert_nothing_raised {
      cp_r Pathname.new('tmp/cprtmp'), 'tmp/tmpdest'
      cp_r 'tmp/cprtmp', Pathname.new('tmp/tmpdest')
      cp_r Pathname.new('tmp/cprtmp'), Pathname.new('tmp/tmpdest')
    }
  end

  def test_mv
    check_singleton :mv

    mkdir 'tmp/dest'
    TARGETS.each do |fname|
      cp fname, 'tmp/mvsrc'
      mv 'tmp/mvsrc', 'tmp/mvdest'
      assert_same_file fname, 'tmp/mvdest'

      mv 'tmp/mvdest', 'tmp/dest/'
      assert_same_file fname, 'tmp/dest/mvdest'

      mv 'tmp/dest/mvdest', 'tmp'
      assert_same_file fname, 'tmp/mvdest'
    end

    mkdir 'tmp/tmpdir'
    mkdir_p 'tmp/dest2/tmpdir'
    assert_raise(Errno::EEXIST) {
      mv 'tmp/tmpdir', 'tmp/dest2'
    }
    mkdir 'tmp/dest2/tmpdir/junk'
    assert_raise(Errno::EEXIST, "[ruby-talk:124368]") {
      mv 'tmp/tmpdir', 'tmp/dest2'
    }

    # src==dest (1) same path
    touch 'tmp/cptmp'
    assert_raise(ArgumentError) {
      mv 'tmp/cptmp', 'tmp/cptmp'
    }
  end

  def test_mv_symlink
    touch 'tmp/cptmp'
    # src==dest (2) symlink and its target
    File.symlink 'cptmp', 'tmp/cptmp_symlink'
    assert_raise(ArgumentError) {
      mv 'tmp/cptmp', 'tmp/cptmp_symlink'
    }
    assert_raise(ArgumentError) {
      mv 'tmp/cptmp_symlink', 'tmp/cptmp'
    }
    # src==dest (3) looped symlink
    File.symlink 'symlink', 'tmp/symlink'
    assert_raise(Errno::ELOOP) {
      mv 'tmp/symlink', 'tmp/symlink'
    }
  end if have_symlink?

  def test_mv_pathname
    # pathname
    assert_nothing_raised {
      touch 'tmp/mvtmpsrc'
      mv Pathname.new('tmp/mvtmpsrc'), 'tmp/mvtmpdest'
      touch 'tmp/mvtmpsrc'
      mv 'tmp/mvtmpsrc', Pathname.new('tmp/mvtmpdest')
      touch 'tmp/mvtmpsrc'
      mv Pathname.new('tmp/mvtmpsrc'), Pathname.new('tmp/mvtmpdest')
    }
  end

  def test_rm
    check_singleton :rm

    TARGETS.each do |fname|
      cp fname, 'tmp/rmsrc'
      rm 'tmp/rmsrc'
      assert_file_not_exist 'tmp/rmsrc'
    end

    # pathname
    touch 'tmp/rmtmp1'
    touch 'tmp/rmtmp2'
    touch 'tmp/rmtmp3'
    assert_nothing_raised {
      rm Pathname.new('tmp/rmtmp1')
      rm [Pathname.new('tmp/rmtmp2'), Pathname.new('tmp/rmtmp3')]
    }
    assert_file_not_exist 'tmp/rmtmp1'
    assert_file_not_exist 'tmp/rmtmp2'
    assert_file_not_exist 'tmp/rmtmp3'
  end

  def test_rm_f
    check_singleton :rm_f

    TARGETS.each do |fname|
      cp fname, 'tmp/rmsrc'
      rm_f 'tmp/rmsrc'
      assert_file_not_exist 'tmp/rmsrc'
    end
  end

  def test_rm_symlink
    File.open('tmp/lnf_symlink_src', 'w') {|f| f.puts 'dummy' }
    File.symlink 'tmp/lnf_symlink_src', 'tmp/lnf_symlink_dest'
    rm_f 'tmp/lnf_symlink_dest'
    assert_file_not_exist 'tmp/lnf_symlink_dest'
    assert_file_exist     'tmp/lnf_symlink_src'

    rm_f 'notexistdatafile'
    rm_f 'tmp/notexistdatafile'
    my_rm_rf 'tmpdatadir'
    Dir.mkdir 'tmpdatadir'
    # rm_f 'tmpdatadir'
    Dir.rmdir 'tmpdatadir'
  end if have_symlink?

  def test_rm_f_2
    Dir.mkdir 'tmp/tmpdir'
    File.open('tmp/tmpdir/a', 'w') {|f| f.puts 'dummy' }
    File.open('tmp/tmpdir/c', 'w') {|f| f.puts 'dummy' }
    rm_f ['tmp/tmpdir/a', 'tmp/tmpdir/b', 'tmp/tmpdir/c']
    assert_file_not_exist 'tmp/tmpdir/a'
    assert_file_not_exist 'tmp/tmpdir/c'
    Dir.rmdir 'tmp/tmpdir'
  end

  def test_rm_pathname
    # pathname
    touch 'tmp/rmtmp1'
    touch 'tmp/rmtmp2'
    touch 'tmp/rmtmp3'
    touch 'tmp/rmtmp4'
    assert_nothing_raised {
      rm_f Pathname.new('tmp/rmtmp1')
      rm_f [Pathname.new('tmp/rmtmp2'), Pathname.new('tmp/rmtmp3')]
    }
    assert_file_not_exist 'tmp/rmtmp1'
    assert_file_not_exist 'tmp/rmtmp2'
    assert_file_not_exist 'tmp/rmtmp3'
    assert_file_exist 'tmp/rmtmp4'

    # [ruby-dev:39345]
    touch 'tmp/[rmtmp]'
    FileUtils.rm_f 'tmp/[rmtmp]'
    assert_file_not_exist 'tmp/[rmtmp]'
  end

  def test_rm_r
    check_singleton :rm_r

    my_rm_rf 'tmpdatadir'

    Dir.mkdir 'tmpdatadir'
    rm_r 'tmpdatadir'
    assert_file_not_exist 'tmpdatadir'

    Dir.mkdir 'tmpdatadir'
    rm_r 'tmpdatadir/'
    assert_file_not_exist 'tmpdatadir'

    Dir.mkdir 'tmp/tmpdir'
    rm_r 'tmp/tmpdir/'
    assert_file_not_exist 'tmp/tmpdir'
    assert_file_exist     'tmp'

    Dir.mkdir 'tmp/tmpdir'
    rm_r 'tmp/tmpdir'
    assert_file_not_exist 'tmp/tmpdir'
    assert_file_exist     'tmp'

    Dir.mkdir 'tmp/tmpdir'
    File.open('tmp/tmpdir/a', 'w') {|f| f.puts 'dummy' }
    File.open('tmp/tmpdir/b', 'w') {|f| f.puts 'dummy' }
    File.open('tmp/tmpdir/c', 'w') {|f| f.puts 'dummy' }
    rm_r 'tmp/tmpdir'
    assert_file_not_exist 'tmp/tmpdir'
    assert_file_exist     'tmp'

    Dir.mkdir 'tmp/tmpdir'
    File.open('tmp/tmpdir/a', 'w') {|f| f.puts 'dummy' }
    File.open('tmp/tmpdir/c', 'w') {|f| f.puts 'dummy' }
    rm_r ['tmp/tmpdir/a', 'tmp/tmpdir/b', 'tmp/tmpdir/c'], :force => true
    assert_file_not_exist 'tmp/tmpdir/a'
    assert_file_not_exist 'tmp/tmpdir/c'
    Dir.rmdir 'tmp/tmpdir'
  end

  def test_rm_r_symlink
    # [ruby-talk:94635] a symlink to the directory
    Dir.mkdir 'tmp/tmpdir'
    File.symlink '..', 'tmp/tmpdir/symlink_to_dir'
    rm_r 'tmp/tmpdir'
    assert_file_not_exist 'tmp/tmpdir'
    assert_file_exist     'tmp'
  end if have_symlink?

  def test_rm_r_pathname
    # pathname
    Dir.mkdir 'tmp/tmpdir1'; touch 'tmp/tmpdir1/tmp'
    Dir.mkdir 'tmp/tmpdir2'; touch 'tmp/tmpdir2/tmp'
    Dir.mkdir 'tmp/tmpdir3'; touch 'tmp/tmpdir3/tmp'
    assert_nothing_raised {
      rm_r Pathname.new('tmp/tmpdir1')
      rm_r [Pathname.new('tmp/tmpdir2'), Pathname.new('tmp/tmpdir3')]
    }
    assert_file_not_exist 'tmp/tmpdir1'
    assert_file_not_exist 'tmp/tmpdir2'
    assert_file_not_exist 'tmp/tmpdir3'
  end

  def test_remove_entry_secure
    check_singleton :remove_entry_secure

    my_rm_rf 'tmpdatadir'

    Dir.mkdir 'tmpdatadir'
    remove_entry_secure 'tmpdatadir'
    assert_file_not_exist 'tmpdatadir'

    Dir.mkdir 'tmpdatadir'
    remove_entry_secure 'tmpdatadir/'
    assert_file_not_exist 'tmpdatadir'

    Dir.mkdir 'tmp/tmpdir'
    remove_entry_secure 'tmp/tmpdir/'
    assert_file_not_exist 'tmp/tmpdir'
    assert_file_exist     'tmp'

    Dir.mkdir 'tmp/tmpdir'
    remove_entry_secure 'tmp/tmpdir'
    assert_file_not_exist 'tmp/tmpdir'
    assert_file_exist     'tmp'

    Dir.mkdir 'tmp/tmpdir'
    File.open('tmp/tmpdir/a', 'w') {|f| f.puts 'dummy' }
    File.open('tmp/tmpdir/b', 'w') {|f| f.puts 'dummy' }
    File.open('tmp/tmpdir/c', 'w') {|f| f.puts 'dummy' }
    remove_entry_secure 'tmp/tmpdir'
    assert_file_not_exist 'tmp/tmpdir'
    assert_file_exist     'tmp'

    Dir.mkdir 'tmp/tmpdir'
    File.open('tmp/tmpdir/a', 'w') {|f| f.puts 'dummy' }
    File.open('tmp/tmpdir/c', 'w') {|f| f.puts 'dummy' }
    remove_entry_secure 'tmp/tmpdir/a', true
    remove_entry_secure 'tmp/tmpdir/b', true
    remove_entry_secure 'tmp/tmpdir/c', true
    assert_file_not_exist 'tmp/tmpdir/a'
    assert_file_not_exist 'tmp/tmpdir/c'
    Dir.rmdir 'tmp/tmpdir'
  end

  def test_remove_entry_secure_symlink
    # [ruby-talk:94635] a symlink to the directory
    Dir.mkdir 'tmp/tmpdir'
    File.symlink '..', 'tmp/tmpdir/symlink_to_dir'
    remove_entry_secure 'tmp/tmpdir'
    assert_file_not_exist 'tmp/tmpdir'
    assert_file_exist     'tmp'
  end if have_symlink?

  def test_remove_entry_secure_pathname
    # pathname
    Dir.mkdir 'tmp/tmpdir1'; touch 'tmp/tmpdir1/tmp'
    assert_nothing_raised {
      remove_entry_secure Pathname.new('tmp/tmpdir1')
    }
    assert_file_not_exist 'tmp/tmpdir1'
  end

  def test_with_big_file
    prepare_big_file

    cp BIGFILE, 'tmp/cpdest'
    assert_same_file BIGFILE, 'tmp/cpdest'
    assert cmp(BIGFILE, 'tmp/cpdest'), 'orig != copied'

    mv 'tmp/cpdest', 'tmp/mvdest'
    assert_same_file BIGFILE, 'tmp/mvdest'
    assert_file_not_exist 'tmp/cpdest'

    rm 'tmp/mvdest'
    assert_file_not_exist 'tmp/mvdest'
  end

  def test_ln
    TARGETS.each do |fname|
      ln fname, 'tmp/lndest'
      assert_same_file fname, 'tmp/lndest'
      File.unlink 'tmp/lndest'
    end

    ln TARGETS, 'tmp'
    TARGETS.each do |fname|
      assert_same_file fname, 'tmp/' + File.basename(fname)
    end
    TARGETS.each do |fname|
      File.unlink 'tmp/' + File.basename(fname)
    end

    # src==dest (1) same path
    touch 'tmp/cptmp'
    assert_raise(Errno::EEXIST) {
      ln 'tmp/cptmp', 'tmp/cptmp'
    }
  end if have_hardlink?

  def test_ln_symlink
    touch 'tmp/cptmp'
    # src==dest (2) symlink and its target
    File.symlink 'cptmp', 'tmp/symlink'
    assert_raise(Errno::EEXIST) {
      ln 'tmp/cptmp', 'tmp/symlink'   # normal file -> symlink
    }
    assert_raise(Errno::EEXIST) {
      ln 'tmp/symlink', 'tmp/cptmp'   # symlink -> normal file
    }
    # src==dest (3) looped symlink
    File.symlink 'cptmp_symlink', 'tmp/cptmp_symlink'
    begin
      ln 'tmp/cptmp_symlink', 'tmp/cptmp_symlink'
    rescue => err
      assert_kind_of SystemCallError, err
    end
  end if have_symlink?

  def test_ln_pathname
    # pathname
    touch 'tmp/lntmp'
    assert_nothing_raised {
      ln Pathname.new('tmp/lntmp'), 'tmp/lndesttmp1'
      ln 'tmp/lntmp', Pathname.new('tmp/lndesttmp2')
      ln Pathname.new('tmp/lntmp'), Pathname.new('tmp/lndesttmp3')
    }
  end if have_hardlink?

  def test_ln_s
    check_singleton :ln_s

    TARGETS.each do |fname|
      ln_s fname, 'tmp/lnsdest'
      assert FileTest.symlink?('tmp/lnsdest'), 'not symlink'
      assert_equal fname, File.readlink('tmp/lnsdest')
      rm_f 'tmp/lnsdest'
    end
    assert_nothing_raised {
      ln_s 'symlink', 'tmp/symlink'
    }
    assert_symlink 'tmp/symlink'

    # pathname
    touch 'tmp/lnsdest'
    assert_nothing_raised {
      ln_s Pathname.new('lnsdest'), 'tmp/symlink_tmp1'
      ln_s 'lnsdest', Pathname.new('tmp/symlink_tmp2')
      ln_s Pathname.new('lnsdest'), Pathname.new('tmp/symlink_tmp3')
    }
  end if have_symlink?

  def test_ln_sf
    check_singleton :ln_sf

    TARGETS.each do |fname|
      ln_sf fname, 'tmp/lnsdest'
      assert FileTest.symlink?('tmp/lnsdest'), 'not symlink'
      assert_equal fname, File.readlink('tmp/lnsdest')
      ln_sf fname, 'tmp/lnsdest'
      ln_sf fname, 'tmp/lnsdest'
    end
    assert_nothing_raised {
      ln_sf 'symlink', 'tmp/symlink'
    }

    # pathname
    touch 'tmp/lns_dest'
    assert_nothing_raised {
      ln_sf Pathname.new('lns_dest'), 'tmp/symlink_tmp1'
      ln_sf 'lns_dest', Pathname.new('tmp/symlink_tmp2')
      ln_sf Pathname.new('lns_dest'), Pathname.new('tmp/symlink_tmp3')
    }
  end if have_symlink?

  def test_mkdir
    check_singleton :mkdir

    my_rm_rf 'tmpdatadir'
    mkdir 'tmpdatadir'
    assert_directory 'tmpdatadir'
    Dir.rmdir 'tmpdatadir'

    mkdir 'tmpdatadir/'
    assert_directory 'tmpdatadir'
    Dir.rmdir 'tmpdatadir'

    mkdir 'tmp/mkdirdest'
    assert_directory 'tmp/mkdirdest'
    Dir.rmdir 'tmp/mkdirdest'

    mkdir 'tmp/tmp', :mode => 0700
    assert_directory 'tmp/tmp'
    assert_equal 0700, (File.stat('tmp/tmp').mode & 0777) if have_file_perm?
    Dir.rmdir 'tmp/tmp'
  end

  def test_mkdir_file_perm
    mkdir 'tmp/tmp', :mode => 07777
    assert_directory 'tmp/tmp'
    assert_equal 07777, (File.stat('tmp/tmp').mode & 07777)
    Dir.rmdir 'tmp/tmp'
  end if have_file_perm?

  def test_mkdir_lf_in_path
    mkdir "tmp-first-line\ntmp-second-line"
    assert_directory "tmp-first-line\ntmp-second-line"
    Dir.rmdir "tmp-first-line\ntmp-second-line"
  end if lf_in_path_allowed?

  def test_mkdir_pathname
    # pathname
    assert_nothing_raised {
      mkdir Pathname.new('tmp/tmpdirtmp')
      mkdir [Pathname.new('tmp/tmpdirtmp2'), Pathname.new('tmp/tmpdirtmp3')]
    }
  end

  def test_mkdir_p
    check_singleton :mkdir_p

    dirs = %w(
      tmpdir/dir/
      tmpdir/dir/./
      tmpdir/dir/./.././dir/
      tmpdir/a
      tmpdir/a/
      tmpdir/a/b
      tmpdir/a/b/
      tmpdir/a/b/c/
      tmpdir/a/b/c
      tmpdir/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a/a
      tmpdir/a/a
    )
    my_rm_rf 'tmpdir'
    dirs.each do |d|
      mkdir_p d
      assert_directory d
      assert_file_not_exist "#{d}/a"
      assert_file_not_exist "#{d}/b"
      assert_file_not_exist "#{d}/c"
      my_rm_rf 'tmpdir'
    end
    dirs.each do |d|
      mkdir_p d
      assert_directory d
    end
    rm_rf 'tmpdir'
    dirs.each do |d|
      mkdir_p "#{Dir.pwd}/#{d}"
      assert_directory d
    end
    rm_rf 'tmpdir'

    mkdir_p 'tmp/tmp/tmp', :mode => 0700
    assert_directory 'tmp/tmp'
    assert_directory 'tmp/tmp/tmp'
    assert_equal 0700, (File.stat('tmp/tmp').mode & 0777) if have_file_perm?
    assert_equal 0700, (File.stat('tmp/tmp/tmp').mode & 0777) if have_file_perm?
    rm_rf 'tmp/tmp'

    mkdir_p 'tmp/tmp', :mode => 0
    assert_directory 'tmp/tmp'
    assert_equal 0, (File.stat('tmp/tmp').mode & 0777) if have_file_perm?
    # DO NOT USE rm_rf here.
    # (rm(1) try to chdir to parent directory, it fails to remove directory.)
    Dir.rmdir 'tmp/tmp'
    Dir.rmdir 'tmp'
  end

  def test_mkdir_p_file_perm
    mkdir_p 'tmp/tmp/tmp', :mode => 07777
    assert_directory 'tmp/tmp/tmp'
    assert_equal 07777, (File.stat('tmp/tmp/tmp').mode & 07777)
    Dir.rmdir 'tmp/tmp/tmp'
    Dir.rmdir 'tmp/tmp'
  end if have_file_perm?

  def test_mkdir_p_pathname
    # pathname
    assert_nothing_raised {
      mkdir_p Pathname.new('tmp/tmp/tmp')
    }
  end

  def test_install
    check_singleton :install

    File.open('tmp/aaa', 'w') {|f| f.puts 'aaa' }
    File.open('tmp/bbb', 'w') {|f| f.puts 'bbb' }
    install 'tmp/aaa', 'tmp/bbb', :mode => 0600
    assert_equal "aaa\n", File.read('tmp/bbb')
    assert_equal 0600, (File.stat('tmp/bbb').mode & 0777) if have_file_perm?

    t = File.mtime('tmp/bbb')
    install 'tmp/aaa', 'tmp/bbb'
    assert_equal "aaa\n", File.read('tmp/bbb')
    assert_equal 0600, (File.stat('tmp/bbb').mode & 0777) if have_file_perm?
    assert_equal_time t, File.mtime('tmp/bbb')

    File.unlink 'tmp/aaa'
    File.unlink 'tmp/bbb'

    # src==dest (1) same path
    touch 'tmp/cptmp'
    assert_raise(ArgumentError) {
      install 'tmp/cptmp', 'tmp/cptmp'
    }
  end

  def test_install_symlink
    touch 'tmp/cptmp'
    # src==dest (2) symlink and its target
    File.symlink 'cptmp', 'tmp/cptmp_symlink'
    assert_raise(ArgumentError) {
      install 'tmp/cptmp', 'tmp/cptmp_symlink'
    }
    assert_raise(ArgumentError) {
      install 'tmp/cptmp_symlink', 'tmp/cptmp'
    }
    # src==dest (3) looped symlink
    File.symlink 'symlink', 'tmp/symlink'
    assert_raise(Errno::ELOOP) {
      # File#install invokes open(2), always ELOOP must be raised
      install 'tmp/symlink', 'tmp/symlink'
    }
  end if have_symlink?

  def test_install_pathname
    # pathname
    assert_nothing_raised {
      rm_f 'tmp/a'; touch 'tmp/a'
      install 'tmp/a', Pathname.new('tmp/b')
      rm_f 'tmp/a'; touch 'tmp/a'
      install Pathname.new('tmp/a'), 'tmp/b'
      rm_f 'tmp/a'; touch 'tmp/a'
      install Pathname.new('tmp/a'), Pathname.new('tmp/b')
      rm_f 'tmp/a'
      touch 'tmp/a'
      touch 'tmp/b'
      mkdir 'tmp/dest'
      install [Pathname.new('tmp/a'), Pathname.new('tmp/b')], 'tmp/dest'
      my_rm_rf 'tmp/dest'
      mkdir 'tmp/dest'
      install [Pathname.new('tmp/a'), Pathname.new('tmp/b')], Pathname.new('tmp/dest')
    }
  end

  def test_chmod
    check_singleton :chmod

    touch 'tmp/a'
    chmod 0700, 'tmp/a'
    assert_equal 0700, File.stat('tmp/a').mode & 0777
    chmod 0500, 'tmp/a'
    assert_equal 0500, File.stat('tmp/a').mode & 0777
  end if have_file_perm?

  def test_chmod_symbol_mode
    check_singleton :chmod

    touch 'tmp/a'
    chmod "u=wrx,g=,o=", 'tmp/a'
    assert_equal 0700, File.stat('tmp/a').mode & 0777
    chmod "u=rx,go=", 'tmp/a'
    assert_equal 0500, File.stat('tmp/a').mode & 0777
    chmod "+wrx", 'tmp/a'
    assert_equal 0777, File.stat('tmp/a').mode & 0777
    chmod "u+s,o=s", 'tmp/a'
    assert_equal 04770, File.stat('tmp/a').mode & 07777
    chmod "u-w,go-wrx", 'tmp/a'
    assert_equal 04500, File.stat('tmp/a').mode & 07777
    chmod "+s", 'tmp/a'
    assert_equal 06500, File.stat('tmp/a').mode & 07777

    # FreeBSD ufs and tmpfs don't allow to change sticky bit against
    # regular file. It's slightly strange. Anyway it's no effect bit.
    # see /usr/src/sys/ufs/ufs/ufs_chmod()
    # NetBSD, OpenBSD and Solaris also denies it.
    if /freebsd|netbsd|openbsd|solaris/ !~ RUBY_PLATFORM
      chmod "u+t,o+t", 'tmp/a'
      assert_equal 07500, File.stat('tmp/a').mode & 07777
      chmod "a-t,a-s", 'tmp/a'
      assert_equal 0500, File.stat('tmp/a').mode & 07777
    end

  end if have_file_perm?


  def test_chmod_R
    check_singleton :chmod_R

    mkdir_p 'tmp/dir/dir'
    touch %w( tmp/dir/file tmp/dir/dir/file )
    chmod_R 0700, 'tmp/dir'
    assert_equal 0700, File.stat('tmp/dir').mode & 0777
    assert_equal 0700, File.stat('tmp/dir/file').mode & 0777
    assert_equal 0700, File.stat('tmp/dir/dir').mode & 0777
    assert_equal 0700, File.stat('tmp/dir/dir/file').mode & 0777
    chmod_R 0500, 'tmp/dir'
    assert_equal 0500, File.stat('tmp/dir').mode & 0777
    assert_equal 0500, File.stat('tmp/dir/file').mode & 0777
    assert_equal 0500, File.stat('tmp/dir/dir').mode & 0777
    assert_equal 0500, File.stat('tmp/dir/dir/file').mode & 0777
    chmod_R 0700, 'tmp/dir'   # to remove
  end if have_file_perm?

  def test_chmod_symbol_mode_R
    check_singleton :chmod_R

    mkdir_p 'tmp/dir/dir'
    touch %w( tmp/dir/file tmp/dir/dir/file )
    chmod_R "u=wrx,g=,o=", 'tmp/dir'
    assert_equal 0700, File.stat('tmp/dir').mode & 0777
    assert_equal 0700, File.stat('tmp/dir/file').mode & 0777
    assert_equal 0700, File.stat('tmp/dir/dir').mode & 0777
    assert_equal 0700, File.stat('tmp/dir/dir/file').mode & 0777
    chmod_R "u=xr,g+X,o=", 'tmp/dir'
    assert_equal 0510, File.stat('tmp/dir').mode & 0777
    assert_equal 0500, File.stat('tmp/dir/file').mode & 0777
    assert_equal 0510, File.stat('tmp/dir/dir').mode & 0777
    assert_equal 0500, File.stat('tmp/dir/dir/file').mode & 0777
    chmod_R 0700, 'tmp/dir'   # to remove
  end if have_file_perm?

  # FIXME: How can I test this method?
  def test_chown
    check_singleton :chown
  end if have_file_perm?

  # FIXME: How can I test this method?
  def test_chown_R
    check_singleton :chown_R
  end if have_file_perm?

  def test_copy_entry
    check_singleton :copy_entry

    each_srcdest do |srcpath, destpath|
      copy_entry srcpath, destpath
      assert_same_file srcpath, destpath
      assert_equal File.stat(srcpath).ftype, File.stat(destpath).ftype
    end
  end

  def test_copy_entry_symlink
    # root is a symlink
    File.symlink 'somewhere', 'tmp/symsrc'
    copy_entry 'tmp/symsrc', 'tmp/symdest'
    assert_symlink 'tmp/symdest'
    assert_equal 'somewhere', File.readlink('tmp/symdest')

    # content is a symlink
    mkdir 'tmp/dir'
    File.symlink 'somewhere', 'tmp/dir/sym'
    copy_entry 'tmp/dir', 'tmp/dirdest'
    assert_directory 'tmp/dirdest'
    assert_not_symlink 'tmp/dirdest'
    assert_symlink 'tmp/dirdest/sym'
    assert_equal 'somewhere', File.readlink('tmp/dirdest/sym')
  end if have_symlink?

  def test_copy_file
    check_singleton :copy_file

    each_srcdest do |srcpath, destpath|
      copy_file srcpath, destpath
      assert_same_file srcpath, destpath
    end
  end

  def test_copy_stream
    check_singleton :copy_stream
    # IO
    each_srcdest do |srcpath, destpath|
      File.open(srcpath, 'rb') {|src|
        File.open(destpath, 'wb') {|dest|
          copy_stream src, dest
        }
      }
      assert_same_file srcpath, destpath
    end
  end

  def test_copy_stream_duck
    check_singleton :copy_stream
    # duck typing test  [ruby-dev:25369]
    each_srcdest do |srcpath, destpath|
      File.open(srcpath, 'rb') {|src|
        File.open(destpath, 'wb') {|dest|
          copy_stream Stream.new(src), Stream.new(dest)
        }
      }
      assert_same_file srcpath, destpath
    end
  end

  def test_remove_file
    check_singleton :remove_file
    File.open('data/tmp', 'w') {|f| f.puts 'dummy' }
    remove_file 'data/tmp'
    assert_file_not_exist 'data/tmp'
  end

  def test_remove_file_file_perm
    File.open('data/tmp', 'w') {|f| f.puts 'dummy' }
    File.chmod 0, 'data/tmp'
    remove_file 'data/tmp'
    assert_file_not_exist 'data/tmp'
  end if have_file_perm?

  def test_remove_dir
    check_singleton :remove_dir
    Dir.mkdir 'data/tmpdir'
    File.open('data/tmpdir/a', 'w') {|f| f.puts 'dummy' }
    remove_dir 'data/tmpdir'
    assert_file_not_exist 'data/tmpdir'
  end

  def test_remove_dir_file_perm
    Dir.mkdir 'data/tmpdir'
    File.chmod 0555, 'data/tmpdir'
    remove_dir 'data/tmpdir'
    assert_file_not_exist 'data/tmpdir'
  end if have_file_perm?

  def test_compare_file
    check_singleton :compare_file
    # FIXME
  end

  def test_compare_stream
    check_singleton :compare_stream
    # FIXME
  end

  class Stream
    def initialize(f)
      @f = f
    end

    def read(*args)
      @f.read(*args)
    end

    def write(str)
      @f.write str
    end
  end

  def test_uptodate?
    check_singleton :uptodate?
    prepare_time_data
    Dir.chdir('data') {
      assert(   uptodate?('newest', %w(old newer notexist)) )
      assert( ! uptodate?('newer', %w(old newest notexist)) )
      assert( ! uptodate?('notexist', %w(old newest newer)) )
    }

    # pathname
    touch 'tmp/a'
    touch 'tmp/b'
    touch 'tmp/c'
    assert_nothing_raised {
      uptodate? Pathname.new('tmp/a'), ['tmp/b', 'tmp/c']
      uptodate? 'tmp/a', [Pathname.new('tmp/b'), 'tmp/c']
      uptodate? 'tmp/a', ['tmp/b', Pathname.new('tmp/c')]
      uptodate? Pathname.new('tmp/a'), [Pathname.new('tmp/b'), Pathname.new('tmp/c')]
    }
  end

  def test_cd
    check_singleton :cd
  end

  def test_chdir
    check_singleton :chdir
  end

  def test_getwd
    check_singleton :getwd
  end

  def test_identical?
    check_singleton :identical?
  end

  def test_link
    check_singleton :link
  end

  def test_makedirs
    check_singleton :makedirs
  end

  def test_mkpath
    check_singleton :mkpath
  end

  def test_move
    check_singleton :move
  end

  def test_rm_rf
    check_singleton :rm_rf
  end

  def test_rmdir
    check_singleton :rmdir
  end

  def test_rmtree
    check_singleton :rmtree
  end

  def test_safe_unlink
    check_singleton :safe_unlink
  end

  def test_symlink
    check_singleton :symlink
  end

  def test_touch
    check_singleton :touch
  end

  def test_collect_methods
  end

  def test_commands
  end

  def test_have_option?
  end

  def test_options
  end

  def test_options_of
  end

end
