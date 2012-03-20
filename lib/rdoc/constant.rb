require 'rdoc/code_object'

##
# A constant

class RDoc::Constant < RDoc::CodeObject

  ##
  # If this constant is an alias for a module or class,
  # this is the RDoc::ClassModule it is an alias for.
  # +nil+ otherwise.

  attr_accessor :is_alias_for

  ##
  # The constant's name

  attr_accessor :name

  ##
  # The constant's value

  attr_accessor :value

  ##
  # Creates a new constant with +name+, +value+ and +comment+

  def initialize(name, value, comment)
    super()
    @name = name
    @value = value
    @is_alias_for = nil
    self.comment = comment
  end

  ##
  # Constants are ordered by name

  def <=> other
    return unless self.class === other

    [parent_name, name] <=> [other.parent_name, other.name]
  end

  ##
  # Constants are equal when their #parent and #name is the same

  def == other
    self.class == other.class and
      @parent == other.parent and
      @name == other.name
  end

  ##
  # A constant is documented if it has a comment, or is an alias
  # for a documented class or module.

  def documented?
    super or is_alias_for && is_alias_for.documented?
  end

  def inspect # :nodoc:
    "#<%s:0x%x %s::%s>" % [
      self.class, object_id,
      parent_name, @name,
    ]
  end

  ##
  # Path to this constant

  def path
    "#{@parent.path}##{@name}"
  end

  def to_s # :nodoc:
    parent_name = parent ? parent.full_name : '(unknown)'
    if is_alias_for
      "constant #{parent_name}::#@name -> #{is_alias_for}"
    else
      "constant #{parent_name}::#@name"
    end
  end

end

