require 'rdoc/code_object'

##
# A Module include in a class with \#include

class RDoc::Include < RDoc::CodeObject

  ##
  # Name of included module

  attr_accessor :name

  ##
  # Creates a new Include for +name+ with +comment+

  def initialize(name, comment)
    super()
    @name = name
    self.comment = comment
    @module = nil   # cache for module if found
  end

  ##
  # Includes are sorted by name

  def <=> other
    return unless self.class === other

    name <=> other.name
  end

  def == other # :nodoc:
    self.class == other.class and
      self.name == other.name
  end

  ##
  # Full name based on #module

  def full_name
    m = self.module
    RDoc::ClassModule === m ? m.full_name : @name
  end

  def inspect # :nodoc:
    "#<%s:0x%x %s.include %s>" % [
      self.class,
      object_id,
      parent_name, @name,
    ]
  end

  ##
  # Attempts to locate the included module object.  Returns the name if not
  # known.
  #
  # The scoping rules of Ruby to resolve the name of an included module are:
  # - first look into the children of the current context;
  # - if not found, look into the children of included modules,
  #   in reverse inclusion order;
  # - if still not found, go up the hierarchy of names.

  def module
    return @module if @module

    # search the current context
    return @name unless parent
    full_name = parent.child_name(@name)
    @module = RDoc::TopLevel.modules_hash[full_name]
    return @module if @module
    return @name if @name =~ /^::/

    # search the includes before this one, in reverse order
    searched = parent.includes.take_while { |i| i != self }.reverse
    searched.each do |i|
      inc = i.module
      next if String === inc
      full_name = inc.child_name(@name)
      @module = RDoc::TopLevel.modules_hash[full_name]
      return @module if @module
    end

    # go up the hierarchy of names
    p = parent.parent
    while p
      full_name = p.child_name(@name)
      @module = RDoc::TopLevel.modules_hash[full_name]
      return @module if @module
      p = p.parent
    end

    @name
  end

  def to_s # :nodoc:
    "include #@name in: #{parent}"
  end

end

