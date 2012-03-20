#--
# Copyright 2006 by Chad Fowler, Rich Kilmer, Jim Weirich and others.
# All rights reserved.
# See LICENSE.txt for permissions.
#++

require 'rubygems'

##
# The format class knows the guts of the RubyGem .gem file format and provides
# the capability to read gem files

class Gem::OldFormat

  attr_accessor :spec, :file_entries, :gem_path

  ##
  # Constructs an instance of a Format object, representing the gem's data
  # structure.
  #
  # gem:: [String] The file name of the gem

  def initialize(gem_path)
    require 'fileutils'
    require 'zlib'
    Gem.load_yaml

    @gem_path = gem_path
  end

  ##
  # Reads the named gem file and returns a Format object, representing the
  # data from the gem file
  #
  # file_path:: [String] Path to the gem file

  def self.from_file_by_path(file_path)
    unless File.exist?(file_path)
      raise Gem::Exception, "Cannot load gem file [#{file_path}]"
    end

    File.open(file_path, 'rb') do |file|
      from_io(file, file_path)
    end
  end

  ##
  # Reads a gem from an io stream and returns a Format object, representing
  # the data from the gem file
  #
  # io:: [IO] Stream from which to read the gem

  def self.from_io(io, gem_path="(io)")
    format = self.new(gem_path)
    skip_ruby(io)
    format.spec = read_spec(io)
    format.file_entries = []
    read_files_from_gem(io) do |entry, file_data|
      format.file_entries << [entry, file_data]
    end
    format
  end

  private

  ##
  # Skips the Ruby self-install header.  After calling this method, the
  # IO index will be set after the Ruby code.
  #
  # file:: [IO] The IO to process (skip the Ruby code)

  def self.skip_ruby(file)
    end_seen = false
    loop {
      line = file.gets
      if(line == nil || line.chomp == "__END__") then
        end_seen = true
        break
      end
    }

    if end_seen == false then
      raise Gem::Exception.new("Failed to find end of ruby script while reading gem")
    end
  end

  ##
  # Reads the specification YAML from the supplied IO and constructs
  # a Gem::Specification from it.  After calling this method, the
  # IO index will be set after the specification header.
  #
  # file:: [IO] The IO to process

  def self.read_spec(file)
    yaml = ''

    read_until_dashes file do |line|
      yaml << line
    end

    Gem::Specification.from_yaml yaml
  rescue YAML::Error => e
    raise Gem::Exception, "Failed to parse gem specification out of gem file"
  rescue ArgumentError => e
    raise Gem::Exception, "Failed to parse gem specification out of gem file"
  end

  ##
  # Reads lines from the supplied IO until a end-of-yaml (---) is
  # reached
  #
  # file:: [IO] The IO to process
  # block:: [String] The read line

  def self.read_until_dashes(file)
    while((line = file.gets) && line.chomp.strip != "---") do
      yield line
    end
  end

  ##
  # Reads the embedded file data from a gem file, yielding an entry
  # containing metadata about the file and the file contents themselves
  # for each file that's archived in the gem.
  # NOTE: Many of these methods should be extracted into some kind of
  # Gem file read/writer
  #
  # gem_file:: [IO] The IO to process

  def self.read_files_from_gem(gem_file)
    errstr = "Error reading files from gem"
    header_yaml = ''
    begin
      self.read_until_dashes(gem_file) do |line|
        header_yaml << line
      end
      header = YAML.load(header_yaml)
      raise Gem::Exception, errstr unless header

      header.each do |entry|
        file_data = ''
        self.read_until_dashes(gem_file) do |line|
          file_data << line
        end
        yield [entry, Zlib::Inflate.inflate(file_data.strip.unpack("m")[0])]
      end
    rescue Zlib::DataError
      raise Gem::Exception, errstr
    end
  end

end

