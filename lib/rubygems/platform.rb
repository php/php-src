require "rubygems/deprecate"

##
# Available list of platforms for targeting Gem installations.

class Gem::Platform

  @local = nil

  attr_accessor :cpu

  attr_accessor :os

  attr_accessor :version

  def self.local
    arch = Gem::ConfigMap[:arch]
    arch = "#{arch}_60" if arch =~ /mswin32$/
    @local ||= new(arch)
  end

  def self.match(platform)
    Gem.platforms.any? do |local_platform|
      platform.nil? or local_platform == platform or
        (local_platform != Gem::Platform::RUBY and local_platform =~ platform)
    end
  end

  def self.new(arch) # :nodoc:
    case arch
    when Gem::Platform::CURRENT then
      Gem::Platform.local
    when Gem::Platform::RUBY, nil, '' then
      Gem::Platform::RUBY
    else
      super
    end
  end

  def initialize(arch)
    case arch
    when Array then
      @cpu, @os, @version = arch
    when String then
      arch = arch.split '-'

      if arch.length > 2 and arch.last !~ /\d/ then # reassemble x86-linux-gnu
        extra = arch.pop
        arch.last << "-#{extra}"
      end

      cpu = arch.shift

      @cpu = case cpu
             when /i\d86/ then 'x86'
             else cpu
             end

      if arch.length == 2 and arch.last =~ /^\d+(\.\d+)?$/ then # for command-line
        @os, @version = arch
        return
      end

      os, = arch
      @cpu, os = nil, cpu if os.nil? # legacy jruby

      @os, @version = case os
                      when /aix(\d+)/ then             [ 'aix',       $1  ]
                      when /cygwin/ then               [ 'cygwin',    nil ]
                      when /darwin(\d+)?/ then         [ 'darwin',    $1  ]
                      when /freebsd(\d+)/ then         [ 'freebsd',   $1  ]
                      when /hpux(\d+)/ then            [ 'hpux',      $1  ]
                      when /^java$/, /^jruby$/ then    [ 'java',      nil ]
                      when /^java([\d.]*)/ then        [ 'java',      $1  ]
                      when /^dotnet$/ then             [ 'dotnet',    nil ]
                      when /^dotnet([\d.]*)/ then      [ 'dotnet',    $1  ]
                      when /linux/ then                [ 'linux',     $1  ]
                      when /mingw32/ then              [ 'mingw32',   nil ]
                      when /(mswin\d+)(\_(\d+))?/ then
                        os, version = $1, $3
                        @cpu = 'x86' if @cpu.nil? and os =~ /32$/
                        [os, version]
                      when /netbsdelf/ then            [ 'netbsdelf', nil ]
                      when /openbsd(\d+\.\d+)/ then    [ 'openbsd',   $1  ]
                      when /solaris(\d+\.\d+)/ then    [ 'solaris',   $1  ]
                      # test
                      when /^(\w+_platform)(\d+)/ then [ $1,          $2  ]
                      else                             [ 'unknown',   nil ]
                      end
    when Gem::Platform then
      @cpu = arch.cpu
      @os = arch.os
      @version = arch.version
    else
      raise ArgumentError, "invalid argument #{arch.inspect}"
    end
  end

  def inspect
    "#<%s:0x%x @cpu=%p, @os=%p, @version=%p>" % [self.class, object_id, *to_a]
  end

  def to_a
    [@cpu, @os, @version]
  end

  def to_s
    to_a.compact.join '-'
  end

  def empty?
    to_s.empty?
  end

  ##
  # Is +other+ equal to this platform?  Two platforms are equal if they have
  # the same CPU, OS and version.

  def ==(other)
    self.class === other and to_a == other.to_a
  end

  alias :eql? :==

  def hash # :nodoc:
    to_a.hash
  end

  ##
  # Does +other+ match this platform?  Two platforms match if they have the
  # same CPU, or either has a CPU of 'universal', they have the same OS, and
  # they have the same version, or either has no version.

  def ===(other)
    return nil unless Gem::Platform === other

    # cpu
    (@cpu == 'universal' or other.cpu == 'universal' or @cpu == other.cpu) and

    # os
    @os == other.os and

    # version
    (@version.nil? or other.version.nil? or @version == other.version)
  end

  ##
  # Does +other+ match this platform?  If +other+ is a String it will be
  # converted to a Gem::Platform first.  See #=== for matching rules.

  def =~(other)
    case other
    when Gem::Platform then # nop
    when String then
      # This data is from http://gems.rubyforge.org/gems/yaml on 19 Aug 2007
      other = case other
              when /^i686-darwin(\d)/     then ['x86',       'darwin',  $1    ]
              when /^i\d86-linux/         then ['x86',       'linux',   nil   ]
              when 'java', 'jruby'        then [nil,         'java',    nil   ]
              when /dotnet(\-(\d+\.\d+))?/ then ['universal','dotnet',  $2    ]
              when /mswin32(\_(\d+))?/    then ['x86',       'mswin32', $2    ]
              when 'powerpc-darwin'       then ['powerpc',   'darwin',  nil   ]
              when /powerpc-darwin(\d)/   then ['powerpc',   'darwin',  $1    ]
              when /sparc-solaris2.8/     then ['sparc',     'solaris', '2.8' ]
              when /universal-darwin(\d)/ then ['universal', 'darwin',  $1    ]
              else                             other
              end

      other = Gem::Platform.new other
    else
      return nil
    end

    self === other
  end

  ##
  # A pure-ruby gem that may use Gem::Specification#extensions to build
  # binary files.

  RUBY = 'ruby'

  ##
  # A platform-specific gem that is built for the packaging ruby's platform.
  # This will be replaced with Gem::Platform::local.

  CURRENT = 'current'

  extend Gem::Deprecate

  deprecate :empty?, :none, 2011, 11
end

