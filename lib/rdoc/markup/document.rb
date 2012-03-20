##
# A Document containing lists, headings, paragraphs, etc.

class RDoc::Markup::Document

  ##
  # The file this document was created from.  See also
  # RDoc::ClassModule#add_comment

  attr_accessor :file

  ##
  # The parts of the Document

  attr_reader :parts

  ##
  # Creates a new Document with +parts+

  def initialize *parts
    @parts = []
    @parts.push(*parts)

    @file = nil
  end

  ##
  # Appends +part+ to the document

  def << part
    case part
    when RDoc::Markup::Document then
      unless part.empty? then
        parts.push(*part.parts)
        parts << RDoc::Markup::BlankLine.new
      end
    when String then
      raise ArgumentError,
            "expected RDoc::Markup::Document and friends, got String" unless
        part.empty?
    else
      parts << part
    end
  end

  def == other # :nodoc:
    self.class == other.class and
      @file == other.file and
      @parts == other.parts
  end

  ##
  # Runs this document and all its #items through +visitor+

  def accept visitor
    visitor.start_accepting

    @parts.each do |item|
      case item
      when RDoc::Markup::Document then # HACK
        visitor.accept_document item
      else
        item.accept visitor
      end
    end

    visitor.end_accepting
  end

  ##
  # Does this document have no parts?

  def empty?
    @parts.empty? or (@parts.length == 1 and merged? and @parts.first.empty?)
  end

  ##
  # When this is a collection of documents (#file is not set and this document
  # contains only other documents as its direct children) #merge replaces
  # documents in this class with documents from +other+ when the file matches
  # and adds documents from +other+ when the files do not.
  #
  # The information in +other+ is preferred over the receiver

  def merge other
    if empty? then
      @parts = other.parts
      return self
    end

    other.parts.each do |other_part|
      self.parts.delete_if do |self_part|
        self_part.file and self_part.file == other_part.file
      end

      self.parts << other_part
    end

    self
  end

  ##
  # Does this Document contain other Documents?

  def merged?
    RDoc::Markup::Document === @parts.first
  end

  def pretty_print q # :nodoc:
    start = @file ? "[doc (#{@file}): " : '[doc: '

    q.group 2, start, ']' do
      q.seplist @parts do |part|
        q.pp part
      end
    end
  end

  ##
  # Appends +parts+ to the document

  def push *parts
    self.parts.push(*parts)
  end

end

