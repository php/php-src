#--
# Copyright 2006 by Chad Fowler, Rich Kilmer, Jim Weirich and others.
# All rights reserved.
# See LICENSE.txt for permissions.
#++

require 'rubygems'

##
# Mixin methods for --version and --platform Gem::Command options.

module Gem::VersionOption

  ##
  # Add the --platform option to the option parser.

  def add_platform_option(task = command, *wrap)
    OptionParser.accept Gem::Platform do |value|
      if value == Gem::Platform::RUBY then
        value
      else
        Gem::Platform.new value
      end
    end

    add_option('--platform PLATFORM', Gem::Platform,
               "Specify the platform of gem to #{task}", *wrap) do
                 |value, options|
      unless options[:added_platform] then
        Gem.platforms = [Gem::Platform::RUBY]
        options[:added_platform] = true
      end

      Gem.platforms << value unless Gem.platforms.include? value
    end
  end

  ##
  # Add the --prerelease option to the option parser.

  def add_prerelease_option(*wrap)
    add_option("--[no-]prerelease",
               "Allow prerelease versions of a gem", *wrap) do |value, options|
      options[:prerelease] = value
    end
  end

  ##
  # Add the --version option to the option parser.

  def add_version_option(task = command, *wrap)
    OptionParser.accept Gem::Requirement do |value|
      Gem::Requirement.new value
    end

    add_option('-v', '--version VERSION', Gem::Requirement,
               "Specify version of gem to #{task}", *wrap) do
                 |value, options|
      options[:version] = value
      options[:prerelease] = true if value.prerelease?
    end
  end

end

