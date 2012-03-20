require 'rubygems/command'

class Gem::Commands::StaleCommand < Gem::Command
  def initialize
    super('stale', 'List gems along with access times')
  end

  def usage # :nodoc:
    "#{program_name}"
  end

  def execute
    gem_to_atime = {}
    Gem::Specification.each do |spec|
      name = spec.full_name
      Dir["#{spec.full_gem_path}/**/*.*"].each do |file|
        next if File.directory?(file)
        stat = File.stat(file)
        gem_to_atime[name] ||= stat.atime
        gem_to_atime[name] = stat.atime if gem_to_atime[name] < stat.atime
      end
    end

    gem_to_atime.sort_by { |_, atime| atime }.each do |name, atime|
      say "#{name} at #{atime.strftime '%c'}"
    end
  end
end
