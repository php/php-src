##
# An item within a List that contains paragraphs, headings, etc.

class RDoc::Markup::ListItem

  ##
  # The label for the ListItem

  attr_accessor :label

  ##
  # Parts of the ListItem

  attr_reader :parts

  ##
  # Creates a new ListItem with an optional +label+ containing +parts+

  def initialize label = nil, *parts
    @label = label
    @parts = []
    @parts.push(*parts)
  end

  ##
  # Appends +part+ to the ListItem

  def << part
    @parts << part
  end

  def == other # :nodoc:
    self.class == other.class and
      @label == other.label and
      @parts == other.parts
  end

  ##
  # Runs this list item and all its #parts through +visitor+

  def accept visitor
    visitor.accept_list_item_start self

    @parts.each do |part|
      part.accept visitor
    end

    visitor.accept_list_item_end self
  end

  ##
  # Is the ListItem empty?

  def empty?
    @parts.empty?
  end

  ##
  # Length of parts in the ListItem

  def length
    @parts.length
  end

  def pretty_print q # :nodoc:
    q.group 2, '[item: ', ']' do
      if @label then
        q.text @label
        q.breakable
      end

      q.seplist @parts do |part|
        q.pp part
      end
    end
  end

  ##
  # Adds +parts+ to the ListItem

  def push *parts
    @parts.push(*parts)
  end

end

