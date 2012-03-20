require 'rdoc/markup/formatter'
require 'rdoc/markup/inline'

require 'cgi'

##
# Outputs RDoc markup as HTML

class RDoc::Markup::ToHtml < RDoc::Markup::Formatter

  include RDoc::Text

  # :section: Utilities

  ##
  # Maps RDoc::Markup::Parser::LIST_TOKENS types to HTML tags

  LIST_TYPE_TO_HTML = {
    :BULLET => ['<ul>', '</ul>'],
    :LABEL  => ['<dl class="rdoc-list">', '</dl>'],
    :LALPHA => ['<ol style="display: lower-alpha">', '</ol>'],
    :NOTE   => ['<table class="rdoc-list">', '</table>'],
    :NUMBER => ['<ol>', '</ol>'],
    :UALPHA => ['<ol style="display: upper-alpha">', '</ol>'],
  }

  attr_reader :res # :nodoc:
  attr_reader :in_list_entry # :nodoc:
  attr_reader :list # :nodoc:

  ##
  # Path to this document for relative links

  attr_accessor :from_path

  ##
  # Converts a target url to one that is relative to a given path

  def self.gen_relative_url(path, target)
    from        = File.dirname path
    to, to_file = File.split target

    from = from.split "/"
    to   = to.split "/"

    from.delete '.'
    to.delete '.'

    while from.size > 0 and to.size > 0 and from[0] == to[0] do
      from.shift
      to.shift
    end

    from.fill ".."
    from.concat to
    from << to_file
    File.join(*from)
  end

  # :section:

  ##
  # Creates a new formatter that will output HTML

  def initialize markup = nil
    super

    @th = nil
    @in_list_entry = nil
    @list = nil
    @from_path = ''

    # external links
    @markup.add_special(/((link:|https?:|mailto:|ftp:|www\.)\S+\w)/, :HYPERLINK)

    # and links of the form  <text>[<url>]
    @markup.add_special(/(((\{.*?\})|\b\S+?)\[\S+?\])/, :TIDYLINK)

    init_tags
  end

  # :section: Special Handling
  #
  # These methods handle special markup added by RDoc::Markup#add_special.

  ##
  # +special+ is a potential link.  The following schemes are handled:
  #
  # <tt>mailto:</tt>::
  #   Inserted as-is.
  # <tt>http:</tt>::
  #   Links are checked to see if they reference an image. If so, that image
  #   gets inserted using an <tt><img></tt> tag. Otherwise a conventional
  #   <tt><a href></tt> is used.
  # <tt>link:</tt>::
  #   Reference to a local file relative to the output directory.

  def handle_special_HYPERLINK(special)
    url = special.text

    gen_url url, url
  end

  ##
  # This +special+ is a link where the label is different from the URL
  # <tt>label[url]</tt> or <tt>{long label}[url]</tt>

  def handle_special_TIDYLINK(special)
    text = special.text

    return text unless text =~ /\{(.*?)\}\[(.*?)\]/ or text =~ /(\S+)\[(.*?)\]/

    label = $1
    url   = $2
    gen_url url, label
  end

  # :section: Visitor
  #
  # These methods implement the HTML visitor.

  ##
  # Prepares the visitor for HTML generation

  def start_accepting
    @res = []
    @in_list_entry = []
    @list = []
  end

  ##
  # Returns the generated output

  def end_accepting
    @res.join
  end

  ##
  # Adds +paragraph+ to the output

  def accept_paragraph(paragraph)
    @res << "\n<p>"
    @res << wrap(to_html(paragraph.text))
    @res << "</p>\n"
  end

  ##
  # Adds +verbatim+ to the output

  def accept_verbatim(verbatim)
    @res << "\n<pre>"
    @res << CGI.escapeHTML(verbatim.text.rstrip)
    @res << "</pre>\n"
  end

  ##
  # Adds +rule+ to the output

  def accept_rule(rule)
    size = rule.weight
    size = 10 if size > 10
    @res << "<hr style=\"height: #{size}px\">\n"
  end

  ##
  # Prepares the visitor for consuming +list+

  def accept_list_start(list)
    @list << list.type
    @res << html_list_name(list.type, true)
    @in_list_entry.push false
  end

  ##
  # Finishes consumption of +list+

  def accept_list_end(list)
    @list.pop
    if tag = @in_list_entry.pop
      @res << tag
    end
    @res << html_list_name(list.type, false) << "\n"
  end

  ##
  # Prepares the visitor for consuming +list_item+

  def accept_list_item_start(list_item)
    if tag = @in_list_entry.last
      @res << tag
    end

    @res << list_item_start(list_item, @list.last)
  end

  ##
  # Finishes consumption of +list_item+

  def accept_list_item_end(list_item)
    @in_list_entry[-1] = list_end_for(@list.last)
  end

  ##
  # Adds +blank_line+ to the output

  def accept_blank_line(blank_line)
    # @res << annotate("<p />") << "\n"
  end

  ##
  # Adds +heading+ to the output

  def accept_heading(heading)
    @res << "\n<h#{heading.level}>"
    @res << to_html(heading.text)
    @res << "</h#{heading.level}>\n"
  end

  ##
  # Adds +raw+ to the output

  def accept_raw raw
    @res << raw.parts.join("\n")
  end

  # :section: Utilities

  ##
  # CGI escapes +text+

  def convert_string(text)
    CGI.escapeHTML text
  end

  ##
  # Generate a link for +url+, labeled with +text+.  Handles the special cases
  # for img: and link: described under handle_special_HYPERLINK

  def gen_url(url, text)
    if url =~ /([A-Za-z]+):(.*)/ then
      type = $1
      path = $2
    else
      type = "http"
      path = url
      url  = "http://#{url}"
    end

    if type == "link" then
      url = if path[0, 1] == '#' then # is this meaningful?
              path
            else
              self.class.gen_relative_url @from_path, path
            end
    end

    if (type == "http" or type == "https" or type == "link") and
       url =~ /\.(gif|png|jpg|jpeg|bmp)$/ then
      "<img src=\"#{url}\" />"
    else
      "<a href=\"#{url}\">#{text.sub(%r{^#{type}:/*}, '')}</a>"
    end
  end

  ##
  # Determines the HTML list element for +list_type+ and +open_tag+

  def html_list_name(list_type, open_tag)
    tags = LIST_TYPE_TO_HTML[list_type]
    raise RDoc::Error, "Invalid list type: #{list_type.inspect}" unless tags
    tags[open_tag ? 0 : 1]
  end

  ##
  # Maps attributes to HTML tags

  def init_tags
    add_tag :BOLD, "<b>",  "</b>"
    add_tag :TT,   "<tt>", "</tt>"
    add_tag :EM,   "<em>", "</em>"
  end

  ##
  # Returns the HTML tag for +list_type+, possible using a label from
  # +list_item+

  def list_item_start(list_item, list_type)
    case list_type
    when :BULLET, :LALPHA, :NUMBER, :UALPHA then
      "<li>"
    when :LABEL then
      "<dt>#{to_html list_item.label}</dt>\n<dd>"
    when :NOTE then
      "<tr><td class=\"rdoc-term\"><p>#{to_html list_item.label}</p></td>\n<td>"
    else
      raise RDoc::Error, "Invalid list type: #{list_type.inspect}"
    end
  end

  ##
  # Returns the HTML end-tag for +list_type+

  def list_end_for(list_type)
    case list_type
    when :BULLET, :LALPHA, :NUMBER, :UALPHA then
      "</li>"
    when :LABEL then
      "</dd>"
    when :NOTE then
      "</td></tr>"
    else
      raise RDoc::Error, "Invalid list type: #{list_type.inspect}"
    end
  end

  ##
  # Converts +item+ to HTML using RDoc::Text#to_html

  def to_html item
    super convert_flow @am.flow item
  end

end

