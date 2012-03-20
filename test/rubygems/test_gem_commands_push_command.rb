require 'rubygems/test_case'
require 'rubygems/commands/push_command'

module Gem
  class << self; remove_method :latest_rubygems_version; end

  def self.latest_rubygems_version
    Gem::Version.new Gem::VERSION
  end
end

class TestGemCommandsPushCommand < Gem::TestCase

  def setup
    super

    @gems_dir  = File.join @tempdir, 'gems'
    @cache_dir = File.join @gemhome, "cache"

    FileUtils.mkdir @gems_dir

    Gem.configuration.rubygems_api_key =
      "ed244fbf2b1a52e012da8616c512fa47f9aa5250"

    @spec, @path = util_gem "freewill", "1.0.0"

    @fetcher = Gem::FakeFetcher.new
    Gem::RemoteFetcher.fetcher = @fetcher

    @cmd = Gem::Commands::PushCommand.new
  end

  def send_battery
    use_ui @ui do
      @cmd.send_gem(@path)
    end

    assert_match %r{Pushing gem to #{Gem.host}...}, @ui.output

    assert_equal Net::HTTP::Post, @fetcher.last_request.class
    assert_equal Gem.read_binary(@path), @fetcher.last_request.body
    assert_equal File.size(@path), @fetcher.last_request["Content-Length"].to_i
    assert_equal "application/octet-stream", @fetcher.last_request["Content-Type"]
    assert_equal Gem.configuration.rubygems_api_key, @fetcher.last_request["Authorization"]

    assert_match @response, @ui.output
  end

  def test_sending_gem_default
    @response = "Successfully registered gem: freewill (1.0.0)"
    @fetcher.data["#{Gem.host}/api/v1/gems"]  = [@response, 200, 'OK']

    send_battery
  end

  def test_sending_gem_host
    @response = "Successfully registered gem: freewill (1.0.0)"
    @fetcher.data["#{Gem.host}/api/v1/gems"] = [@response, 200, 'OK']
    @cmd.options[:host] = "#{Gem.host}"

    send_battery
  end

  def test_sending_gem_ENV
    @response = "Successfully registered gem: freewill (1.0.0)"
    @fetcher.data["#{Gem.host}/api/v1/gems"] = [@response, 200, 'OK']
    ENV["RUBYGEMS_HOST"] = "#{Gem.host}"

    send_battery
  end

  def test_raises_error_with_no_arguments
    def @cmd.sign_in; end
    assert_raises Gem::CommandLineError do
      @cmd.execute
    end
  end

  def test_sending_gem_denied
    response = "You don't have permission to push to this gem"
    @fetcher.data["#{Gem.host}/api/v1/gems"] = [response, 403, 'Forbidden']

    assert_raises Gem::MockGemUi::TermError do
      use_ui @ui do
        @cmd.send_gem(@path)
      end
    end

    assert_match response, @ui.output
  end

  def test_sending_gem_key
    @response = "Successfully registered gem: freewill (1.0.0)"
    @fetcher.data["#{Gem.host}/api/v1/gems"] = [@response, 200, "OK"]
    File.open Gem.configuration.credentials_path, 'a' do |f|
      f.write ':other: 701229f217cdf23b1344c7b4b54ca97'
    end
    Gem.configuration.load_api_keys

    @cmd.handle_options %w(-k other)
    @cmd.send_gem(@path)

    assert_equal Gem.configuration.api_keys[:other],
                 @fetcher.last_request["Authorization"]
  end

end
