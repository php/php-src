require 'rubygems/command'

class Gem::Commands::WhichCommand < Gem::Command
  def initialize
    super 'which', 'Find the location of a library file you can require',
          :search_gems_first => false, :show_all => false

    add_option '-a', '--[no-]all', 'show all matching files' do |show_all, options|
      options[:show_all] = show_all
    end

    add_option '-g', '--[no-]gems-first',
               'search gems before non-gems' do |gems_first, options|
      options[:search_gems_first] = gems_first
    end
  end

  def arguments # :nodoc:
    "FILE          name of file to find"
  end

  def defaults_str # :nodoc:
    "--no-gems-first --no-all"
  end

  def execute
    found = false

    options[:args].each do |arg|
      arg = arg.sub(/#{Regexp.union(*Gem.suffixes)}$/, '')
      dirs = $LOAD_PATH

      spec = Gem::Specification.find_by_path arg

      if spec then
        if options[:search_gems_first] then
          dirs = gem_paths(spec) + $LOAD_PATH
        else
          dirs = $LOAD_PATH + gem_paths(spec)
        end
      end

      # TODO: this is totally redundant and stupid
      paths = find_paths arg, dirs

      if paths.empty? then
        alert_error "Can't find ruby library file or shared library #{arg}"
      else
        say paths
        found = true
      end
    end

    terminate_interaction 1 unless found
  end

  def find_paths(package_name, dirs)
    result = []

    dirs.each do |dir|
      Gem.suffixes.each do |ext|
        full_path = File.join dir, "#{package_name}#{ext}"
        if File.exist? full_path and not File.directory? full_path then
          result << full_path
          return result unless options[:show_all]
        end
      end
    end

    result
  end

  def gem_paths(spec)
    spec.require_paths.collect { |d| File.join spec.full_gem_path, d }
  end

  def usage # :nodoc:
    "#{program_name} FILE [FILE ...]"
  end

end

