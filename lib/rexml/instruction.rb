require "rexml/child"
require "rexml/source"

module REXML
  # Represents an XML Instruction; IE, <? ... ?>
  # TODO: Add parent arg (3rd arg) to constructor
  class Instruction < Child
    START = '<\?'
    STOP = '\?>'

    # target is the "name" of the Instruction; IE, the "tag" in <?tag ...?>
    # content is everything else.
    attr_accessor :target, :content

    # Constructs a new Instruction
    # @param target can be one of a number of things.  If String, then
    # the target of this instruction is set to this.  If an Instruction,
    # then the Instruction is shallowly cloned (target and content are
    # copied).  If a Source, then the source is scanned and parsed for
    # an Instruction declaration.
    # @param content Must be either a String, or a Parent.  Can only
    # be a Parent if the target argument is a Source.  Otherwise, this
    # String is set as the content of this instruction.
    def initialize(target, content=nil)
      if target.kind_of? String
        super()
        @target = target
        @content = content
      elsif target.kind_of? Instruction
        super(content)
        @target = target.target
        @content = target.content
      end
      @content.strip! if @content
    end

    def clone
      Instruction.new self
    end

    # == DEPRECATED
    # See the rexml/formatters package
    #
    def write writer, indent=-1, transitive=false, ie_hack=false
      Kernel.warn( "#{self.class.name}.write is deprecated" )
      indent(writer, indent)
      writer << START.sub(/\\/u, '')
      writer << @target
      writer << ' '
      writer << @content
      writer << STOP.sub(/\\/u, '')
    end

    # @return true if other is an Instruction, and the content and target
    # of the other matches the target and content of this object.
    def ==( other )
      other.kind_of? Instruction and
      other.target == @target and
      other.content == @content
    end

    def node_type
      :processing_instruction
    end

    def inspect
      "<?p-i #{target} ...?>"
    end
  end
end
