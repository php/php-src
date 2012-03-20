##
# Manages changes of attributes in a block of text

class RDoc::Markup::AttributeManager

  ##
  # The NUL character

  NULL = "\000".freeze

  #--
  # We work by substituting non-printing characters in to the text. For now
  # I'm assuming that I can substitute a character in the range 0..8 for a 7
  # bit character without damaging the encoded string, but this might be
  # optimistic
  #++

  A_PROTECT = 004 # :nodoc:

  ##
  # Special mask character to prevent inline markup handling

  PROTECT_ATTR = A_PROTECT.chr # :nodoc:

  ##
  # This maps delimiters that occur around words (such as *bold* or +tt+)
  # where the start and end delimiters and the same. This lets us optimize
  # the regexp

  attr_reader :matching_word_pairs

  ##
  # And this is used when the delimiters aren't the same. In this case the
  # hash maps a pattern to the attribute character

  attr_reader :word_pair_map

  ##
  # This maps HTML tags to the corresponding attribute char

  attr_reader :html_tags

  ##
  # A \ in front of a character that would normally be processed turns off
  # processing. We do this by turning \< into <#{PROTECT}

  attr_reader :protectable

  ##
  # And this maps _special_ sequences to a name. A special sequence is
  # something like a WikiWord

  attr_reader :special

  ##
  # Creates a new attribute manager that understands bold, emphasized and
  # teletype text.

  def initialize
    @html_tags = {}
    @matching_word_pairs = {}
    @protectable = %w[<]
    @special = {}
    @word_pair_map = {}

    add_word_pair "*", "*", :BOLD
    add_word_pair "_", "_", :EM
    add_word_pair "+", "+", :TT

    add_html "em", :EM
    add_html "i",  :EM
    add_html "b",  :BOLD
    add_html "tt",   :TT
    add_html "code", :TT
  end

  ##
  # Return an attribute object with the given turn_on and turn_off bits set

  def attribute(turn_on, turn_off)
    RDoc::Markup::AttrChanger.new turn_on, turn_off
  end

  ##
  # Changes the current attribute from +current+ to +new+

  def change_attribute current, new
    diff = current ^ new
    attribute(new & diff, current & diff)
  end

  ##
  # Used by the tests to change attributes by name from +current_set+ to
  # +new_set+

  def changed_attribute_by_name current_set, new_set
    current = new = 0
    current_set.each do |name|
      current |= RDoc::Markup::Attribute.bitmap_for(name)
    end

    new_set.each do |name|
      new |= RDoc::Markup::Attribute.bitmap_for(name)
    end

    change_attribute(current, new)
  end

  ##
  # Copies +start_pos+ to +end_pos+ from the current string

  def copy_string(start_pos, end_pos)
    res = @str[start_pos...end_pos]
    res.gsub!(/\000/, '')
    res
  end

  ##
  # Map attributes like <b>text</b>to the sequence
  # \001\002<char>\001\003<char>, where <char> is a per-attribute specific
  # character

  def convert_attrs(str, attrs)
    # first do matching ones
    tags = @matching_word_pairs.keys.join("")

    re = /(^|\W)([#{tags}])([#:\\]?[\w.\/-]+?\S?)\2(\W|$)/

    1 while str.gsub!(re) do
      attr = @matching_word_pairs[$2]
      attrs.set_attrs($`.length + $1.length + $2.length, $3.length, attr)
      $1 + NULL * $2.length + $3 + NULL * $2.length + $4
    end

    # then non-matching
    unless @word_pair_map.empty? then
      @word_pair_map.each do |regexp, attr|
        str.gsub!(regexp) {
          attrs.set_attrs($`.length + $1.length, $2.length, attr)
          NULL * $1.length + $2 + NULL * $3.length
        }
      end
    end
  end

  ##
  # Converts HTML tags to RDoc attributes

  def convert_html(str, attrs)
    tags = @html_tags.keys.join '|'

    1 while str.gsub!(/<(#{tags})>(.*?)<\/\1>/i) {
      attr = @html_tags[$1.downcase]
      html_length = $1.length + 2
      seq = NULL * html_length
      attrs.set_attrs($`.length + html_length, $2.length, attr)
      seq + $2 + seq + NULL
    }
  end

  ##
  # Converts special sequences to RDoc attributes

  def convert_specials(str, attrs)
    unless @special.empty?
      @special.each do |regexp, attr|
        str.scan(regexp) do
          attrs.set_attrs($`.length, $&.length,
                          attr | RDoc::Markup::Attribute::SPECIAL)
        end
      end
    end
  end

  ##
  # Escapes special sequences of text to prevent conversion to RDoc

  def mask_protected_sequences
    # protect __send__, __FILE__, etc.
    @str.gsub!(/__([a-z]+)__/i,
      "_#{PROTECT_ATTR}_#{PROTECT_ATTR}\\1_#{PROTECT_ATTR}_#{PROTECT_ATTR}")
    @str.gsub!(/\\([#{Regexp.escape @protectable.join('')}])/,
               "\\1#{PROTECT_ATTR}")
  end

  ##
  # Unescapes special sequences of text

  def unmask_protected_sequences
    @str.gsub!(/(.)#{PROTECT_ATTR}/, "\\1\000")
  end

  ##
  # Adds a markup class with +name+ for words wrapped in the +start+ and
  # +stop+ character.  To make words wrapped with "*" bold:
  #
  #   am.add_word_pair '*', '*', :BOLD

  def add_word_pair(start, stop, name)
    raise ArgumentError, "Word flags may not start with '<'" if
      start[0,1] == '<'

    bitmap = RDoc::Markup::Attribute.bitmap_for name

    if start == stop then
      @matching_word_pairs[start] = bitmap
    else
      pattern = /(#{Regexp.escape start})(\S+)(#{Regexp.escape stop})/
      @word_pair_map[pattern] = bitmap
    end

    @protectable << start[0,1]
    @protectable.uniq!
  end

  ##
  # Adds a markup class with +name+ for words surrounded by HTML tag +tag+.
  # To process emphasis tags:
  #
  #   am.add_html 'em', :EM

  def add_html(tag, name)
    @html_tags[tag.downcase] = RDoc::Markup::Attribute.bitmap_for name
  end

  ##
  # Adds a special handler for +pattern+ with +name+.  A simple URL handler
  # would be:
  #
  #   @am.add_special(/((https?:)\S+\w)/, :HYPERLINK)

  def add_special(pattern, name)
    @special[pattern] = RDoc::Markup::Attribute.bitmap_for name
  end

  ##
  # Processes +str+ converting attributes, HTML and specials

  def flow(str)
    @str = str

    mask_protected_sequences

    @attrs = RDoc::Markup::AttrSpan.new @str.length

    convert_attrs    @str, @attrs
    convert_html     @str, @attrs
    convert_specials @str, @attrs

    unmask_protected_sequences

    split_into_flow
  end

  ##
  # Debug method that prints a string along with its attributes

  def display_attributes
    puts
    puts @str.tr(NULL, "!")
    bit = 1
    16.times do |bno|
      line = ""
      @str.length.times do |i|
        if (@attrs[i] & bit) == 0
          line << " "
        else
          if bno.zero?
            line << "S"
          else
            line << ("%d" % (bno+1))
          end
        end
      end
      puts(line) unless line =~ /^ *$/
      bit <<= 1
    end
  end

  ##
  # Splits the string into chunks by attribute change

  def split_into_flow
    res = []
    current_attr = 0

    str_len = @str.length

    # skip leading invisible text
    i = 0
    i += 1 while i < str_len and @str[i].chr == "\0"
    start_pos = i

    # then scan the string, chunking it on attribute changes
    while i < str_len
      new_attr = @attrs[i]
      if new_attr != current_attr
        if i > start_pos
          res << copy_string(start_pos, i)
          start_pos = i
        end

        res << change_attribute(current_attr, new_attr)
        current_attr = new_attr

        if (current_attr & RDoc::Markup::Attribute::SPECIAL) != 0 then
          i += 1 while
            i < str_len and (@attrs[i] & RDoc::Markup::Attribute::SPECIAL) != 0

          res << RDoc::Markup::Special.new(current_attr,
                                           copy_string(start_pos, i))
          start_pos = i
          next
        end
      end

      # move on, skipping any invisible characters
      begin
        i += 1
      end while i < str_len and @str[i].chr == "\0"
    end

    # tidy up trailing text
    if start_pos < str_len
      res << copy_string(start_pos, str_len)
    end

    # and reset to all attributes off
    res << change_attribute(current_attr, 0) if current_attr != 0

    res
  end

end

