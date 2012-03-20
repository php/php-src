require 'rubygems/package/tar_test_case'
require 'rubygems/package/tar_writer'

class TestTarWriter < Gem::Package::TarTestCase

  def setup
    super

    @data = 'abcde12345'
    @io = TempIO.new
    @tar_writer = Gem::Package::TarWriter.new @io
  end

  def teardown
    @tar_writer.close unless @tar_writer.closed?

    super
  end

  def test_add_file
    @tar_writer.add_file 'x', 0644 do |f| f.write 'a' * 10 end

    assert_headers_equal(tar_file_header('x', '', 0644, 10),
                         @io.string[0, 512])
    assert_equal "aaaaaaaaaa#{"\0" * 502}", @io.string[512, 512]
    assert_equal 1024, @io.pos
  end

  def test_add_file_simple
    @tar_writer.add_file_simple 'x', 0644, 10 do |io| io.write "a" * 10 end

    assert_headers_equal(tar_file_header('x', '', 0644, 10),
                         @io.string[0, 512])

    assert_equal "aaaaaaaaaa#{"\0" * 502}", @io.string[512, 512]
    assert_equal 1024, @io.pos
  end

  def test_add_file_simple_padding
    @tar_writer.add_file_simple 'x', 0, 100

    assert_headers_equal tar_file_header('x', '', 0, 100),
                         @io.string[0, 512]

    assert_equal "\0" * 512, @io.string[512, 512]
  end

  def test_add_file_simple_data
    @tar_writer.add_file_simple("lib/foo/bar", 0, 10) { |f| f.write @data }
    @tar_writer.flush

    assert_equal @data + ("\0" * (512-@data.size)),
                 @io.string[512, 512]
  end

  def test_add_file_simple_size
    assert_raises Gem::Package::TarWriter::FileOverflow do
      @tar_writer.add_file_simple("lib/foo/bar", 0, 10) do |io|
        io.write "1" * 11
      end
    end
  end

  def test_add_file_unseekable
    assert_raises Gem::Package::NonSeekableIO do
      Gem::Package::TarWriter.new(Object.new).add_file 'x', 0
    end
  end

  def test_close
    @tar_writer.close

    assert_equal "\0" * 1024, @io.string

    e = assert_raises IOError do
      @tar_writer.close
    end
    assert_equal 'closed Gem::Package::TarWriter', e.message

    e = assert_raises IOError do
      @tar_writer.flush
    end
    assert_equal 'closed Gem::Package::TarWriter', e.message

    e = assert_raises IOError do
      @tar_writer.add_file 'x', 0
    end
    assert_equal 'closed Gem::Package::TarWriter', e.message

    e = assert_raises IOError do
      @tar_writer.add_file_simple 'x', 0, 0
    end
    assert_equal 'closed Gem::Package::TarWriter', e.message

    e = assert_raises IOError do
      @tar_writer.mkdir 'x', 0
    end
    assert_equal 'closed Gem::Package::TarWriter', e.message
  end

  def test_mkdir
    @tar_writer.mkdir 'foo', 0644

    assert_headers_equal tar_dir_header('foo', '', 0644),
                         @io.string[0, 512]
    assert_equal 512, @io.pos
  end

  def test_split_name
    assert_equal ['b' * 100, 'a' * 155],
                 @tar_writer.split_name("#{'a' * 155}/#{'b' * 100}")

    assert_equal ["#{'qwer/' * 19}bla", 'a' * 151],
                 @tar_writer.split_name("#{'a' * 151}/#{'qwer/' * 19}bla")
  end

  def test_split_name_too_long_name
    name = File.join 'a', 'b' * 100
    assert_equal ['b' * 100, 'a'], @tar_writer.split_name(name)

    assert_raises Gem::Package::TooLongFileName do
      name = File.join 'a', 'b' * 101
      @tar_writer.split_name name
    end
  end

  def test_split_name_too_long_prefix
    name = File.join 'a' * 155, 'b'
    assert_equal ['b', 'a' * 155], @tar_writer.split_name(name)

    assert_raises Gem::Package::TooLongFileName do
      name = File.join 'a' * 156, 'b'
      @tar_writer.split_name name
    end
  end

  def test_split_name_too_long_total
    assert_raises Gem::Package::TooLongFileName do
      @tar_writer.split_name 'a' * 257
    end
  end

end

