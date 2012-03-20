require 'rubygems/package/tar_test_case'
require 'rubygems/package/tar_output'
require 'rubygems/security'

class TestGemPackageTarOutput < Gem::Package::TarTestCase

  def setup
    super

    @file = File.join @tempdir, 'bla2.tar'
  end

  def test_self_open
    open @file, 'wb' do |tar_io|
      Gem::Package::TarOutput.open tar_io do |tar_writer|
        tar_writer.add_file_simple 'README', 0, 17 do |io|
          io.write "This is a README\n"
        end

        tar_writer.metadata = "This is some metadata\n"
      end
    end

    files = util_extract

    name, data = files.shift
    assert_equal 'data.tar.gz', name

    gz = Zlib::GzipReader.new StringIO.new(data)

    Gem::Package::TarReader.new gz do |tar_reader|
      tar_reader.each do |entry|
        assert_equal 'README', entry.full_name
        assert_equal "This is a README\n", entry.read
      end
    end

    gz.close

    name, data = files.shift
    assert_equal 'metadata.gz', name

    gz = Zlib::GzipReader.new StringIO.new(data)
    assert_equal "This is some metadata\n", gz.read

    assert_empty files
  ensure
    gz.close if gz
  end

  if defined? OpenSSL then
    def test_self_open_signed
      @private_key = File.expand_path('test/rubygems/private_key.pem', @@project_dir)
      @public_cert = File.expand_path('test/rubygems/public_cert.pem', @@project_dir)

      signer = Gem::Security::Signer.new @private_key, [@public_cert]

      open @file, 'wb' do |tar_io|
        Gem::Package::TarOutput.open tar_io, signer do |tar_writer|
          tar_writer.add_file_simple 'README', 0, 17 do |io|
            io.write "This is a README\n"
          end

          tar_writer.metadata = "This is some metadata\n"
        end
      end

      files = util_extract

      name, data = files.shift
      assert_equal 'data.tar.gz', name

      name, data = files.shift
      assert_equal 'metadata.gz', name

      name, data = files.shift
      assert_equal 'data.tar.gz.sig', name

      name, data = files.shift
      assert_equal 'metadata.gz.sig', name

      assert_empty files
    end
  end

  def util_extract
    files = []

    open @file, 'rb' do |io|
      Gem::Package::TarReader.new io do |tar_reader|
        tar_reader.each do |entry|
          files << [entry.full_name, entry.read]
        end
      end
    end

    files
  end

end

