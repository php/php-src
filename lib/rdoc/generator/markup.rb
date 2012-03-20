# This file is loaded by generators.  It allows RDoc's CodeObject tree to
# avoid loading generator code to increase startup time (for ri).

require 'rdoc/text'
require 'rdoc/code_objects'
require 'rdoc/generator'
require 'rdoc/markup/to_html_crossref'
require 'rdoc/ruby_token'

##
# Handle common RDoc::Markup tasks for various CodeObjects

module RDoc::Generator::Markup

  ##
  # Generates a relative URL from this object's path to +target_path+

  def aref_to(target_path)
    RDoc::Markup::ToHtml.gen_relative_url path, target_path
  end

  ##
  # Generates a relative URL from +from_path+ to this object's path

  def as_href(from_path)
    RDoc::Markup::ToHtml.gen_relative_url from_path, path
  end

  ##
  # Handy wrapper for marking up this object's comment

  def description
    markup @comment
  end

  ##
  # Creates an RDoc::Markup::ToHtmlCrossref formatter

  def formatter
    return @formatter if defined? @formatter

    show_hash = RDoc::RDoc.current.options.show_hash
    hyperlink_all = RDoc::RDoc.current.options.hyperlink_all
    this = RDoc::Context === self ? self : @parent

    @formatter = RDoc::Markup::ToHtmlCrossref.new(this.path, this, show_hash,
                                                  hyperlink_all)
  end

  ##
  # Build a webcvs URL starting for the given +url+ with +full_path+ appended
  # as the destination path.  If +url+ contains '%s' +full_path+ will be
  # sprintf'd into +url+ instead.

  def cvs_url(url, full_path)
    if /%s/ =~ url then
      sprintf url, full_path
    else
      url + full_path
    end
  end

end

class RDoc::AnyMethod

  include RDoc::Generator::Markup

  @add_line_numbers = false

  class << self
    ##
    # Allows controlling whether <tt>#markup_code</tt> adds line numbers to
    # the source code.

    attr_accessor :add_line_numbers
  end

  ##
  # Prepend +src+ with line numbers.  Relies on the first line of a source
  # code listing having:
  #
  #   # File xxxxx, line dddd
  #
  # If it has, line numbers are added an ', line dddd' is removed.

  def add_line_numbers(src)
    return unless src.sub!(/\A(.*)(, line (\d+))/, '\1')
    first = $3.to_i - 1
    last  = first + src.count("\n")
    size = last.to_s.length

    line = first
    src.gsub!(/^/) do
      res = if line == first then
              " " * (size + 1)
            else
              "<span class=\"line-num\">%2$*1$d</span> " % [size, line]
            end

      line += 1
      res
    end
  end

  ##
  # Turns the method's token stream into HTML.
  #
  # Prepends line numbers if +add_line_numbers+ is true.

  def markup_code
    return '' unless @token_stream

    src = ""

    @token_stream.each do |t|
      next unless t

      style = case t
              when RDoc::RubyToken::TkCONSTANT then 'ruby-constant'
              when RDoc::RubyToken::TkKW       then 'ruby-keyword'
              when RDoc::RubyToken::TkIVAR     then 'ruby-ivar'
              when RDoc::RubyToken::TkOp       then 'ruby-operator'
              when RDoc::RubyToken::TkId       then 'ruby-identifier'
              when RDoc::RubyToken::TkNode     then 'ruby-node'
              when RDoc::RubyToken::TkCOMMENT  then 'ruby-comment'
              when RDoc::RubyToken::TkREGEXP   then 'ruby-regexp'
              when RDoc::RubyToken::TkSTRING   then 'ruby-string'
              when RDoc::RubyToken::TkVal      then 'ruby-value'
              end

      text = CGI.escapeHTML t.text

      if style then
        src << "<span class=\"#{style}\">#{text}</span>"
      else
        src << text
      end
    end

    # dedent the source
    indent = src.length
    lines = src.lines.to_a
    lines.shift if src =~ /\A.*#\ *File/i # remove '# File' comment
    lines.each do |line|
      if line =~ /^ *(?=\S)/
        n = $&.length
        indent = n if n < indent
        break if n == 0
      end
    end
    src.gsub!(/^#{' ' * indent}/, '') if indent > 0

    add_line_numbers(src) if self.class.add_line_numbers

    src
  end

end

class RDoc::Attr

  include RDoc::Generator::Markup

end

class RDoc::Alias

  include RDoc::Generator::Markup

end

class RDoc::Constant

  include RDoc::Generator::Markup

end

class RDoc::Context

  include RDoc::Generator::Markup

end

class RDoc::Context::Section

  include RDoc::Generator::Markup

end

class RDoc::TopLevel

  ##
  # Returns a URL for this source file on some web repository.  Use the -W
  # command line option to set.

  def cvs_url
    url = RDoc::RDoc.current.options.webcvs

    if /%s/ =~ url then
      url % @absolute_name
    else
      url + @absolute_name
    end
  end

end

