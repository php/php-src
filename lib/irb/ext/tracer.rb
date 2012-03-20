#
#   irb/lib/tracer.rb -
#   	$Release Version: 0.9.6$
#   	$Revision$
#   	by Keiju ISHITSUKA(keiju@ruby-lang.org)
#
# --
#
#
#
require "tracer"

module IRB

  # initialize tracing function
  def IRB.initialize_tracer
    Tracer.verbose = false
    Tracer.add_filter {
      |event, file, line, id, binding, *rests|
      /^#{Regexp.quote(@CONF[:IRB_LIB_PATH])}/ !~ file and
	File::basename(file) != "irb.rb"
    }
  end

  class Context
    attr_reader :use_tracer
    alias use_tracer? use_tracer

    def use_tracer=(opt)
      if opt
	Tracer.set_get_line_procs(@irb_path) {
	  |line_no, *rests|
	  @io.line(line_no)
	}
      elsif !opt && @use_tracer
	Tracer.off
      end
      @use_tracer=opt
    end
  end

  class WorkSpace
    alias __evaluate__ evaluate
    def evaluate(context, statements, file = nil, line = nil)
      if context.use_tracer? && file != nil && line != nil
	Tracer.on
	begin
	  __evaluate__(context, statements, file, line)
	ensure
	  Tracer.off
	end
      else
	__evaluate__(context, statements, file || __FILE__, line || __LINE__)
      end
    end
  end

  IRB.initialize_tracer
end

