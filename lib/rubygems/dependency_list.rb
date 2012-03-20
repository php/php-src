#--
# Copyright 2006 by Chad Fowler, Rich Kilmer, Jim Weirich and others.
# All rights reserved.
# See LICENSE.txt for permissions.
#++

require 'tsort'
require 'rubygems/deprecate'

##
# Gem::DependencyList is used for installing and uninstalling gems in the
# correct order to avoid conflicts.

class Gem::DependencyList
  attr_reader :specs

  include Enumerable
  include TSort

  ##
  # Allows enabling/disabling use of development dependencies

  attr_accessor :development

  ##
  # Creates a DependencyList from the current specs.

  def self.from_specs
    list = new
    list.add(*Gem::Specification.map)
    list
  end

  ##
  # Creates a DependencyList from a Gem::SourceIndex +source_index+

  def self.from_source_index(ignored=nil)
    warn "NOTE: DependencyList.from_source_index ignores it's arg" if ignored

    from_specs
  end

  ##
  # Creates a new DependencyList.  If +development+ is true, development
  # dependencies will be included.

  def initialize development = false
    @specs = []

    @development = development
  end

  ##
  # Adds +gemspecs+ to the dependency list.

  def add(*gemspecs)
    @specs.push(*gemspecs)
  end

  def clear
    @specs.clear
  end

  ##
  # Return a list of the gem specifications in the dependency list, sorted in
  # order so that no gemspec in the list depends on a gemspec earlier in the
  # list.
  #
  # This is useful when removing gems from a set of installed gems.  By
  # removing them in the returned order, you don't get into as many dependency
  # issues.
  #
  # If there are circular dependencies (yuck!), then gems will be returned in
  # order until only the circular dependents and anything they reference are
  # left.  Then arbitrary gemspecs will be returned until the circular
  # dependency is broken, after which gems will be returned in dependency
  # order again.

  def dependency_order
    sorted = strongly_connected_components.flatten

    result = []
    seen = {}

    sorted.each do |spec|
      if index = seen[spec.name] then
        if result[index].version < spec.version then
          result[index] = spec
        end
      else
        seen[spec.name] = result.length
        result << spec
      end
    end

    result.reverse
  end

  ##
  # Iterator over dependency_order

  def each(&block)
    dependency_order.each(&block)
  end

  def find_name(full_name)
    @specs.find { |spec| spec.full_name == full_name }
  end

  def inspect # :nodoc:
    "#<%s:0x%x %p>" % [self.class, object_id, map { |s| s.full_name }]
  end

  ##
  # Are all the dependencies in the list satisfied?

  def ok?
    why_not_ok?(:quick).empty?
  end

  def why_not_ok? quick = false
    unsatisfied = Hash.new { |h,k| h[k] = [] }
    each do |spec|
      spec.runtime_dependencies.each do |dep|
        inst = Gem::Specification.any? { |installed_spec|
          dep.name == installed_spec.name and
            dep.requirement.satisfied_by? installed_spec.version
        }

        unless inst or @specs.find { |s| s.satisfies_requirement? dep } then
          unsatisfied[spec.name] << dep
          return unsatisfied if quick
        end
      end
    end

    unsatisfied
  end

  ##
  # Is is ok to remove a gemspec from the dependency list?
  #
  # If removing the gemspec creates breaks a currently ok dependency, then it
  # is NOT ok to remove the gemspec.

  def ok_to_remove?(full_name)
    gem_to_remove = find_name full_name

    siblings = @specs.find_all { |s|
      s.name == gem_to_remove.name &&
        s.full_name != gem_to_remove.full_name
    }

    deps = []

    @specs.each do |spec|
      spec.dependencies.each do |dep|
        deps << dep if gem_to_remove.satisfies_requirement?(dep)
      end
    end

    deps.all? { |dep|
      siblings.any? { |s|
        s.satisfies_requirement? dep
      }
    }
  end

  ##
  # Remove everything in the DependencyList that matches but doesn't
  # satisfy items in +dependencies+ (a hash of gem names to arrays of
  # dependencies).

  def remove_specs_unsatisfied_by dependencies
    specs.reject! { |spec|
      dep = dependencies[spec.name]
      dep and not dep.requirement.satisfied_by? spec.version
    }
  end

  ##
  # Removes the gemspec matching +full_name+ from the dependency list

  def remove_by_name(full_name)
    @specs.delete_if { |spec| spec.full_name == full_name }
  end

  ##
  # Return a hash of predecessors.  <tt>result[spec]</tt> is an Array of
  # gemspecs that have a dependency satisfied by the named gemspec.

  def spec_predecessors
    result = Hash.new { |h,k| h[k] = [] }

    specs = @specs.sort.reverse

    specs.each do |spec|
      specs.each do |other|
        next if spec == other

        other.dependencies.each do |dep|
          if spec.satisfies_requirement? dep then
            result[spec] << other
          end
        end
      end
    end

    result
  end

  def tsort_each_node(&block)
    @specs.each(&block)
  end

  def tsort_each_child(node, &block)
    specs = @specs.sort.reverse

    dependencies = node.runtime_dependencies
    dependencies.push(*node.development_dependencies) if @development

    dependencies.each do |dep|
      specs.each do |spec|
        if spec.satisfies_requirement? dep then
          begin
            yield spec
          rescue TSort::Cyclic
            # do nothing
          end
          break
        end
      end
    end
  end

  private

  ##
  # Count the number of gemspecs in the list +specs+ that are not in
  # +ignored+.

  def active_count(specs, ignored)
    specs.count { |spec| ignored[spec.full_name].nil? }
  end
end

class Gem::DependencyList
  class << self
    extend Gem::Deprecate
    deprecate :from_source_index, "from_specs", 2011, 11
  end
end
