require 'rubygems/test_case'
require 'rubygems/commands/owner_command'

class TestGemCommandsOwnerCommand < Gem::TestCase

  def setup
    super

    @fetcher = Gem::FakeFetcher.new
    Gem::RemoteFetcher.fetcher = @fetcher
    Gem.configuration.rubygems_api_key = "ed244fbf2b1a52e012da8616c512fa47f9aa5250"

    @cmd = Gem::Commands::OwnerCommand.new
  end

  def test_show_owners
    response = <<EOF
---
- email: user1@example.com
- email: user2@example.com
EOF

    @fetcher.data["#{Gem.host}/api/v1/gems/freewill/owners.yaml"] = [response, 200, 'OK']

    use_ui @ui do
      @cmd.show_owners("freewill")
    end

    assert_equal Net::HTTP::Get, @fetcher.last_request.class
    assert_equal Gem.configuration.rubygems_api_key, @fetcher.last_request["Authorization"]

    assert_match %r{Owners for gem: freewill}, @ui.output
    assert_match %r{- user1@example.com}, @ui.output
    assert_match %r{- user2@example.com}, @ui.output
  end

  def test_show_owners_denied
    response = "You don't have permission to push to this gem"
    @fetcher.data["#{Gem.host}/api/v1/gems/freewill/owners.yaml"] = [response, 403, 'Forbidden']

    assert_raises Gem::MockGemUi::TermError do
      use_ui @ui do
        @cmd.show_owners("freewill")
      end
    end

    assert_match response, @ui.output
  end

  def test_show_owners_key
    response = "- email: user1@example.com\n"
    @fetcher.data["#{Gem.host}/api/v1/gems/freewill/owners.yaml"] = [response, 200, 'OK']
    File.open Gem.configuration.credentials_path, 'a' do |f|
      f.write ':other: 701229f217cdf23b1344c7b4b54ca97'
    end
    Gem.configuration.load_api_keys

    @cmd.handle_options %w(-k other)
    @cmd.show_owners('freewill')

    assert_equal '701229f217cdf23b1344c7b4b54ca97', @fetcher.last_request['Authorization']
  end

  def test_add_owners
    response = "Owner added successfully."
    @fetcher.data["#{Gem.host}/api/v1/gems/freewill/owners"] = [response, 200, 'OK']

    use_ui @ui do
      @cmd.add_owners("freewill", ["user-new1@example.com"])
    end

    assert_equal Net::HTTP::Post, @fetcher.last_request.class
    assert_equal Gem.configuration.rubygems_api_key, @fetcher.last_request["Authorization"]
    assert_equal "email=user-new1%40example.com", @fetcher.last_request.body

    assert_match response, @ui.output
  end

  def test_add_owners_denied
    response = "You don't have permission to push to this gem"
    @fetcher.data["#{Gem.host}/api/v1/gems/freewill/owners"] = [response, 403, 'Forbidden']

    assert_raises Gem::MockGemUi::TermError do
      use_ui @ui do
        @cmd.add_owners("freewill", ["user-new1@example.com"])
      end
    end

    assert_match response, @ui.output
  end

  def test_add_owners_key
    response = "Owner added successfully."
    @fetcher.data["#{Gem.host}/api/v1/gems/freewill/owners"] = [response, 200, 'OK']
    File.open Gem.configuration.credentials_path, 'a' do |f|
      f.write ':other: 701229f217cdf23b1344c7b4b54ca97'
    end
    Gem.configuration.load_api_keys

    @cmd.handle_options %w(-k other)
    @cmd.add_owners('freewill', ['user-new1@example.com'])

    assert_equal '701229f217cdf23b1344c7b4b54ca97', @fetcher.last_request['Authorization']
  end

  def test_remove_owners
    response = "Owner removed successfully."
    @fetcher.data["#{Gem.host}/api/v1/gems/freewill/owners"] = [response, 200, 'OK']

    use_ui @ui do
      @cmd.remove_owners("freewill", ["user-remove1@example.com"])
    end

    assert_equal Net::HTTP::Delete, @fetcher.last_request.class
    assert_equal Gem.configuration.rubygems_api_key, @fetcher.last_request["Authorization"]
    assert_equal "email=user-remove1%40example.com", @fetcher.last_request.body

    assert_match response, @ui.output
  end

  def test_remove_owners_denied
    response = "You don't have permission to push to this gem"
    @fetcher.data["#{Gem.host}/api/v1/gems/freewill/owners"] = [response, 403, 'Forbidden']

    assert_raises Gem::MockGemUi::TermError do
      use_ui @ui do
        @cmd.remove_owners("freewill", ["user-remove1@example.com"])
      end
    end

    assert_match response, @ui.output
  end

  def test_remove_owners_key
    response = "Owner removed successfully."
    @fetcher.data["#{Gem.host}/api/v1/gems/freewill/owners"] = [response, 200, 'OK']
    File.open Gem.configuration.credentials_path, 'a' do |f|
      f.write ':other: 701229f217cdf23b1344c7b4b54ca97'
    end
    Gem.configuration.load_api_keys

    @cmd.handle_options %w(-k other)
    @cmd.remove_owners('freewill', ['user-remove1@example.com'])

    assert_equal '701229f217cdf23b1344c7b4b54ca97', @fetcher.last_request['Authorization']
  end
end
