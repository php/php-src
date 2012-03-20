#
#               remote-tk.rb - supports to control remote Tk interpreters
#                       by Hidetoshi NAGAI <nagai@ai.kyutech.ac.jp>

if defined? MultiTkIp
  fail RuntimeError, "'remote-tk' library must be required before requiring 'multi-tk'"
end

class MultiTkIp; end
class RemoteTkIp < MultiTkIp; end

class MultiTkIp
  @@IP_TABLE = TkUtil.untrust({}) unless defined?(@@IP_TABLE)
  @@TK_TABLE_LIST = TkUtil.untrust([]) unless defined?(@@TK_TABLE_LIST)
  def self._IP_TABLE; @@IP_TABLE; end
  def self._TK_TABLE_LIST; @@TK_TABLE_LIST; end

  @flag = true
  def self._DEFAULT_MASTER
    # work only once
    if @flag
      @flag = nil
      @@DEFAULT_MASTER
    else
      nil
    end
  end
end
class RemoteTkIp
  @@IP_TABLE = MultiTkIp._IP_TABLE unless defined?(@@IP_TABLE)
  @@TK_TABLE_LIST = MultiTkIp._TK_TABLE_LIST unless defined?(@@TK_TABLE_LIST)
end
class << MultiTkIp
  undef _IP_TABLE
  undef _TK_TABLE_LIST
end

require 'multi-tk'

class RemoteTkIp
  if defined?(@@DEFAULT_MASTER)
    MultiTkIp._DEFAULT_MASTER
  else
    @@DEFAULT_MASTER = MultiTkIp._DEFAULT_MASTER
  end
end


###############################

class << RemoteTkIp
  undef new_master, new_slave, new_safe_slave
  undef new_trusted_slave, new_safeTk

  def new(*args, &b)
    ip = __new(*args)
    ip.eval_proc(&b) if b
    ip
  end
end

class RemoteTkIp
  def initialize(remote_ip, displayof=nil, timeout=5)
    if $SAFE >= 4
      fail SecurityError, "cannot access another interpreter at level #{$SAFE}"
    end

    @interp = MultiTkIp.__getip
    if @interp.safe?
      fail SecurityError, "safe-IP cannot create RemoteTkIp"
    end


    @interp.allow_ruby_exit = false
    @appname = @interp._invoke('tk', 'appname')
    @remote = remote_ip.to_s.dup.freeze
    if displayof.kind_of?(TkWindow)
      @displayof = displayof.path.dup.freeze
    else
      @displayof = nil
    end
    if self.deleted?
      fail RuntimeError, "no Tk application named \"#{@remote}\""
    end

    @tk_windows = {}
    @tk_table_list = []
    @slave_ip_tbl = {}
    @slave_ip_top = {}

    @force_default_encoding ||= TkUtil.untrust([false])
    @encoding ||= TkUtil.untrust([nil])
    def @encoding.to_s; self.join(nil); end

    TkUtil.untrust(@tk_windows)    unless @tk_windows.tainted?
    TkUtil.untrust(@tk_table_list) unless @tk_table_list.tainted?
    TkUtil.untrust(@slave_ip_tbl)  unless @slave_ip_tbl.tainted?
    TkUtil.untrust(@slave_ip_top)  unless @slave_ip_top.tainted?

    @system = Object.new

    @threadgroup  = ThreadGroup.new

    @safe_level = [$SAFE]

    @wait_on_mainloop = [true, 0]

    @cmd_queue = Queue.new

=begin
    @cmd_receiver, @receiver_watchdog = _create_receiver_and_watchdog()

    @threadgroup.add @cmd_receiver
    @threadgroup.add @receiver_watchdog

    @threadgroup.enclose
=end
    @@DEFAULT_MASTER.assign_receiver_and_watchdog(self)

    @@IP_TABLE[@threadgroup] = self
    @@TK_TABLE_LIST.size.times{
      (tbl = {}).tainted? || TkUtil.untrust(tbl)
      @tk_table_list << tbl
    }

    @ret_val = TkVariable.new
    if timeout > 0 && ! _available_check(timeout)
      fail RuntimeError, "cannot create connection"
    end
    @ip_id = _create_connection

    class << self
      undef :instance_eval
    end

    self.freeze  # defend against modification
  end

  def manipulable?
    return true if (Thread.current.group == ThreadGroup::Default)
    MultiTkIp.__getip == @interp && ! @interp.safe?
  end
  def self.manipulable?
    true
  end

  def _is_master_of?(tcltkip_obj)
    tcltkip_obj == @interp
  end
  protected :_is_master_of?

  def _ip_id_
    @ip_id
  end

  def _available_check(timeout = 5)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?

    return nil if timeout < 1
    @ret_val.value = ''
    @interp._invoke('send', '-async', @remote,
                    'send', '-async', Tk.appname,
                    "set #{@ret_val.id} ready")
    Tk.update
    if @ret_val != 'ready'
      (1..(timeout*5)).each{
        sleep 0.2
        Tk.update
        break if @ret_val == 'ready'
      }
    end
    @ret_val.value == 'ready'
  end
  private :_available_check

  def _create_connection
    raise SecurityError, "no permission to manipulate" unless self.manipulable?

    ip_id = '_' + @interp._invoke('send', @remote, <<-'EOS') + '_'
      if {[catch {set _rubytk_control_ip_id_} ret] != 0} {
        set _rubytk_control_ip_id_ 0
      } else {
        set _rubytk_control_ip_id_ [expr $ret + 1]
      }
      return $_rubytk_control_ip_id_
    EOS

    @interp._invoke('send', @remote, <<-EOS)
      proc rb_out#{ip_id} args {
        send #{@appname} rb_out \$args
      }
    EOS

    ip_id
  end
  private :_create_connection

  def _appsend(enc_mode, async, *cmds)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?

    p ['_appsend', [@remote, @displayof], enc_mode, async, cmds] if $DEBUG
    if $SAFE >= 4
      fail SecurityError, "cannot send commands at level 4"
    elsif $SAFE >= 1 && cmds.find{|obj| obj.tainted?}
      fail SecurityError, "cannot send tainted commands at level #{$SAFE}"
    end

    cmds = @interp._merge_tklist(*TkUtil::_conv_args([], enc_mode, *cmds))
    if @displayof
      if async
        @interp.__invoke('send', '-async', '-displayof', @displayof,
                         '--', @remote, *cmds)
      else
        @interp.__invoke('send', '-displayof', @displayof,
                         '--', @remote, *cmds)
      end
    else
      if async
        @interp.__invoke('send', '-async', '--', @remote, *cmds)
      else
        @interp.__invoke('send', '--', @remote, *cmds)
      end
    end
  end
  private :_appsend

  def ready?(timeout=5)
    if timeout < 0
      fail ArgumentError, "timeout must be positive number"
    end
    _available_check(timeout)
  end

  def is_rubytk?
    return false if _appsend(false, false, 'info', 'command', 'ruby') == ""
    [ _appsend(false, false, 'ruby', 'RUBY_VERSION'),
      _appsend(false, false, 'set', 'tk_patchLevel') ]
  end

  def appsend(async, *args)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?

    if async != true && async != false && async != nil
      args.unshift(async)
      async = false
    end
    if @displayof
      Tk.appsend_displayof(@remote, @displayof, async, *args)
    else
      Tk.appsend(@remote, async, *args)
    end
  end

  def rb_appsend(async, *args)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?

    if async != true && async != false && async != nil
      args.unshift(async)
      async = false
    end
    if @displayof
      Tk.rb_appsend_displayof(@remote, @displayof, async, *args)
    else
      Tk.rb_appsend(@remote, async, *args)
    end
  end

  def create_slave(name, safe=false)
    if safe
      safe_opt = ''
    else
      safe_opt = '-safe'
    end
    _appsend(false, false, "interp create #{safe_opt} -- #{name}")
  end

  def make_safe
    fail RuntimeError, 'cannot change safe mode of the remote interpreter'
  end

  def safe?
    _appsend(false, false, 'interp issafe')
  end

  def safe_base?
    false
  end

  def allow_ruby_exit?
    false
  end

  def allow_ruby_exit= (mode)
    fail RuntimeError, 'cannot change mode of the remote interpreter'
  end

  def delete
    _appsend(false, true, 'exit')
  end

  def deleted?
    raise SecurityError, "no permission to manipulate" unless self.manipulable?

    if @displayof
      lst = @interp._invoke_without_enc('winfo', 'interps',
                                        '-displayof', @displayof)
    else
      lst = @interp._invoke_without_enc('winfo', 'interps')
    end
    # unless @interp._split_tklist(lst).index(@remote)
    unless @interp._split_tklist(lst).index(_toUTF8(@remote))
      true
    else
      false
    end
  end

  def has_mainwindow?
    raise SecurityError, "no permission to manipulate" unless self.manipulable?

    begin
      inf = @interp._invoke_without_enc('info', 'command', '.')
    rescue Exception
      return nil
    end
    if !inf.kind_of?(String) || inf != '.'
      false
    else
      true
    end
  end

  def invalid_namespace?
    false
  end

  def restart
    fail RuntimeError, 'cannot restart the remote interpreter'
  end

  def __eval(str)
    _appsend(false, false, str)
  end
  def _eval(str)
    _appsend(nil, false, str)
  end
  def _eval_without_enc(str)
    _appsend(false, false, str)
  end
  def _eval_with_enc(str)
    _appsend(true, false, str)
  end

  def _invoke(*args)
    _appsend(nil, false, *args)
  end

  def __invoke(*args)
    _appsend(false, false, *args)
  end
  def _invoke(*args)
    _appsend(nil, false, *args)
  end
  def _invoke_without_enc(*args)
    _appsend(false, false, *args)
  end
  def _invoke_with_enc(*args)
    _appsend(true, false, *args)
  end

  def _toUTF8(str, encoding=nil)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._toUTF8(str, encoding)
  end

  def _fromUTF8(str, encoding=nil)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._fromUTF8(str, encoding)
  end

  def _thread_vwait(var_name)
    _appsend(false, 'thread_vwait', varname)
  end

  def _thread_tkwait(mode, target)
    _appsend(false, 'thread_tkwait', mode, target)
  end

  def _return_value
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._return_value
  end

  def _get_variable(var_name, flag)
    # ignore flag
    _appsend(false, 'set', TkComm::_get_eval_string(var_name))
  end
  def _get_variable2(var_name, index_name, flag)
    # ignore flag
    _appsend(false, 'set', "#{TkComm::_get_eval_string(var_name)}(#{TkComm::_get_eval_string(index_name)})")
  end

  def _set_variable(var_name, value, flag)
    # ignore flag
    _appsend(false, 'set', TkComm::_get_eval_string(var_name), TkComm::_get_eval_string(value))
  end
  def _set_variable2(var_name, index_name, value, flag)
    # ignore flag
    _appsend(false, 'set', "#{TkComm::_get_eval_string(var_name)}(#{TkComm::_get_eval_string(index_name)})", TkComm::_get_eval_string(value))
  end

  def _unset_variable(var_name, flag)
    # ignore flag
    _appsend(false, 'unset', TkComm::_get_eval_string(var_name))
  end
  def _unset_variable2(var_name, index_name, flag)
    # ignore flag
    _appsend(false, 'unset', "#{var_name}(#{index_name})")
  end

  def _get_global_var(var_name)
    _appsend(false, 'set', TkComm::_get_eval_string(var_name))
  end
  def _get_global_var2(var_name, index_name)
    _appsend(false, 'set', "#{TkComm::_get_eval_string(var_name)}(#{TkComm::_get_eval_string(index_name)})")
  end

  def _set_global_var(var_name, value)
    _appsend(false, 'set', TkComm::_get_eval_string(var_name), TkComm::_get_eval_string(value))
  end
  def _set_global_var2(var_name, index_name, value)
    _appsend(false, 'set', "#{TkComm::_get_eval_string(var_name)}(#{TkComm::_get_eval_string(index_name)})", TkComm::_get_eval_string(value))
  end

  def _unset_global_var(var_name)
    _appsend(false, 'unset', TkComm::_get_eval_string(var_name))
  end
  def _unset_global_var2(var_name, index_name)
    _appsend(false, 'unset', "#{var_name}(#{index_name})")
  end

  def _split_tklist(str)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._split_tklist(str)
  end

  def _merge_tklist(*args)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._merge_tklist(*args)
  end

  def _conv_listelement(str)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._conv_listelement(str)
  end

  def _create_console
    fail RuntimeError, 'not support "_create_console" on the remote interpreter'
  end

  def mainloop
    fail RuntimeError, 'not support "mainloop" on the remote interpreter'
  end
  def mainloop_watchdog
    fail RuntimeError, 'not support "mainloop_watchdog" on the remote interpreter'
  end
  def do_one_event(flag = nil)
    fail RuntimeError, 'not support "do_one_event" on the remote interpreter'
  end
  def mainloop_abort_on_exception
    fail RuntimeError, 'not support "mainloop_abort_on_exception" on the remote interpreter'
  end
  def mainloop_abort_on_exception=(mode)
    fail RuntimeError, 'not support "mainloop_abort_on_exception=" on the remote interpreter'
  end
  def set_eventloop_tick(*args)
    fail RuntimeError, 'not support "set_eventloop_tick" on the remote interpreter'
  end
  def get_eventloop_tick
    fail RuntimeError, 'not support "get_eventloop_tick" on the remote interpreter'
  end
  def set_no_event_wait(*args)
    fail RuntimeError, 'not support "set_no_event_wait" on the remote interpreter'
  end
  def get_no_event_wait
    fail RuntimeError, 'not support "get_no_event_wait" on the remote interpreter'
  end
  def set_eventloop_weight(*args)
    fail RuntimeError, 'not support "set_eventloop_weight" on the remote interpreter'
  end
  def get_eventloop_weight
    fail RuntimeError, 'not support "get_eventloop_weight" on the remote interpreter'
  end
end

class << RemoteTkIp
  def mainloop(*args)
    fail RuntimeError, 'not support "mainloop" on the remote interpreter'
  end
  def mainloop_watchdog(*args)
    fail RuntimeError, 'not support "mainloop_watchdog" on the remote interpreter'
  end
  def do_one_event(flag = nil)
    fail RuntimeError, 'not support "do_one_event" on the remote interpreter'
  end
  def mainloop_abort_on_exception
    fail RuntimeError, 'not support "mainloop_abort_on_exception" on the remote interpreter'
  end
  def mainloop_abort_on_exception=(mode)
    fail RuntimeError, 'not support "mainloop_abort_on_exception=" on the remote interpreter'
  end
  def set_eventloop_tick(*args)
    fail RuntimeError, 'not support "set_eventloop_tick" on the remote interpreter'
  end
  def get_eventloop_tick
    fail RuntimeError, 'not support "get_eventloop_tick" on the remote interpreter'
  end
  def set_no_event_wait(*args)
    fail RuntimeError, 'not support "set_no_event_wait" on the remote interpreter'
  end
  def get_no_event_wait
    fail RuntimeError, 'not support "get_no_event_wait" on the remote interpreter'
  end
  def set_eventloop_weight(*args)
    fail RuntimeError, 'not support "set_eventloop_weight" on the remote interpreter'
  end
  def get_eventloop_weight
    fail RuntimeError, 'not support "get_eventloop_weight" on the remote interpreter'
  end
end
