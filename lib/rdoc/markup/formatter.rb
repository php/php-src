require 'rdoc/markup'

##
# Base class for RDoc markup formatters
#
# Formatters use a visitor pattern to convert content into output.
#
# If you'd like to write your own Formatter use
# RDoc::Markup::FormatterTestCase.  If you're writing a text-output formatter
# use RDoc::Markup::TextFormatterTestCase which provides extra test cases.

class RDoc::Markup::Formatter

  ##
  # Tag for inline markup containing a +bit+ for the bitmask and the +on+ and
  # +off+ triggers.

  InlineTag = Struct.new(:bit, :on, :off)

  ##
  # Creates a new Formatter

  def initialize markup = nil
    @markup = markup || RDoc::Markup.new
    @am     = @markup.attribute_manager

    @attr_tags = []

    @in_tt = 0
    @tt_bit = RDoc::Markup::Attribute.bitmap_for :TT
  end

  ##
  # Adds +document+ to the output

  def accept_document document
    document.parts.each do |item|
      item.accept self
    end
  end

  ##
  # Add a new set of tags for an attribute. We allow separate start and end
  # tags for flexibility

  def add_tag(name, start, stop)
    attr = RDoc::Markup::Attribute.bitmap_for name
    @attr_tags << InlineTag.new(attr, start, stop)
  end

  ##
  # Allows +tag+ to be decorated with additional information.

  def annotate(tag)
    tag
  end

  ##
  # Marks up +content+

  def convert(content)
    @markup.convert content, self
  end

  ##
  # Converts flow items +flow+

  def convert_flow(flow)
    res = []

    flow.each do |item|
      case item
      when String then
        res << convert_string(item)
      when RDoc::Markup::AttrChanger then
        off_tags res, item
        on_tags res, item
      when RDoc::Markup::Special then
        res << convert_special(item)
      else
        raise "Unknown flow element: #{item.inspect}"
      end
    end

    res.join
  end

  ##
  # Converts added specials.  See RDoc::Markup#add_special

  def convert_special special
    return special.text if in_tt?

    handled = false

    RDoc::Markup::Attribute.each_name_of special.type do |name|
      method_name = "handle_special_#{name}"

      if respond_to? method_name then
        special.text = send method_name, special
        handled = true
      end
    end

    raise "Unhandled special: #{special}" unless handled

    special.text
  end

  ##
  # Converts a string to be fancier if desired

  def convert_string string
    string
  end

  ##
  # Are we currently inside tt tags?

  def in_tt?
    @in_tt > 0
  end

  ##
  # Turns on tags for +item+ on +res+

  def on_tags res, item
    attr_mask = item.turn_on
    return if attr_mask.zero?

    @attr_tags.each do |tag|
      if attr_mask & tag.bit != 0 then
        res << annotate(tag.on)
        @in_tt += 1 if tt? tag
      end
    end
  end

  ##
  # Turns off tags for +item+ on +res+

  def off_tags res, item
    attr_mask = item.turn_off
    return if attr_mask.zero?

    @attr_tags.reverse_each do |tag|
      if attr_mask & tag.bit != 0 then
        @in_tt -= 1 if tt? tag
        res << annotate(tag.off)
      end
    end
  end

  ##
  # Is +tag+ a tt tag?

  def tt? tag
    tag.bit == @tt_bit
  end

end

class RDoc::Markup
  autoload :ToAnsi,         'rdoc/markup/to_ansi'
  autoload :ToBs,           'rdoc/markup/to_bs'
  autoload :ToHtml,         'rdoc/markup/to_html'
  autoload :ToHtmlCrossref, 'rdoc/markup/to_html_crossref'
  autoload :ToRdoc,         'rdoc/markup/to_rdoc'
end
