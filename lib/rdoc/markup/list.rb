##
# A List of ListItems

class RDoc::Markup::List

  ##
  # The list's type

  attr_accessor :type

  ##
  # Items in the list

  attr_reader :items

  ##
  # Creates a new list of +type+ with +items+

  def initialize type = nil, *items
    @type = type
    @items = []
    @items.push(*items)
  end

  ##
  # Appends +item+ to the list

  def << item
    @items << item
  end

  def == other # :nodoc:
    self.class == other.class and
      @type == other.type and
      @items == other.items
  end

  ##
  # Runs this list and all its #items through +visitor+

  def accept visitor
    visitor.accept_list_start self

    @items.each do |item|
      item.accept visitor
    end

    visitor.accept_list_end self
  end

  ##
  # Is the list empty?

  def empty?
    @items.empty?
  end

  ##
  # Returns the last item in the list

  def last
    @items.last
  end

  def pretty_print q # :nodoc:
    q.group 2, "[list: #{@type} ", ']' do
      q.seplist @items do |item|
        q.pp item
      end
    end
  end

  ##
  # Appends +items+ to the list

  def push *items
    @items.push(*items)
  end

end

