require 'rubygems/command'
require 'rubygems/version_option'
require 'rubygems/doc_manager'

class Gem::Commands::RdocCommand < Gem::Command
  include Gem::VersionOption

  def initialize
    super 'rdoc', 'Generates RDoc for pre-installed gems',
          :version => Gem::Requirement.default,
          :include_rdoc => true, :include_ri => true, :overwrite => false

    add_option('--all',
               'Generate RDoc/RI documentation for all',
               'installed gems') do |value, options|
      options[:all] = value
    end

    add_option('--[no-]rdoc',
               'Generate RDoc HTML') do |value, options|
      options[:include_rdoc] = value
    end

    add_option('--[no-]ri',
               'Generate RI data') do |value, options|
      options[:include_ri] = value
    end

    add_option('--[no-]overwrite',
               'Overwrite installed documents') do |value, options|
      options[:overwrite] = value
    end

    add_version_option
  end

  def arguments # :nodoc:
    "GEMNAME       gem to generate documentation for (unless --all)"
  end

  def defaults_str # :nodoc:
    "--version '#{Gem::Requirement.default}' --rdoc --ri --no-overwrite"
  end

  def description # :nodoc:
    <<-DESC
The rdoc command builds RDoc and RI documentation for installed gems.  Use
--overwrite to force rebuilding of documentation.
    DESC
  end

  def usage # :nodoc:
    "#{program_name} [args]"
  end

  def execute
    if options[:all] then
      specs = Gem::SourceIndex.from_installed_gems.collect { |name, spec|
        spec
      }
    else
      gem_name = get_one_gem_name
      dep = Gem::Dependency.new gem_name, options[:version]
      specs = Gem::SourceIndex.from_installed_gems.search dep
    end

    if specs.empty?
      raise "Failed to find gem #{gem_name} to generate RDoc for #{options[:version]}"
    end

    if options[:include_ri]
      specs.sort.each do |spec|
        doc = Gem::DocManager.new(spec)
        doc.generate_ri if options[:overwrite] || !doc.ri_installed?
      end

      Gem::DocManager.update_ri_cache
    end

    if options[:include_rdoc]
      specs.sort.each do |spec|
        doc = Gem::DocManager.new(spec)
        doc.generate_rdoc if options[:overwrite] || !doc.rdoc_installed?
      end
    end

    true
  end

end

