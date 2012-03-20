require 'strscan'
require 'rdoc/text'

##
# A recursive-descent parser for RDoc markup.
#
# The parser tokenizes an input string then parses the tokens into a Document.
# Documents can be converted into output formats by writing a visitor like
# RDoc::Markup::ToHTML.
#
# The parser only handles the block-level constructs Paragraph, List,
# ListItem, Heading, Verbatim, BlankLine and Rule.  Inline markup such as
# <tt>\+blah\+</tt> is handled separately by RDoc::Markup::AttributeManager.
#
# To see what markup the Parser implements read RDoc.  To see how to use
# RDoc markup to format text in your program read RDoc::Markup.

class RDoc::Markup::Parser

  include RDoc::Text

  ##
  # List token types

  LIST_TOKENS = [
    :BULLET,
    :LABEL,
    :LALPHA,
    :NOTE,
    :NUMBER,
    :UALPHA,
  ]

  ##
  # Parser error subclass

  class Error < RuntimeError; end

  ##
  # Raised when the parser is unable to handle the given markup

  class ParseError < Error; end

  ##
  # Enables display of debugging information

  attr_accessor :debug

  ##
  # Token accessor

  attr_reader :tokens

  ##
  # Parses +str+ into a Document

  def self.parse str
    parser = new
    parser.tokenize str
    doc = RDoc::Markup::Document.new
    parser.parse doc
  end

  ##
  # Returns a token stream for +str+, for testing

  def self.tokenize str
    parser = new
    parser.tokenize str
    parser.tokens
  end

  ##
  # Creates a new Parser.  See also ::parse

  def initialize
    @tokens = []
    @current_token = nil
    @debug = false

    @line = 0
    @line_pos = 0
  end

  ##
  # Builds a Heading of +level+

  def build_heading level
    type, text, = get

    text = case type
           when :TEXT then
             skip :NEWLINE
             text
           else
             unget
             ''
           end

    RDoc::Markup::Heading.new level, text
  end

  ##
  # Builds a List flush to +margin+

  def build_list margin
    p :list_start => margin if @debug

    list = RDoc::Markup::List.new

    until @tokens.empty? do
      type, data, column, = get

      case type
      when :BULLET, :LABEL, :LALPHA, :NOTE, :NUMBER, :UALPHA then

        if column < margin || (list.type && list.type != type) then
          unget
          break
        end

        list.type = type
        peek_type, _, column, = peek_token

        case type
        when :NOTE, :LABEL then
          if peek_type == :NEWLINE then
            # description not on the same line as LABEL/NOTE
            # skip the trailing newline & any blank lines below
            while peek_type == :NEWLINE
              get
              peek_type, _, column, = peek_token
            end

            # we may be:
            #   - at end of stream
            #   - at a column < margin:
            #         [text]
            #       blah blah blah
            #   - at the same column, but with a different type of list item
            #       [text]
            #       * blah blah
            #   - at the same column, with the same type of list item
            #       [one]
            #       [two]
            # In all cases, we have an empty description.
            # In the last case only, we continue.
            if peek_type.nil? || column < margin then
              empty = 1
            elsif column == margin then
              case peek_type
              when type
                empty = 2 # continue
              when *LIST_TOKENS
                empty = 1
              else
                empty = 0
              end
            else
              empty = 0
            end

            if empty > 0 then
              item = RDoc::Markup::ListItem.new(data)
              item << RDoc::Markup::BlankLine.new
              list << item
              break if empty == 1
              next
            end
          end
        else
          data = nil
        end

        list_item = RDoc::Markup::ListItem.new data
        parse list_item, column
        list << list_item

      else
        unget
        break
      end
    end

    p :list_end => margin if @debug

    return nil if list.empty?

    list
  end

  ##
  # Builds a Paragraph that is flush to +margin+

  def build_paragraph margin
    p :paragraph_start => margin if @debug

    paragraph = RDoc::Markup::Paragraph.new

    until @tokens.empty? do
      type, data, column, = get

      if type == :TEXT && column == margin then
        paragraph << data
        skip :NEWLINE
      else
        unget
        break
      end
    end

    p :paragraph_end => margin if @debug

    paragraph
  end

  ##
  # Builds a Verbatim that is indented from +margin+.
  #
  # The verbatim block is shifted left (the least indented lines start in
  # column 0).  Each part of the verbatim is one line of text, always
  # terminated by a newline.  Blank lines always consist of a single newline
  # character, and there is never a single newline at the end of the verbatim.

  def build_verbatim margin
    p :verbatim_begin => margin if @debug
    verbatim = RDoc::Markup::Verbatim.new

    min_indent = nil
    generate_leading_spaces = true
    line = ''

    until @tokens.empty? do
      type, data, column, = get

      if type == :NEWLINE then
        line << data
        verbatim << line
        line = ''
        generate_leading_spaces = true
        next
      end

      if column <= margin
        unget
        break
      end

      if generate_leading_spaces then
        indent = column - margin
        line << ' ' * indent
        min_indent = indent if min_indent.nil? || indent < min_indent
        generate_leading_spaces = false
      end

      case type
      when :HEADER then
        line << '=' * data
        _, _, peek_column, = peek_token
        peek_column ||= column + data
        indent = peek_column - column - data
        line << ' ' * indent
      when :RULE then
        width = 2 + data
        line << '-' * width
        _, _, peek_column, = peek_token
        peek_column ||= column + width
        indent = peek_column - column - width
        line << ' ' * indent
      when :TEXT then
        line << data
      else # *LIST_TOKENS
        list_marker = case type
                      when :BULLET then data
                      when :LABEL  then "[#{data}]"
                      when :NOTE   then "#{data}::"
                      else # :LALPHA, :NUMBER, :UALPHA
                        "#{data}."
                      end
        line << list_marker
        peek_type, _, peek_column = peek_token
        unless peek_type == :NEWLINE then
          peek_column ||= column + list_marker.length
          indent = peek_column - column - list_marker.length
          line << ' ' * indent
        end
      end

    end

    verbatim << line << "\n" unless line.empty?
    verbatim.parts.each { |p| p.slice!(0, min_indent) unless p == "\n" } if min_indent > 0
    verbatim.normalize

    p :verbatim_end => margin if @debug

    verbatim
  end

  ##
  # Pulls the next token from the stream.

  def get
    @current_token = @tokens.shift
    p :get => @current_token if @debug
    @current_token
  end

  ##
  # Parses the tokens into an array of RDoc::Markup::XXX objects,
  # and appends them to the passed +parent+ RDoc::Markup::YYY object.
  #
  # Exits at the end of the token stream, or when it encounters a token
  # in a column less than +indent+ (unless it is a NEWLINE).
  #
  # Returns +parent+.

  def parse parent, indent = 0
    p :parse_start => indent if @debug

    until @tokens.empty? do
      type, data, column, = get

      if type == :NEWLINE then
        # trailing newlines are skipped below, so this is a blank line
        parent << RDoc::Markup::BlankLine.new
        skip :NEWLINE, false
        next
      end

      # indentation change: break or verbatim
      if column < indent then
        unget
        break
      elsif column > indent then
        unget
        parent << build_verbatim(indent)
        next
      end

      # indentation is the same
      case type
      when :HEADER then
        parent << build_heading(data)
      when :RULE then
        parent << RDoc::Markup::Rule.new(data)
        skip :NEWLINE
      when :TEXT then
        unget
        parent << build_paragraph(indent)
      when *LIST_TOKENS then
        unget
        parent << build_list(indent)
      else
        type, data, column, line = @current_token
        raise ParseError, "Unhandled token #{type} (#{data.inspect}) at #{line}:#{column}"
      end
    end

    p :parse_end => indent if @debug

    parent

  end

  ##
  # Returns the next token on the stream without modifying the stream

  def peek_token
    token = @tokens.first || []
    p :peek => token if @debug
    token
  end

  ##
  # Skips the next token if its type is +token_type+.
  #
  # Optionally raises an error if the next token is not of the expected type.

  def skip token_type, error = true
    type, = get
    return unless type # end of stream
    return @current_token if token_type == type
    unget
    raise ParseError, "expected #{token_type} got #{@current_token.inspect}" if error
  end

  ##
  # Turns text +input+ into a stream of tokens

  def tokenize input
    s = StringScanner.new input

    @line = 0
    @line_pos = 0

    until s.eos? do
      pos = s.pos

      # leading spaces will be reflected by the column of the next token
      # the only thing we loose are trailing spaces at the end of the file
      next if s.scan(/ +/)

      # note: after BULLET, LABEL, etc.,
      # indent will be the column of the next non-newline token

      @tokens << case
                 # [CR]LF => :NEWLINE
                 when s.scan(/\r?\n/) then
                   token = [:NEWLINE, s.matched, *token_pos(pos)]
                   @line_pos = s.pos
                   @line += 1
                   token
                 # === text => :HEADER then :TEXT
                 when s.scan(/(=+)(\s*)/) then
                   level = s[1].length
                   header = [:HEADER, level, *token_pos(pos)]

                   if s[2] =~ /^\r?\n/ then
                     s.pos -= s[2].length
                     header
                   else
                     pos = s.pos
                     s.scan(/.*/)
                     @tokens << header
                     [:TEXT, s.matched.sub(/\r$/, ''), *token_pos(pos)]
                   end
                 # --- (at least 3) and nothing else on the line => :RULE
                 when s.scan(/(-{3,}) *$/) then
                   [:RULE, s[1].length - 2, *token_pos(pos)]
                 # * or - followed by white space and text => :BULLET
                 when s.scan(/([*-]) +(\S)/) then
                   s.pos -= s[2].bytesize # unget \S
                   [:BULLET, s[1], *token_pos(pos)]
                 # A. text, a. text, 12. text => :UALPHA, :LALPHA, :NUMBER
                 when s.scan(/([a-z]|\d+)\. +(\S)/i) then
                   # FIXME if tab(s), the column will be wrong
                   # either support tabs everywhere by first expanding them to
                   # spaces, or assume that they will have been replaced
                   # before (and provide a check for that at least in debug
                   # mode)
                   list_label = s[1]
                   s.pos -= s[2].bytesize # unget \S
                   list_type =
                     case list_label
                     when /[a-z]/ then :LALPHA
                     when /[A-Z]/ then :UALPHA
                     when /\d/    then :NUMBER
                     else
                       raise ParseError, "BUG token #{list_label}"
                     end
                   [list_type, list_label, *token_pos(pos)]
                 # [text] followed by spaces or end of line => :LABEL
                 when s.scan(/\[(.*?)\]( +|$)/) then
                   [:LABEL, s[1], *token_pos(pos)]
                 # text:: followed by spaces or end of line => :NOTE
                 when s.scan(/(.*?)::( +|$)/) then
                   [:NOTE, s[1], *token_pos(pos)]
                 # anything else: :TEXT
                 else s.scan(/.*/)
                   [:TEXT, s.matched.sub(/\r$/, ''), *token_pos(pos)]
                 end
    end

    self
  end

  ##
  # Calculates the column and line of the current token based on +offset+.

  def token_pos offset
    [offset - @line_pos, @line]
  end

  ##
  # Returns the current token to the token stream

  def unget
    token = @current_token
    p :unget => token if @debug
    raise Error, 'too many #ungets' if token == @tokens.first
    @tokens.unshift token if token
  end

end

require 'rdoc/markup/blank_line'
require 'rdoc/markup/document'
require 'rdoc/markup/heading'
require 'rdoc/markup/list'
require 'rdoc/markup/list_item'
require 'rdoc/markup/raw'
require 'rdoc/markup/paragraph'
require 'rdoc/markup/indented_paragraph'
require 'rdoc/markup/rule'
require 'rdoc/markup/verbatim'

