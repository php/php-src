#
#   irb/context.rb - irb context
#   	$Release Version: 0.9.6$
#   	$Revision$
#   	by Keiju ISHITSUKA(keiju@ruby-lang.org)
#
# --
#
#
#
require "irb/workspace"
require "irb/inspector"

module IRB
  class Context
    #
    # Arguments:
    #   input_method: nil -- stdin or readline
    #		      String -- File
    #		      other -- using this as InputMethod
    #
    def initialize(irb, workspace = nil, input_method = nil, output_method = nil)
      @irb = irb
      if workspace
	@workspace = workspace
      else
	@workspace = WorkSpace.new
      end
      @thread = Thread.current if defined? Thread
#      @irb_level = 0

      # copy of default configuration
      @ap_name = IRB.conf[:AP_NAME]
      @rc = IRB.conf[:RC]
      @load_modules = IRB.conf[:LOAD_MODULES]

      @use_readline = IRB.conf[:USE_READLINE]
      @verbose = IRB.conf[:VERBOSE]
      @io = nil

      self.inspect_mode = IRB.conf[:INSPECT_MODE]
      self.math_mode = IRB.conf[:MATH_MODE] if IRB.conf[:MATH_MODE]
      self.use_tracer = IRB.conf[:USE_TRACER] if IRB.conf[:USE_TRACER]
      self.use_loader = IRB.conf[:USE_LOADER] if IRB.conf[:USE_LOADER]
      self.eval_history = IRB.conf[:EVAL_HISTORY] if IRB.conf[:EVAL_HISTORY]

      @ignore_sigint = IRB.conf[:IGNORE_SIGINT]
      @ignore_eof = IRB.conf[:IGNORE_EOF]

      @back_trace_limit = IRB.conf[:BACK_TRACE_LIMIT]

      self.prompt_mode = IRB.conf[:PROMPT_MODE]

      if IRB.conf[:SINGLE_IRB] or !defined?(JobManager)
	@irb_name = IRB.conf[:IRB_NAME]
      else
	@irb_name = "irb#"+IRB.JobManager.n_jobs.to_s
      end
      @irb_path = "(" + @irb_name + ")"

      case input_method
      when nil
	case use_readline?
	when nil
	  if (defined?(ReadlineInputMethod) && STDIN.tty? &&
	      IRB.conf[:PROMPT_MODE] != :INF_RUBY)
	    @io = ReadlineInputMethod.new
	  else
	    @io = StdioInputMethod.new
	  end
	when false
	  @io = StdioInputMethod.new
	when true
	  if defined?(ReadlineInputMethod)
	    @io = ReadlineInputMethod.new
	  else
	    @io = StdioInputMethod.new
	  end
	end

      when String
	@io = FileInputMethod.new(input_method)
	@irb_name = File.basename(input_method)
	@irb_path = input_method
      else
	@io = input_method
      end
      self.save_history = IRB.conf[:SAVE_HISTORY] if IRB.conf[:SAVE_HISTORY]

      if output_method
	@output_method = output_method
      else
	@output_method = StdioOutputMethod.new
      end

      @echo = IRB.conf[:ECHO]
      if @echo.nil?
	@echo = true
      end
      @debug_level = IRB.conf[:DEBUG_LEVEL]
    end

    def main
      @workspace.main
    end

    attr_reader :workspace_home
    attr_accessor :workspace
    attr_reader :thread
    attr_accessor :io

    attr_accessor :irb
    attr_accessor :ap_name
    attr_accessor :rc
    attr_accessor :load_modules
    attr_accessor :irb_name
    attr_accessor :irb_path

    attr_reader :use_readline
    attr_reader :inspect_mode

    attr_reader :prompt_mode
    attr_accessor :prompt_i
    attr_accessor :prompt_s
    attr_accessor :prompt_c
    attr_accessor :prompt_n
    attr_accessor :auto_indent_mode
    attr_accessor :return_format

    attr_accessor :ignore_sigint
    attr_accessor :ignore_eof
    attr_accessor :echo
    attr_accessor :verbose
    attr_reader :debug_level

    attr_accessor :back_trace_limit

    alias use_readline? use_readline
    alias rc? rc
    alias ignore_sigint? ignore_sigint
    alias ignore_eof? ignore_eof
    alias echo? echo

    def verbose?
      if @verbose.nil?
	if defined?(ReadlineInputMethod) && @io.kind_of?(ReadlineInputMethod)
	  false
	elsif !STDIN.tty? or @io.kind_of?(FileInputMethod)
	  true
	else
	  false
	end
      else
	@verbose
      end
    end

    def prompting?
      verbose? || (STDIN.tty? && @io.kind_of?(StdioInputMethod) ||
		(defined?(ReadlineInputMethod) && @io.kind_of?(ReadlineInputMethod)))
    end

    attr_reader :last_value

    def set_last_value(value)
      @last_value = value
      @workspace.evaluate self, "_ = IRB.CurrentContext.last_value"
    end

    def prompt_mode=(mode)
      @prompt_mode = mode
      pconf = IRB.conf[:PROMPT][mode]
      @prompt_i = pconf[:PROMPT_I]
      @prompt_s = pconf[:PROMPT_S]
      @prompt_c = pconf[:PROMPT_C]
      @prompt_n = pconf[:PROMPT_N]
      @return_format = pconf[:RETURN]
      if ai = pconf.include?(:AUTO_INDENT)
	@auto_indent_mode = ai
      else
	@auto_indent_mode = IRB.conf[:AUTO_INDENT]
      end
    end

    def inspect?
      @inspect_mode.nil? or @inspect_mode
    end

    def file_input?
      @io.class == FileInputMethod
    end

    def inspect_mode=(opt)

      if i = INSPECTORS[opt]
	@inspect_mode = opt
	@inspect_method = i
	i.init
      else
	case opt
	when nil
	  if INSPECTORS.keys_with_inspector(INSPECTORS[true]).include?(@inspect_mode)
	    self.inspect_mode = false
	  elsif INSPECTORS.keys_with_inspector(INSPECTORS[false]).include?(@inspect_mode)
	    self.inspect_mode = true
	  else
	    puts "Can't switch inspect mode."
	    return
	  end
	when /^\s*\{.*\}\s*$/
	  begin
	    inspector = eval "proc#{opt}"
	  rescue Exception
	    puts "Can't switch inspect mode(#{opt})."
	    return
	  end
	  self.inspect_mode = inspector
	when Proc
	  self.inspect_mode = IRB::Inspector(opt)
	when Inspector
	  prefix = "usr%d"
	  i = 1
	  while INSPECTORS[format(prefix, i)]; i += 1; end
	  @inspect_mode = format(prefix, i)
	  @inspect_method = opt
	  INSPECTORS.def_inspector(format(prefix, i), @inspect_method)
	else
	  puts "Can't switch inspect mode(#{opt})."
	  return
	end
      end
      print "Switch to#{unless @inspect_mode; ' non';end} inspect mode.\n" if verbose?
      @inspect_mode
    end


    def use_readline=(opt)
      @use_readline = opt
      print "use readline module\n" if @use_readline
    end

    def debug_level=(value)
      @debug_level = value
      RubyLex.debug_level = value
      SLex.debug_level = value
    end

    def debug?
      @debug_level > 0
    end

    def evaluate(line, line_no)
      @line_no = line_no
      set_last_value(@workspace.evaluate(self, line, irb_path, line_no))
#      @workspace.evaluate("_ = IRB.conf[:MAIN_CONTEXT]._")
#      @_ = @workspace.evaluate(line, irb_path, line_no)
    end

    def inspect_last_value
      @inspect_method.inspect_value(@last_value)
    end

    alias __exit__ exit
    def exit(ret = 0)
      IRB.irb_exit(@irb, ret)
    end

    NOPRINTING_IVARS = ["@last_value"]
    NO_INSPECTING_IVARS = ["@irb", "@io"]
    IDNAME_IVARS = ["@prompt_mode"]

    alias __inspect__ inspect
    def inspect
      array = []
      for ivar in instance_variables.sort{|e1, e2| e1 <=> e2}
	ivar = ivar.to_s
	name = ivar.sub(/^@(.*)$/, '\1')
	val = instance_eval(ivar)
	case ivar
	when *NOPRINTING_IVARS
	  array.push format("conf.%s=%s", name, "...")
	when *NO_INSPECTING_IVARS
	  array.push format("conf.%s=%s", name, val.to_s)
	when *IDNAME_IVARS
	  array.push format("conf.%s=:%s", name, val.id2name)
	else
	  array.push format("conf.%s=%s", name, val.inspect)
	end
      end
      array.join("\n")
    end
    alias __to_s__ to_s
    alias to_s inspect
  end
end
