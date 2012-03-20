require 'rubygems/package/tar_test_case'
require 'rubygems/package'

class TestGemPackageTarReaderEntry < Gem::Package::TarTestCase

  def setup
    super

    @contents = ('a'..'z').to_a.join * 100

    @tar = ''
    @tar << tar_file_header("lib/foo", "", 0, @contents.size)
    @tar << @contents
    @tar << "\0" * (512 - (@tar.size % 512))

    @entry = util_entry @tar
  end

  def test_bytes_read
    assert_equal 0, @entry.bytes_read

    @entry.getc

    assert_equal 1, @entry.bytes_read
  end

  def test_close
    @entry.close

    assert @entry.bytes_read

    e = assert_raises IOError do @entry.eof? end
    assert_equal 'closed Gem::Package::TarReader::Entry', e.message

    e = assert_raises IOError do @entry.getc end
    assert_equal 'closed Gem::Package::TarReader::Entry', e.message

    e = assert_raises IOError do @entry.pos end
    assert_equal 'closed Gem::Package::TarReader::Entry', e.message

    e = assert_raises IOError do @entry.read end
    assert_equal 'closed Gem::Package::TarReader::Entry', e.message

    e = assert_raises IOError do @entry.rewind end
    assert_equal 'closed Gem::Package::TarReader::Entry', e.message
  end

  def test_closed_eh
    @entry.close

    assert @entry.closed?
  end

  def test_eof_eh
    @entry.read

    assert @entry.eof?
  end

  def test_full_name
    assert_equal 'lib/foo', @entry.full_name
  end

  def test_full_name_null
    @entry.header.prefix << "\000"

    e = assert_raises Gem::Package::TarInvalidError do
      @entry.full_name
    end

    assert_equal 'tar is corrupt, name contains null byte', e.message
  end

  def test_getc
    assert_equal ?a, @entry.getc
  end

  def test_directory_eh
    assert_equal false, @entry.directory?
    assert_equal true, util_dir_entry.directory?
  end

  def test_file_eh
    assert_equal true, @entry.file?
    assert_equal false, util_dir_entry.file?
  end

  def test_pos
    assert_equal 0, @entry.pos

    @entry.getc

    assert_equal 1, @entry.pos
  end

  def test_read
    assert_equal @contents, @entry.read
  end

  def test_read_big
    assert_equal @contents, @entry.read(@contents.size * 2)
  end

  def test_read_small
    assert_equal @contents[0...100], @entry.read(100)
  end

  def test_rewind
    char = @entry.getc

    @entry.rewind

    assert_equal 0, @entry.pos

    assert_equal char, @entry.getc
  end

end

