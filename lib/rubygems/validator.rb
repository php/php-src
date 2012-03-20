#--
# Copyright 2006 by Chad Fowler, Rich Kilmer, Jim Weirich and others.
# All rights reserved.
# See LICENSE.txt for permissions.
#++

require 'rubygems/format'
require 'rubygems/installer'

##
# Validator performs various gem file and gem database validation

class Gem::Validator

  include Gem::UserInteraction

  def initialize
    require 'find'
    require 'digest'
  end

  ##
  # Given a gem file's contents, validates against its own MD5 checksum
  # gem_data:: [String] Contents of the gem file

  def verify_gem(gem_data)
    raise Gem::VerificationError, 'empty gem file' if gem_data.size == 0

    unless gem_data =~ /MD5SUM/ then
      return # Don't worry about it...this sucks.  Need to fix MD5 stuff for
      # new format
      # FIXME
    end

    sum_data = gem_data.gsub(/MD5SUM = "([a-z0-9]+)"/,
                             "MD5SUM = \"#{"F" * 32}\"")

    unless Digest::MD5.hexdigest(sum_data) == $1.to_s then
      raise Gem::VerificationError, 'invalid checksum for gem file'
    end
  end

  ##
  # Given the path to a gem file, validates against its own MD5 checksum
  #
  # gem_path:: [String] Path to gem file

  def verify_gem_file(gem_path)
    open gem_path, Gem.binary_mode do |file|
      gem_data = file.read
      verify_gem gem_data
    end
  rescue Errno::ENOENT, Errno::EINVAL
    raise Gem::VerificationError, "missing gem file #{gem_path}"
  end

  private

  def find_files_for_gem(gem_directory)
    installed_files = []
    Find.find gem_directory do |file_name|
      fn = file_name[gem_directory.size..file_name.size-1].sub(/^\//, "")
      installed_files << fn unless
        fn =~ /CVS/ || fn.empty? || File.directory?(file_name)
    end
    installed_files
  end

  public

  ErrorData = Struct.new :path, :problem

  ##
  # Checks the gem directory for the following potential
  # inconsistencies/problems:
  #
  # * Checksum gem itself
  # * For each file in each gem, check consistency of installed versions
  # * Check for files that aren't part of the gem but are in the gems directory
  # * 1 cache - 1 spec - 1 directory.
  #
  # returns a hash of ErrorData objects, keyed on the problem gem's name.

  def alien(gems=[])
    errors = Hash.new { |h,k| h[k] = {} }

    Gem::SourceIndex.from_installed_gems.each do |gem_name, gem_spec|
      next unless gems.include? gem_spec.name unless gems.empty?

      install_dir = gem_spec.installation_path
      gem_path = Gem.cache_gem(gem_spec.file_name, install_dir)
      spec_path = File.join install_dir, "specifications", gem_spec.spec_name
      gem_directory = gem_spec.full_gem_path

      unless File.directory? gem_directory then
        errors[gem_name][gem_spec.full_name] =
          "Gem registered but doesn't exist at #{gem_directory}"
        next
      end

      unless File.exist? spec_path then
        errors[gem_name][spec_path] = "Spec file missing for installed gem"
      end

      begin
        verify_gem_file(gem_path)

        good, gone, unreadable = nil, nil, nil, nil

        open gem_path, Gem.binary_mode do |file|
          format = Gem::Format.from_file_by_path(gem_path)

          good, gone = format.file_entries.partition { |entry, _|
            File.exist? File.join(gem_directory, entry['path'])
          }

          gone.map! { |entry, _| entry['path'] }
          gone.sort.each do |path|
            errors[gem_name][path] = "Missing file"
          end

          good, unreadable = good.partition { |entry, _|
            File.readable? File.join(gem_directory, entry['path'])
          }

          unreadable.map! { |entry, _| entry['path'] }
          unreadable.sort.each do |path|
            errors[gem_name][path] = "Unreadable file"
          end

          good.each do |entry, data|
            begin
              next unless data # HACK `gem check -a mkrf`

              open File.join(gem_directory, entry['path']), Gem.binary_mode do |f|
                unless Digest::MD5.hexdigest(f.read).to_s ==
                    Digest::MD5.hexdigest(data).to_s then
                  errors[gem_name][entry['path']] = "Modified from original"
                end
              end
            end
          end
        end

        installed_files = find_files_for_gem(gem_directory)
        good.map! { |entry, _| entry['path'] }
        extras = installed_files - good - unreadable

        extras.each do |extra|
          errors[gem_name][extra] = "Extra file"
        end
      rescue Gem::VerificationError => e
        errors[gem_name][gem_path] = e.message
      end
    end

    errors.each do |name, subhash|
      errors[name] = subhash.map { |path, msg| ErrorData.new(path, msg) }
    end

    errors
  end

  def remove_leading_dot_dir(path)
    path.sub(/^\.\//, "")
  end

end

