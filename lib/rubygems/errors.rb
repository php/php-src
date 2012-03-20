class Gem::ErrorReason; end

# Generated when trying to lookup a gem to indicate that the gem
# was found, but that it isn't usable on the current platform.
#
# fetch and install read these and report them to the user to aid
# in figuring out why a gem couldn't be installed.
#
class Gem::PlatformMismatch < Gem::ErrorReason

  attr_reader :name
  attr_reader :version
  attr_reader :platforms

  def initialize(name, version)
    @name = name
    @version = version
    @platforms = []
  end

  def add_platform(platform)
    @platforms << platform
  end

  def wordy
    prefix = "Found #{@name} (#{@version})"

    if @platforms.size == 1
      "#{prefix}, but was for platform #{@platforms[0]}"
    else
      "#{prefix}, but was for platforms #{@platforms.join(' ,')}"
    end
  end

end
