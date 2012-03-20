require 'rdoc'
class RDoc::Markup

  ##
  # We manage a set of attributes. Each attribute has a symbol name and a bit
  # value.

  class Attribute

    ##
    # Special attribute type.  See RDoc::Markup#add_special

    SPECIAL = 1

    @@name_to_bitmap = { :_SPECIAL_ => SPECIAL }
    @@next_bitmap = 2

    ##
    # Returns a unique bit for +name+

    def self.bitmap_for(name)
      bitmap = @@name_to_bitmap[name]
      unless bitmap then
        bitmap = @@next_bitmap
        @@next_bitmap <<= 1
        @@name_to_bitmap[name] = bitmap
      end
      bitmap
    end

    ##
    # Returns a string representation of +bitmap+

    def self.as_string(bitmap)
      return "none" if bitmap.zero?
      res = []
      @@name_to_bitmap.each do |name, bit|
        res << name if (bitmap & bit) != 0
      end
      res.join(",")
    end

    ##
    # yields each attribute name in +bitmap+

    def self.each_name_of(bitmap)
      @@name_to_bitmap.each do |name, bit|
        next if bit == SPECIAL
        yield name.to_s if (bitmap & bit) != 0
      end
    end

  end

  AttrChanger = Struct.new :turn_on, :turn_off # :nodoc:

  ##
  # An AttrChanger records a change in attributes. It contains a bitmap of the
  # attributes to turn on, and a bitmap of those to turn off.

  class AttrChanger
    def to_s # :nodoc:
      "Attr: +#{Attribute.as_string turn_on}/-#{Attribute.as_string turn_off}"
    end

    def inspect # :nodoc:
      "+%s/-%s" % [
        Attribute.as_string(turn_on),
        Attribute.as_string(turn_off),
      ]
    end
  end

  ##
  # An array of attributes which parallels the characters in a string.

  class AttrSpan

    ##
    # Creates a new AttrSpan for +length+ characters

    def initialize(length)
      @attrs = Array.new(length, 0)
    end

    ##
    # Toggles +bits+ from +start+ to +length+
    def set_attrs(start, length, bits)
      for i in start ... (start+length)
        @attrs[i] |= bits
      end
    end

    ##
    # Accesses flags for character +n+

    def [](n)
      @attrs[n]
    end

  end

  ##
  # Hold details of a special sequence

  class Special

    ##
    # Special type

    attr_reader   :type

    ##
    # Special text

    attr_accessor :text

    ##
    # Creates a new special sequence of +type+ with +text+

    def initialize(type, text)
      @type, @text = type, text
    end

    ##
    # Specials are equal when the have the same text and type

    def ==(o)
      self.text == o.text && self.type == o.type
    end

    def inspect # :nodoc:
      "#<RDoc::Markup::Special:0x%x @type=%p, name=%p @text=%p>" % [
        object_id, @type, RDoc::Markup::Attribute.as_string(type), text.dump]
    end

    def to_s # :nodoc:
      "Special: type=#{type}, name=#{RDoc::Markup::Attribute.as_string type}, text=#{text.dump}"
    end

  end

end

