# coding: US-ASCII

require 'rdoc'

##
# This class is a wrapper around File IO and Encoding that helps RDoc load
# files and convert them to the correct encoding.

module RDoc::Encoding

  ##
  # Reads the contents of +filename+ and handles any encoding directives in
  # the file.
  #
  # The content will be converted to the +encoding+.  If the file cannot be
  # converted a warning will be printed and nil will be returned.
  #
  # If +force_transcode+ is true the document will be transcoded and any
  # unknown character in the target encoding will be replaced with '?'

  def self.read_file filename, encoding, force_transcode = false
    content = open filename, "rb" do |f| f.read end
    content.gsub!("\r\n", "\n") if RUBY_PLATFORM =~ /mswin|mingw/

    utf8 = content.sub!(/\A\xef\xbb\xbf/, '')

    RDoc::Encoding.set_encoding content

    if Object.const_defined? :Encoding then
      encoding ||= Encoding.default_external
      orig_encoding = content.encoding

      if utf8 then
        content.force_encoding Encoding::UTF_8
        content.encode! encoding
      else
        # assume the content is in our output encoding
        content.force_encoding encoding
      end

      unless content.valid_encoding? then
        # revert and try to transcode
        content.force_encoding orig_encoding
        content.encode! encoding
      end

      unless content.valid_encoding? then
        warn "unable to convert #{filename} to #{encoding}, skipping"
        content = nil
      end
    end

    content
  rescue ArgumentError => e
    raise unless e.message =~ /unknown encoding name - (.*)/
    warn "unknown encoding name \"#{$1}\" for #{filename}, skipping"
    nil
  rescue Encoding::UndefinedConversionError => e
    if force_transcode then
      content.force_encoding orig_encoding
      content.encode! encoding, :undef => :replace, :replace => '?'
      content
    else
      warn "unable to convert #{e.message} for #{filename}, skipping"
      nil
    end
  rescue Errno::EISDIR, Errno::ENOENT
    nil
  end

  ##
  # Sets the encoding of +string+ based on the magic comment

  def self.set_encoding string
    first_line = string[/\A(?:#!.*\n)?.*\n/]

    name = case first_line
           when /^<\?xml[^?]*encoding=(["'])(.*?)\1/ then $2
           when /\b(?:en)?coding[=:]\s*([^\s;]+)/i   then $1
           else                                           return
           end

    string.sub! first_line, ''

    return unless Object.const_defined? :Encoding

    enc = Encoding.find name
    string.force_encoding enc if enc
  end

end

