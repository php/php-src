##
# The Version class processes string versions into comparable
# values. A version string should normally be a series of numbers
# separated by periods. Each part (digits separated by periods) is
# considered its own number, and these are used for sorting. So for
# instance, 3.10 sorts higher than 3.2 because ten is greater than
# two.
#
# If any part contains letters (currently only a-z are supported) then
# that version is considered prerelease. Versions with a prerelease
# part in the Nth part sort less than versions with N-1
# parts. Prerelease parts are sorted alphabetically using the normal
# Ruby string sorting rules. If a prerelease part contains both
# letters and numbers, it will be broken into multiple parts to
# provide expected sort behavior (1.0.a10 becomes 1.0.a.10, and is
# greater than 1.0.a9).
#
# Prereleases sort between real releases (newest to oldest):
#
# 1. 1.0
# 2. 1.0.b1
# 3. 1.0.a.2
# 4. 0.9
#
# == How Software Changes
#
# Users expect to be able to specify a version constraint that gives them
# some reasonable expectation that new versions of a library will work with
# their software if the version constraint is true, and not work with their
# software if the version constraint is false.  In other words, the perfect
# system will accept all compatible versions of the library and reject all
# incompatible versions.
#
# Libraries change in 3 ways (well, more than 3, but stay focused here!).
#
# 1. The change may be an implementation detail only and have no effect on
#    the client software.
# 2. The change may add new features, but do so in a way that client software
#    written to an earlier version is still compatible.
# 3. The change may change the public interface of the library in such a way
#    that old software is no longer compatible.
#
# Some examples are appropriate at this point.  Suppose I have a Stack class
# that supports a <tt>push</tt> and a <tt>pop</tt> method.
#
# === Examples of Category 1 changes:
#
# * Switch from an array based implementation to a linked-list based
#   implementation.
# * Provide an automatic (and transparent) backing store for large stacks.
#
# === Examples of Category 2 changes might be:
#
# * Add a <tt>depth</tt> method to return the current depth of the stack.
# * Add a <tt>top</tt> method that returns the current top of stack (without
#   changing the stack).
# * Change <tt>push</tt> so that it returns the item pushed (previously it
#   had no usable return value).
#
# === Examples of Category 3 changes might be:
#
# * Changes <tt>pop</tt> so that it no longer returns a value (you must use
#   <tt>top</tt> to get the top of the stack).
# * Rename the methods to <tt>push_item</tt> and <tt>pop_item</tt>.
#
# == RubyGems Rational Versioning
#
# * Versions shall be represented by three non-negative integers, separated
#   by periods (e.g. 3.1.4).  The first integers is the "major" version
#   number, the second integer is the "minor" version number, and the third
#   integer is the "build" number.
#
# * A category 1 change (implementation detail) will increment the build
#   number.
#
# * A category 2 change (backwards compatible) will increment the minor
#   version number and reset the build number.
#
# * A category 3 change (incompatible) will increment the major build number
#   and reset the minor and build numbers.
#
# * Any "public" release of a gem should have a different version.  Normally
#   that means incrementing the build number.  This means a developer can
#   generate builds all day long for himself, but as soon as he/she makes a
#   public release, the version must be updated.
#
# === Examples
#
# Let's work through a project lifecycle using our Stack example from above.
#
# Version 0.0.1:: The initial Stack class is release.
# Version 0.0.2:: Switched to a linked=list implementation because it is
#                 cooler.
# Version 0.1.0:: Added a <tt>depth</tt> method.
# Version 1.0.0:: Added <tt>top</tt> and made <tt>pop</tt> return nil
#                 (<tt>pop</tt> used to return the  old top item).
# Version 1.1.0:: <tt>push</tt> now returns the value pushed (it used it
#                 return nil).
# Version 1.1.1:: Fixed a bug in the linked list implementation.
# Version 1.1.2:: Fixed a bug introduced in the last fix.
#
# Client A needs a stack with basic push/pop capability.  He writes to the
# original interface (no <tt>top</tt>), so his version constraint looks
# like:
#
#   gem 'stack', '~> 0.0'
#
# Essentially, any version is OK with Client A.  An incompatible change to
# the library will cause him grief, but he is willing to take the chance (we
# call Client A optimistic).
#
# Client B is just like Client A except for two things: (1) He uses the
# <tt>depth</tt> method and (2) he is worried about future
# incompatibilities, so he writes his version constraint like this:
#
#   gem 'stack', '~> 0.1'
#
# The <tt>depth</tt> method was introduced in version 0.1.0, so that version
# or anything later is fine, as long as the version stays below version 1.0
# where incompatibilities are introduced.  We call Client B pessimistic
# because he is worried about incompatible future changes (it is OK to be
# pessimistic!).
#
# == Preventing Version Catastrophe:
#
# From: http://blog.zenspider.com/2008/10/rubygems-howto-preventing-cata.html
#
# Let's say you're depending on the fnord gem version 2.y.z. If you
# specify your dependency as ">= 2.0.0" then, you're good, right? What
# happens if fnord 3.0 comes out and it isn't backwards compatible
# with 2.y.z? Your stuff will break as a result of using ">=". The
# better route is to specify your dependency with a "spermy" version
# specifier. They're a tad confusing, so here is how the dependency
# specifiers work:
#
#   Specification From  ... To (exclusive)
#   ">= 3.0"      3.0   ... &infin;
#   "~> 3.0"      3.0   ... 4.0
#   "~> 3.0.0"    3.0.0 ... 3.1
#   "~> 3.5"      3.5   ... 4.0
#   "~> 3.5.0"    3.5.0 ... 3.6

class Gem::Version
  autoload :Requirement, 'rubygems/requirement'

  include Comparable

  VERSION_PATTERN = '[0-9]+(\.[0-9a-zA-Z]+)*' # :nodoc:
  ANCHORED_VERSION_PATTERN = /\A\s*(#{VERSION_PATTERN})*\s*\z/ # :nodoc:

  ##
  # A string representation of this Version.

  attr_reader :version
  alias to_s version

  ##
  # True if the +version+ string matches RubyGems' requirements.

  def self.correct? version
    version.to_s =~ ANCHORED_VERSION_PATTERN
  end

  ##
  # Factory method to create a Version object. Input may be a Version
  # or a String. Intended to simplify client code.
  #
  #   ver1 = Version.create('1.3.17')   # -> (Version object)
  #   ver2 = Version.create(ver1)       # -> (ver1)
  #   ver3 = Version.create(nil)        # -> nil

  def self.create input
    if input.respond_to? :version then
      input
    elsif input.nil? then
      nil
    else
      new input
    end
  end

  ##
  # Constructs a Version from the +version+ string.  A version string is a
  # series of digits or ASCII letters separated by dots.

  def initialize version
    raise ArgumentError, "Malformed version number string #{version}" unless
      self.class.correct?(version)

    @version = version.to_s
    @version.strip!
  end

  ##
  # Return a new version object where the next to the last revision
  # number is one greater (e.g., 5.3.1 => 5.4).
  #
  # Pre-release (alpha) parts, e.g, 5.3.1.b.2 => 5.4, are ignored.

  def bump
    segments = self.segments.dup
    segments.pop while segments.any? { |s| String === s }
    segments.pop if segments.size > 1

    segments[-1] = segments[-1].succ
    self.class.new segments.join(".")
  end

  ##
  # A Version is only eql? to another version if it's specified to the
  # same precision. Version "1.0" is not the same as version "1".

  def eql? other
    self.class === other and @version == other.version
  end

  def hash # :nodoc:
    @hash ||= segments.hash
  end

  def inspect # :nodoc:
    "#<#{self.class} #{version.inspect}>"
  end

  ##
  # Dump only the raw version string, not the complete object. It's a
  # string for backwards (RubyGems 1.3.5 and earlier) compatibility.

  def marshal_dump
    [version]
  end

  ##
  # Load custom marshal format. It's a string for backwards (RubyGems
  # 1.3.5 and earlier) compatibility.

  def marshal_load array
    initialize array[0]
  end

  ##
  # A version is considered a prerelease if it contains a letter.

  def prerelease?
    @prerelease ||= @version =~ /[a-zA-Z]/
  end

  def pretty_print q # :nodoc:
    q.text "Gem::Version.new(#{version.inspect})"
  end

  ##
  # The release for this version (e.g. 1.2.0.a -> 1.2.0).
  # Non-prerelease versions return themselves.

  def release
    return self unless prerelease?

    segments = self.segments.dup
    segments.pop while segments.any? { |s| String === s }
    self.class.new segments.join('.')
  end

  def segments # :nodoc:

    # segments is lazy so it can pick up version values that come from
    # old marshaled versions, which don't go through marshal_load.

    @segments ||= @version.scan(/[0-9]+|[a-z]+/i).map do |s|
      /^\d+$/ =~ s ? s.to_i : s
    end
  end

  ##
  # A recommended version for use with a ~> Requirement.

  def spermy_recommendation
    segments = self.segments.dup

    segments.pop    while segments.any? { |s| String === s }
    segments.pop    while segments.size > 2
    segments.push 0 while segments.size < 2

    "~> #{segments.join(".")}"
  end

  ##
  # Compares this version with +other+ returning -1, 0, or 1 if the
  # other version is larger, the same, or smaller than this
  # one. Attempts to compare to something that's not a
  # <tt>Gem::Version</tt> return +nil+.

  def <=> other
    return unless Gem::Version === other
    return 0 if @version == other.version

    lhsegments = segments
    rhsegments = other.segments

    lhsize = lhsegments.size
    rhsize = rhsegments.size
    limit  = (lhsize > rhsize ? lhsize : rhsize) - 1

    i = 0

    while i <= limit
      lhs, rhs = lhsegments[i] || 0, rhsegments[i] || 0
      i += 1

      next      if lhs == rhs
      return -1 if String  === lhs && Numeric === rhs
      return  1 if Numeric === lhs && String  === rhs

      return lhs <=> rhs
    end

    return 0
  end
end
