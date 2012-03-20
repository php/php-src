require 'rdoc'
require 'rdoc/text'

##
# Base class for the RDoc code tree.
#
# We contain the common stuff for contexts (which are containers) and other
# elements (methods, attributes and so on)
#
# Here's the tree of the CodeObject subclasses:
#
# * RDoc::Context
#   * RDoc::TopLevel
#   * RDoc::ClassModule
#     * RDoc::AnonClass (never used so far)
#     * RDoc::NormalClass
#     * RDoc::NormalModule
#     * RDoc::SingleClass
# * RDoc::MethodAttr
#   * RDoc::Attr
#   * RDoc::AnyMethod
#     * RDoc::GhostMethod
#     * RDoc::MetaMethod
# * RDoc::Alias
# * RDoc::Constant
# * RDoc::Require
# * RDoc::Include

class RDoc::CodeObject

  include RDoc::Text

  ##
  # Our comment

  attr_reader :comment

  ##
  # Do we document our children?

  attr_reader :document_children

  ##
  # Do we document ourselves?

  attr_reader :document_self

  ##
  # Are we done documenting (ie, did we come across a :enddoc:)?

  attr_reader :done_documenting

  ##
  # Which file this code object was defined in

  attr_reader :file

  ##
  # Force documentation of this CodeObject

  attr_reader :force_documentation

  ##
  # Line in #file where this CodeObject was defined

  attr_accessor :line

  ##
  # Hash of arbitrary metadata for this CodeObject

  attr_reader :metadata

  ##
  # Offset in #file where this CodeObject was defined
  #--
  # TODO character or byte?

  attr_accessor :offset

  ##
  # Our parent CodeObject

  attr_accessor :parent

  ##
  # Did we ever receive a +:nodoc:+ directive?

  attr_reader :received_nodoc

  ##
  # Which section are we in

  attr_accessor :section

  ##
  # We are the model of the code, but we know that at some point we will be
  # worked on by viewers. By implementing the Viewable protocol, viewers can
  # associated themselves with these objects.

  attr_accessor :viewer

  ##
  # Creates a new CodeObject that will document itself and its children

  def initialize
    @metadata  = {}
    @comment   = ''
    @parent    = nil
    @file      = nil
    @full_name = nil

    @document_children   = true
    @document_self       = true
    @done_documenting    = false
    @force_documentation = false
    @received_nodoc      = false
    @ignored             = false
  end

  ##
  # Replaces our comment with +comment+, unless it is empty.

  def comment=(comment)
    @comment = case comment
               when NilClass               then ''
               when RDoc::Markup::Document then comment
               else
                 if comment and not comment.empty? then
                   normalize_comment comment
                 else
                   # TODO is this sufficient?
                   # HACK correct fix is to have #initialize create @comment
                   #      with the correct encoding
                   if String === @comment and
                      Object.const_defined? :Encoding and @comment.empty? then
                     @comment.force_encoding comment.encoding
                   end
                   @comment
                 end
               end
  end

  ##
  # Should this CodeObject be shown in documentation?

  def display?
    @document_self and not @ignored
  end

  ##
  # Enables or disables documentation of this CodeObject's children unless it
  # has been turned off by :enddoc:

  def document_children=(document_children)
    @document_children = document_children unless @done_documenting
  end

  ##
  # Enables or disables documentation of this CodeObject unless it has been
  # turned off by :enddoc:.  If the argument is +nil+ it means the
  # documentation is turned off by +:nodoc:+.

  def document_self=(document_self)
    return if @done_documenting

    @document_self = document_self
    @received_nodoc = true if document_self.nil?
  end

  ##
  # Does this object have a comment with content or is #received_nodoc true?

  def documented?
    @received_nodoc or !@comment.empty?
  end

  ##
  # Turns documentation on/off, and turns on/off #document_self
  # and #document_children.
  #
  # Once documentation has been turned off (by +:enddoc:+),
  # the object will refuse to turn #document_self or
  # #document_children on, so +:doc:+ and +:start_doc:+ directives
  # will have no effect in the current file.

  def done_documenting=(value)
    @done_documenting = value
    @document_self = !value
    @document_children = @document_self
  end

  ##
  # Yields each parent of this CodeObject.  See also
  # RDoc::ClassModule#each_ancestor

  def each_parent
    code_object = self

    while code_object = code_object.parent do
      yield code_object
    end

    self
  end

  ##
  # File name where this CodeObject was found.
  #
  # See also RDoc::Context#in_files

  def file_name
    return unless @file

    @file.absolute_name
  end

  ##
  # Force the documentation of this object unless documentation
  # has been turned off by :endoc:
  #--
  # HACK untested, was assigning to an ivar

  def force_documentation=(value)
    @force_documentation = value unless @done_documenting
  end

  ##
  # Sets the full_name overriding any computed full name.
  #
  # Set to +nil+ to clear RDoc's cached value

  def full_name= full_name
    @full_name = full_name
  end

  ##
  # Use this to ignore a CodeObject and all its children until found again
  # (#record_location is called).  An ignored item will not be shown in
  # documentation.
  #
  # See github issue #55
  #
  # The ignored status is temporary in order to allow implementation details
  # to be hidden.  At the end of processing a file RDoc allows all classes
  # and modules to add new documentation to previously created classes.
  #
  # If a class was ignored (via stopdoc) then reopened later with additional
  # documentation it should be shown.  If a class was ignored and never
  # reopened it should not be shown.  The ignore flag allows this to occur.

  def ignore
    @ignored = true

    stop_doc
  end

  ##
  # Has this class been ignored?

  def ignored?
    @ignored
  end

  ##
  # File name of our parent

  def parent_file_name
    @parent ? @parent.base_name : '(unknown)'
  end

  ##
  # Name of our parent

  def parent_name
    @parent ? @parent.full_name : '(unknown)'
  end

  ##
  # Records the RDoc::TopLevel (file) where this code object was defined

  def record_location top_level
    @ignored = false
    @file = top_level
  end

  ##
  # Enable capture of documentation unless documentation has been
  # turned off by :endoc:

  def start_doc
    return if @done_documenting

    @document_self = true
    @document_children = true
    @ignored = false
  end

  ##
  # Disable capture of documentation

  def stop_doc
    @document_self = false
    @document_children = false
  end

end

