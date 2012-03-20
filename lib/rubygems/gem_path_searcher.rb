require "rubygems"
require "rubygems/deprecate"

##
# GemPathSearcher has the capability to find loadable files inside
# gems.  It generates data up front to speed up searches later.

class Gem::GemPathSearcher

  ##
  # Initialise the data we need to make searches later.

  def initialize
    # We want a record of all the installed gemspecs, in the order we wish to
    # examine them.
    # TODO: remove this stupid method
    @gemspecs = init_gemspecs

    # Map gem spec to glob of full require_path directories.  Preparing this
    # information may speed up searches later.
    @lib_dirs = {}

    @gemspecs.each do |spec|
      @lib_dirs[spec.object_id] = lib_dirs_for spec
    end
  end

  ##
  # Look in all the installed gems until a matching +glob+ is found.
  # Return the _gemspec_ of the gem where it was found.  If no match
  # is found, return nil.
  #
  # The gems are searched in alphabetical order, and in reverse
  # version order.
  #
  # For example:
  #
  #   find('log4r')              # -> (log4r-1.1 spec)
  #   find('log4r.rb')           # -> (log4r-1.1 spec)
  #   find('rake/rdoctask')      # -> (rake-0.4.12 spec)
  #   find('foobarbaz')          # -> nil
  #
  # Matching paths can have various suffixes ('.rb', '.so', and
  # others), which may or may not already be attached to _file_.
  # This method doesn't care about the full filename that matches;
  # only that there is a match.

  def find(glob)
    # HACK violation of encapsulation
    @gemspecs.find do |spec|
      # TODO: inverted responsibility
      matching_file? spec, glob
    end
  end

  # Looks through the available gemspecs and finds the first
  # one that contains +file+ as a requirable file.

  def find_spec_for_file(file)
    @gemspecs.find do |spec|
      return spec if spec.contains_requirable_file?(file)
    end
  end

  def find_active(glob)
    # HACK violation of encapsulation
    @gemspecs.find do |spec|
      # TODO: inverted responsibility
      spec.loaded? and matching_file? spec, glob
    end
  end

  ##
  # Works like #find, but finds all gemspecs matching +glob+.

  def find_all(glob)
    # HACK violation of encapsulation
    @gemspecs.select do |spec|
      # TODO: inverted responsibility
      matching_file? spec, glob
    end || []
  end

  def find_in_unresolved(glob)
    # HACK violation
    specs = Gem.unresolved_deps.values.map { |dep|
      Gem.source_index.search dep, true
    }.flatten

    specs.select do |spec|
      # TODO: inverted responsibility
      matching_file? spec, glob
    end || []
  end

  def find_in_unresolved_tree glob
    # HACK violation
    # TODO: inverted responsibility
    specs = Gem.unresolved_deps.values.map { |dep|
      Gem.source_index.search dep, true
    }.flatten

    specs.reverse_each do |spec|
      trails = matching_paths(spec, glob)
      next if trails.empty?
      return trails.map(&:reverse).sort.first.reverse
    end

    []
  end

  ##
  # Attempts to find a matching path using the require_paths of the given
  # +spec+.

  def matching_file?(spec, path)
    not matching_files(spec, path).empty?
  end

  def matching_paths(spec, path)
    trails = []

    spec.traverse do |from_spec, dep, to_spec, trail|
      next unless to_spec.conflicts.empty?
      trails << trail unless matching_files(to_spec, path).empty?
    end

    trails
  end

  ##
  # Returns files matching +path+ in +spec+.
  #--
  # Some of the intermediate results are cached in @lib_dirs for speed.

  def matching_files(spec, path)
    return [] unless @lib_dirs[spec.object_id] # case no paths
    glob = File.join @lib_dirs[spec.object_id], "#{path}#{Gem.suffix_pattern}"
    Dir[glob].select { |f| File.file? f.untaint }
  end

  ##
  # Return a list of all installed gemspecs, sorted by alphabetical order and
  # in reverse version order.  (bar-2, bar-1, foo-2)

  def init_gemspecs
    Gem::Specification.sort { |a, b|
      names = a.name <=> b.name
      next names if names.nonzero?
      b.version <=> a.version
    }
  end

  ##
  # Returns library directories glob for a gemspec.  For example,
  #   '/usr/local/lib/ruby/gems/1.8/gems/foobar-1.0/{lib,ext}'

  def lib_dirs_for(spec)
    "#{spec.full_gem_path}/{#{spec.require_paths.join(',')}}" if
      spec.require_paths
  end

  extend Gem::Deprecate

  deprecate :initialize,              :none,  2011, 10
  deprecate :find,                    :none,  2011, 10
  deprecate :find_active,             :none,  2011, 10
  deprecate :find_all,                :none,  2011, 10
  deprecate :find_in_unresolved,      :none,  2011, 10
  deprecate :find_in_unresolved_tree, :none,  2011, 10
  deprecate :find_spec_for_file,      :none,  2011, 10
end
