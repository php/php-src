# This class implements a pretty printing algorithm. It finds line breaks and
# nice indentations for grouped structure.
#
# By default, the class assumes that primitive elements are strings and each
# byte in the strings have single column in width. But it can be used for
# other situations by giving suitable arguments for some methods:
# * newline object and space generation block for PrettyPrint.new
# * optional width argument for PrettyPrint#text
# * PrettyPrint#breakable
#
# There are several candidate uses:
# * text formatting using proportional fonts
# * multibyte characters which has columns different to number of bytes
# * non-string formatting
#
# == Bugs
# * Box based formatting?
# * Other (better) model/algorithm?
#
# == References
# Christian Lindig, Strictly Pretty, March 2000,
# http://www.st.cs.uni-sb.de/~lindig/papers/#pretty
#
# Philip Wadler, A prettier printer, March 1998,
# http://homepages.inf.ed.ac.uk/wadler/topics/language-design.html#prettier
#
# == Author
# Tanaka Akira <akr@m17n.org>
#
class PrettyPrint

  # This is a convenience method which is same as follows:
  #
  #   begin
  #     q = PrettyPrint.new(output, maxwidth, newline, &genspace)
  #     ...
  #     q.flush
  #     output
  #   end
  #
  def PrettyPrint.format(output='', maxwidth=79, newline="\n", genspace=lambda {|n| ' ' * n})
    q = PrettyPrint.new(output, maxwidth, newline, &genspace)
    yield q
    q.flush
    output
  end

  # This is similar to PrettyPrint::format but the result has no breaks.
  #
  # +maxwidth+, +newline+ and +genspace+ are ignored.
  #
  # The invocation of +breakable+ in the block doesn't break a line and is
  # treated as just an invocation of +text+.
  #
  def PrettyPrint.singleline_format(output='', maxwidth=nil, newline=nil, genspace=nil)
    q = SingleLine.new(output)
    yield q
    output
  end

  # Creates a buffer for pretty printing.
  #
  # +output+ is an output target. If it is not specified, '' is assumed. It
  # should have a << method which accepts the first argument +obj+ of
  # PrettyPrint#text, the first argument +sep+ of PrettyPrint#breakable, the
  # first argument +newline+ of PrettyPrint.new, and the result of a given
  # block for PrettyPrint.new.
  #
  # +maxwidth+ specifies maximum line length. If it is not specified, 79 is
  # assumed. However actual outputs may overflow +maxwidth+ if long
  # non-breakable texts are provided.
  #
  # +newline+ is used for line breaks. "\n" is used if it is not specified.
  #
  # The block is used to generate spaces. {|width| ' ' * width} is used if it
  # is not given.
  #
  def initialize(output='', maxwidth=79, newline="\n", &genspace)
    @output = output
    @maxwidth = maxwidth
    @newline = newline
    @genspace = genspace || lambda {|n| ' ' * n}

    @output_width = 0
    @buffer_width = 0
    @buffer = []

    root_group = Group.new(0)
    @group_stack = [root_group]
    @group_queue = GroupQueue.new(root_group)
    @indent = 0
  end
  attr_reader :output, :maxwidth, :newline, :genspace
  attr_reader :indent, :group_queue

  # Returns the group most recently added to the stack.
  def current_group
    @group_stack.last
  end

  # first? is a predicate to test the call is a first call to first? with
  # current group.
  #
  # It is useful to format comma separated values as:
  #
  #   q.group(1, '[', ']') {
  #     xxx.each {|yyy|
  #       unless q.first?
  #         q.text ','
  #         q.breakable
  #       end
  #       ... pretty printing yyy ...
  #     }
  #   }
  #
  # first? is obsoleted in 1.8.2.
  #
  def first?
    warn "PrettyPrint#first? is obsoleted at 1.8.2."
    current_group.first?
  end

  # Breaks the buffer into lines that are shorter than #maxwidth
  def break_outmost_groups
    while @maxwidth < @output_width + @buffer_width
      return unless group = @group_queue.deq
      until group.breakables.empty?
        data = @buffer.shift
        @output_width = data.output(@output, @output_width)
        @buffer_width -= data.width
      end
      while !@buffer.empty? && Text === @buffer.first
        text = @buffer.shift
        @output_width = text.output(@output, @output_width)
        @buffer_width -= text.width
      end
    end
  end

  # This adds +obj+ as a text of +width+ columns in width.
  #
  # If +width+ is not specified, obj.length is used.
  #
  def text(obj, width=obj.length)
    if @buffer.empty?
      @output << obj
      @output_width += width
    else
      text = @buffer.last
      unless Text === text
        text = Text.new
        @buffer << text
      end
      text.add(obj, width)
      @buffer_width += width
      break_outmost_groups
    end
  end

  # This is similar to #breakable except
  # the decision to break or not is determined individually.
  #
  # Two #fill_breakable under a group may cause 4 results:
  # (break,break), (break,non-break), (non-break,break), (non-break,non-break).
  # This is different to #breakable because two #breakable under a group
  # may cause 2 results:
  # (break,break), (non-break,non-break).
  #
  # The text sep+ is inserted if a line is not broken at this point.
  #
  # If +sep+ is not specified, " " is used.
  #
  # If +width+ is not specified, +sep.length+ is used. You will have to
  # specify this when +sep+ is a multibyte character, for example.
  #
  def fill_breakable(sep=' ', width=sep.length)
    group { breakable sep, width }
  end

  # This says "you can break a line here if necessary", and a +width+\-column
  # text +sep+ is inserted if a line is not broken at the point.
  #
  # If +sep+ is not specified, " " is used.
  #
  # If +width+ is not specified, +sep.length+ is used. You will have to
  # specify this when +sep+ is a multibyte character, for example.
  #
  def breakable(sep=' ', width=sep.length)
    group = @group_stack.last
    if group.break?
      flush
      @output << @newline
      @output << @genspace.call(@indent)
      @output_width = @indent
      @buffer_width = 0
    else
      @buffer << Breakable.new(sep, width, self)
      @buffer_width += width
      break_outmost_groups
    end
  end

  # Groups line break hints added in the block. The line break hints are all
  # to be used or not.
  #
  # If +indent+ is specified, the method call is regarded as nested by
  # nest(indent) { ... }.
  #
  # If +open_obj+ is specified, <tt>text open_obj, open_width</tt> is called
  # before grouping. If +close_obj+ is specified, <tt>text close_obj,
  # close_width</tt> is called after grouping.
  #
  def group(indent=0, open_obj='', close_obj='', open_width=open_obj.length, close_width=close_obj.length)
    text open_obj, open_width
    group_sub {
      nest(indent) {
        yield
      }
    }
    text close_obj, close_width
  end

  def group_sub
    group = Group.new(@group_stack.last.depth + 1)
    @group_stack.push group
    @group_queue.enq group
    begin
      yield
    ensure
      @group_stack.pop
      if group.breakables.empty?
        @group_queue.delete group
      end
    end
  end

  # Increases left margin after newline with +indent+ for line breaks added in
  # the block.
  #
  def nest(indent)
    @indent += indent
    begin
      yield
    ensure
      @indent -= indent
    end
  end

  # outputs buffered data.
  #
  def flush
    @buffer.each {|data|
      @output_width = data.output(@output, @output_width)
    }
    @buffer.clear
    @buffer_width = 0
  end

  class Text
    def initialize
      @objs = []
      @width = 0
    end
    attr_reader :width

    def output(out, output_width)
      @objs.each {|obj| out << obj}
      output_width + @width
    end

    def add(obj, width)
      @objs << obj
      @width += width
    end
  end

  class Breakable
    def initialize(sep, width, q)
      @obj = sep
      @width = width
      @pp = q
      @indent = q.indent
      @group = q.current_group
      @group.breakables.push self
    end
    attr_reader :obj, :width, :indent

    def output(out, output_width)
      @group.breakables.shift
      if @group.break?
        out << @pp.newline
        out << @pp.genspace.call(@indent)
        @indent
      else
        @pp.group_queue.delete @group if @group.breakables.empty?
        out << @obj
        output_width + @width
      end
    end
  end

  class Group
    def initialize(depth)
      @depth = depth
      @breakables = []
      @break = false
    end
    attr_reader :depth, :breakables

    def break
      @break = true
    end

    def break?
      @break
    end

    def first?
      if defined? @first
        false
      else
        @first = false
        true
      end
    end
  end

  class GroupQueue
    def initialize(*groups)
      @queue = []
      groups.each {|g| enq g}
    end

    def enq(group)
      depth = group.depth
      @queue << [] until depth < @queue.length
      @queue[depth] << group
    end

    def deq
      @queue.each {|gs|
        (gs.length-1).downto(0) {|i|
          unless gs[i].breakables.empty?
            group = gs.slice!(i, 1).first
            group.break
            return group
          end
        }
        gs.each {|group| group.break}
        gs.clear
      }
      return nil
    end

    def delete(group)
      @queue[group.depth].delete(group)
    end
  end

  class SingleLine
    def initialize(output, maxwidth=nil, newline=nil)
      @output = output
      @first = [true]
    end

    def text(obj, width=nil)
      @output << obj
    end

    def breakable(sep=' ', width=nil)
      @output << sep
    end

    def nest(indent)
      yield
    end

    def group(indent=nil, open_obj='', close_obj='', open_width=nil, close_width=nil)
      @first.push true
      @output << open_obj
      yield
      @output << close_obj
      @first.pop
    end

    def flush
    end

    def first?
      result = @first[-1]
      @first[-1] = false
      result
    end
  end
end
