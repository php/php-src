require 'rdoc'
require 'rdoc/code_objects'
require 'rdoc/markup/pre_process'
require 'rdoc/stats'

##
# A parser is a class that subclasses RDoc::Parser and implements
#
#   #initialize top_level, file_name, body, options, stats
#
# and
#
#   #scan
#
# The initialize method takes a file name to be used, the body of the file,
# and an RDoc::Options object. The scan method is then called to return an
# appropriately parsed TopLevel code object.
#
# RDoc::Parser::for is a factory that creates the correct parser for a
# given filename extension.  Parsers have to register themselves RDoc::Parser
# using parse_files_matching as when they are loaded:
#
#   require "rdoc/parser"
#
#   class RDoc::Parser::Xyz < RDoc::Parser
#     parse_files_matching /\.xyz$/ # <<<<
#
#     def initialize top_level, file_name, body, options, stats
#       ...
#     end
#
#     def scan
#       ...
#     end
#   end
#
# If a plain text file is detected, RDoc also looks for a shebang line in case
# the file is a shell script.

class RDoc::Parser

  @parsers = []

  class << self

    ##
    # An Array of arrays that maps file extension (or name) regular
    # expressions to parser classes that will parse matching filenames.
    #
    # Use parse_files_matching to register a parser's file extensions.

    attr_reader :parsers

  end

  ##
  # Alias an extension to another extension. After this call, files ending
  # "new_ext" will be parsed using the same parser as "old_ext"

  def self.alias_extension(old_ext, new_ext)
    old_ext = old_ext.sub(/^\.(.*)/, '\1')
    new_ext = new_ext.sub(/^\.(.*)/, '\1')

    parser = can_parse "xxx.#{old_ext}"
    return false unless parser

    RDoc::Parser.parsers.unshift [/\.#{new_ext}$/, parser]

    true
  end

  ##
  # Determines if the file is a "binary" file which basically means it has
  # content that an RDoc parser shouldn't try to consume.

  def self.binary?(file)
    return false if file =~ /\.(rdoc|txt)$/

    s = File.read(file, 1024) or return false

    have_encoding = s.respond_to? :encoding

    if have_encoding then
      return false if s.encoding != Encoding::ASCII_8BIT and s.valid_encoding?
    end

    return true if s[0, 2] == Marshal.dump('')[0, 2] or s.index("\x00")

    if have_encoding then
      s.force_encoding Encoding.default_external

      not s.valid_encoding?
    else
      if 0.respond_to? :fdiv then
        s.count("\x00-\x7F", "^ -~\t\r\n").fdiv(s.size) > 0.3
      else # HACK 1.8.6
        (s.count("\x00-\x7F", "^ -~\t\r\n").to_f / s.size) > 0.3
      end
    end
  end

  ##
  # Processes common directives for CodeObjects for the C and Ruby parsers.
  #
  # Applies +directive+'s +value+ to +code_object+, if appropriate

  def self.process_directive code_object, directive, value
    warn "RDoc::Parser::process_directive is deprecated and wil be removed in RDoc 4.  Use RDoc::Markup::PreProcess#handle_directive instead" if $-w

    case directive
    when 'nodoc' then
      code_object.document_self = nil # notify nodoc
      code_object.document_children = value.downcase != 'all'
    when 'doc' then
      code_object.document_self = true
      code_object.force_documentation = true
    when 'yield', 'yields' then
      # remove parameter &block
      code_object.params.sub!(/,?\s*&\w+/, '') if code_object.params

      code_object.block_params = value
    when 'arg', 'args' then
      code_object.params = value
    end
  end

  ##
  # Checks if +file+ is a zip file in disguise.  Signatures from
  # http://www.garykessler.net/library/file_sigs.html

  def self.zip? file
    zip_signature = File.read file, 4

    zip_signature == "PK\x03\x04" or
      zip_signature == "PK\x05\x06" or
      zip_signature == "PK\x07\x08"
  end

  ##
  # Return a parser that can handle a particular extension

  def self.can_parse(file_name)
    parser = RDoc::Parser.parsers.find { |regexp,| regexp =~ file_name }.last

    # HACK Selenium hides a jar file using a .txt extension
    return if parser == RDoc::Parser::Simple and zip? file_name

    # The default parser must not parse binary files
    ext_name = File.extname file_name
    return parser if ext_name.empty?
    return if parser == RDoc::Parser::Simple and ext_name !~ /txt|rdoc/

    parser
  end

  ##
  # Find the correct parser for a particular file name. Return a SimpleParser
  # for ones that we don't know

  def self.for(top_level, file_name, body, options, stats)
    return if binary? file_name

    # If no extension, look for shebang
    if file_name !~ /\.\w+$/ && body =~ %r{\A#!(.+)} then
      shebang = $1
      case shebang
      when %r{env\s+ruby}, %r{/ruby}
        file_name = "dummy.rb"
      end
    end

    parser = can_parse file_name

    return unless parser

    parser.new top_level, file_name, body, options, stats
  end

  ##
  # Record which file types this parser can understand.
  #
  # It is ok to call this multiple times.

  def self.parse_files_matching(regexp)
    RDoc::Parser.parsers.unshift [regexp, self]
  end

  ##
  # Creates a new Parser storing +top_level+, +file_name+, +content+,
  # +options+ and +stats+ in instance variables.
  #
  # Usually invoked by +super+

  def initialize(top_level, file_name, content, options, stats)
    @top_level = top_level
    @file_name = file_name
    @content = content
    @options = options
    @stats = stats

    @preprocess = RDoc::Markup::PreProcess.new @file_name, @options.rdoc_include
    @preprocess.options = @options
  end

end

require 'rdoc/parser/simple'

