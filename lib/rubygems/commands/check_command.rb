require 'rubygems/command'
require 'rubygems/version_option'
require 'rubygems/validator'

class Gem::Commands::CheckCommand < Gem::Command

  include Gem::VersionOption

  def initialize
    super 'check', 'Check installed gems',
          :verify => false, :alien => false

    add_option(      '--verify FILE',
               'Verify gem file against its internal',
               'checksum') do |value, options|
      options[:verify] = value
    end

    add_option('-a', '--alien', "Report 'unmanaged' or rogue files in the",
               "gem repository") do |value, options|
      options[:alien] = true
    end

    add_version_option 'check'
  end

  def execute
    if options[:alien]
      say "Performing the 'alien' operation"
      say
      gems = get_all_gem_names rescue []
      Gem::Validator.new.alien(gems).sort.each do |key, val|
        unless val.empty? then
          say "#{key} has #{val.size} problems"
          val.each do |error_entry|
            say "  #{error_entry.path}:"
            say "    #{error_entry.problem}"
          end
        else
          say "#{key} is error-free" if Gem.configuration.verbose
        end
        say
      end
    end

    if options[:verify]
      gem_name = options[:verify]
      unless gem_name
        alert_error "Must specify a .gem file with --verify NAME"
        return
      end
      unless File.exist?(gem_name)
        alert_error "Unknown file: #{gem_name}."
        return
      end
      say "Verifying gem: '#{gem_name}'"
      begin
        Gem::Validator.new.verify_gem_file(gem_name)
      rescue Exception
        alert_error "#{gem_name} is invalid."
      end
    end
  end

end
