warn 'Sys has been deprecated in favor of FileUtils'

#--
# Copyright 2003-2010 by Jim Weirich (jim.weirich@gmail.com)
# All rights reserved.
#++
#
begin
  require 'ftools'
rescue LoadError
end
require 'rbconfig'

######################################################################
# Sys provides a number of file manipulation tools for the convenience
# of writing Rakefiles.  All commands in this module will announce
# their activity on standard output if the $verbose flag is set
# ($verbose = true is the default).  You can control this by globally
# setting $verbose or by using the +verbose+ and +quiet+ methods.
#
# Sys has been deprecated in favor of the FileUtils module available
# in Ruby 1.8.
#
module Sys
  RUBY = RbConfig::CONFIG['ruby_install_name']

  # Install all the files matching +wildcard+ into the +dest_dir+
  # directory.  The permission mode is set to +mode+.
  def install(wildcard, dest_dir, mode)
    Dir[wildcard].each do |fn|
      File.install(fn, dest_dir, mode, $verbose)
    end
  end

  # Run the system command +cmd+.
  def run(cmd)
    log cmd
    system(cmd) or fail "Command Failed: [#{cmd}]"
  end

  # Run a Ruby interpreter with the given arguments.
  def ruby(*args)
    run "#{RUBY} #{args.join(' ')}"
  end

  # Copy a single file from +file_name+ to +dest_file+.
  def copy(file_name, dest_file)
    log "Copying file #{file_name} to #{dest_file}"
    File.copy(file_name, dest_file)
  end

  # Copy all files matching +wildcard+ into the directory +dest_dir+.
  def copy_files(wildcard, dest_dir)
    for_matching_files(wildcard, dest_dir) { |from, to| copy(from, to) }
  end

  # Link +file_name+ to +dest_file+.
  def link(file_name, dest_file)
    log "Linking file #{file_name} to #{dest_file}"
    File.link(file_name, dest_file)
  end

  # Link all files matching +wildcard+ into the directory +dest_dir+.
  def link_files(wildcard, dest_dir)
    for_matching_files(wildcard, dest_dir) { |from, to| link(from, to) }
  end

  # Symlink +file_name+ to +dest_file+.
  def symlink(file_name, dest_file)
    log "Symlinking file #{file_name} to #{dest_file}"
    File.symlink(file_name, dest_file)
  end

  # Symlink all files matching +wildcard+ into the directory +dest_dir+.
  def symlink_files(wildcard, dest_dir)
    for_matching_files(wildcard, dest_dir) { |from, to| link(from, to) }
  end

  # Remove all files matching +wildcard+.  If a matching file is a
  # directory, it must be empty to be removed.  used +delete_all+ to
  # recursively delete directories.
  def delete(*wildcards)
    wildcards.each do |wildcard|
      Dir[wildcard].each do |fn|
        if File.directory?(fn)
          log "Deleting directory #{fn}"
          Dir.delete(fn)
        else
          log "Deleting file #{fn}"
          File.delete(fn)
        end
      end
    end
  end

  # Recursively delete all files and directories matching +wildcard+.
  def delete_all(*wildcards)
    wildcards.each do |wildcard|
      Dir[wildcard].each do |fn|
        next if ! File.exist?(fn)
        if File.directory?(fn)
          Dir["#{fn}/*"].each do |subfn|
            next if subfn=='.' || subfn=='..'
            delete_all(subfn)
          end
          log "Deleting directory #{fn}"
          Dir.delete(fn)
        else
          log "Deleting file #{fn}"
          File.delete(fn)
        end
      end
    end
  end

  # Make the directories given in +dirs+.
  def makedirs(*dirs)
    dirs.each do |fn|
      log "Making directory #{fn}"
      File.makedirs(fn)
    end
  end

  # Make +dir+ the current working directory for the duration of
  # executing the given block.
  def indir(dir)
    olddir = Dir.pwd
    Dir.chdir(dir)
    yield
  ensure
    Dir.chdir(olddir)
  end

  # Split a file path into individual directory names.
  #
  # For example:
  #   split_all("a/b/c") =>  ['a', 'b', 'c']
  def split_all(path)
    head, tail = File.split(path)
    return [tail] if head == '.' || tail == '/'
    return [head, tail] if head == '/'
    return split_all(head) + [tail]
  end

  # Write a message to standard error if $verbose is enabled.
  def log(msg)
    print "  " if $trace && $verbose
    $stderr.puts msg if $verbose
  end

  # Perform a block with $verbose disabled.
  def quiet(&block)
    with_verbose(false, &block)
  end

  # Perform a block with $verbose enabled.
  def verbose(&block)
    with_verbose(true, &block)
  end

  # Perform a block with each file matching a set of wildcards.
  def for_files(*wildcards)
    wildcards.each do |wildcard|
      Dir[wildcard].each do |fn|
        yield(fn)
      end
    end
  end

  extend(self)

  private # ----------------------------------------------------------

  def for_matching_files(wildcard, dest_dir)
    Dir[wildcard].each do |fn|
      dest_file = File.join(dest_dir, fn)
      parent = File.dirname(dest_file)
      makedirs(parent) if ! File.directory?(parent)
      yield(fn, dest_file)
    end
  end

  def with_verbose(v)
    oldverbose = $verbose
    $verbose = v
    yield
  ensure
    $verbose = oldverbose
  end

end
