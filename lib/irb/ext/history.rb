#
#   history.rb -
#   	$Release Version: 0.9.6$
#   	$Revision$
#   	by Keiju ISHITSUKA(keiju@ruby-lang.org)
#
# --
#
#
#

module IRB

  class Context

    NOPRINTING_IVARS.push "@eval_history_values"

    alias _set_last_value set_last_value

    def set_last_value(value)
      _set_last_value(value)

#      @workspace.evaluate self, "_ = IRB.CurrentContext.last_value"
      if @eval_history #and !@eval_history_values.equal?(llv)
 	@eval_history_values.push @line_no, @last_value
 	@workspace.evaluate self, "__ = IRB.CurrentContext.instance_eval{@eval_history_values}"
      end

      @last_value
    end

    attr_reader :eval_history
    def eval_history=(no)
      if no
	if defined?(@eval_history) && @eval_history
	  @eval_history_values.size(no)
	else
	  @eval_history_values = History.new(no)
	  IRB.conf[:__TMP__EHV__] = @eval_history_values
	  @workspace.evaluate(self, "__ = IRB.conf[:__TMP__EHV__]")
	  IRB.conf.delete(:__TMP_EHV__)
	end
      else
	@eval_history_values = nil
      end
      @eval_history = no
    end
  end

  class History
    @RCS_ID='-$Id$-'

    def initialize(size = 16)
      @size = size
      @contents = []
    end

    def size(size)
      if size != 0 && size < @size
	@contents = @contents[@size - size .. @size]
      end
      @size = size
    end

    def [](idx)
      begin
	if idx >= 0
	  @contents.find{|no, val| no == idx}[1]
	else
	  @contents[idx][1]
	end
      rescue NameError
	nil
      end
    end

    def push(no, val)
      @contents.push [no, val]
      @contents.shift if @size != 0 && @contents.size > @size
    end

    alias real_inspect inspect

    def inspect
      if @contents.empty?
	return real_inspect
      end

      unless (last = @contents.pop)[1].equal?(self)
	@contents.push last
	last = nil
      end
      str = @contents.collect{|no, val|
	if val.equal?(self)
	  "#{no} ...self-history..."
	else
	  "#{no} #{val.inspect}"
	end
      }.join("\n")
      if str == ""
	str = "Empty."
      end
      @contents.push last if last
      str
    end
  end
end


