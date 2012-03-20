require 'rubygems/test_case'
require 'rubygems/commands/cert_command'
require 'rubygems/fix_openssl_warnings' if RUBY_VERSION < "1.9"

unless defined? OpenSSL then
  warn "`gem cert` tests are being skipped, module OpenSSL not found"
end

class TestGemCommandsCertCommand < Gem::TestCase

  def setup
    super

    @orig_security_trust_dir = Gem::Security::OPT[:trust_dir]
    Gem::Security::OPT[:trust_dir] = @tempdir

    @cmd = Gem::Commands::CertCommand.new

    root = File.expand_path(File.dirname(__FILE__), @@project_dir)

    FileUtils.cp File.join(root, 'data', 'gem-private_key.pem'), @tempdir
    FileUtils.cp File.join(root, 'data', 'gem-public_cert.pem'), @tempdir

    @cert_file_name = File.join @tempdir, 'gem-public_cert.pem'
    @pkey_file_name = File.join @tempdir, 'gem-private_key.pem'
  end

  def teardown
    Gem::Security::OPT[:trust_dir] = @orig_security_trust_dir

    super
  end

  def test_execute_add
    use_ui @ui do
      @cmd.send :handle_options, %W[--add #{@cert_file_name}]
    end

    assert_equal "Added '/CN=rubygems/DC=example/DC=com'\n", @ui.output
    assert_equal '', @ui.error
  end

  def test_execute_build
    FileUtils.rm @cert_file_name
    FileUtils.rm @pkey_file_name

    use_ui @ui do
      Dir.chdir @tempdir do
        @cmd.send :handle_options, %W[--build nobody@example.com]
      end
    end

    output = @ui.output.split "\n"

    assert_equal 'Public Cert: gem-public_cert.pem', output.shift
    assert_equal 'Private Key: gem-private_key.pem', output.shift
    assert_equal 'Don\'t forget to move the key file to somewhere private...',
                 output.shift
    assert_equal [], output

    assert_equal '', @ui.error

    assert File.exist?(File.join(@tempdir, 'gem-private_key.pem'))
    assert File.exist?(File.join(@tempdir, 'gem-public_cert.pem'))
  end

  def test_execute_certificate
    use_ui @ui do
      @cmd.send :handle_options, %W[--certificate #{@cert_file_name}]
    end

    assert_equal '', @ui.output
    assert_equal '', @ui.error

    assert_equal File.read(@cert_file_name),
                 @cmd.options[:issuer_cert].to_s
  end

  def test_execute_list
    use_ui @ui do
      @cmd.send :handle_options, %W[--list]
    end

    assert_equal "/CN=rubygems/DC=example/DC=com\n", @ui.output
    assert_equal '', @ui.error
  end

  def test_execute_private_key
    use_ui @ui do
      @cmd.send :handle_options, %W[--private-key #{@pkey_file_name}]
    end

    assert_equal '', @ui.output
    assert_equal '', @ui.error

    assert_equal File.read(@pkey_file_name),
                 @cmd.options[:issuer_key].to_s
  end

  def test_execute_remove
    use_ui @ui do
      @cmd.send :handle_options, %W[--remove rubygems]
    end

    assert_equal "Removed '/CN=rubygems/DC=example/DC=com'\n", @ui.output
    assert_equal '', @ui.error

    refute File.exist?(@cert_file_name)
  end

  def test_execute_sign
    use_ui @ui do
      @cmd.send :handle_options, %W[
        -K #{@pkey_file_name} -C #{@cert_file_name} --sign #{@cert_file_name}
      ]
    end

    assert_equal '', @ui.output
    assert_equal '', @ui.error

    # HACK this test sucks
  end

end if defined? OpenSSL

