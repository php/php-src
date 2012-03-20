require 'rubygems/remote_fetcher'

module Gem::GemcutterUtilities
  OptionParser.accept Symbol do |value|
    value.to_sym
  end

  ##
  # Add the --key option

  def add_key_option
    add_option('-k', '--key KEYNAME', Symbol,
               'Use the given API key',
               'from ~/.gem/credentials') do |value,options|
      options[:key] = value
    end
  end

  def api_key
    if options[:key] then
      verify_api_key options[:key]
    else
      Gem.configuration.rubygems_api_key
    end
  end

  def sign_in
    return if Gem.configuration.rubygems_api_key

    say "Enter your RubyGems.org credentials."
    say "Don't have an account yet? Create one at http://rubygems.org/sign_up"

    email    =              ask "   Email: "
    password = ask_for_password "Password: "
    say "\n"

    response = rubygems_api_request :get, "api/v1/api_key" do |request|
      request.basic_auth email, password
    end

    with_response response do |resp|
      say "Signed in."
      Gem.configuration.rubygems_api_key = resp.body
    end
  end

  def rubygems_api_request(method, path, host = Gem.host, &block)
    require 'net/http'
    host = ENV['RUBYGEMS_HOST'] if ENV['RUBYGEMS_HOST']
    uri = URI.parse "#{host}/#{path}"

    say "Pushing gem to #{host}..."

    request_method = Net::HTTP.const_get method.to_s.capitalize

    Gem::RemoteFetcher.fetcher.request(uri, request_method, &block)
  end

  def with_response(resp)
    case resp
    when Net::HTTPSuccess then
      if block_given? then
        yield resp
      else
        say resp.body
      end
    else
      say resp.body
      terminate_interaction 1
    end
  end

  def verify_api_key(key)
    if Gem.configuration.api_keys.key? key then
      Gem.configuration.api_keys[key]
    else
      alert_error "No such API key. You can add it with gem keys --add #{key}"
      terminate_interaction 1
    end
  end

end
