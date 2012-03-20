##
# Gem::PathSupport facilitates the GEM_HOME and GEM_PATH environment settings
# to the rest of RubyGems.
#
class Gem::PathSupport
  ##
  # The default system path for managing Gems.
  attr_reader :home

  ##
  # Array of paths to search for Gems.
  attr_reader :path

  ##
  #
  # Constructor. Takes a single argument which is to be treated like a
  # hashtable, or defaults to ENV, the system environment.
  #
  def initialize(env=ENV)
    @env = env

    # note 'env' vs 'ENV'...
    @home     = env["GEM_HOME"] || ENV["GEM_HOME"] || Gem.default_dir

    if File::ALT_SEPARATOR then
      @home   = @home.gsub(File::ALT_SEPARATOR, File::SEPARATOR)
    end

    self.path = env["GEM_PATH"] || ENV["GEM_PATH"]
  end

  private

  ##
  # Set the Gem home directory (as reported by Gem.dir).

  def home=(home)
    @home = home.to_s
  end

  ##
  # Set the Gem search path (as reported by Gem.path).

  def path=(gpaths)
    gem_path = [@home]

    # FIX: I can't tell wtf this is doing.
    gpaths ||= (ENV['GEM_PATH'] || "").empty? ? nil : ENV["GEM_PATH"]

    if gpaths then
      if gpaths.kind_of?(Array) then
        gem_path.push(*gpaths)
      else
        gem_path.push(*gpaths.split(File::PATH_SEPARATOR))
      end

      if File::ALT_SEPARATOR then
        gem_path.map! do |this_path|
          this_path.gsub File::ALT_SEPARATOR, File::SEPARATOR
        end
      end
    else
      gem_path.push(*Gem.default_path)

      gem_path << APPLE_GEM_HOME if defined?(APPLE_GEM_HOME)
    end

    @path = gem_path.uniq
  end
end
