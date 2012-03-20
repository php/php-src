require 'rubygems/command'
require 'rubygems/builder'

class Gem::Commands::BuildCommand < Gem::Command

  def initialize
    super('build', 'Build a gem from a gemspec')
  end

  def arguments # :nodoc:
    "GEMSPEC_FILE  gemspec file name to build a gem for"
  end

  def usage # :nodoc:
    "#{program_name} GEMSPEC_FILE"
  end

  def execute
    gemspec = get_one_gem_name

    if File.exist? gemspec
      spec = load_gemspec gemspec

      if spec then
        Gem::Builder.new(spec).build
      else
        alert_error "Error loading gemspec. Aborting."
        terminate_interaction 1
      end
    else
      alert_error "Gemspec file not found: #{gemspec}"
      terminate_interaction 1
    end
  end

  def load_gemspec filename
    if yaml?(filename)
      open(filename) do |f|
        begin
          Gem::Specification.from_yaml(f)
        rescue Gem::EndOfYAMLException
          nil
        end
      end
    else
      Gem::Specification.load(filename) # can return nil
    end
  end

  def yaml?(filename)
    line = open(filename) { |f| line = f.gets }
    result = line =~ %r{!ruby/object:Gem::Specification}
    result
  end
end
