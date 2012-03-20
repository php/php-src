require 'rubygems/test_case'
require 'rubygems'
require 'rubygems/command'
require 'rubygems/gemcutter_utilities'

class TestGemGemcutterUtilities < Gem::TestCase

  def setup
    super

    ENV['RUBYGEMS_HOST'] = nil
    Gem.configuration.rubygems_api_key = nil

    @cmd = Gem::Command.new '', 'summary'
    @cmd.extend Gem::GemcutterUtilities
  end

  def test_api_key
    keys = { :rubygems_api_key => 'KEY' }
    FileUtils.mkdir_p File.dirname Gem.configuration.credentials_path

    open Gem.configuration.credentials_path, 'w' do |f|
      f.write keys.to_yaml
    end

    Gem.configuration.load_api_keys

    assert_equal 'KEY', @cmd.api_key
  end

  def test_api_key_override
    keys = { :rubygems_api_key => 'KEY', :other => 'OTHER' }
    FileUtils.mkdir_p File.dirname Gem.configuration.credentials_path

    open Gem.configuration.credentials_path, 'w' do |f|
      f.write keys.to_yaml
    end

    Gem.configuration.load_api_keys

    @cmd.add_key_option
    @cmd.handle_options %w[--key other]

    assert_equal 'OTHER', @cmd.api_key
  end

  def test_sign_in
    api_key     = 'a5fdbb6ba150cbb83aad2bb2fede64cf040453903'
    util_sign_in [api_key, 200, 'OK']

    assert_match %r{Enter your RubyGems.org credentials.}, @sign_in_ui.output
    assert @fetcher.last_request["authorization"]
    assert_match %r{Signed in.}, @sign_in_ui.output

    credentials = YAML.load_file Gem.configuration.credentials_path
    assert_equal api_key, credentials[:rubygems_api_key]
  end

  def test_sign_in_with_host
    api_key     = 'a5fdbb6ba150cbb83aad2bb2fede64cf040453903'
    util_sign_in [api_key, 200, 'OK'], 'http://example.com'

    assert_match %r{Enter your RubyGems.org credentials.}, @sign_in_ui.output
    assert @fetcher.last_request["authorization"]
    assert_match %r{Signed in.}, @sign_in_ui.output

    credentials = YAML.load_file Gem.configuration.credentials_path
    assert_equal api_key, credentials[:rubygems_api_key]
  end

  def test_sign_in_skips_with_existing_credentials
    api_key     = 'a5fdbb6ba150cbb83aad2bb2fede64cf040453903'
    Gem.configuration.rubygems_api_key = api_key

    util_sign_in [api_key, 200, 'OK']

    assert_equal "", @sign_in_ui.output
  end

  def test_sign_in_with_other_credentials_doesnt_overwrite_other_keys
    api_key       = 'a5fdbb6ba150cbb83aad2bb2fede64cf040453903'
    other_api_key = 'f46dbb18bb6a9c97cdc61b5b85c186a17403cdcbf'

    FileUtils.mkdir_p File.dirname(Gem.configuration.credentials_path)
    open Gem.configuration.credentials_path, 'w' do |f|
      f.write Hash[:other_api_key, other_api_key].to_yaml
    end
    util_sign_in [api_key, 200, 'OK']

    assert_match %r{Enter your RubyGems.org credentials.}, @sign_in_ui.output
    assert_match %r{Signed in.}, @sign_in_ui.output

    credentials   = YAML.load_file Gem.configuration.credentials_path
    assert_equal api_key, credentials[:rubygems_api_key]
    assert_equal other_api_key, credentials[:other_api_key]
  end

  def test_sign_in_with_bad_credentials
    skip 'Always uses $stdin on windows' if Gem.win_platform?

    assert_raises Gem::MockGemUi::TermError do
      util_sign_in ['Access Denied.', 403, 'Forbidden']
    end

    assert_match %r{Enter your RubyGems.org credentials.}, @sign_in_ui.output
    assert_match %r{Access Denied.}, @sign_in_ui.output
  end

  def util_sign_in response, host = nil
    skip 'Always uses $stdin on windows' if Gem.win_platform?

    email    = 'you@example.com'
    password = 'secret'

    if host
      ENV['RUBYGEMS_HOST'] = host
    else
      host = Gem.host
    end

    @fetcher = Gem::FakeFetcher.new
    @fetcher.data["#{host}/api/v1/api_key"] = response
    Gem::RemoteFetcher.fetcher = @fetcher

    @sign_in_ui = Gem::MockGemUi.new "#{email}\n#{password}\n"

    use_ui @sign_in_ui do
      @cmd.sign_in
    end
  end

  def test_verify_api_key
    keys = {:other => 'a5fdbb6ba150cbb83aad2bb2fede64cf040453903'}
    FileUtils.mkdir_p File.dirname(Gem.configuration.credentials_path)
    File.open Gem.configuration.credentials_path, 'w' do |f|
      f.write keys.to_yaml
    end
    Gem.configuration.load_api_keys

    assert_equal 'a5fdbb6ba150cbb83aad2bb2fede64cf040453903',
                 @cmd.verify_api_key(:other)
  end

  def test_verify_missing_api_key
    assert_raises Gem::MockGemUi::TermError do
      @cmd.verify_api_key :missing
    end
  end

end

