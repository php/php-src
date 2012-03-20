# tof

#### tcltk library, more direct manipulation of tcl/tk
####    Sep. 5, 1997    Y. Shigehiro

require "tcltklib"

################

# module TclTk: collection of tcl/tk utilities (supplies namespace.)
module TclTk

  # initialize Hash to hold unique symbols and such
  @namecnt = {}

  # initialize Hash to hold callbacks
  @callback = {}
end

# TclTk.mainloop(): call TclTkLib.mainloop()
def TclTk.mainloop()
  print("mainloop: start\n") if $DEBUG
  TclTkLib.mainloop()
  print("mainloop: end\n") if $DEBUG
end

# TclTk.deletecallbackkey(ca): remove callback from TclTk module
#     this does not remove callbacks from tcl/tk interpreter
#     without calling this method, TclTkInterpreter will not be GCed
#   ca: callback(TclTkCallback)
def TclTk.deletecallbackkey(ca)
  print("deletecallbackkey: ", ca.to_s(), "\n") if $DEBUG
  @callback.delete(ca.to_s)
end

# TclTk.dcb(ca, wid, W): call TclTk.deletecallbackkey() for each callbacks
#     in an array.
#     this is for callback for top-level <Destroy>
#   ca: array of callbacks(TclTkCallback)
#   wid: top-level widget(TclTkWidget)
#   w: information about window given by %W(String)
def TclTk.dcb(ca, wid, w)
  if wid.to_s() == w
    ca.each{|i|
      TclTk.deletecallbackkey(i)
    }
  end
end

# TclTk._addcallback(ca): register callback
#   ca: callback(TclTkCallback)
def TclTk._addcallback(ca)
  print("_addcallback: ", ca.to_s(), "\n") if $DEBUG
  @callback[ca.to_s()] = ca
end

# TclTk._callcallback(key, arg): invoke registered callback
#   key: key to select callback (to_s value of the TclTkCallback)
#   arg: parameter from tcl/tk interpreter
def TclTk._callcallback(key, arg)
  print("_callcallback: ", @callback[key].inspect, "\n") if $DEBUG
  @callback[key]._call(arg)
  # throw out callback value
  # should return String to satisfy rb_eval_string()
  return ""
end

# TclTk._newname(prefix): generate unique name(String)
#   prefix: prefix of the unique name
def TclTk._newname(prefix)
  # generated name counter is stored in @namecnt
  if !@namecnt.key?(prefix)
    # first appearing prefix, initialize
    @namecnt[prefix] = 1
  else
    # already appeared prefix, generate next name
    @namecnt[prefix] += 1
  end
  return "#{prefix}#{@namecnt[prefix]}"
end

################

# class TclTkInterpreter: tcl/tk interpreter
class TclTkInterpreter

  # initialize():
  def initialize()
    # generate interpreter object
    @ip = TclTkIp.new()

    # add ruby_fmt command to tcl interpreter
    # ruby_fmt command format arguments by `format' and call `ruby' command
    # (notice ruby command receives only one argument)
    if $DEBUG
      @ip._eval("proc ruby_fmt {fmt args} { puts \"ruby_fmt: $fmt $args\" ; set cmd [list ruby [format $fmt $args]] ; uplevel $cmd }")
    else
      @ip._eval("proc ruby_fmt {fmt args} { set cmd [list ruby [format $fmt $args]] ; uplevel $cmd }")
    end

    # @ip._get_eval_string(*args): generate string to evaluate in tcl interpreter
    #   *args: script which is going to be evaluated under tcl/tk
    def @ip._get_eval_string(*args)
      argstr = ""
      args.each{|arg|
        argstr += " " if argstr != ""
        # call to_eval if it is defined
        if (arg.respond_to?(:to_eval))
          argstr += arg.to_eval()
        else
          # call to_s unless defined
          argstr += arg.to_s()
        end
      }
      return argstr
    end

    # @ip._eval_args(*args): evaluate string under tcl/tk interpreter
    #     returns result string.
    #   *args: script which is going to be evaluated under tcl/tk
    def @ip._eval_args(*args)
      # calculate the string to eval in the interpreter
      argstr = _get_eval_string(*args)

      # evaluate under the interpreter
      print("_eval: \"", argstr, "\"") if $DEBUG
      res = _eval(argstr)
      if $DEBUG
        print(" -> \"", res, "\"\n")
      elsif  _return_value() != 0
        print(res, "\n")
      end
      fail(%Q/can't eval "#{argstr}"/) if _return_value() != 0 #'
      return res
    end

    # generate tcl/tk command object and register in the hash
    @commands = {}
    # for all commands registered in tcl/tk interpreter:
    @ip._eval("info command").split(/ /).each{|comname|
      if comname =~ /^[.]/
        # if command is a widget (path), generate TclTkWidget,
        # and register it in the hash
        @commands[comname] = TclTkWidget.new(@ip, comname)
      else
        # otherwise, generate TclTkCommand
        @commands[comname] = TclTkCommand.new(@ip, comname)
      end
    }
  end

  # commands(): returns hash of the tcl/tk commands
  def commands()
    return @commands
  end

  # rootwidget(): returns root widget(TclTkWidget)
  def rootwidget()
    return @commands["."]
  end

  # _tcltkip(): returns @ip(TclTkIp)
  def _tcltkip()
    return @ip
  end

  # method_missing(id, *args): execute undefined method as tcl/tk command
  #   id: method symbol
  #   *args: method arguments
  def method_missing(id, *args)
    # if command named by id registered, then execute it
    if @commands.key?(id.id2name)
      return @commands[id.id2name].e(*args)
    else
      # otherwise, exception
      super
    end
  end
end

# class TclTkObject: base class of the tcl/tk objects
class TclTkObject

  # initialize(ip, exp):
  #   ip: interpreter(TclTkIp)
  #   exp: tcl/tk representation
  def initialize(ip, exp)
    fail("type is not TclTkIp") if !ip.kind_of?(TclTkIp)
    @ip = ip
    @exp = exp
  end

  # to_s(): returns tcl/tk representation
  def to_s()
    return @exp
  end
end

# class TclTkCommand: tcl/tk commands
# you should not call TclTkCommand.new()
# commands are created by TclTkInterpreter:initialize()
class TclTkCommand < TclTkObject

  # e(*args): execute command.  returns String (e is for exec or eval)
  #   *args: command arguments
  def e(*args)
    return @ip._eval_args(to_s(), *args)
  end
end

# class TclTkLibCommand: tcl/tk commands in the library
class TclTkLibCommand < TclTkCommand

  # initialize(ip, name):
  #   ip: interpreter(TclTkInterpreter)
  #   name: command name (String)
  def initialize(ip, name)
    super(ip._tcltkip, name)
  end
end

# class TclTkVariable: tcl/tk variable
class TclTkVariable < TclTkObject

  # initialize(interp, dat):
  #   interp: interpreter(TclTkInterpreter)
  #   dat: the value to set(String)
  #       if nil, not initialize variable
  def initialize(interp, dat)
    # auto-generate tcl/tk representation (variable name)
    exp = TclTk._newname("v_")
    # initialize TclTkObject
    super(interp._tcltkip(), exp)
    # safe this for `set' command
    @set = interp.commands()["set"]
    # set value
    set(dat) if dat
  end

  # although you can set/read variables by using set in tcl/tk,
  # we provide the method for accessing variables

  # set(data): set tcl/tk variable using `set'
  #   data: new value
  def set(data)
    @set.e(to_s(), data.to_s())
  end

  # get(): read tcl/tk variable(String) using `set'
  def get()
    return @set.e(to_s())
  end
end

# class TclTkWidget: tcl/tk widget
class TclTkWidget < TclTkCommand

  # initialize(*args):
  #   *args: parameters
  def initialize(*args)
    if args[0].kind_of?(TclTkIp)
      # in case the 1st argument is TclTkIp:

      # Wrap tcl/tk widget by TclTkWidget
      # (used in TclTkInterpreter#initialize())

      # need two arguments
      fail("invalid # of parameter") if args.size != 2

      # ip: interpreter(TclTkIp)
      # exp: tcl/tk representation
      ip, exp = args

      # initialize TclTkObject
      super(ip, exp)
    elsif args[0].kind_of?(TclTkInterpreter)
      # in case 1st parameter is TclTkInterpreter:

      # generate new widget from parent widget

      # interp: interpreter(TclTkInterpreter)
      # parent: parent widget
      # command: widget generating tk command(label 等)
      # *args: argument to the command
      interp, parent, command, *args = args

      # generate widget name
      exp = parent.to_s()
      exp += "." if exp !~ /[.]$/
      exp += TclTk._newname("w_")
      # initialize TclTkObject
      super(interp._tcltkip(), exp)
      # generate widget
      res = @ip._eval_args(command, exp, *args)
#      fail("can't create Widget") if res != exp
      # for tk_optionMenu, it is legal res != exp
    else
      fail("first parameter is not TclTkInterpreter")
    end
  end
end

# class TclTkCallback: tcl/tk callbacks
class TclTkCallback < TclTkObject

  # initialize(interp, pr, arg):
  #   interp: interpreter(TclTkInterpreter)
  #   pr: callback procedure(Proc)
  #   arg: string to pass as block parameters of pr
  #       bind command of tcl/tk uses % replacement for parameters
  #       pr can receive replaced data using block parameter
  #       its format is specified by arg string
  #       You should not specify arg for the command like
  #       scrollbar with -command option, which receives parameters
  #       without specifying any replacement
  def initialize(interp, pr, arg = nil)
    # auto-generate tcl/tk representation (variable name)
    exp = TclTk._newname("c_")
    # initialize TclTkObject
    super(interp._tcltkip(), exp)
    # save parameters
    @pr = pr
    @arg = arg
    # register in the module
    TclTk._addcallback(self)
  end

  # to_eval(): retuens string representation for @ip._eval_args
  def to_eval()
    if @arg
      # bind replaces %s before calling ruby_fmt, so %%s is used
      s = %Q/{ruby_fmt {TclTk._callcallback("#{to_s()}", "%%s")} #{@arg}}/
    else
      s = %Q/{ruby_fmt {TclTk._callcallback("#{to_s()}", "%s")}}/
    end

    return s
  end

  # _call(arg): invoke callback
  #   arg: callback parameter
  def _call(arg)
    @pr.call(arg)
  end
end

# class TclTkImage: tcl/tk images
class TclTkImage < TclTkCommand

  # initialize(interp, t, *args):
  #     generating image is done by TclTkImage.new()
  #     destrying is done by image delete (inconsistent, sigh)
  #   interp: interpreter(TclTkInterpreter)
  #   t: image type (photo, bitmap, etc.)
  #   *args: command argument
  def initialize(interp, t, *args)
    # auto-generate tcl/tk representation
    exp = TclTk._newname("i_")
    # initialize TclTkObject
    super(interp._tcltkip(), exp)
    # generate image
    res = @ip._eval_args("image create", t, exp, *args)
    fail("can't create Image") if res != exp
  end
end

# eof
