#
# httputils.rb -- HTTPUtils Module
#
# Author: IPR -- Internet Programming with Ruby -- writers
# Copyright (c) 2000, 2001 TAKAHASHI Masayoshi, GOTOU Yuuzou
# Copyright (c) 2002 Internet Programming with Ruby writers. All rights
# reserved.
#
# $IPR: httputils.rb,v 1.34 2003/06/05 21:34:08 gotoyuzo Exp $

require 'socket'
require 'tempfile'

module WEBrick
  CR   = "\x0d"
  LF   = "\x0a"
  CRLF = "\x0d\x0a"

  module HTTPUtils

    def normalize_path(path)
      raise "abnormal path `#{path}'" if path[0] != ?/
      ret = path.dup

      ret.gsub!(%r{/+}o, '/')                    # //      => /
      while ret.sub!(%r'/\.(?:/|\Z)', '/'); end  # /.      => /
      while ret.sub!(%r'/(?!\.\./)[^/]+/\.\.(?:/|\Z)', '/'); end # /foo/.. => /foo

      raise "abnormal path `#{path}'" if %r{/\.\.(/|\Z)} =~ ret
      ret
    end
    module_function :normalize_path

    #####

    DefaultMimeTypes = {
      "ai"    => "application/postscript",
      "asc"   => "text/plain",
      "avi"   => "video/x-msvideo",
      "bin"   => "application/octet-stream",
      "bmp"   => "image/bmp",
      "class" => "application/octet-stream",
      "cer"   => "application/pkix-cert",
      "crl"   => "application/pkix-crl",
      "crt"   => "application/x-x509-ca-cert",
     #"crl"   => "application/x-pkcs7-crl",
      "css"   => "text/css",
      "dms"   => "application/octet-stream",
      "doc"   => "application/msword",
      "dvi"   => "application/x-dvi",
      "eps"   => "application/postscript",
      "etx"   => "text/x-setext",
      "exe"   => "application/octet-stream",
      "gif"   => "image/gif",
      "htm"   => "text/html",
      "html"  => "text/html",
      "jpe"   => "image/jpeg",
      "jpeg"  => "image/jpeg",
      "jpg"   => "image/jpeg",
      "js"    => "application/javascript",
      "lha"   => "application/octet-stream",
      "lzh"   => "application/octet-stream",
      "mov"   => "video/quicktime",
      "mpe"   => "video/mpeg",
      "mpeg"  => "video/mpeg",
      "mpg"   => "video/mpeg",
      "pbm"   => "image/x-portable-bitmap",
      "pdf"   => "application/pdf",
      "pgm"   => "image/x-portable-graymap",
      "png"   => "image/png",
      "pnm"   => "image/x-portable-anymap",
      "ppm"   => "image/x-portable-pixmap",
      "ppt"   => "application/vnd.ms-powerpoint",
      "ps"    => "application/postscript",
      "qt"    => "video/quicktime",
      "ras"   => "image/x-cmu-raster",
      "rb"    => "text/plain",
      "rd"    => "text/plain",
      "rtf"   => "application/rtf",
      "sgm"   => "text/sgml",
      "sgml"  => "text/sgml",
      "svg"   => "image/svg+xml",
      "tif"   => "image/tiff",
      "tiff"  => "image/tiff",
      "txt"   => "text/plain",
      "xbm"   => "image/x-xbitmap",
      "xhtml" => "text/html",
      "xls"   => "application/vnd.ms-excel",
      "xml"   => "text/xml",
      "xpm"   => "image/x-xpixmap",
      "xwd"   => "image/x-xwindowdump",
      "zip"   => "application/zip",
    }

    # Load Apache compatible mime.types file.
    def load_mime_types(file)
      open(file){ |io|
        hash = Hash.new
        io.each{ |line|
          next if /^#/ =~ line
          line.chomp!
          mimetype, ext0 = line.split(/\s+/, 2)
          next unless ext0
          next if ext0.empty?
          ext0.split(/\s+/).each{ |ext| hash[ext] = mimetype }
        }
        hash
      }
    end
    module_function :load_mime_types

    def mime_type(filename, mime_tab)
      suffix1 = (/\.(\w+)$/ =~ filename && $1.downcase)
      suffix2 = (/\.(\w+)\.[\w\-]+$/ =~ filename && $1.downcase)
      mime_tab[suffix1] || mime_tab[suffix2] || "application/octet-stream"
    end
    module_function :mime_type

    #####

    def parse_header(raw)
      header = Hash.new([].freeze)
      field = nil
      raw.each_line{|line|
        case line
        when /^([A-Za-z0-9!\#$%&'*+\-.^_`|~]+):\s*(.*?)\s*\z/om
          field, value = $1, $2
          field.downcase!
          header[field] = [] unless header.has_key?(field)
          header[field] << value
        when /^\s+(.*?)\s*\z/om
          value = $1
          unless field
            raise HTTPStatus::BadRequest, "bad header '#{line}'."
          end
          header[field][-1] << " " << value
        else
          raise HTTPStatus::BadRequest, "bad header '#{line}'."
        end
      }
      header.each{|key, values|
        values.each{|value|
          value.strip!
          value.gsub!(/\s+/, " ")
        }
      }
      header
    end
    module_function :parse_header

    def split_header_value(str)
      str.scan(%r'\G((?:"(?:\\.|[^"])+?"|[^",]+)+)
                    (?:,\s*|\Z)'xn).flatten
    end
    module_function :split_header_value

    def parse_range_header(ranges_specifier)
      if /^bytes=(.*)/ =~ ranges_specifier
        byte_range_set = split_header_value($1)
        byte_range_set.collect{|range_spec|
          case range_spec
          when /^(\d+)-(\d+)/ then $1.to_i .. $2.to_i
          when /^(\d+)-/      then $1.to_i .. -1
          when /^-(\d+)/      then -($1.to_i) .. -1
          else return nil
          end
        }
      end
    end
    module_function :parse_range_header

    def parse_qvalues(value)
      tmp = []
      if value
        parts = value.split(/,\s*/)
        parts.each {|part|
          if m = %r{^([^\s,]+?)(?:;\s*q=(\d+(?:\.\d+)?))?$}.match(part)
            val = m[1]
            q = (m[2] or 1).to_f
            tmp.push([val, q])
          end
        }
        tmp = tmp.sort_by{|val, q| -q}
        tmp.collect!{|val, q| val}
      end
      return tmp
    end
    module_function :parse_qvalues

    #####

    def dequote(str)
      ret = (/\A"(.*)"\Z/ =~ str) ? $1 : str.dup
      ret.gsub!(/\\(.)/, "\\1")
      ret
    end
    module_function :dequote

    def quote(str)
      '"' << str.gsub(/[\\\"]/o, "\\\1") << '"'
    end
    module_function :quote

    #####

    class FormData < String
      EmptyRawHeader = [].freeze
      EmptyHeader = {}.freeze

      attr_accessor :name, :filename, :next_data
      protected :next_data

      def initialize(*args)
        @name = @filename = @next_data = nil
        if args.empty?
          @raw_header = []
          @header = nil
          super("")
        else
          @raw_header = EmptyRawHeader
          @header = EmptyHeader
          super(args.shift)
          unless args.empty?
            @next_data = self.class.new(*args)
          end
        end
      end

      def [](*key)
        begin
          @header[key[0].downcase].join(", ")
        rescue StandardError, NameError
          super
        end
      end

      def <<(str)
        if @header
          super
        elsif str == CRLF
          @header = HTTPUtils::parse_header(@raw_header.join)
          if cd = self['content-disposition']
            if /\s+name="(.*?)"/ =~ cd then @name = $1 end
            if /\s+filename="(.*?)"/ =~ cd then @filename = $1 end
          end
        else
          @raw_header << str
        end
        self
      end

      def append_data(data)
        tmp = self
        while tmp
          unless tmp.next_data
            tmp.next_data = data
            break
          end
          tmp = tmp.next_data
        end
        self
      end

      def each_data
        tmp = self
        while tmp
          next_data = tmp.next_data
          yield(tmp)
          tmp = next_data
        end
      end

      def list
        ret = []
        each_data{|data|
          ret << data.to_s
        }
        ret
      end

      alias :to_ary :list

      def to_s
        String.new(self)
      end
    end

    def parse_query(str)
      query = Hash.new
      if str
        str.split(/[&;]/).each{|x|
          next if x.empty?
          key, val = x.split(/=/,2)
          key = unescape_form(key)
          val = unescape_form(val.to_s)
          val = FormData.new(val)
          val.name = key
          if query.has_key?(key)
            query[key].append_data(val)
            next
          end
          query[key] = val
        }
      end
      query
    end
    module_function :parse_query

    def parse_form_data(io, boundary)
      boundary_regexp = /\A--#{Regexp.quote(boundary)}(--)?#{CRLF}\z/
      form_data = Hash.new
      return form_data unless io
      data = nil
      io.each_line{|line|
        if boundary_regexp =~ line
          if data
            data.chop!
            key = data.name
            if form_data.has_key?(key)
              form_data[key].append_data(data)
            else
              form_data[key] = data
            end
          end
          data = FormData.new
          next
        else
          if data
            data << line
          end
        end
      }
      return form_data
    end
    module_function :parse_form_data

    #####

    reserved = ';/?:@&=+$,'
    num      = '0123456789'
    lowalpha = 'abcdefghijklmnopqrstuvwxyz'
    upalpha  = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'
    mark     = '-_.!~*\'()'
    unreserved = num + lowalpha + upalpha + mark
    control  = (0x0..0x1f).collect{|c| c.chr }.join + "\x7f"
    space    = " "
    delims   = '<>#%"'
    unwise   = '{}|\\^[]`'
    nonascii = (0x80..0xff).collect{|c| c.chr }.join

    module_function

    def _make_regex(str) /([#{Regexp.escape(str)}])/n end
    def _make_regex!(str) /([^#{Regexp.escape(str)}])/n end
    def _escape(str, regex) str.gsub(regex){ "%%%02X" % $1.ord } end
    def _unescape(str, regex) str.gsub(regex){ $1.hex.chr } end

    UNESCAPED = _make_regex(control+space+delims+unwise+nonascii)
    UNESCAPED_FORM = _make_regex(reserved+control+delims+unwise+nonascii)
    NONASCII  = _make_regex(nonascii)
    ESCAPED   = /%([0-9a-fA-F]{2})/
    UNESCAPED_PCHAR = _make_regex!(unreserved+":@&=+$,")

    def escape(str)
      _escape(str, UNESCAPED)
    end

    def unescape(str)
      _unescape(str, ESCAPED)
    end

    def escape_form(str)
      ret = _escape(str, UNESCAPED_FORM)
      ret.gsub!(/ /, "+")
      ret
    end

    def unescape_form(str)
      _unescape(str.gsub(/\+/, " "), ESCAPED)
    end

    def escape_path(str)
      result = ""
      str.scan(%r{/([^/]*)}).each{|i|
        result << "/" << _escape(i[0], UNESCAPED_PCHAR)
      }
      return result
    end

    def escape8bit(str)
      _escape(str, NONASCII)
    end
  end
end
