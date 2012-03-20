require 'rubygems/command'
require 'rubygems/local_remote_options'
require 'rubygems/gemcutter_utilities'

class Gem::Commands::OwnerCommand < Gem::Command
  include Gem::LocalRemoteOptions
  include Gem::GemcutterUtilities

  def description # :nodoc:
    'Manage gem owners on RubyGems.org.'
  end

  def arguments # :nodoc:
    "GEM       gem to manage owners for"
  end

  def initialize
    super 'owner', description
    add_proxy_option
    add_key_option
    defaults.merge! :add => [], :remove => []

    add_option '-a', '--add EMAIL', 'Add an owner' do |value, options|
      options[:add] << value
    end

    add_option '-r', '--remove EMAIL', 'Remove an owner' do |value, options|
      options[:remove] << value
    end
  end

  def execute
    sign_in
    name = get_one_gem_name

    add_owners    name, options[:add]
    remove_owners name, options[:remove]
    show_owners   name
  end

  def show_owners name
    response = rubygems_api_request :get, "api/v1/gems/#{name}/owners.yaml" do |request|
      request.add_field "Authorization", api_key
    end

    with_response response do |resp|
      owners = YAML.load resp.body

      say "Owners for gem: #{name}"
      owners.each do |owner|
        say "- #{owner['email']}"
      end
    end
  end

  def add_owners name, owners
    manage_owners :post, name, owners
  end

  def remove_owners name, owners
    manage_owners :delete, name, owners
  end

  def manage_owners method, name, owners
    owners.each do |owner|
      response = rubygems_api_request method, "api/v1/gems/#{name}/owners" do |request|
        request.set_form_data 'email' => owner
        request.add_field "Authorization", api_key
      end

      with_response response
    end
  end

end

