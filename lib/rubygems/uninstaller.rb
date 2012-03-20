#--
# Copyright 2006 by Chad Fowler, Rich Kilmer, Jim Weirich and others.
# All rights reserved.
# See LICENSE.txt for permissions.
#++

require 'fileutils'
require 'rubygems'
require 'rubygems/dependency_list'
require 'rubygems/doc_manager'
require 'rubygems/user_interaction'

##
# An Uninstaller.
#
# The uninstaller fires pre and post uninstall hooks.  Hooks can be added
# either through a rubygems_plugin.rb file in an installed gem or via a
# rubygems/defaults/#{RUBY_ENGINE}.rb or rubygems/defaults/operating_system.rb
# file.  See Gem.pre_uninstall and Gem.post_uninstall for details.

class Gem::Uninstaller

  include Gem::UserInteraction

  ##
  # The directory a gem's executables will be installed into

  attr_reader :bin_dir

  ##
  # The gem repository the gem will be installed into

  attr_reader :gem_home

  ##
  # The Gem::Specification for the gem being uninstalled, only set during
  # #uninstall_gem

  attr_reader :spec

  ##
  # Constructs an uninstaller that will uninstall +gem+

  def initialize(gem, options = {})
    @gem               = gem
    @version           = options[:version] || Gem::Requirement.default
    @gem_home          = File.expand_path(options[:install_dir] || Gem.dir)
    @force_executables = options[:executables]
    @force_all         = options[:all]
    @force_ignore      = options[:ignore]
    @bin_dir           = options[:bin_dir]
    @format_executable = options[:format_executable]

    # only add user directory if install_dir is not set
    @user_install = false
    @user_install = options[:user_install] unless options[:install_dir]

    if @user_install then
      Gem.use_paths Gem.user_dir, @gem_home
    else
      Gem.use_paths @gem_home
    end
  end

  ##
  # Performs the uninstall of the gem.  This removes the spec, the Gem
  # directory, and the cached .gem file.

  def uninstall
    list = Gem::Specification.find_all_by_name(@gem, @version)

    if list.empty? then
      raise Gem::InstallError, "gem #{@gem.inspect} is not installed"

    elsif list.size > 1 and @force_all then
      remove_all list

    elsif list.size > 1 then
      gem_names = list.collect {|gem| gem.full_name} + ["All versions"]

      say
      _, index = choose_from_list "Select gem to uninstall:", gem_names

      if index == list.size then
        remove_all list
      elsif index >= 0 && index < list.size then
        uninstall_gem list[index]
      else
        say "Error: must enter a number [1-#{list.size+1}]"
      end
    else
      uninstall_gem list.first
    end
  end

  ##
  # Uninstalls gem +spec+

  def uninstall_gem(spec)
    @spec = spec

    unless dependencies_ok? spec
      unless ask_if_ok(spec)
        raise Gem::DependencyRemovalException,
          "Uninstallation aborted due to dependent gem(s)"
      end
    end

    Gem.pre_uninstall_hooks.each do |hook|
      hook.call self
    end

    remove_executables @spec
    remove @spec

    Gem.post_uninstall_hooks.each do |hook|
      hook.call self
    end

    @spec = nil
  end

  ##
  # Removes installed executables and batch files (windows only) for
  # +gemspec+.

  def remove_executables(spec)
    return if spec.nil? or spec.executables.empty?

    list = Gem::Specification.find_all { |s|
      s.name == spec.name && s.version != spec.version
    }

    executables = spec.executables.clone

    list.each do |s|
      s.executables.each do |exe_name|
        executables.delete exe_name
      end
    end

    return if executables.empty?

    executables = executables.map { |exec| formatted_program_filename exec }

    remove = if @force_executables.nil? then
               ask_yes_no("Remove executables:\n" \
                          "\t#{executables.join ', '}\n\n" \
                          "in addition to the gem?",
                          true)
             else
               @force_executables
             end

    unless remove then
      say "Executables and scripts will remain installed."
    else
      bin_dir = @bin_dir || Gem.bindir(spec.base_dir)

      raise Gem::FilePermissionError, bin_dir unless File.writable? bin_dir

      executables.each do |exe_name|
        say "Removing #{exe_name}"

        exe_file = File.join bin_dir, exe_name

        FileUtils.rm_f exe_file
        FileUtils.rm_f "#{exe_file}.bat"
      end
    end
  end

  ##
  # Removes all gems in +list+.
  #
  # NOTE: removes uninstalled gems from +list+.

  def remove_all(list)
    list.each { |spec| uninstall_gem spec }
  end

  ##
  # spec:: the spec of the gem to be uninstalled
  # list:: the list of all such gems
  #
  # Warning: this method modifies the +list+ parameter.  Once it has
  # uninstalled a gem, it is removed from that list.

  def remove(spec)
    unless path_ok?(@gem_home, spec) or
           (@user_install and path_ok?(Gem.user_dir, spec)) then
      e = Gem::GemNotInHomeException.new \
            "Gem is not installed in directory #{@gem_home}"
      e.spec = spec

      raise e
    end

    raise Gem::FilePermissionError, spec.base_dir unless
      File.writable?(spec.base_dir)

    FileUtils.rm_rf spec.full_gem_path

    # TODO: should this be moved to spec?... I vote eww (also exists in docmgr)
    old_platform_name = [spec.name,
                         spec.version,
                         spec.original_platform].join '-'

    gemspec = spec.spec_file

    unless File.exist? gemspec then
      gemspec = File.join(File.dirname(gemspec), "#{old_platform_name}.gemspec")
    end

    FileUtils.rm_rf gemspec

    gem = spec.cache_file
    gem = File.join(spec.cache_dir, "#{old_platform_name}.gem") unless
      File.exist? gem

    FileUtils.rm_rf gem

    Gem::DocManager.new(spec).uninstall_doc

    say "Successfully uninstalled #{spec.full_name}"

    Gem::Specification.remove_spec spec
  end

  ##
  # Is +spec+ in +gem_dir+?

  def path_ok?(gem_dir, spec)
    full_path     = File.join gem_dir, 'gems', spec.full_name
    original_path = File.join gem_dir, 'gems', spec.original_name

    full_path == spec.full_gem_path || original_path == spec.full_gem_path
  end

  def dependencies_ok?(spec)
    return true if @force_ignore

    deplist = Gem::DependencyList.from_specs
    deplist.ok_to_remove?(spec.full_name)
  end

  def ask_if_ok(spec)
    msg = ['']
    msg << 'You have requested to uninstall the gem:'
    msg << "\t#{spec.full_name}"

    spec.dependent_gems.each do |dep_spec, dep, satlist|
      msg <<
        ("#{dep_spec.name}-#{dep_spec.version} depends on " +
         "[#{dep.name} (#{dep.requirement})]")
    end

    msg << 'If you remove this gems, one or more dependencies will not be met.'
    msg << 'Continue with Uninstall?'
    return ask_yes_no(msg.join("\n"), true)
  end

  def formatted_program_filename(filename)
    if @format_executable then
      require 'rubygems/installer'
      Gem::Installer.exec_format % File.basename(filename)
    else
      filename
    end
  end
end
