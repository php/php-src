require 'rubygems/package/tar_test_case'
require 'rubygems/package/tar_input'

class TestGemPackageTarInput < Gem::Package::TarTestCase

  # Sometimes the setgid bit doesn't take.  Don't know if this is a problem on
  # all systems, or just some.  But for now, we will ignore it in the tests.
  SETGID_BIT = 02000

  def setup
    super

    inner_tar = tar_file_header("bla", "", 0612, 10)
    inner_tar += "0123456789" + "\0" * 502
    inner_tar += tar_file_header("foo", "", 0636, 5)
    inner_tar += "01234" + "\0" * 507
    inner_tar += tar_dir_header("__dir__", "", 0600)
    inner_tar += "\0" * 1024
    str = TempIO.new

    begin
      os = Zlib::GzipWriter.new str
      os.write inner_tar
    ensure
      os.finish
    end

    str.rewind

    @file = File.join @tempdir, 'bla.tar'

    File.open @file, 'wb' do |f|
      f.write tar_file_header("data.tar.gz", "", 0644, str.string.size)
      f.write str.string
      f.write "\0" * ((512 - (str.string.size % 512)) % 512 )

      @spec = Gem::Specification.new do |spec|
        spec.author = "Mauricio :)"
      end

      meta = @spec.to_yaml

      f.write tar_file_header("metadata", "", 0644, meta.size)
      f.write meta + "\0" * (1024 - meta.size)
      f.write "\0" * 1024
    end

    @entry_names = %w{bla foo __dir__}
    @entry_sizes = [10, 5, 0]
    #FIXME: are these modes system dependent?
    @entry_modes = [0100612, 0100636, 040600]
    @entry_files = %W[#{@tempdir}/bla #{@tempdir}/foo]
    @entry_contents = %w[0123456789 01234]
  end

  def test_initialize_no_metadata_file
    Tempfile.open 'no_meta' do |io|
      io.write tar_file_header('a', '', 0644, 1)
      io.write 'a'
      io.rewind

      e = assert_raises Gem::Package::FormatError do
        open io.path, Gem.binary_mode do |file|
          Gem::Package::TarInput.open file do end
        end
      end

      assert_equal "no metadata found in #{io.path}", e.message
      assert_equal io.path, e.path
    end
  end

  def test_each
    open @file, 'rb' do |io|
      Gem::Package::TarInput.open io do |tar_input|
        count = 0

        tar_input.each_with_index do |entry, i|
          count = i

          assert_kind_of Gem::Package::TarReader::Entry, entry
          assert_equal @entry_names[i], entry.header.name
          assert_equal @entry_sizes[i], entry.header.size
        end

        assert_equal 2, count

        assert_equal @spec, tar_input.metadata
      end
    end
  end

  def test_extract_entry
    open @file, 'rb' do |io|
      Gem::Package::TarInput.open io do |tar_input|
        assert_equal @spec, tar_input.metadata

        count = 0

        tar_input.each_with_index do |entry, i|
          count = i
          tar_input.extract_entry @tempdir, entry
          name = File.join @tempdir, entry.header.name

          if entry.directory? then
            assert File.directory?(name)
          else
            assert File.file?(name)
            assert_equal @entry_sizes[i], File.stat(name).size
            #FIXME: win32? !!
          end

          unless Gem.win_platform? then
            assert_equal @entry_modes[i], File.stat(name).mode & (~SETGID_BIT)
          end
        end

        assert_equal 2, count
      end
    end

    @entry_files.each_with_index do |x, i|
      assert File.file?(x)
      assert_equal @entry_contents[i], Gem.read_binary(x)
    end
  end

end

