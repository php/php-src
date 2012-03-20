require 'rubygems/package/tar_test_case'
require 'rubygems/package'

class TestGemPackageTarHeader < Gem::Package::TarTestCase

  def setup
    super

    header = {
      :name     => 'x',
      :mode     => 0644,
      :uid      => 1000,
      :gid      => 10000,
      :size     => 100,
      :mtime    => 12345,
      :typeflag => '0',
      :linkname => 'link',
      :uname    => 'user',
      :gname    => 'group',
      :devmajor => 1,
      :devminor => 2,
      :prefix   => 'y',
    }

    @tar_header = Gem::Package::TarHeader.new header
  end

  def test_self_from
    io = TempIO.new @tar_header.to_s

    new_header = Gem::Package::TarHeader.from io

    assert_headers_equal @tar_header, new_header
  end

  def test_initialize
    assert_equal '',      @tar_header.checksum, 'checksum'
    assert_equal 1,       @tar_header.devmajor, 'devmajor'
    assert_equal 2,       @tar_header.devminor, 'devminor'
    assert_equal 10000,   @tar_header.gid,      'gid'
    assert_equal 'group', @tar_header.gname,    'gname'
    assert_equal 'link',  @tar_header.linkname, 'linkname'
    assert_equal 'ustar', @tar_header.magic,    'magic'
    assert_equal 0644,    @tar_header.mode,     'mode'
    assert_equal 12345,   @tar_header.mtime,    'mtime'
    assert_equal 'x',     @tar_header.name,     'name'
    assert_equal 'y',     @tar_header.prefix,   'prefix'
    assert_equal 100,     @tar_header.size,     'size'
    assert_equal '0',     @tar_header.typeflag, 'typeflag'
    assert_equal 1000,    @tar_header.uid,      'uid'
    assert_equal 'user',  @tar_header.uname,    'uname'
    assert_equal '00',    @tar_header.version,  'version'

    refute_empty @tar_header, 'empty'
  end

  def test_initialize_bad
    assert_raises ArgumentError do
      Gem::Package::TarHeader.new :name => '', :size => '', :mode => ''
    end

    assert_raises ArgumentError do
      Gem::Package::TarHeader.new :name => '', :size => '', :prefix => ''
    end

    assert_raises ArgumentError do
      Gem::Package::TarHeader.new :name => '', :prefix => '', :mode => ''
    end

    assert_raises ArgumentError do
      Gem::Package::TarHeader.new :prefix => '', :size => '', :mode => ''
    end
  end

  def test_empty_eh
    refute_empty @tar_header

    @tar_header = Gem::Package::TarHeader.new :name => 'x', :prefix => '',
                                              :mode => 0, :size => 0,
                                              :empty => true

    assert_empty @tar_header
  end

  def test_equals2
    assert_equal @tar_header, @tar_header
    assert_equal @tar_header, @tar_header.dup
  end

  def test_to_s
    expected = <<-EOF.split("\n").join
x\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000
\000\000\000\0000000644\0000001750\0000023420\00000000000144\00000000030071
\000012467\000 0link\000\000\000\000\000\000\000\000\000\000\000\000\000\000
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000
\000\000\000\000\000\000ustar\00000user\000\000\000\000\000\000\000\000\000
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000
group\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000
\000\000\000\000\000\000\000\000\0000000001\0000000002\000y\000\000\000\000
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000
\000\000\000\000\000\000\000\000\000\000
    EOF

    assert_headers_equal expected, @tar_header
  end

  def test_update_checksum
    assert_equal '', @tar_header.checksum

    @tar_header.update_checksum

    assert_equal '012467', @tar_header.checksum
  end

end

