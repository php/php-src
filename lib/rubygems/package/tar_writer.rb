# -*- coding: utf-8 -*-
#--
# Copyright (C) 2004 Mauricio Julio Fernández Pradier
# See LICENSE.txt for additional licensing information.
#++

##
# Allows writing of tar files

class Gem::Package::TarWriter

  class FileOverflow < StandardError; end

  ##
  # IO wrapper that allows writing a limited amount of data

  class BoundedStream

    ##
    # Maximum number of bytes that can be written

    attr_reader :limit

    ##
    # Number of bytes written

    attr_reader :written

    ##
    # Wraps +io+ and allows up to +limit+ bytes to be written

    def initialize(io, limit)
      @io = io
      @limit = limit
      @written = 0
    end

    ##
    # Writes +data+ onto the IO, raising a FileOverflow exception if the
    # number of bytes will be more than #limit

    def write(data)
      if data.size + @written > @limit
        raise FileOverflow, "You tried to feed more data than fits in the file."
      end
      @io.write data
      @written += data.size
      data.size
    end

  end

  ##
  # IO wrapper that provides only #write

  class RestrictedStream

    ##
    # Creates a new RestrictedStream wrapping +io+

    def initialize(io)
      @io = io
    end

    ##
    # Writes +data+ onto the IO

    def write(data)
      @io.write data
    end

  end

  ##
  # Creates a new TarWriter, yielding it if a block is given

  def self.new(io)
    writer = super

    return writer unless block_given?

    begin
      yield writer
    ensure
      writer.close
    end

    nil
  end

  ##
  # Creates a new TarWriter that will write to +io+

  def initialize(io)
    @io = io
    @closed = false
  end

  ##
  # Adds file +name+ with permissions +mode+, and yields an IO for writing the
  # file to

  def add_file(name, mode) # :yields: io
    check_closed

    raise Gem::Package::NonSeekableIO unless @io.respond_to? :pos=

    name, prefix = split_name name

    init_pos = @io.pos
    @io.write "\0" * 512 # placeholder for the header

    yield RestrictedStream.new(@io) if block_given?

    size = @io.pos - init_pos - 512

    remainder = (512 - (size % 512)) % 512
    @io.write "\0" * remainder

    final_pos = @io.pos
    @io.pos = init_pos

    header = Gem::Package::TarHeader.new :name => name, :mode => mode,
                                         :size => size, :prefix => prefix

    @io.write header
    @io.pos = final_pos

    self
  end

  ##
  # Add file +name+ with permissions +mode+ +size+ bytes long.  Yields an IO
  # to write the file to.

  def add_file_simple(name, mode, size) # :yields: io
    check_closed

    name, prefix = split_name name

    header = Gem::Package::TarHeader.new(:name => name, :mode => mode,
                                         :size => size, :prefix => prefix).to_s

    @io.write header
    os = BoundedStream.new @io, size

    yield os if block_given?

    min_padding = size - os.written
    @io.write("\0" * min_padding)

    remainder = (512 - (size % 512)) % 512
    @io.write("\0" * remainder)

    self
  end

  ##
  # Raises IOError if the TarWriter is closed

  def check_closed
    raise IOError, "closed #{self.class}" if closed?
  end

  ##
  # Closes the TarWriter

  def close
    check_closed

    @io.write "\0" * 1024
    flush

    @closed = true
  end

  ##
  # Is the TarWriter closed?

  def closed?
    @closed
  end

  ##
  # Flushes the TarWriter's IO

  def flush
    check_closed

    @io.flush if @io.respond_to? :flush
  end

  ##
  # Creates a new directory in the tar file +name+ with +mode+

  def mkdir(name, mode)
    check_closed

    name, prefix = split_name(name)

    header = Gem::Package::TarHeader.new :name => name, :mode => mode,
                                         :typeflag => "5", :size => 0,
                                         :prefix => prefix

    @io.write header

    self
  end

  ##
  # Splits +name+ into a name and prefix that can fit in the TarHeader

  def split_name(name) # :nodoc:
    raise Gem::Package::TooLongFileName if name.size > 256

    if name.size <= 100 then
      prefix = ""
    else
      parts = name.split(/\//)
      newname = parts.pop
      nxt = ""

      loop do
        nxt = parts.pop
        break if newname.size + 1 + nxt.size > 100
        newname = nxt + "/" + newname
      end

      prefix = (parts + [nxt]).join "/"
      name = newname

      if name.size > 100 or prefix.size > 155 then
        raise Gem::Package::TooLongFileName
      end
    end

    return name, prefix
  end

end

