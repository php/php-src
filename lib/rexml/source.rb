require 'rexml/encoding'

module REXML
  # Generates Source-s.  USE THIS CLASS.
  class SourceFactory
    # Generates a Source object
    # @param arg Either a String, or an IO
    # @return a Source, or nil if a bad argument was given
    def SourceFactory::create_from(arg)
      if arg.respond_to? :read and
          arg.respond_to? :readline and
          arg.respond_to? :nil? and
          arg.respond_to? :eof?
        IOSource.new(arg)
      elsif arg.respond_to? :to_str
        require 'stringio'
        IOSource.new(StringIO.new(arg))
      elsif arg.kind_of? Source
        arg
      else
        raise "#{arg.class} is not a valid input stream.  It must walk \n"+
          "like either a String, an IO, or a Source."
      end
    end
  end

  # A Source can be searched for patterns, and wraps buffers and other
  # objects and provides consumption of text
  class Source
    include Encoding
    # The current buffer (what we're going to read next)
    attr_reader :buffer
    # The line number of the last consumed text
    attr_reader :line
    attr_reader :encoding

    # Constructor
    # @param arg must be a String, and should be a valid XML document
    # @param encoding if non-null, sets the encoding of the source to this
    # value, overriding all encoding detection
    def initialize(arg, encoding=nil)
      @orig = @buffer = arg
      if encoding
        self.encoding = encoding
      else
        self.encoding = check_encoding( @buffer )
      end
      @line = 0
    end


    # Inherited from Encoding
    # Overridden to support optimized en/decoding
    def encoding=(enc)
      return unless super
      @line_break = encode( '>' )
      if @encoding != 'UTF-8'
        @buffer = decode(@buffer)
        @to_utf = true
      else
        @to_utf = false
        @buffer.force_encoding ::Encoding::UTF_8
      end
    end

    # Scans the source for a given pattern.  Note, that this is not your
    # usual scan() method.  For one thing, the pattern argument has some
    # requirements; for another, the source can be consumed.  You can easily
    # confuse this method.  Originally, the patterns were easier
    # to construct and this method more robust, because this method
    # generated search regexes on the fly; however, this was
    # computationally expensive and slowed down the entire REXML package
    # considerably, since this is by far the most commonly called method.
    # @param pattern must be a Regexp, and must be in the form of
    # /^\s*(#{your pattern, with no groups})(.*)/.  The first group
    # will be returned; the second group is used if the consume flag is
    # set.
    # @param consume if true, the pattern returned will be consumed, leaving
    # everything after it in the Source.
    # @return the pattern, if found, or nil if the Source is empty or the
    # pattern is not found.
    def scan(pattern, cons=false)
      return nil if @buffer.nil?
      rv = @buffer.scan(pattern)
      @buffer = $' if cons and rv.size>0
      rv
    end

    def read
    end

    def consume( pattern )
      @buffer = $' if pattern.match( @buffer )
    end

    def match_to( char, pattern )
      return pattern.match(@buffer)
    end

    def match_to_consume( char, pattern )
      md = pattern.match(@buffer)
      @buffer = $'
      return md
    end

    def match(pattern, cons=false)
      md = pattern.match(@buffer)
      @buffer = $' if cons and md
      return md
    end

    # @return true if the Source is exhausted
    def empty?
      @buffer == ""
    end

    def position
      @orig.index( @buffer )
    end

    # @return the current line in the source
    def current_line
      lines = @orig.split
      res = lines.grep @buffer[0..30]
      res = res[-1] if res.kind_of? Array
      lines.index( res ) if res
    end
  end

  # A Source that wraps an IO.  See the Source class for method
  # documentation
  class IOSource < Source
    #attr_reader :block_size

    # block_size has been deprecated
    def initialize(arg, block_size=500, encoding=nil)
      @er_source = @source = arg
      @to_utf = false

      # Determining the encoding is a deceptively difficult issue to resolve.
      # First, we check the first two bytes for UTF-16.  Then we
      # assume that the encoding is at least ASCII enough for the '>', and
      # we read until we get one of those.  This gives us the XML declaration,
      # if there is one.  If there isn't one, the file MUST be UTF-8, as per
      # the XML spec.  If there is one, we can determine the encoding from
      # it.
      @buffer = ""
      str = @source.read( 2 ) || ''
      if encoding
        self.encoding = encoding
      elsif str[0,2] == "\xfe\xff"
        @line_break = "\000>"
      elsif str[0,2] == "\xff\xfe"
        @line_break = ">\000"
      elsif str[0,2] == "\xef\xbb"
        str += @source.read(1)
        str = '' if (str[2,1] == "\xBF")
        @line_break = ">"
      else
        @line_break = ">"
      end
      super( @source.eof? ? str : str+@source.readline( @line_break ) )

      if !@to_utf and
          @buffer.respond_to?(:force_encoding) and
          @source.respond_to?(:external_encoding) and
          @source.external_encoding != ::Encoding::UTF_8
        @force_utf8 = true
      else
        @force_utf8 = false
      end
    end

    def scan(pattern, cons=false)
      rv = super
      # You'll notice that this next section is very similar to the same
      # section in match(), but just a liiittle different.  This is
      # because it is a touch faster to do it this way with scan()
      # than the way match() does it; enough faster to warrent duplicating
      # some code
      if rv.size == 0
        until @buffer =~ pattern or @source.nil?
          begin
            @buffer << readline
          rescue Iconv::IllegalSequence
            raise
          rescue
            @source = nil
          end
        end
        rv = super
      end
      rv.taint
      rv
    end

    def read
      begin
        @buffer << readline
      rescue Exception, NameError
        @source = nil
      end
    end

    def consume( pattern )
      match( pattern, true )
    end

    def match( pattern, cons=false )
      rv = pattern.match(@buffer)
      @buffer = $' if cons and rv
      while !rv and @source
        begin
          @buffer << readline
          rv = pattern.match(@buffer)
          @buffer = $' if cons and rv
        rescue
          @source = nil
        end
      end
      rv.taint
      rv
    end

    def empty?
      super and ( @source.nil? || @source.eof? )
    end

    def position
      @er_source.pos rescue 0
    end

    # @return the current line in the source
    def current_line
      begin
        pos = @er_source.pos        # The byte position in the source
        lineno = @er_source.lineno  # The XML < position in the source
        @er_source.rewind
        line = 0                    # The \r\n position in the source
        begin
          while @er_source.pos < pos
            @er_source.readline
            line += 1
          end
        rescue
        end
      rescue IOError
        pos = -1
        line = -1
      end
      [pos, lineno, line]
    end

    private
    def readline
      str = @source.readline(@line_break)
      return nil if str.nil?

      if @to_utf
        decode(str)
      else
        str.force_encoding(::Encoding::UTF_8) if @force_utf8
        str
      end
    end
  end
end
