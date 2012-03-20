#
#               multi-tk.rb - supports multi Tk interpreters
#                       by Hidetoshi NAGAI <nagai@ai.kyutech.ac.jp>

require 'tcltklib'
require 'tkutil'
require 'thread'

if defined? Tk
  fail RuntimeError,"'multi-tk' library must be required before requiring 'tk'"
end

################################################
# ignore exception on the mainloop?

TclTkLib.mainloop_abort_on_exception = true
# TclTkLib.mainloop_abort_on_exception = false
# TclTkLib.mainloop_abort_on_exception = nil


################################################
# add ThreadGroup check to TclTkIp.new
class << TclTkIp
  alias __new__ new
  private :__new__

  def new(*args)
    if Thread.current.group != ThreadGroup::Default
      raise SecurityError, 'only ThreadGroup::Default can call TclTkIp.new'
    end
    obj = __new__(*args)
    obj.instance_eval{
      @force_default_encoding ||= TkUtil.untrust([false])
      @encoding ||= TkUtil.untrust([nil])
      def @encoding.to_s; self.join(nil); end
    }
    obj
  end
end


################################################
# exceptiopn to treat the return value from IP
class MultiTkIp_OK < Exception
  def self.send(thread, ret=nil)
    thread.raise self.new(ret)
  end

  def initialize(ret=nil)
    super('succeed')
    @return_value = ret
  end

  attr_reader :return_value
  alias value return_value
end
MultiTkIp_OK.freeze


################################################
# methods for construction
class MultiTkIp
  class Command_Queue < Queue
    def initialize(interp)
      @interp = interp
      super()
    end

    def push(value)
      if !@interp || @interp.deleted?
        fail RuntimeError, "Tk interpreter is already deleted"
      end
      super(value)
    end
    alias << push
    alias enq push

    def close
      @interp = nil
    end
  end
  Command_Queue.freeze

  BASE_DIR = File.dirname(__FILE__)

  WITH_RUBY_VM  = Object.const_defined?(:RubyVM) && ::RubyVM.class == Class
  WITH_ENCODING = defined?(::Encoding.default_external)
  #WITH_ENCODING = Object.const_defined?(:Encoding) && ::Encoding.class == Class

  (@@SLAVE_IP_ID = ['slave'.freeze, TkUtil.untrust('0')]).instance_eval{
    @mutex = Mutex.new
    def mutex; @mutex; end
    freeze
  }

  @@IP_TABLE = TkUtil.untrust({}) unless defined?(@@IP_TABLE)

  @@INIT_IP_ENV  = TkUtil.untrust([]) unless defined?(@@INIT_IP_ENV)  # table of Procs
  @@ADD_TK_PROCS = TkUtil.untrust([]) unless defined?(@@ADD_TK_PROCS) # table of [name, args, body]

  @@TK_TABLE_LIST = TkUtil.untrust([]) unless defined?(@@TK_TABLE_LIST)

  unless defined?(@@TK_CMD_TBL)
    @@TK_CMD_TBL = TkUtil.untrust(Object.new)

    # @@TK_CMD_TBL.instance_variable_set('@tbl', {}.taint)
    tbl_obj = TkUtil.untrust(Hash.new{|hash,key|
                               fail IndexError, "unknown command ID '#{key}'"
                             })
    @@TK_CMD_TBL.instance_variable_set('@tbl', tbl_obj)

    class << @@TK_CMD_TBL
      allow = [
        '__send__', '__id__', 'freeze', 'inspect', 'kind_of?', 'object_id',
        '[]', '[]=', 'delete', 'each', 'has_key?'
      ]
      instance_methods.each{|m| undef_method(m) unless allow.index(m.to_s)}

      def kind_of?(klass)
        @tbl.kind_of?(klass)
      end

      def inspect
        if Thread.current.group == ThreadGroup::Default
          @tbl.inspect
        else
          ip = MultiTkIp.__getip
          @tbl.reject{|idx, ent| ent.respond_to?(:ip) && ent.ip != ip}.inspect
        end
      end

      def [](idx)
        return unless (ent = @tbl[idx])
        if Thread.current.group == ThreadGroup::Default
          ent
        elsif ent.respond_to?(:ip)
          (ent.ip == MultiTkIp.__getip)? ent: nil
        else
          ent
        end
      end

      def []=(idx,val)
        if self.has_key?(idx) && Thread.current.group != ThreadGroup::Default
          fail SecurityError,"cannot change the entried command"
        end
        @tbl[idx] = val
      end

      def delete(idx, &blk)
        # if gets an entry, is permited to delete
        if self[idx]
          @tbl.delete(idx)
        elsif blk
          blk.call(idx)
        else
          nil
        end
      end

      def each(&blk)
        if Thread.current.group == ThreadGroup::Default
          @tbl.each(&blk)
        else
          ip = MultiTkIp.__getip
          @tbl.each{|idx, ent|
            blk.call(idx, ent) unless ent.respond_to?(:ip) && ent.ip != ip
          }
        end
        self
      end

      def has_key?(k)
        @tbl.has_key?(k)
      end
      alias include? has_key?
      alias key? has_key?
      alias member? has_key?
    end

    @@TK_CMD_TBL.freeze
  end

  ######################################

  @@CB_ENTRY_CLASS = Class.new(TkCallbackEntry){
    def initialize(ip, cmd)
      @ip = ip
      @safe = safe = $SAFE
      # @cmd = cmd
      cmd = MultiTkIp._proc_on_safelevel(&cmd)
      @cmd = proc{|*args| cmd.call(safe, *args)}
      self.freeze
    end
    attr_reader :ip, :cmd
    def inspect
      cmd.inspect
    end
    def call(*args)
      unless @ip.deleted?
        current = Thread.current
        backup_ip = current[:callback_ip]
        current[:callback_ip] = @ip
        begin
          ret = @ip.cb_eval(@cmd, *args)
          fail ret if ret.kind_of?(Exception)
          ret
        rescue TkCallbackBreak, TkCallbackContinue => e
          fail e
	rescue SecurityError => e
          # in 'exit', 'exit!', and 'abort' : security error --> delete IP
          if e.backtrace[0] =~ /^(.+?):(\d+):in `(exit|exit!|abort)'/
	    @ip.delete
          elsif @ip.safe?
	    if @ip.respond_to?(:cb_error)
              @ip.cb_error(e)
            else
              nil # ignore
            end
          else
            fail e
          end
        rescue Exception => e
          fail e if e.message =~ /^TkCallback/

          if @ip.safe?
	    if @ip.respond_to?(:cb_error)
              @ip.cb_error(e)
            else
              nil # ignore
            end
          else
            fail e
          end
        ensure
          current[:callback_ip] = backup_ip
        end
      end
    end
  }.freeze

  ######################################

  def _keys2opts(src_keys)
    return nil if src_keys == nil
    keys = {}; src_keys.each{|k, v| keys[k.to_s] = v}
    #keys.collect{|k,v| "-#{k} #{v}"}.join(' ')
    keys.collect{|k,v| "-#{k} #{TclTkLib._conv_listelement(TkComm::_get_eval_string(v))}"}.join(' ')
  end
  private :_keys2opts

  def _check_and_return(thread, exception, wait=0)
    unless thread
      unless exception.kind_of?(MultiTkIp_OK)
        msg = "#{exception.class}: #{exception.message}"

        if @interp.deleted?
          warn("Warning (#{self}): " + msg)
          return nil
        end

        if safe?
          warn("Warning (#{self}): " + msg) if $DEBUG
          return nil
        end

        begin
          @interp._eval_without_enc(@interp._merge_tklist('bgerror', msg))
        rescue Exception => e
          warn("Warning (#{self}): " + msg)
        end
      end
      return nil
    end

    if wait == 0
      # no wait
      Thread.pass
      if thread.stop?
        thread.raise exception
      end
      return thread
    end

    # wait to stop the caller thread
    wait.times{
      if thread.stop?
        # ready to send exception
        thread.raise exception
        return thread
      end

      # wait
      Thread.pass
    }

    # unexpected error
    thread.raise RuntimeError, "the thread may not wait for the return value"
    return thread
  end

  ######################################

  def set_cb_error(cmd = Proc.new)
    @cb_error_proc[0] = cmd
  end

  def cb_error(e)
    if @cb_error_proc[0].respond_to?(:call)
      @cb_error_proc[0].call(e)
    end
  end

  ######################################

  def set_safe_level(safe)
    if safe > @safe_level[0]
      @safe_level[0] = safe
      @cmd_queue.enq([@system, 'set_safe_level', safe])
    end
    @safe_level[0]
  end
  def safe_level=(safe)
    set_safe_level(safe)
  end
  def self.set_safe_level(safe)
    __getip.set_safe_level(safe)
  end
  def self.safe_level=(safe)
    self.set_safe_level(safe)
  end
  def safe_level
    @safe_level[0]
  end
  def self.safe_level
    __getip.safe_level
  end

  def wait_on_mainloop?
    @wait_on_mainloop[0]
  end
  def wait_on_mainloop=(bool)
    @wait_on_mainloop[0] = bool
  end

  def running_mainloop?
    @wait_on_mainloop[1] > 0
  end

  def _destroy_slaves_of_slaveIP(ip)
    unless ip.deleted?
      # ip._split_tklist(ip._invoke('interp', 'slaves')).each{|name|
      ip._split_tklist(ip._invoke_without_enc('interp', 'slaves')).each{|name|
        name = _fromUTF8(name)
        begin
          # ip._eval_without_enc("#{name} eval {foreach i [after info] {after cancel $i}}")
          after_ids = ip._eval_without_enc("#{name} eval {after info}")
          ip._eval_without_enc("#{name} eval {foreach i {#{after_ids}} {after cancel $i}}")
        rescue Exception
        end
        begin
          # ip._invoke('interp', 'eval', name, 'destroy', '.')
          ip._invoke(name, 'eval', 'destroy', '.')
        rescue Exception
        end

        # safe_base?
        if ip._eval_without_enc("catch {::safe::interpConfigure #{name}}") == '0'
          begin
            ip._eval_without_enc("::safe::interpDelete #{name}")
          rescue Exception
          end
        end
=begin
        if ip._invoke('interp', 'exists', name) == '1'
          begin
            ip._invoke(name, 'eval', 'exit')
          rescue Exception
          end
        end
=end
        unless ip.deleted?
          if ip._invoke('interp', 'exists', name) == '1'
            begin
              ip._invoke('interp', 'delete', name)
            rescue Exception
            end
          end
        end
      }
    end
  end

  def _receiver_eval_proc_core(safe_level, thread, cmd, *args)
    begin
      #ret = proc{$SAFE = safe_level; cmd.call(*args)}.call
      #ret = cmd.call(safe_level, *args)
      normal_ret = false
      ret = catch(:IRB_EXIT) do  # IRB hack
        retval = cmd.call(safe_level, *args)
        normal_ret = true
        retval
      end
      unless normal_ret
        # catch IRB_EXIT
        exit(ret)
      end
      ret
    rescue SystemExit => e
      # delete IP
      unless @interp.deleted?
        @slave_ip_tbl.each{|name, subip|
          _destroy_slaves_of_slaveIP(subip)
          begin
            # subip._eval_without_enc("foreach i [after info] {after cancel $i}")
            after_ids = subip._eval_without_enc("after info")
            subip._eval_without_enc("foreach i {#{after_ids}} {after cancel $i}")
          rescue Exception
          end
=begin
          begin
            subip._invoke('destroy', '.') unless subip.deleted?
          rescue Exception
          end
=end
          # safe_base?
          if @interp._eval_without_enc("catch {::safe::interpConfigure #{name}}") == '0'
            begin
              @interp._eval_without_enc("::safe::interpDelete #{name}")
            rescue Exception
            else
              next if subip.deleted?
            end
          end
          if subip.respond_to?(:safe_base?) && subip.safe_base? &&
              !subip.deleted?
            # do 'exit' to call the delete_hook procedure
            begin
              subip._eval_without_enc('exit')
            rescue Exception
            end
          else
            begin
              subip.delete unless subip.deleted?
            rescue Exception
            end
          end
        }

        begin
          # @interp._eval_without_enc("foreach i [after info] {after cancel $i}")
          after_ids = @interp._eval_without_enc("after info")
          @interp._eval_without_enc("foreach i {#{after_ids}} {after cancel $i}")
        rescue Exception
        end
        begin
          @interp._invoke('destroy', '.') unless @interp.deleted?
        rescue Exception
        end
        if @safe_base && !@interp.deleted?
          # do 'exit' to call the delete_hook procedure
          @interp._eval_without_enc('exit')
        else
          @interp.delete unless @interp.deleted?
        end
      end

      if e.backtrace[0] =~ /^(.+?):(\d+):in `(exit|exit!|abort)'/
        _check_and_return(thread, MultiTkIp_OK.new($3 == 'exit'))
      else
        _check_and_return(thread, MultiTkIp_OK.new(nil))
      end

      # if master? && !safe? && allow_ruby_exit?
      if !@interp.deleted? && master? && !safe? && allow_ruby_exit?
=begin
        ObjectSpace.each_object(TclTkIp){|obj|
          obj.delete unless obj.deleted?
        }
=end
        #exit(e.status)
        fail e
      end
      # break

    rescue SecurityError => e
      # in 'exit', 'exit!', and 'abort' : security error --> delete IP
      if e.backtrace[0] =~ /^(.+?):(\d+):in `(exit|exit!|abort)'/
        ret = ($3 == 'exit')
        unless @interp.deleted?
          @slave_ip_tbl.each{|name, subip|
            _destroy_slaves_of_slaveIP(subip)
            begin
              # subip._eval_without_enc("foreach i [after info] {after cancel $i}")
              after_ids = subip._eval_without_enc("after info")
              subip._eval_without_enc("foreach i {#{after_ids}} {after cancel $i}")
            rescue Exception
            end
=begin
            begin
              subip._invoke('destroy', '.') unless subip.deleted?
            rescue Exception
            end
=end
            # safe_base?
            if @interp._eval_without_enc("catch {::safe::interpConfigure #{name}}") == '0'
              begin
                @interp._eval_without_enc("::safe::interpDelete #{name}")
              rescue Exception
              else
                next if subip.deleted?
              end
            end
            if subip.respond_to?(:safe_base?) && subip.safe_base? &&
                !subip.deleted?
              # do 'exit' to call the delete_hook procedure
              begin
                subip._eval_without_enc('exit')
              rescue Exception
              end
            else
              begin
                subip.delete unless subip.deleted?
              rescue Exception
              end
            end
          }

          begin
            # @interp._eval_without_enc("foreach i [after info] {after cancel $i}")
            after_ids = @interp._eval_without_enc("after info")
            @interp._eval_without_enc("foreach i {#{after_ids}} {after cancel $i}")
          rescue Exception
          end
=begin
          begin
            @interp._invoke('destroy', '.') unless @interp.deleted?
          rescue Exception
          end
=end
          if @safe_base && !@interp.deleted?
            # do 'exit' to call the delete_hook procedure
            @interp._eval_without_enc('exit')
          else
            @interp.delete unless @interp.deleted?
          end
        end
        _check_and_return(thread, MultiTkIp_OK.new(ret))
        # break

      else
        # raise security error
        _check_and_return(thread, e)
      end

    rescue Exception => e
      # raise exception
      begin
        bt = _toUTF8(e.backtrace.join("\n"))
        if MultiTkIp::WITH_ENCODING
          bt.force_encoding('utf-8')
        else
          bt.instance_variable_set(:@encoding, 'utf-8')
        end
      rescue Exception
        bt = e.backtrace.join("\n")
      end
      begin
        @interp._set_global_var('errorInfo', bt)
      rescue Exception
      end
      _check_and_return(thread, e)

    else
      # no exception
      _check_and_return(thread, MultiTkIp_OK.new(ret))
    end
  end

  def _receiver_eval_proc(last_thread, safe_level, thread, cmd, *args)
    if thread
      Thread.new{
        last_thread.join if last_thread
        unless @interp.deleted?
          _receiver_eval_proc_core(safe_level, thread, cmd, *args)
        end
      }
    else
      Thread.new{
        unless  @interp.deleted?
          _receiver_eval_proc_core(safe_level, thread, cmd, *args)
        end
      }
      last_thread
    end
  end

  private :_receiver_eval_proc, :_receiver_eval_proc_core

  def _receiver_mainloop(check_root)
    if @evloop_thread[0] && @evloop_thread[0].alive?
      @evloop_thread[0]
    else
      @evloop_thread[0] = Thread.new{
	while !@interp.deleted?
	  #if check_root
	  #  inf = @interp._invoke_without_enc('info', 'command', '.')
	  #  break if !inf.kind_of?(String) || inf != '.'
	  #end
          break if check_root && !@interp.has_mainwindow?
	  sleep 0.5
	end
      }
      @evloop_thread[0]
    end
  end

  def _create_receiver_and_watchdog(lvl = $SAFE)
    lvl = $SAFE if lvl < $SAFE

    # command-procedures receiver
    receiver = Thread.new(lvl){|safe_level|
      last_thread = {}

      loop do
        break if @interp.deleted?
        thread, cmd, *args = @cmd_queue.deq
        if thread == @system
          # control command
          case cmd
          when 'set_safe_level'
            begin
              safe_level = args[0] if safe_level < args[0]
            rescue Exception
            end
          when 'call_mainloop'
            thread = args.shift
            _check_and_return(thread,
                              MultiTkIp_OK.new(_receiver_mainloop(*args)))
          else
            # ignore
          end

        else
          # procedure
          last_thread[thread] = _receiver_eval_proc(last_thread[thread],
						    safe_level, thread,
						    cmd, *args)
        end
      end
    }

    # watchdog of receiver
    watchdog = Thread.new{
      begin
        loop do
          sleep 1
          if @interp.deleted?
            receiver.kill
            @cmd_queue.close
          end
          break unless receiver.alive?
        end
      rescue Exception
        # ignore all kind of Exception
      end

      # receiver is dead
      retry_count = 3
      loop do
        Thread.pass
        begin
          thread, cmd, *args = @cmd_queue.deq(true) # non-block
        rescue ThreadError
          # queue is empty
          retry_count -= 1
          break if retry_count <= 0
          sleep 0.5
          retry
        end
        next unless thread
        if thread.alive?
          if @interp.deleted?
            thread.raise RuntimeError, 'the interpreter is already deleted'
          else
            thread.raise RuntimeError,
              'the interpreter no longer receives command procedures'
          end
        end
      end
    }

    # return threads
    [receiver, watchdog]
  end
  private :_check_and_return, :_create_receiver_and_watchdog

  ######################################

  unless self.const_defined? :RUN_EVENTLOOP_ON_MAIN_THREAD
    ### Ruby 1.9 !!!!!!!!!!!!!!!!!!!!!!!!!!
    RUN_EVENTLOOP_ON_MAIN_THREAD = false
  end

  if self.const_defined? :DEFAULT_MASTER_NAME
    name = DEFAULT_MASTER_NAME.to_s
  else
    name = nil
  end
  if self.const_defined?(:DEFAULT_MASTER_OPTS) &&
      DEFAULT_MASTER_OPTS.kind_of?(Hash)
    keys = DEFAULT_MASTER_OPTS
  else
    keys = {}
  end

  @@DEFAULT_MASTER = self.allocate
  @@DEFAULT_MASTER.instance_eval{
    @tk_windows = TkUtil.untrust({})

    @tk_table_list = TkUtil.untrust([])

    @slave_ip_tbl = TkUtil.untrust({})

    @slave_ip_top = TkUtil.untrust({})

    @evloop_thread = TkUtil.untrust([])

    unless keys.kind_of? Hash
      fail ArgumentError, "expecting a Hash object for the 2nd argument"
    end

    if !WITH_RUBY_VM || RUN_EVENTLOOP_ON_MAIN_THREAD ### check Ruby 1.9 !!!!!!!
      @interp = TclTkIp.new(name, _keys2opts(keys))
    else ### Ruby 1.9 !!!!!!!!!!!
      @interp_thread = Thread.new{
        current = Thread.current
        begin
          current[:interp] = interp = TclTkIp.new(name, _keys2opts(keys))
        rescue e
          current[:interp] = e
          raise e
        end
        #sleep
        current[:mutex] = mutex = Mutex.new
        current[:root_check] = cond_var = ConditionVariable.new

        status = [nil]
        def status.value
          self[0]
        end
        def status.value=(val)
          self[0] = val
        end
        current[:status] = status

        begin
          begin
            #TclTkLib.mainloop_abort_on_exception = false
            #Thread.current[:status].value = TclTkLib.mainloop(true)
            interp.mainloop_abort_on_exception = true
            current[:status].value = interp.mainloop(true)
          rescue SystemExit=>e
            current[:status].value = e
          rescue Exception=>e
            current[:status].value = e
            retry if interp.has_mainwindow?
          ensure
            mutex.synchronize{ cond_var.broadcast }
          end

          #Thread.current[:status].value = TclTkLib.mainloop(false)
          current[:status].value = interp.mainloop(false)

        ensure
          # interp must be deleted before the thread for interp is dead.
          # If not, raise Tcl_Panic on Tcl_AsyncDelete because async handler
          # deleted by the wrong thread.
          interp.delete
        end
      }
      until @interp_thread[:interp]
        Thread.pass
      end
      # INTERP_THREAD.run
      raise @interp_thread[:interp] if @interp_thread[:interp].kind_of? Exception
      @interp = @interp_thread[:interp]

      # delete the interpreter and kill the eventloop thread at exit
      interp = @interp
      interp_thread = @interp_thread
      END{
        if interp_thread.alive?
          interp.delete
          interp_thread.kill
        end
      }

      def self.mainloop(check_root = true)
        begin
          TclTkLib.set_eventloop_window_mode(true)
          @interp_thread.value
        ensure
          TclTkLib.set_eventloop_window_mode(false)
        end
      end
    end

    @interp.instance_eval{
      @force_default_encoding ||= TkUtil.untrust([false])
      @encoding ||= TkUtil.untrust([nil])
      def @encoding.to_s; self.join(nil); end
    }

    @ip_name = nil

    @callback_status = TkUtil.untrust([])

    @system = Object.new

    @wait_on_mainloop = TkUtil.untrust([true, 0])

    @threadgroup  = Thread.current.group

    @safe_base = false

    @safe_level = [$SAFE]

    @cmd_queue = MultiTkIp::Command_Queue.new(@interp)

    @cmd_receiver, @receiver_watchdog = _create_receiver_and_watchdog(@safe_level[0])

    @threadgroup.add @cmd_receiver
    @threadgroup.add @receiver_watchdog

    # NOT enclose @threadgroup for @@DEFAULT_MASTER

    @@IP_TABLE[ThreadGroup::Default] = self
    @@IP_TABLE[@threadgroup] = self

    #################################

    @pseudo_toplevel = [false, nil]

    def self.__pseudo_toplevel
      Thread.current.group == ThreadGroup::Default &&
        MultiTkIp.__getip == @@DEFAULT_MASTER &&
        self.__pseudo_toplevel_evaluable? && @pseudo_toplevel[1]
    end

    def self.__pseudo_toplevel=(m)
      unless (Thread.current.group == ThreadGroup::Default &&
                MultiTkIp.__getip == @@DEFAULT_MASTER)
        fail SecurityError, "no permission to manipulate"
      end

      # if m.kind_of?(Module) && m.respond_to?(:pseudo_toplevel_evaluable?)
      if m.respond_to?(:pseudo_toplevel_evaluable?)
        @pseudo_toplevel[0] = true
        @pseudo_toplevel[1] = m
      else
        fail ArgumentError, 'fail to set pseudo-toplevel'
      end
      self
    end

    def self.__pseudo_toplevel_evaluable?
      begin
        @pseudo_toplevel[0] && @pseudo_toplevel[1].pseudo_toplevel_evaluable?
      rescue Exception
        false
      end
    end

    def self.__pseudo_toplevel_evaluable=(mode)
      unless (Thread.current.group == ThreadGroup::Default &&
                MultiTkIp.__getip == @@DEFAULT_MASTER)
        fail SecurityError, "no permission to manipulate"
      end

      @pseudo_toplevel[0] = (mode)? true: false
    end

    #################################

    @assign_request = Class.new(Exception){
      def self.new(target, ret)
        obj = super()
        obj.target = target
        obj.ret = ret
        obj
      end
      attr_accessor :target, :ret
    }

    @assign_thread = Thread.new{
      loop do
        begin
          Thread.stop
        rescue @assign_request=>req
          begin
            req.ret[0] = req.target.instance_eval{
              @cmd_receiver, @receiver_watchdog =
                _create_receiver_and_watchdog(@safe_level[0])
              @threadgroup.add @cmd_receiver
              @threadgroup.add @receiver_watchdog
              @threadgroup.enclose
              true
            }
          rescue Exception=>e
            begin
              req.ret[0] = e
            rescue Exception
              # ignore
            end
          end
        rescue Exception
          # ignore
        end
      end
    }

    def self.assign_receiver_and_watchdog(target)
      ret = [nil]
      @assign_thread.raise(@assign_request.new(target, ret))
      while ret[0] == nil
        unless @assign_thread.alive?
          raise RuntimeError, 'lost the thread to assign a receiver and a watchdog thread'
        end
      end
      if ret[0].kind_of?(Exception)
        raise ret[0]
      else
        ret[0]
      end
    end

    #################################

    @init_ip_env_queue = Queue.new
    Thread.new{
      current = Thread.current
      loop {
        mtx, cond, ret, table, script = @init_ip_env_queue.deq
        begin
          ret[0] = table.each{|tg, ip| ip._init_ip_env(script) }
        rescue Exception => e
          ret[0] = e
        ensure
          mtx.synchronize{ cond.signal }
        end
        mtx = cond = ret = table = script = nil  # clear variables for GC
      }
    }

    def self.__init_ip_env__(table, script)
      ret = []
      mtx  = (Thread.current[:MultiTk_ip_Mutex] ||= Mutex.new)
      cond = (Thread.current[:MultiTk_ip_CondVar] ||= ConditionVariable.new)
      mtx.synchronize{
        @init_ip_env_queue.enq([mtx, cond, ret, table, script])
        cond.wait(mtx)
      }
      if ret[0].kind_of?(Exception)
        raise ret[0]
      else
        ret[0]
      end
    end

    #################################

    class << self
      undef :instance_eval
    end
  }

  @@DEFAULT_MASTER.freeze # defend against modification

  ######################################

  def self.inherited(subclass)
    # trust if on ThreadGroup::Default or @@DEFAULT_MASTER's ThreadGroup
    if @@IP_TABLE[Thread.current.group] == @@DEFAULT_MASTER
      begin
        class << subclass
          self.methods.each{|m|
            name = m.to_s
            begin
              unless name == '__id__' || name == '__send__' || name == 'freeze'
                undef_method(m)
              end
            rescue Exception
              # ignore all exceptions
            end
          }
        end
      ensure
        subclass.freeze
        fail SecurityError,
          "cannot create subclass of MultiTkIp on a untrusted ThreadGroup"
      end
    end
  end

  ######################################

  @@SAFE_OPT_LIST = [
    'accessPath'.freeze,
    'statics'.freeze,
    'nested'.freeze,
    'deleteHook'.freeze
  ].freeze

  def _parse_slaveopts(keys)
    name = nil
    safe = false
    safe_opts = {}
    tk_opts   = {}

    keys.each{|k,v|
      k_str = k.to_s
      if k_str == 'name'
        name = v
      elsif k_str == 'safe'
        safe = v
      elsif @@SAFE_OPT_LIST.member?(k_str)
        safe_opts[k_str] = v
      else
        tk_opts[k_str] = v
      end
    }

    if keys['without_tk'] || keys[:without_tk]
      [name, safe, safe_opts, nil]
    else
      [name, safe, safe_opts, tk_opts]
    end
  end
  private :_parse_slaveopts

  def _create_slave_ip_name
    @@SLAVE_IP_ID.mutex.synchronize{
      name = @@SLAVE_IP_ID.join('')
      @@SLAVE_IP_ID[1].succ!
      name.freeze
    }
  end
  private :_create_slave_ip_name

  ######################################

  def __check_safetk_optkeys(optkeys)
    # based on 'safetk.tcl'
    new_keys = {}
    optkeys.each{|k,v| new_keys[k.to_s] = v}

    # check 'display'
    if !new_keys.key?('display')
      begin
        #new_keys['display'] = @interp._invoke('winfo screen .')
        new_keys['display'] = @interp._invoke('winfo', 'screen', '.')
      rescue
        if ENV[DISPLAY]
          new_keys['display'] = ENV[DISPLAY]
        elsif !new_keys.key?('use')
          warn "Warning: no screen info or ENV[DISPLAY], so use ':0.0'"
          new_keys['display'] = ':0.0'
        end
      end
    end

    # check 'use'
    if new_keys.key?('use')
      # given 'use'
      case new_keys['use']
      when TkWindow
        new_keys['use'] = TkWinfo.id(new_keys['use'])
        #assoc_display = @interp._eval('winfo screen .')
        assoc_display = @interp._invoke('winfo', 'screen', '.')
      when /^\..*/
        new_keys['use'] = @interp._invoke('winfo', 'id', new_keys['use'])
        assoc_display = @interp._invoke('winfo', 'screen', new_keys['use'])
      else
        begin
          pathname = @interp._invoke('winfo', 'pathname', new_keys['use'])
          assoc_display = @interp._invoke('winfo', 'screen', pathname)
        rescue
          assoc_display = new_keys['display']
        end
      end

      # match display?
      if assoc_display != new_keys['display']
        if optkeys.key?(:display) || optkeys.key?('display')
          fail RuntimeError,
            "conflicting 'display'=>#{new_keys['display']} " +
            "and display '#{assoc_display}' on 'use'=>#{new_keys['use']}"
        else
          new_keys['display'] = assoc_display
        end
      end
    end

    # return
    new_keys
  end
  private :__check_safetk_optkeys

  def __create_safetk_frame(slave_ip, slave_name, app_name, keys)
    # display option is used by ::safe::loadTk
    loadTk_keys = {}
    loadTk_keys['display'] = keys['display']
    dup_keys = keys.dup

    # keys for toplevel : allow followings
    toplevel_keys = {}
    ['height', 'width', 'background', 'menu'].each{|k|
      toplevel_keys[k] = dup_keys.delete(k) if dup_keys.key?(k)
    }
    toplevel_keys['classname'] = 'SafeTk'
    toplevel_keys['screen'] = dup_keys.delete('display')

    # other keys used by pack option of container frame

    # create toplevel widget
    begin
      top = TkToplevel.new(toplevel_keys)
    rescue NameError => e
      fail e unless @interp.safe?
      fail SecurityError, "unable create toplevel on the safe interpreter"
    end
    msg = "Untrusted Ruby/Tk applet (#{slave_name})"
    if app_name.kind_of?(String)
      top.title "#{app_name} (#{slave_name})"
    else
      top.title msg
    end

    # procedure to delete slave interpreter
    slave_delete_proc = proc{
      unless slave_ip.deleted?
        #if slave_ip._invoke('info', 'command', '.') != ""
        #  slave_ip._invoke('destroy', '.')
        #end
        #slave_ip.delete
        slave_ip._eval_without_enc('exit')
      end
      begin
        top.destroy if top.winfo_exist?
      rescue
        # ignore
      end
    }
    tag = TkBindTag.new.bind('Destroy', slave_delete_proc)

    top.bindtags = top.bindtags.unshift(tag)

    # create control frame
    TkFrame.new(top, :bg=>'red', :borderwidth=>3, :relief=>'ridge') {|fc|
      fc.bindtags = fc.bindtags.unshift(tag)

      TkFrame.new(fc, :bd=>0){|f|
        TkButton.new(f,
                     :text=>'Delete', :bd=>1, :padx=>2, :pady=>0,
                     :highlightthickness=>0, :command=>slave_delete_proc
                     ).pack(:side=>:right, :fill=>:both)
        f.pack(:side=>:right, :fill=>:both, :expand=>true)
      }

      TkLabel.new(fc, :text=>msg, :padx=>2, :pady=>0,
                  :anchor=>:w).pack(:side=>:left, :fill=>:both, :expand=>true)

      fc.pack(:side=>:bottom, :fill=>:x)
    }

    # container frame for slave interpreter
    dup_keys['fill'] = :both  unless dup_keys.key?('fill')
    dup_keys['expand'] = true unless dup_keys.key?('expand')
    c = TkFrame.new(top, :container=>true).pack(dup_keys)
    c.bind('Destroy', proc{top.destroy})

    # return keys
    loadTk_keys['use'] = TkWinfo.id(c)
    [loadTk_keys, top.path]
  end
  private :__create_safetk_frame

  def __create_safe_slave_obj(safe_opts, app_name, tk_opts)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?

    # safe interpreter
    ip_name = _create_slave_ip_name
    slave_ip = @interp.create_slave(ip_name, true)
    slave_ip.instance_eval{
      @force_default_encoding ||= TkUtil.untrust([false])
      @encoding ||= TkUtil.untrust([nil])
      def @encoding.to_s; self.join(nil); end
    }
    @slave_ip_tbl[ip_name] = slave_ip
    def slave_ip.safe_base?
      true
    end

    @interp._eval("::safe::interpInit #{ip_name}")

    slave_ip._invoke('set', 'argv0', app_name) if app_name.kind_of?(String)

    if tk_opts
      tk_opts = __check_safetk_optkeys(tk_opts)
      if tk_opts.key?('use')
        @slave_ip_top[ip_name] = ''
      else
        tk_opts, top_path = __create_safetk_frame(slave_ip, ip_name, app_name,
                                                  tk_opts)
        @slave_ip_top[ip_name] = top_path
      end
      @interp._eval("::safe::loadTk #{ip_name} #{_keys2opts(tk_opts)}")
      @interp._invoke('__replace_slave_tk_commands__', ip_name)
    else
      @slave_ip_top[ip_name] = nil
    end

    if safe_opts.key?('deleteHook') || safe_opts.key?(:deleteHook)
      @interp._eval("::safe::interpConfigure #{ip_name} " +
                    _keys2opts(safe_opts))
    else
      @interp._eval("::safe::interpConfigure #{ip_name} " +
                    _keys2opts(safe_opts) + '-deleteHook {' +
                    TkComm._get_eval_string(proc{|slave|
                                              self._default_delete_hook(slave)
                                            }) + '}')
    end

    [slave_ip, ip_name]
  end

  def __create_trusted_slave_obj(name, keys)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?

    ip_name = _create_slave_ip_name
    slave_ip = @interp.create_slave(ip_name, false)
    slave_ip.instance_eval{
      @force_default_encoding ||= TkUtil.untrust([false])
      @encoding ||= TkUtil.untrust([nil])
      def @encoding.to_s; self.join(nil); end
    }
    slave_ip._invoke('set', 'argv0', name) if name.kind_of?(String)
    slave_ip._invoke('set', 'argv', _keys2opts(keys))
    @interp._invoke('load', '', 'Tk', ip_name)
    @interp._invoke('__replace_slave_tk_commands__', ip_name)
    @slave_ip_tbl[ip_name] = slave_ip
    [slave_ip, ip_name]
  end

  ######################################

  def _create_slave_object(keys={})
    raise SecurityError, "no permission to manipulate" unless self.manipulable?

    ip = MultiTkIp.new_slave(self, keys={})
    @slave_ip_tbl[ip.name] = ip
  end

  ######################################

  def initialize(master, safeip=true, keys={})
    if $SAFE >= 4
      fail SecurityError, "cannot create a new interpreter at level #{$SAFE}"
    end

    if safeip == nil && $SAFE >= 2
      fail SecurityError, "cannot create a master-ip at level #{$SAFE}"
    end

    if master.deleted? && safeip == nil
      fail RuntimeError, "cannot create a slave of a deleted interpreter"
    end

    if !master.deleted? && !master.master? && master.safe?
      fail SecurityError, "safe-slave-ip cannot create a new interpreter"
    end

    if safeip == nil && !master.master?
      fail SecurityError, "slave-ip cannot create a master-ip"
    end

    unless keys.kind_of? Hash
      fail ArgumentError, "expecting a Hash object for the 2nd argument"
    end

    @tk_windows = {}
    @tk_table_list = []
    @slave_ip_tbl = {}
    @slave_ip_top = {}
    @cb_error_proc = []
    @evloop_thread = []

    TkUtil.untrust(@tk_windows)    unless @tk_windows.tainted?
    TkUtil.untrust(@tk_table_list) unless @tk_table_list.tainted?
    TkUtil.untrust(@slave_ip_tbl)  unless @slave_ip_tbl.tainted?
    TkUtil.untrust(@slave_ip_top)  unless @slave_ip_top.tainted?
    TkUtil.untrust(@cb_error_proc) unless @cb_error_proc.tainted?
    TkUtil.untrust(@evloop_thread) unless @evloop_thread.tainted?

    @callback_status = []

    name, safe, safe_opts, tk_opts = _parse_slaveopts(keys)

    safe = 4 if safe && !safe.kind_of?(Fixnum)

    @safe_base = false

    if safeip == nil
      # create master-ip
      unless WITH_RUBY_VM
        @interp = TclTkIp.new(name, _keys2opts(tk_opts))
        @interp.instance_eval{
          @force_default_encoding ||= TkUtil.untrust([false])
          @encoding ||= TkUtil.untrust([nil])
          def @encoding.to_s; self.join(nil); end
        }

      else ### Ruby 1.9 !!!!!!!!!!!
=begin
        @interp_thread = Thread.new{
          Thread.current[:interp] = interp = TclTkIp.new(name, _keys2opts(tk_opts))
          interp.instance_eval{
            @force_default_encoding ||= TkUtil.untrust([false])
            @encoding ||= TkUtil.untrust([nil])
            def @encoding.to_s; self.join(nil); end
          }

          #sleep
          TclTkLib.mainloop(true)
        }
        until @interp_thread[:interp]
          Thread.pass
        end
        # INTERP_THREAD.run
        @interp = @interp_thread[:interp]
=end
        @interp_thread = Thread.new{
          current = Thread.current
          begin
            current[:interp] = interp = TclTkIp.new(name, _keys2opts(tk_opts))
          rescue e
            current[:interp] = e
            raise e
          end
          #sleep
          #TclTkLib.mainloop(true)
          current[:mutex] = mutex = Mutex.new
          current[:root_check] = cond_ver = ConditionVariable.new

          status = [nil]
          def status.value
            self[0]
          end
          def status.value=(val)
            self[0] = val
          end
          current[:status] = status

          begin
            begin
              current[:status].value = interp.mainloop(true)
            rescue SystemExit=>e
              current[:status].value = e
            rescue Exception=>e
              current[:status].value = e
              retry if interp.has_mainwindow?
            ensure
              mutex.synchronize{ cond_var.broadcast }
            end
            current[:status].value = interp.mainloop(false)
          ensure
            interp.delete
          end
        }
        until @interp_thread[:interp]
          Thread.pass
        end
        # INTERP_THREAD.run
        @interp = @interp_thread[:interp]

        @evloop_thread[0] = @interp_thread

        def self.mainloop(check_root = true)
          begin
            TclTkLib.set_eventloop_window_mode(true)
            @interp_thread.value
          ensure
            TclTkLib.set_eventloop_window_mode(false)
          end
        end
      end

      @interp.instance_eval{
        @force_default_encoding ||= TkUtil.untrust([false])
        @encoding ||= TkUtil.untrust([nil])
        def @encoding.to_s; self.join(nil); end
      }

      @ip_name = nil

      if safe
        safe = $SAFE if safe < $SAFE
        @safe_level = [safe]
      else
        @safe_level = [$SAFE]
      end

    else
      # create slave-ip
      if safeip || master.safe?
        @safe_base = true
        @interp, @ip_name = master.__create_safe_slave_obj(safe_opts,
                                                           name, tk_opts)
        # @interp_thread = nil if RUBY_VERSION < '1.9.0' ### !!!!!!!!!!!
        @interp_thread = nil unless WITH_RUBY_VM  ### Ruby 1.9 !!!!!!!!!!!
        if safe
          safe = master.safe_level if safe < master.safe_level
          @safe_level = [safe]
        else
          @safe_level = [4]
        end
      else
        @interp, @ip_name = master.__create_trusted_slave_obj(name, tk_opts)
        # @interp_thread = nil if RUBY_VERSION < '1.9.0' ### !!!!!!!!!!!
        @interp_thread = nil unless WITH_RUBY_VM  ### Ruby 1.9 !!!!!!!!!!!
        if safe
          safe = master.safe_level if safe < master.safe_level
          @safe_level = [safe]
        else
          @safe_level = [master.safe_level]
        end
      end
      @set_alias_proc = proc{|name|
        master._invoke('interp', 'alias', @ip_name, name, '', name)
      }.freeze
    end

    @system = Object.new

    @wait_on_mainloop = TkUtil.untrust([true, 0])
    # @wait_on_mainloop = TkUtil.untrust([false, 0])

    @threadgroup  = ThreadGroup.new

    @pseudo_toplevel = [false, nil]

    @cmd_queue = MultiTkIp::Command_Queue.new(@interp)

=begin
    @cmd_receiver, @receiver_watchdog = _create_receiver_and_watchdog(@safe_level[0])

    @threadgroup.add @cmd_receiver
    @threadgroup.add @receiver_watchdog

    @threadgroup.enclose
=end
    @@DEFAULT_MASTER.assign_receiver_and_watchdog(self)

    @@IP_TABLE[@threadgroup] = self
    @@TK_TABLE_LIST.size.times{
      @tk_table_list << TkUtil.untrust({})
    }
    _init_ip_internal(@@INIT_IP_ENV, @@ADD_TK_PROCS)

    class << self
      undef :instance_eval
    end

    # dummy call for initialization
    self.eval_proc{ Tk.tk_call('set', 'tcl_patchLevel') }

    self.freeze  # defend against modification
  end

  ######################################

  def _default_delete_hook(slave)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @slave_ip_tbl.delete(slave)
    top = @slave_ip_top.delete(slave)
    if top.kind_of?(String)
      # call default hook of safetk.tcl (ignore exceptions)
      if top == ''
        begin
          @interp._eval("::safe::disallowTk #{slave}")
        rescue
          warn("Waring: fail to call '::safe::disallowTk'") if $DEBUG
        end
      else # toplevel path
        begin
          @interp._eval("::safe::tkDelete {} #{top} #{slave}")
        rescue
          warn("Waring: fail to call '::safe::tkDelete'") if $DEBUG
          begin
            @interp._eval("destroy #{top}")
          rescue
            warn("Waring: fail to destroy toplevel") if $DEBUG
          end
        end
      end
    end
  end

end


# get target IP
class MultiTkIp
  @@CALLBACK_SUBTHREAD = Class.new(Thread){
    def self.new(interp, &blk)
      super(interp){|ip| Thread.current[:callback_ip] = ip; blk.call}
    end

    @table = TkUtil.untrust(Hash.new{|h,k| h[k] = TkUtil.untrust([])})
    def self.table
      @table
    end
  }

  def self._ip_id_
    __getip._ip_id_
  end
  def _ip_id_
    # for RemoteTkIp
    ''
  end

  def self.__getip
    current = Thread.current
    if current.kind_of?(@@CALLBACK_SUBTHREAD)
      return current[:callback_ip]
    end
    if TclTkLib.mainloop_thread? != false && current[:callback_ip]
      return current[:callback_ip]
    end
    if current.group == ThreadGroup::Default
      @@DEFAULT_MASTER
    else
      ip = @@IP_TABLE[current.group]
      unless ip
        fail SecurityError,
          "cannot call Tk methods on #{Thread.current.inspect}"
      end
      ip
    end
  end
end


# aliases of constructor
class << MultiTkIp
  alias __new new
  private :__new

  def new_master(safe=nil, keys={}, &blk)
    if MultiTkIp::WITH_RUBY_VM
      #### TODO !!!!!!
      fail RuntimeError,
           'sorry, still not support multiple master-interpreters on RubyVM'
    end

    if safe.kind_of?(Hash)
      keys = safe
    elsif safe.kind_of?(Integer)
      raise ArgumentError, "unexpected argument(s)" unless keys.kind_of?(Hash)
      if !keys.key?(:safe) && !keys.key?('safe')
        keys[:safe] = safe
      end
    elsif safe == nil
      # do nothing
    else
      raise ArgumentError, "unexpected argument(s)"
    end

    ip = __new(__getip, nil, keys)
    #ip.eval_proc(proc{$SAFE=ip.safe_level; Proc.new}.call) if block_given?
    if block_given?
      #Thread.new{ip.eval_proc(proc{$SAFE=ip.safe_level; Proc.new}.call)}
      #Thread.new{ip.eval_proc(proc{$SAFE=ip.safe_level; yield}.call)}
      ip._proc_on_safelevel(&blk).call(ip.safe_level)
    end
    ip
  end

  alias new new_master

  def new_slave(safe=nil, keys={}, &blk)
    if safe.kind_of?(Hash)
      keys = safe
    elsif safe.kind_of?(Integer)
      raise ArgumentError, "unexpected argument(s)" unless keys.kind_of?(Hash)
      if !keys.key?(:safe) && !keys.key?('safe')
        keys[:safe] = safe
      end
    elsif safe == nil
      # do nothing
    else
      raise ArgumentError, "unexpected argument(s)"
    end

    ip = __new(__getip, false, keys)
    # ip.eval_proc(proc{$SAFE=ip.safe_level; Proc.new}.call) if block_given?
    if block_given?
      #Thread.new{ip.eval_proc(proc{$SAFE=ip.safe_level; Proc.new}.call)}
      #Thread.new{ip.eval_proc(proc{$SAFE=ip.safe_level; yield}.call)}
      ip._proc_on_safelevel(&blk).call(ip.safe_level)
    end
    ip
  end
  alias new_trusted_slave new_slave

  def new_safe_slave(safe=4, keys={}, &blk)
    if safe.kind_of?(Hash)
      keys = safe
    elsif safe.kind_of?(Integer)
      raise ArgumentError, "unexpected argument(s)" unless keys.kind_of?(Hash)
      if !keys.key?(:safe) && !keys.key?('safe')
        keys[:safe] = safe
      end
    else
      raise ArgumentError, "unexpected argument(s)"
    end

    ip = __new(__getip, true, keys)
    # ip.eval_proc(proc{$SAFE=ip.safe_level; Proc.new}.call) if block_given?
    if block_given?
      #Thread.new{ip.eval_proc(proc{$SAFE=ip.safe_level; Proc.new}.call)}
      #Thread.new{ip.eval_proc(proc{$SAFE=ip.safe_level; yield}.call)}
      ip._proc_on_safelevel(&blk).call(ip.safe_level)
    end
    ip
  end
  alias new_safeTk new_safe_slave
end


# get info
class MultiTkIp
  def inspect
    s = self.to_s.chop!
    if self.manipulable?
      if master?
        if @interp.deleted?
          s << ':deleted-master'
        else
          s << ':master'
        end
      else
        if @interp.deleted?
          s << ':deleted-slave'
        elsif @interp.safe?
          s << ':safe-slave'
        else
          s << ':trusted-slave'
        end
      end
    end
    s << '>'
  end

  def master?
    if @ip_name
      false
    else
      true
    end
  end
  def self.master?
    __getip.master?
  end

  def slave?
    not master?
  end
  def self.slave?
    not self.master?
  end

  def alive?
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    begin
      return false unless @cmd_receiver.alive?
      return false if @interp.deleted?
      return false if @interp._invoke('interp', 'exists', '') == '0'
    rescue Exception
      return false
    end
    true
  end
  def self.alive?
    __getip.alive?
  end

  def path
    @ip_name || ''
  end
  def self.path
    __getip.path
  end
  def ip_name
    @ip_name || ''
  end
  def self.ip_name
    __getip.ip_name
  end
  def to_eval
    @ip_name || ''
  end
  def self.to_eval
    __getip.to_eval
  end

  def slaves(all = false)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._invoke('interp','slaves').split.map!{|name|
      if @slave_ip_tbl.key?(name)
        @slave_ip_tbl[name]
      elsif all
        name
      else
        nil
      end
    }.compact!
  end
  def self.slaves(all = false)
    __getip.slaves(all)
  end

  def manipulable?
    return true if (Thread.current.group == ThreadGroup::Default)
    ip = MultiTkIp.__getip
    (ip == self) || ip._is_master_of?(@interp)
  end
  def self.manipulable?
    true
  end

  def _is_master_of?(tcltkip_obj)
    tcltkip_obj.slave_of?(@interp)
  end
  protected :_is_master_of?
end


# instance methods to treat tables
class MultiTkIp
  def _tk_cmd_tbl
    tbl = {}
    MultiTkIp.tk_cmd_tbl.each{|id, ent| tbl[id] = ent if ent.ip == self }
    tbl
  end

  def _tk_windows
    @tk_windows
  end

  def _tk_table_list
    @tk_table_list
  end

  def _add_new_tables
    (@@TK_TABLE_LIST.size - @tk_table_list.size).times{
      @tk_table_list << TkUtil.untrust({})
    }
  end

  def _init_ip_env(script)
    self.eval_proc{script.call(self)}
  end

  def _add_tk_procs(name, args, body)
    return if slave?
    @interp._invoke('proc', name, args, body) if args && body
    @interp._invoke('interp', 'slaves').split.each{|slave|
      @interp._invoke('interp', 'alias', slave, name, '', name)
    }
  end

  def _remove_tk_procs(*names)
    return if slave?
    names.each{|name|
      name = name.to_s

      return if @interp.deleted?
      @interp._invoke('rename', name, '')

      return if @interp.deleted?
      @interp._invoke('interp', 'slaves').split.each{|slave|
        return if @interp.deleted?
        @interp._invoke('interp', 'alias', slave, name, '') rescue nil
      }
    }
  end

  def _init_ip_internal(init_ip_env, add_tk_procs)
    #init_ip_env.each{|script| self.eval_proc{script.call(self)}}
    init_ip_env.each{|script| self._init_ip_env(script)}
    add_tk_procs.each{|name, args, body|
      if master?
        @interp._invoke('proc', name, args, body) if args && body
      else
        @set_alias_proc.call(name)
      end
    }
  end
end


# class methods to treat tables
class MultiTkIp
  def self.tk_cmd_tbl
    @@TK_CMD_TBL
  end
  def self.tk_windows
    __getip._tk_windows
  end
  def self.tk_object_table(id)
    __getip._tk_table_list[id]
  end
  def self.create_table
    if __getip.slave?
      begin
        raise SecurityError, "slave-IP has no permission creating a new table"
      rescue SecurityError => e
        #p e.backtrace
        # Is called on a Ruby/Tk library?
        caller_info = e.backtrace[1]
        if caller_info =~ %r{^#{MultiTkIp::BASE_DIR}/(tk|tkextlib)/[^:]+\.rb:}
          # Probably, caller is a Ruby/Tk library  -->  allow creating
        else
          raise e
        end
      end
    end

    id = @@TK_TABLE_LIST.size
    obj = Object.new
    @@TK_TABLE_LIST << obj
    obj.instance_variable_set(:@id, id)
    obj.instance_variable_set(:@mutex, Mutex.new)
    obj.instance_eval{
      def self.mutex
        @mutex
      end
      def self.method_missing(m, *args)
        MultiTkIp.tk_object_table(@id).__send__(m, *args)
      end
    }
    obj.freeze
    @@IP_TABLE.each{|tg, ip| ip._add_new_tables }
    return obj
  end

  def self.init_ip_env(script = Proc.new)
    @@INIT_IP_ENV << script
    if __getip.slave?
      begin
        raise SecurityError, "slave-IP has no permission initializing IP env"
      rescue SecurityError => e
        #p e.backtrace
        # Is called on a Ruby/Tk library?
        caller_info = e.backtrace[1]
        if caller_info =~ %r{^#{MultiTkIp::BASE_DIR}/(tk|tkextlib)/[^:]+\.rb:}
          # Probably, caller is a Ruby/Tk library  -->  allow creating
        else
          raise e
        end
      end
    end

    # @@IP_TABLE.each{|tg, ip|
    #   ip._init_ip_env(script)
    # }
    @@DEFAULT_MASTER.__init_ip_env__(@@IP_TABLE, script)
  end

  def self.add_tk_procs(name, args=nil, body=nil)
    if name.kind_of?(Array) # => an array of [name, args, body]
      name.each{|param| self.add_tk_procs(*param)}
    else
      name = name.to_s
      @@ADD_TK_PROCS << [name, args, body]
      @@IP_TABLE.each{|tg, ip|
        ip._add_tk_procs(name, args, body)
      }
    end
  end

  def self.remove_tk_procs(*names)
    names.each{|name|
      name = name.to_s
      @@ADD_TK_PROCS.delete_if{|elem|
        elem.kind_of?(Array) && elem[0].to_s == name
      }
    }
    @@IP_TABLE.each{|tg, ip|
      ip._remove_tk_procs(*names)
    }
  end

  def self.init_ip_internal
    __getip._init_ip_internal(@@INIT_IP_ENV, @@ADD_TK_PROCS)
  end
end

# for callback operation
class MultiTkIp
  def self.cb_entry_class
    @@CB_ENTRY_CLASS
  end
  def self.get_cb_entry(cmd)
    @@CB_ENTRY_CLASS.new(__getip, cmd).freeze
  end

=begin
  def cb_eval(cmd, *args)
    #self.eval_callback{ TkComm._get_eval_string(TkUtil.eval_cmd(cmd, *args)) }
    #ret = self.eval_callback{ TkComm._get_eval_string(TkUtil.eval_cmd(cmd, *args)) }
    ret = self.eval_callback(*args){|safe, *params|
      $SAFE=safe if $SAFE < safe
      TkComm._get_eval_string(TkUtil.eval_cmd(cmd, *params))
    }
    if ret.kind_of?(Exception)
      raise ret
    end
    ret
  end
=end
  def cb_eval(cmd, *args)
    self.eval_callback(*args,
                       &_proc_on_safelevel{|*params|
                         TkComm._get_eval_string(TkUtil.eval_cmd(cmd, *params))
                       })
  end
=begin
  def cb_eval(cmd, *args)
    self.eval_callback(*args){|safe, *params|
      $SAFE=safe if $SAFE < safe
      # TkUtil.eval_cmd(cmd, *params)
      TkComm._get_eval_string(TkUtil.eval_cmd(cmd, *params))
    }
  end
=end
=begin
  def cb_eval(cmd, *args)
    @callback_status[0] ||= TkVariable.new
    @callback_status[1] ||= TkVariable.new
    st, val = @callback_status
    th = Thread.new{
      self.eval_callback(*args){|safe, *params|
        #p [status, val, safe, *params]
        $SAFE=safe if $SAFE < safe
        begin
          TkComm._get_eval_string(TkUtil.eval_cmd(cmd, *params))
        rescue TkCallbackContinue
          st.value = 4
        rescue TkCallbackBreak
          st.value = 3
        rescue TkCallbackReturn
          st.value = 2
        rescue Exception => e
          val.value = e.message
          st.value = 1
        else
          st.value = 0
        end
      }
    }
    begin
      st.wait
      status = st.numeric
      retval = val.value
    rescue => e
      fail e
    end

    if status == 1
      fail RuntimeError, retval
    elsif status == 2
      fail TkCallbackReturn, "Tk callback returns 'return' status"
    elsif status == 3
      fail TkCallbackBreak, "Tk callback returns 'break' status"
    elsif status == 4
      fail TkCallbackContinue, "Tk callback returns 'continue' status"
    else
      ''
    end
  end
=end

end

# pseudo-toplevel operation support
class MultiTkIp
  # instance method
  def __pseudo_toplevel
    ip = MultiTkIp.__getip
    (ip == @@DEFAULT_MASTER || ip == self) &&
      self.__pseudo_toplevel_evaluable? && @pseudo_toplevel[1]
  end

  def __pseudo_toplevel=(m)
    unless (Thread.current.group == ThreadGroup::Default &&
              MultiTkIp.__getip == @@DEFAULT_MASTER)
      fail SecurityError, "no permission to manipulate"
    end

    # if m.kind_of?(Module) && m.respond_to?(:pseudo_toplevel_evaluable?)
    if m.respond_to?(:pseudo_toplevel_evaluable?)
      @pseudo_toplevel[0] = true
      @pseudo_toplevel[1] = m
    else
      fail ArgumentError, 'fail to set pseudo-toplevel'
    end
    self
  end

  def __pseudo_toplevel_evaluable?
    begin
      @pseudo_toplevel[0] && @pseudo_toplevel[1].pseudo_toplevel_evaluable?
    rescue Exception
      false
    end
  end

  def __pseudo_toplevel_evaluable=(mode)
    unless (Thread.current.group == ThreadGroup::Default &&
              MultiTkIp.__getip == @@DEFAULT_MASTER)
      fail SecurityError, "no permission to manipulate"
    end

    @pseudo_toplevel[0] = (mode)? true: false
  end
end


################################################
# use pseudo-toplevel feature of MultiTkIp ?
if (!defined?(Use_PseudoToplevel_Feature_of_MultiTkIp) ||
      Use_PseudoToplevel_Feature_of_MultiTkIp)
  module MultiTkIp_PseudoToplevel_Evaluable
    #def pseudo_toplevel_eval(body = Proc.new)
    #  Thread.current[:TOPLEVEL] = self
    #  begin
    #    body.call
    #  ensure
    #    Thread.current[:TOPLEVEL] = nil
    #  end
    #end

    def pseudo_toplevel_evaluable?
      @pseudo_toplevel_evaluable
    end

    def pseudo_toplevel_evaluable=(mode)
      @pseudo_toplevel_evaluable = (mode)? true: false
    end

    def self.extended(mod)
      mod.__send__(:extend_object, mod)
      mod.instance_variable_set('@pseudo_toplevel_evaluable', true)
    end
  end

  class Object
    alias __method_missing_alias_for_MultiTkIp__ method_missing
    private :__method_missing_alias_for_MultiTkIp__

    def method_missing(id, *args)
      begin
        has_top = (top = MultiTkIp.__getip.__pseudo_toplevel) &&
          top.respond_to?(:pseudo_toplevel_evaluable?) &&
          top.pseudo_toplevel_evaluable? &&
          top.respond_to?(id)
      rescue Exception => e
        has_top = false
      end

      if has_top
        top.__send__(id, *args)
      else
        __method_missing_alias_for_MultiTkIp__(id, *args)
      end
    end
  end
else
  # dummy
  module MultiTkIp_PseudoToplevel_Evaluable
    def pseudo_toplevel_evaluable?
      false
    end
  end
end


################################################
# evaluate a procedure on the proper interpreter
class MultiTkIp
  # instance & class method
  def _proc_on_safelevel(cmd=nil) # require a block for eval
    if cmd
      if cmd.kind_of?(Method)
        _proc_on_safelevel{|*args| cmd.call(*args)}
      else
        _proc_on_safelevel(&cmd)
      end
    else
      #Proc.new{|safe, *args| $SAFE=safe if $SAFE < safe; yield(*args)}
      Proc.new{|safe, *args|
        # avoid security error on Exception objects
        untrust_proc = proc{|err|
          begin
            err.untrust if err.respond_to?(:untrust)
          rescue SecurityError
          end
          err
        }
        $SAFE=safe if $SAFE < safe;
        begin
          yield(*args)
        rescue Exception => e
          fail untrust_proc.call(e)
        end
      }
    end
  end
  def MultiTkIp._proc_on_safelevel(cmd=nil, &blk)
    MultiTkIp.__getip._proc_on_safelevel(cmd, &blk)
  end

  def _proc_on_current_safelevel(cmd=nil, &blk) # require a block for eval
    safe = $SAFE
    cmd = _proc_on_safelevel(cmd, &blk)
    Proc.new{|*args| cmd.call(safe, *args)}
  end
  def MultiTkIp._proc_on_current_safelevel(cmd=nil, &blk)
    MultiTkIp.__getip._proc_on_current_safelevel(cmd, &blk)
  end

  ######################################
  # instance method
  def eval_proc_core(req_val, cmd, *args)
    # check
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    unless cmd.kind_of?(Proc) || cmd.kind_of?(Method)
      raise RuntimeError, "A Proc/Method object is expected for the 'cmd' argument"
    end

    # on IP thread
    if @cmd_receiver == Thread.current ||
        (!req_val && TclTkLib.mainloop_thread? != false) # callback
      begin
        ret = cmd.call(safe_level, *args)
      rescue SystemExit => e
        # exit IP
        warn("Warning: "+ e.inspect + " on " + self.inspect) if $DEBUG
        begin
          self._eval_without_enc('exit')
        rescue Exception => e
        end
        self.delete
        ret = nil
      rescue Exception => e
        if $DEBUG
          warn("Warning: " + e.class.inspect +
               ((e.message.length > 0)? ' "' + e.message + '"': '') +
               " on " + self.inspect)
        end
=begin
        begin
          bt = _toUTF8(e.backtrace.join("\n"))
          bt.instance_variable_set(:@encoding, 'utf-8')
        rescue Exception
          bt = e.backtrace.join("\n")
        end
        begin
          @interp._set_global_var('errorInfo', bt)
        rescue Exception
        end
=end
        ret = e
      end
      return ret
    end

    # send cmd to the proc-queue
    unless req_val
      begin
        @cmd_queue.enq([nil, cmd, *args])
      rescue Exception => e
        # ignore
        if $DEBUG
          warn("Warning: " + e.class.inspect +
               ((e.message.length > 0)? ' "' + e.message + '"': '') +
               " on " + self.inspect)
        end
        return e
      end
      return nil
    end

    # send and get return value by exception
    begin
      @cmd_queue.enq([Thread.current, cmd, *args])
      Thread.stop
    rescue MultiTkIp_OK => ret
      # return value
      return ret.value
    rescue SystemExit => e
      # exit IP
      warn("Warning: " + e.inspect + " on " + self.inspect) if $DEBUG
      begin
        self._eval_without_enc('exit')
      rescue Exception
      end
      if !self.deleted? && !safe? && allow_ruby_exit?
        self.delete
        fail e
      else
        self.delete
      end
    rescue Exception => e
      if $DEBUG
        warn("Warning: " + e.class.inspect +
             ((e.message.length > 0)? ' "' + e.message + '"': '') +
             " on " + self.inspect)
      end
      return e
    end
    return nil
  end
  private :eval_proc_core

if false && WITH_RUBY_VM  ### Ruby 1.9
  # Not stable, so disable this feature
  def eval_callback(*args)
    if block_given?
      cmd = Proc.new
    else
      cmd = args.shift
    end
    begin
      if @@CALLBACK_SUBTHREAD.table[self].index(Thread.current)
        last_th = nil
      else
        last_th = @@CALLBACK_SUBTHREAD.table[self][-1]
      end
      @@CALLBACK_SUBTHREAD.new(self){
        @@CALLBACK_SUBTHREAD.table[self] << Thread.current
        begin
          last_th.join if last_th
          eval_proc_core(false, cmd, *args)
        rescue Exception=>e
          e
        ensure
          @@CALLBACK_SUBTHREAD.table[self].delete(Thread.current)
        end
      }
    end
  end
else  ### Ruby 1.8
  def eval_callback(*args)
    if block_given?
      cmd = Proc.new
    else
      cmd = args.shift
    end
    begin
      eval_proc_core(false, cmd, *args)
    rescue Exception=>e
      e
    ensure
    end
  end
end

  def eval_proc(*args, &blk)
    if block_given?
      cmd = _proc_on_safelevel(&blk)
    else
      unless (cmd = args.shift)
        fail ArgumentError, "A Proc or Method object is expected for 1st argument"
      end
      cmd = _proc_on_safelevel(&cmd)
    end
    if TclTkLib.mainloop_thread? == true
      # call from eventloop
      current = Thread.current
      backup_ip = current[:callback_ip]
      current[:callback_ip] = self
      begin
        eval_proc_core(false, cmd, *args)
      ensure
        current[:callback_ip] = backup_ip
      end
    else
      eval_proc_core(true,
                     proc{|safe, *params|
                       Thread.new{cmd.call(safe, *params)}.value
                     },
                     *args)
    end
  end
=begin
  def eval_proc(*args)
    # The scope of the eval-block of 'eval_proc' method is different from
    # the external. If you want to pass local values to the eval-block,
    # use arguments of eval_proc method. They are passed to block-arguments.
    if block_given?
      cmd = Proc.new
    else
      unless (cmd = args.shift)
        fail ArgumentError, "A Proc or Method object is expected for 1st argument"
      end
    end
    if TclTkLib.mainloop_thread? == true
      # call from eventloop
      current = Thread.current
      backup_ip = current[:callback_ip]
      current[:callback_ip] = self
      begin
        eval_proc_core(false,
	               proc{|safe, *params|
		         $SAFE=safe if $SAFE < safe
                         cmd.call(*params)
                       }, *args)
      ensure
        current[:callback_ip] = backup_ip
      end
    else
      eval_proc_core(true,
                     proc{|safe, *params|
                       $SAFE=safe if $SAFE < safe
                       Thread.new(*params, &cmd).value
                     },
                     *args)
    end
  end
=end
  alias call eval_proc

  def bg_eval_proc(*args)
    if block_given?
      cmd = Proc.new
    else
      unless (cmd = args.shift)
        fail ArgumentError, "A Proc or Method object is expected for 1st argument"
      end
    end
    Thread.new{
      eval_proc(cmd, *args)
=begin
      eval_proc_core(false,
                     proc{|safe, *params|
                       $SAFE=safe if $SAFE < safe
                       Thread.new(*params, &cmd).value
                     },
                     safe_level, *args)
=end
    }
  end
  alias background_eval_proc bg_eval_proc
  alias thread_eval_proc bg_eval_proc
  alias bg_call bg_eval_proc
  alias background_call bg_eval_proc

  def eval_string(cmd, *eval_args)
    # cmd string ==> proc
    unless cmd.kind_of?(String)
      raise RuntimeError, "A String object is expected for the 'cmd' argument"
    end

    eval_proc_core(true,
                   proc{|safe|
                     Kernel.eval("$SAFE=#{safe} if $SAFE < #{safe};" << cmd,
                                 *eval_args)
                   })
  end
  alias eval_str eval_string

  def bg_eval_string(cmd, *eval_args)
    # cmd string ==> proc
    unless cmd.kind_of?(String)
      raise RuntimeError, "A String object is expected for the 'cmd' argument"
    end
    Thread.new{
      eval_proc_core(true,
                     proc{|safe|
                       Kernel.eval("$SAFE=#{safe} if $SAFE < #{safe};" << cmd,
                                   *eval_args)
                     })
    }
  end
  alias background_eval_string bg_eval_string
  alias bg_eval_str bg_eval_string
  alias background_eval_str bg_eval_string

  def eval(*args, &blk)
    if block_given?
      eval_proc(*args, &blk)
    elsif args[0]
      if args[0].respond_to?(:call)
        eval_proc(*args)
      else
        eval_string(*args)
      end
    else
      fail ArgumentError, "no argument to eval"
    end
  end

  def bg_eval(*args, &blk)
    if block_given?
      bg_eval_proc(*args, &blk)
    elsif args[0]
      if args[0].respond_to?(:call)
        bg_eval_proc(*args)
      else
        bg_eval_string(*args)
      end
    else
      fail ArgumentError, "no argument to eval"
    end
  end
  alias background_eval bg_eval
end

class << MultiTkIp
  # class method
  def eval_proc(*args, &blk)
    # class ==> interp object
    __getip.eval_proc(*args, &blk)
  end
  alias call eval_proc

  def bg_eval_proc(*args, &blk)
    # class ==> interp object
    __getip.bg_eval_proc(*args, &blk)
  end
  alias background_eval_proc bg_eval_proc
  alias thread_eval_proc bg_eval_proc
  alias bg_call bg_eval_proc
  alias background_call bg_eval_proc

  def eval_string(cmd, *eval_args)
    # class ==> interp object
    __getip.eval_string(cmd, *eval_args)
  end
  alias eval_str eval_string

  def bg_eval_string(cmd, *eval_args)
    # class ==> interp object
    __getip.bg_eval_string(cmd, *eval_args)
  end
  alias background_eval_string bg_eval_string
  alias bg_eval_str bg_eval_string
  alias background_eval_str bg_eval_string

  def eval(*args, &blk)
    # class ==> interp object
    __getip.eval(*args, &blk)
  end
  def bg_eval(*args, &blk)
    # class ==> interp object
    __getip.bg_eval(*args, &blk)
  end
  alias background_eval bg_eval
end


# event loop
# all master/slave IPs are controled by only one event-loop
class MultiTkIp
  def self.default_master?
    __getip == @@DEFAULT_MASTER
  end
end
class << MultiTkIp
  def mainloop(check_root = true)
    __getip.mainloop(check_root)
  end
  def mainloop_watchdog(check_root = true)
    __getip.mainloop_watchdog(check_root)
  end
  def do_one_event(flag = TclTkLib::EventFlag::ALL)
    __getip.do_one_event(flag)
  end
  def mainloop_abort_on_exception
    # __getip.mainloop_abort_on_exception
    TclTkLib.mainloop_abort_on_exception
  end
  def mainloop_abort_on_exception=(mode)
    # __getip.mainloop_abort_on_exception=(mode)
    TclTkLib.mainloop_abort_on_exception=(mode)
  end
  def set_eventloop_tick(tick)
    __getip.set_eventloop_tick(tick)
  end
  def get_eventloop_tick
    __getip.get_eventloop_tick
  end
  def set_no_event_wait(tick)
    __getip.set_no_event_wait(tick)
  end
  def get_no_event_wait
    __getip.get_no_event_wait
  end
  def set_eventloop_weight(loop_max, no_event_tick)
    __getip.set_eventloop_weight(loop_max, no_event_tick)
  end
  def get_eventloop_weight
    __getip.get_eventloop_weight
  end
end

# class methods to delegate to TclTkIp
class << MultiTkIp
  def method_missing(id, *args)
    __getip.__send__(id, *args)
  end

  def make_safe
    __getip.make_safe
  end

  def safe?
    __getip.safe?
  end

  def safe_base?
    begin
      __getip.safe_base?
    rescue
      false
    end
  end

  def allow_ruby_exit?
    __getip.allow_ruby_exit?
  end

  def allow_ruby_exit= (mode)
    __getip.allow_ruby_exit = mode
  end

  def delete
    __getip.delete
  end

  def deleted?
    __getip.deleted?
  end

  def has_mainwindow?
    __getip.has_mainwindow?
  end

  def invalid_namespace?
    __getip.invalid_namespace?
  end

  def abort(msg = nil)
    __getip.abort(msg)
  end

  def exit(st = true)
    __getip.exit(st)
  end

  def exit!(st = false)
    __getip.exit!(st)
  end

  def restart(app_name = nil, keys = {})
    init_ip_internal

    __getip._invoke('set', 'argv0', app_name) if app_name
    if keys.kind_of?(Hash)
      __getip._invoke('set', 'argv', _keys2opts(keys))
    end

    __getip.restart
  end

  def _eval(str)
    __getip._eval(str)
  end

  def _invoke(*args)
    __getip._invoke(*args)
  end

  def _eval_without_enc(str)
    __getip._eval_without_enc(str)
  end

  def _invoke_without_enc(*args)
    __getip._invoke_without_enc(*args)
  end

  def _eval_with_enc(str)
    __getip._eval_with_enc(str)
  end

  def _invoke_with_enc(*args)
    __getip._invoke_with_enc(*args)
  end

  def _toUTF8(str, encoding=nil)
    __getip._toUTF8(str, encoding)
  end

  def _fromUTF8(str, encoding=nil)
    __getip._fromUTF8(str, encoding)
  end

  def _thread_vwait(var)
    __getip._thread_vwait(var)
  end

  def _thread_tkwait(mode, target)
    __getip._thread_tkwait(mode, target)
  end

  def _return_value
    __getip._return_value
  end

  def _get_variable(var, flag)
    __getip._get_variable(var, flag)
  end
  def _get_variable2(var, idx, flag)
    __getip._get_variable2(var, idx, flag)
  end
  def _set_variable(var, value, flag)
    __getip._set_variable(var, value, flag)
  end
  def _set_variable2(var, idx, value, flag)
    __getip._set_variable2(var, idx, value, flag)
  end
  def _unset_variable(var, flag)
    __getip._unset_variable(var, flag)
  end
  def _unset_variable2(var, idx, flag)
    __getip._unset_variable2(var, idx, flag)
  end

  def _get_global_var(var)
    __getip._get_global_var(var)
  end
  def _get_global_var2(var, idx)
    __getip._get_global_var2(var, idx)
  end
  def _set_global_var(var, value)
    __getip._set_global_var(var, value)
  end
  def _set_global_var2(var, idx, value)
    __getip._set_global_var2(var, idx, value)
  end
  def _unset_global_var(var)
    __getip._unset_global_var(var)
  end
  def _unset_global_var2(var, idx)
    __getip._unset_global_var2(var, idx)
  end

  def _make_menu_embeddable(menu_path)
    __getip._make_menu_embeddable(menu_path)
  end

  def _split_tklist(str)
    __getip._split_tklist(str)
  end
  def _merge_tklist(*args)
    __getip._merge_tklist(*args)
  end
  def _conv_listelement(arg)
    __getip._conv_listelement(arg)
  end

  def _create_console
    __getip._create_console
  end
end


# wrap methods on TclTkLib : not permit calling TclTkLib module methods
class << TclTkLib
  def mainloop(check_root = true)
    MultiTkIp.mainloop(check_root)
  end
  def mainloop_watchdog(check_root = true)
    MultiTkIp.mainloop_watchdog(check_root)
  end
  def do_one_event(flag = TclTkLib::EventFlag::ALL)
    MultiTkIp.do_one_event(flag)
  end
  #def mainloop_abort_on_exception
  #  MultiTkIp.mainloop_abort_on_exception
  #end
  #def mainloop_abort_on_exception=(mode)
  #  MultiTkIp.mainloop_abort_on_exception=(mode)
  #end
  def set_eventloop_tick(tick)
    MultiTkIp.set_eventloop_tick(tick)
  end
  def get_eventloop_tick
    MultiTkIp.get_eventloop_tick
  end
  def set_no_event_wait(tick)
    MultiTkIp.set_no_event_wait(tick)
  end
  def get_no_event_wait
    MultiTkIp.get_no_event_wait
  end
  def set_eventloop_weight(loop_max, no_event_tick)
    MultiTkIp.set_eventloop_weight(loop_max, no_event_tick)
  end
  def get_eventloop_weight
    MultiTkIp.get_eventloop_weight
  end
  def restart(*args)
    MultiTkIp.restart(*args)
  end

  def _merge_tklist(*args)
    MultiTkIp._merge_tklist(*args)
  end
  def _conv_listelement(arg)
    MultiTkIp._conv_listelement(arg)
  end
end


# depend on TclTkIp
class MultiTkIp
#  def mainloop(check_root = true, restart_on_dead = true)
  def mainloop(check_root = true, restart_on_dead = false)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?

    if WITH_RUBY_VM  ### Ruby 1.9 !!!!!!!!!!!
      return @interp_thread.value if @interp_thread
    end

    #return self if self.slave?
    #return self if self != @@DEFAULT_MASTER
    if self != @@DEFAULT_MASTER
      if @wait_on_mainloop[0]
        begin
          @wait_on_mainloop[1] += 1
          if $SAFE >= 4
	    _receiver_mainloop(check_root).join
          else
            @cmd_queue.enq([@system, 'call_mainloop',
                            Thread.current, check_root])
            Thread.stop
          end
        rescue MultiTkIp_OK => ret
          # return value
          if ret.value.kind_of?(Thread)
            return ret.value.value
          else
            return ret.value
          end
        rescue SystemExit => e
          # exit IP
          warn("Warning: " + e.inspect + " on " + self.inspect) if $DEBUG
          begin
            self._eval_without_enc('exit')
          rescue Exception
          end
          self.delete
        rescue StandardError => e
          if $DEBUG
            warn("Warning: " + e.class.inspect +
                 ((e.message.length > 0)? ' "' + e.message + '"': '') +
                 " on " + self.inspect)
          end
          return e
        rescue Exception => e
	  return e
        ensure
          @wait_on_mainloop[1] -= 1
        end
      end
      return
    end

    unless restart_on_dead
      @wait_on_mainloop[1] += 1
=begin
      begin
        @interp.mainloop(check_root)
      rescue StandardError => e
        if $DEBUG
          warn("Warning: " + e.class.inspect +
               ((e.message.length > 0)? ' "' + e.message + '"': '') +
               " on " + self.inspect)
        end
      end
=end
      begin
	@interp.mainloop(check_root)
      ensure
	@wait_on_mainloop[1] -= 1
      end
    else
      loop do
        break unless self.alive?
        if check_root
          begin
            break if TclTkLib.num_of_mainwindows == 0
          rescue StandardError
            break
          end
        end
        break if @interp.deleted?
        begin
	  @wait_on_mainloop[1] += 1
          @interp.mainloop(check_root)
        rescue StandardError => e
          if TclTkLib.mainloop_abort_on_exception != nil
            #STDERR.print("Warning: Tk mainloop receives ", $!.class.inspect,
            #             " exception (ignore) : ", $!.message, "\n");
            if $DEBUG
              warn("Warning: Tk mainloop receives " << e.class.inspect <<
                   " exception (ignore) : " << e.message);
            end
          end
          #raise e
        rescue Exception => e
=begin
          if TclTkLib.mainloop_abort_on_exception != nil
            #STDERR.print("Warning: Tk mainloop receives ", $!.class.inspect,
            #             " exception (ignore) : ", $!.message, "\n");
            if $DEBUG
              warn("Warning: Tk mainloop receives " << e.class.inspect <<
                   " exception (ignore) : " << e.message);
            end
          end
=end
          raise e
        ensure
          @wait_on_mainloop[1] -= 1
          Thread.pass  # avoid eventloop conflict
        end
      end
    end
    self
  end

  def make_safe
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp.make_safe
  end

  def safe?
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp.safe?
  end

  def safe_base?
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @safe_base
  end

  def allow_ruby_exit?
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp.allow_ruby_exit?
  end

  def allow_ruby_exit= (mode)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp.allow_ruby_exit = mode
  end

  def delete
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @slave_ip_tbl.each{|name, subip|
      _destroy_slaves_of_slaveIP(subip)
=begin
      begin
        subip._invoke('destroy', '.') unless subip.deleted?
      rescue Exception
      end
=end
      begin
        # subip._eval_without_enc("foreach i [after info] {after cancel $i}")
	unless subip.deleted?
	  after_ids = subip._eval_without_enc("after info")
	  subip._eval_without_enc("foreach i {#{after_ids}} {after cancel $i}")
	end
      rescue Exception
      end

      # safe_base?
      if @interp._eval_without_enc("catch {::safe::interpConfigure #{name}}") == '0'
        begin
          @interp._eval_without_enc("::safe::interpDelete #{name}")
        rescue Exception
        else
          next if subip.deleted?
        end
      end
      if subip.respond_to?(:safe_base?) && subip.safe_base? &&
          !subip.deleted?
        # do 'exit' to call the delete_hook procedure
        begin
          subip._eval_without_enc('exit')
        rescue Exception
        end
      else
        begin
          subip.delete unless subip.deleted?
        rescue Exception
        end
      end
    }

    begin
      # @interp._eval_without_enc("foreach i [after info] {after cancel $i}")
      after_ids = @interp._eval_without_enc("after info")
      @interp._eval_without_enc("foreach i {#{after_ids}} {after cancel $i}")
    rescue Exception
    end

    begin
      @interp._invoke('destroy', '.') unless @interp.deleted?
    rescue Exception
    end

    if @safe_base && !@interp.deleted?
      # do 'exit' to call the delete_hook procedure
      @interp._eval_without_enc('exit')
    end
    @interp.delete
    self
  end

  def deleted?
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp.deleted?
  end

  def has_mainwindow?
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp.has_mainwindow?
  end

  def invalid_namespace?
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp.invalid_namespace?
  end

  def abort(msg = nil)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    if master? && !safe? && allow_ruby_exit?
      if msg
        Kernel.abort(msg)
      else
        Kernel.abort
      end
    else
      # ignore msg
      delete
      1
    end
  end

  def exit(st = true)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    if master? && !safe? && allow_ruby_exit?
      Kernel.exit(st)
    else
      delete
      st
    end
  end

  def exit!(st = false)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    if master? && !safe? && allow_ruby_exit?
      Kernel.exit!(st)
    else
      delete
      st
    end
  end

  def restart(app_name = nil, keys = {})
    raise SecurityError, "no permission to manipulate" unless self.manipulable?

    _init_ip_internal(@@INIT_IP_ENV, @@ADD_TK_PROCS)

    @interp._invoke('set', 'argv0', app_name) if app_name
    if keys.kind_of?(Hash)
      @interp._invoke('set', 'argv', _keys2opts(keys))
    end

    @interp.restart
  end

  def __eval(str)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp.__eval(str)
  end

  def __invoke(*args)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp.__invoke(*args)
  end

  def _eval(str)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._eval(str)
  end

  def _invoke(*args)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._invoke(*args)
  end

  def _eval_without_enc(str)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._eval_without_enc(str)
  end

  def _invoke_without_enc(*args)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._invoke_without_enc(*args)
  end

  def _eval_with_enc(str)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._eval_with_enc(str)
  end

  def _invoke_with_enc(*args)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._invoke_with_enc(*args)
  end

  def _toUTF8(str, encoding=nil)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._toUTF8(str, encoding)
  end

  def _fromUTF8(str, encoding=nil)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._fromUTF8(str, encoding)
  end

  def _thread_vwait(var)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._thread_vwait(var)
  end

  def _thread_tkwait(mode, target)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._thread_tkwait(mode, target)
  end

  def _return_value
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._return_value
  end

  def _get_variable(var, flag)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._get_variable(var, flag)
  end
  def _get_variable2(var, idx, flag)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._get_variable2(var, idx, flag)
  end
  def _set_variable(var, value, flag)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._set_variable(var, value, flag)
  end
  def _set_variable2(var, idx, value, flag)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._set_variable2(var, idx, value, flag)
  end
  def _unset_variable(var, flag)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._unset_variable(var, flag)
  end
  def _unset_variable2(var, idx, flag)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._unset_variable2(var, idx, flag)
  end

  def _get_global_var(var)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._get_global_var(var)
  end
  def _get_global_var2(var, idx)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._get_global_var2(var, idx)
  end
  def _set_global_var(var, value)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._set_global_var(var, value)
  end
  def _set_global_var2(var, idx, value)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._set_global_var2(var, idx, value)
  end
  def _unset_global_var(var)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._unset_global_var(var)
  end
  def _unset_global_var2(var, idx)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._unset_global_var2(var, idx)
  end

  def _make_menu_embeddable(menu_path)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._make_menu_embeddable(menu_path)
  end

  def _split_tklist(str)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._split_tklist(str)
  end
  def _merge_tklist(*args)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._merge_tklist(*args)
  end
  def _conv_listelement(arg)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._conv_listelement(arg)
  end
end


# interp command support
class MultiTkIp
  def _lst2ary(str)
    return [] if str == ""
    idx = str.index('{')
    while idx and idx > 0 and str[idx-1] == ?\\
      idx = str.index('{', idx+1)
    end
    return str.split unless idx

    list = str[0,idx].split
    str = str[idx+1..-1]
    i = -1
    brace = 1
    str.each_byte {|c|
      c = c.chr
      i += 1
      brace += 1 if c == '{'
      brace -= 1 if c == '}'
      break if brace == 0
    }
    if i == 0
      list.push ''
    elsif str[0, i] == ' '
      list.push ' '
    else
      list.push str[0..i-1]
    end
    #list += _lst2ary(str[i+1..-1])
    list.concat(_lst2ary(str[i+1..-1]))
    list
  end
  private :_lst2ary

  def _slavearg(slave)
    if slave.kind_of?(MultiTkIp)
      slave.path
    elsif slave.kind_of?(String)
      slave
    else
      slave.to_s
    end
  end
  private :_slavearg

  def alias_info(slave, cmd_name)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    _lst2ary(@interp._invoke('interp', 'alias', _slavearg(slave), cmd_name))
  end
  def self.alias_info(slave, cmd_name)
    __getip.alias_info(slave, cmd_name)
  end

  def alias_delete(slave, cmd_name)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._invoke('interp', 'alias', _slavearg(slave), cmd_name, '')
    self
  end
  def self.alias_delete(slave, cmd_name)
    __getip.alias_delete(slave, cmd_name)
    self
  end

  def def_alias(slave, new_cmd, org_cmd, *args)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    ret = @interp._invoke('interp', 'alias', _slavearg(slave), new_cmd,
                          '', org_cmd, *args)
    (ret == new_cmd)? self: nil
  end
  def self.def_alias(slave, new_cmd, org_cmd, *args)
    ret = __getip.def_alias(slave, new_cmd, org_cmd, *args)
    (ret == new_cmd)? self: nil
  end

  def aliases(slave = '')
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    _lst2ary(@interp._invoke('interp', 'aliases', _slavearg(slave)))
  end
  def self.aliases(slave = '')
    __getip.aliases(slave)
  end

  def delete_slaves(*args)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    slaves = args.collect{|s| _slavearg(s)}
    @interp._invoke('interp', 'delete', *slaves) if slaves.size > 0
    self
  end
  def self.delete_slaves(*args)
    __getip.delete_slaves(*args)
    self
  end

  def exist?(slave = '')
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    ret = @interp._invoke('interp', 'exists', _slavearg(slave))
    (ret == '1')? true: false
  end
  def self.exist?(slave = '')
    __getip.exist?(slave)
  end

  def delete_cmd(slave, cmd)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    slave_invoke = @interp._invoke('list', 'rename', cmd, '')
    @interp._invoke('interp', 'eval', _slavearg(slave), slave_invoke)
    self
  end
  def self.delete_cmd(slave, cmd)
    __getip.delete_cmd(slave, cmd)
    self
  end

  def expose_cmd(slave, cmd, aliasname = nil)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    if aliasname
      @interp._invoke('interp', 'expose', _slavearg(slave), cmd, aliasname)
    else
      @interp._invoke('interp', 'expose', _slavearg(slave), cmd)
    end
    self
  end
  def self.expose_cmd(slave, cmd, aliasname = nil)
    __getip.expose_cmd(slave, cmd, aliasname)
    self
  end

  def hide_cmd(slave, cmd, aliasname = nil)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    if aliasname
      @interp._invoke('interp', 'hide', _slavearg(slave), cmd, aliasname)
    else
      @interp._invoke('interp', 'hide', _slavearg(slave), cmd)
    end
    self
  end
  def self.hide_cmd(slave, cmd, aliasname = nil)
    __getip.hide_cmd(slave, cmd, aliasname)
    self
  end

  def hidden_cmds(slave = '')
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    _lst2ary(@interp._invoke('interp', 'hidden', _slavearg(slave)))
  end
  def self.hidden_cmds(slave = '')
    __getip.hidden_cmds(slave)
  end

  def invoke_hidden(slave, cmd, *args)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    if args[-1].kind_of?(Hash)
      keys = _symbolkey2str(args.pop)
    else
      keys = []
    end
    keys << _slavearg(slave)
    if Tk::TCL_MAJOR_VERSION > 8 ||
        (Tk::TCL_MAJOR_VERSION == 8 && Tk::TCL_MINOR_VERSION >= 5)
      keys << '--'
    end
    keys << cmd
    keys.concat(args)
    @interp._invoke('interp', 'invokehidden', *keys)
  end
  def self.invoke_hidden(slave, cmd, *args)
    __getip.invoke_hidden(slave, cmd, *args)
  end

  def invoke_hidden_on_global(slave, cmd, *args)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    if args[-1].kind_of?(Hash)
      keys = _symbolkey2str(args.pop)
    else
      keys = []
    end
    keys << _slavearg(slave)
    keys << '-global'
    if Tk::TCL_MAJOR_VERSION > 8 ||
        (Tk::TCL_MAJOR_VERSION == 8 && Tk::TCL_MINOR_VERSION >= 5)
      keys << '--'
    end
    keys << cmd
    keys.concat(args)
    @interp._invoke('interp', 'invokehidden', *keys)
  end
  def self.invoke_hidden_on_global(slave, cmd, *args)
    __getip.invoke_hidden_on_global(slave, cmd, *args)
  end

  def invoke_hidden_on_namespace(slave, ns, cmd, *args)
    # for Tcl8.5 or later
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    if args[-1].kind_of?(Hash)
      keys = _symbolkey2str(args.pop)
    else
      keys = []
    end
    keys << _slavearg(slave)
    keys << '-namespace' << TkComm._get_eval_string(ns)
    keys << '--' << cmd
    keys.concat(args)
    @interp._invoke('interp', 'invokehidden', *keys)
  end
  def self.invoke_hidden_on_namespace(slave, ns, cmd, *args)
    __getip.invoke_hidden_on_namespace(slave, ns, cmd, *args)
  end

  def mark_trusted(slave = '')
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._invoke('interp', 'marktrusted', _slavearg(slave))
    self
  end
  def self.mark_trusted(slave = '')
    __getip.mark_trusted(slave)
    self
  end

  def set_bgerror_handler(cmd = Proc.new, slave = nil, &b)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?

    unless TkComm._callback_entry?(cmd)
      if !slave && b
        slave = cmd
        cmd = Proc.new(&b)
      end
    end
    slave = '' unless slave

    @interp._invoke('interp', 'bgerror', _slavearg(slave), cmd)
  end
  def self.bgerror(cmd = Proc.new, slave = nil, &b)
    __getip.bgerror(cmd, slave, &b)
  end

  def get_bgerror_handler(slave = '')
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    procedure(@interp._invoke('interp', 'bgerror', _slavearg(slave)))
  end
  def self.bgerror(slave = '')
    __getip.bgerror(slave)
  end

  def set_limit(limit_type, slave = '', opts = {})
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._invoke('interp', 'limit', _slavearg(slave), limit_type, opts)
  end
  def self.set_limit(limit_type, slave = '', opts = {})
    __getip.set_limit(limit_type, slave, opts)
  end

  def get_limit(limit_type, slave = '', slot = nil)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?

    if slot
      num_or_str(@interp._invoke('interp', 'limit', _slavearg(slave),
                                 limit_type, slot))
    else
      l = @interp._split_tklist(@interp._invoke_without_enc('interp', 'limit',
                                                            _slavearg(slave),
                                                            limit_type))
      l.map!{|s| _fromUTF8(s)}
      r = {}
      until l.empty?
        key = l.shift[1..-1]
        val = l.shift
        val = num_or_str(val) if val
        r[key] = val
      end
      r
    end
  end
  def self.get_limit(limit_type, slave = '', slot = nil)
    __getip.get_limit(limit_type, slave, slot)
  end

  def recursion_limit(slave = '', limit = None)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    number(@interp._invoke('interp', 'recursionlimit',
                           _slavearg(slave), limit))
  end
  def self.recursion_limit(slave = '', limit = None)
    __getip.recursion_limit(slave)
  end

  def alias_target(aliascmd, slave = '')
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._invoke('interp', 'target', _slavearg(slave), aliascmd)
  end
  def self.alias_target(aliascmd, slave = '')
    __getip.alias_target(aliascmd, slave)
  end

  def share_stdin(dist, src = '')
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._invoke('interp', 'share', src, 'stdin', dist)
    self
  end
  def self.share_stdin(dist, src = '')
    __getip.share_stdin(dist, src)
    self
  end

  def share_stdout(dist, src = '')
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._invoke('interp', 'share', src, 'stdout', dist)
    self
  end
  def self.share_stdout(dist, src = '')
    __getip.share_stdout(dist, src)
    self
  end

  def share_stderr(dist, src = '')
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._invoke('interp', 'share', src, 'stderr', dist)
    self
  end
  def self.share_stderr(dist, src = '')
    __getip.share_stderr(dist, src)
    self
  end

  def transfer_stdin(dist, src = '')
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._invoke('interp', 'transfer', src, 'stdin', dist)
    self
  end
  def self.transfer_stdin(dist, src = '')
    __getip.transfer_stdin(dist, src)
    self
  end

  def transfer_stdout(dist, src = '')
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._invoke('interp', 'transfer', src, 'stdout', dist)
    self
  end
  def self.transfer_stdout(dist, src = '')
    __getip.transfer_stdout(dist, src)
    self
  end

  def transfer_stderr(dist, src = '')
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._invoke('interp', 'transfer', src, 'stderr', dist)
    self
  end
  def self.transfer_stderr(dist, src = '')
    __getip.transfer_stderr(dist, src)
    self
  end

  def share_stdio(dist, src = '')
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._invoke('interp', 'share', src, 'stdin',  dist)
    @interp._invoke('interp', 'share', src, 'stdout', dist)
    @interp._invoke('interp', 'share', src, 'stderr', dist)
    self
  end
  def self.share_stdio(dist, src = '')
    __getip.share_stdio(dist, src)
    self
  end

  def transfer_stdio(dist, src = '')
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp._invoke('interp', 'transfer', src, 'stdin',  dist)
    @interp._invoke('interp', 'transfer', src, 'stdout', dist)
    @interp._invoke('interp', 'transfer', src, 'stderr', dist)
    self
  end
  def self.transfer_stdio(dist, src = '')
    __getip.transfer_stdio(dist, src)
    self
  end
end


# Safe Base :: manipulating safe interpreter
class MultiTkIp
  def safeip_configure(slot, value=None)
    # use for '-noStatics' option ==> {statics=>false}
    #     for '-nestedLoadOk' option ==> {nested=>true}
    if slot.kind_of?(Hash)
      ip = MultiTkIp.__getip
      ip._eval('::safe::interpConfigure ' + @ip_name + ' ' + _keys2opts(slot))
    else
      ip._eval('::safe::interpConfigure ' + @ip_name + ' ' +
               "-#{slot} #{_get_eval_string(value)}")
    end
    self
  end

  def safeip_configinfo(slot = nil)
    ip = MultiTkIp.__getip
    ret = {}
    if slot
      conf = _lst2ary(ip._eval("::safe::interpConfigure " +
                               @ip_name + " -#{slot}"))
      if conf[0] == '-deleteHook'
=begin
        if conf[1] =~ /^rb_out\S* (c(_\d+_)?\d+)/
          ret[conf[0][1..-1]] = MultiTkIp._tk_cmd_tbl[$1]
=end
        if conf[1] =~ /rb_out\S*(?:\s+(::\S*|[{](::.*)[}]|["](::.*)["]))? (c(_\d+_)?(\d+))/
          ret[conf[0][1..-1]] = MultiTkIp._tk_cmd_tbl[$4]
        else
          ret[conf[0][1..-1]] = conf[1]
        end
      else
        ret[conf[0][1..-1]] = conf[1]
      end
    else
      Hash[*_lst2ary(ip._eval("::safe::interpConfigure " +
                              @ip_name))].each{|k, v|
        if k == '-deleteHook'
=begin
          if v =~ /^rb_out\S* (c(_\d+_)?\d+)/
            ret[k[1..-1]] = MultiTkIp._tk_cmd_tbl[$1]
=end
          if v =~ /rb_out\S*(?:\s+(::\S*|[{](::.*)[}]|["](::.*)["]))? (c(_\d+_)?(\d+))/
            ret[k[1..-1]] = MultiTkIp._tk_cmd_tbl[$4]
          else
            ret[k[1..-1]] = v
          end
        else
          ret[k[1..-1]] = v
        end
      }
    end
    ret
  end

  def safeip_delete
    ip = MultiTkIp.__getip
    ip._eval("::safe::interpDelete " + @ip_name)
  end

  def safeip_add_to_access_path(dir)
    ip = MultiTkIp.__getip
    ip._eval("::safe::interpAddToAccessPath #{@ip_name} #{dir}")
  end

  def safeip_find_in_access_path(dir)
    ip = MultiTkIp.__getip
    ip._eval("::safe::interpFindInAccessPath #{@ip_name} #{dir}")
  end

  def safeip_set_log_cmd(cmd = Proc.new)
    ip = MultiTkIp.__getip
    ip._eval("::safe::setLogCmd #{@ip_name} #{_get_eval_string(cmd)}")
  end
end


# encoding convert
class << MultiTkIp
  def encoding_table
    __getip.encoding_table
  end

  def force_default_encoding=(mode)
    __getip.force_default_encoding=(mode)
  end

  def force_default_encoding?
    __getip.force_default_encoding?
  end

  def default_encoding=(enc)
    __getip.default_encoding=(enc)
  end

  def encoding=(enc)
    __getip.encoding=(enc)
  end

  def encoding_name
    __getip.encoding_name
  end

  def encoding_obj
    __getip.encoding_obj
  end
  alias encoding encoding_name
  alias default_encoding encoding_name

  def encoding_convertfrom(str, enc=None)
    __getip.encoding_convertfrom(str, enc)
  end
  alias encoding_convert_from encoding_convertfrom

  def encoding_convertto(str, enc=None)
    __getip.encoding_convertto(str, enc)
  end
  alias encoding_convert_to encoding_convertto
end
class MultiTkIp
  def encoding_table
    @interp.encoding_table
  end

  def force_default_encoding=(mode)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp.force_default_encoding = mode
  end
  def force_default_encoding?
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp.force_default_encoding?
  end

  def default_encoding=(enc)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp.default_encoding = enc
  end

  def encoding=(enc)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp.encoding = enc
  end
  def encoding_name
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp.encoding_name
  end
  def encoding_obj
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp.encoding_obj
  end
  alias encoding encoding_name
  alias default_encoding encoding_name

  def encoding_convertfrom(str, enc=None)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp.encoding_convertfrom(str, enc)
  end
  alias encoding_convert_from encoding_convertfrom

  def encoding_convertto(str, enc=None)
    raise SecurityError, "no permission to manipulate" unless self.manipulable?
    @interp.encoding_convertto(str, enc)
  end
  alias encoding_convert_to encoding_convertto
end


# remove methods for security
=begin
class MultiTkIp
  INTERP_THREAD = @@DEFAULT_MASTER.instance_variable_get('@interp_thread')
  INTERP_MUTEX  = INTERP_THREAD[:mutex]
  INTERP_ROOT_CHECK = INTERP_THREAD[:root_check]

  # undef_method :instance_eval
  undef_method :instance_variable_get
  undef_method :instance_variable_set
end

module TkCore
  if MultiTkIp::WITH_RUBY_VM &&
      ! MultiTkIp::RUN_EVENTLOOP_ON_MAIN_THREAD ### check Ruby 1.9 !!!!!!!
    INTERP_THREAD = MultiTkIp::INTERP_THREAD
    INTERP_MUTEX  = MultiTkIp::INTERP_MUTEX
    INTERP_ROOT_CHECK = MultiTkIp::INTERP_ROOT_CHECK
  end
end
class MultiTkIp
  remove_const(:INTERP_THREAD)
  remove_const(:INTERP_MUTEX)
  remove_const(:INTERP_ROOT_CHECK)
end
=end
if MultiTkIp::WITH_RUBY_VM &&
    ! MultiTkIp::RUN_EVENTLOOP_ON_MAIN_THREAD ### check Ruby 1.9 !!!!!!!
  class MultiTkIp
    INTERP_THREAD = @@DEFAULT_MASTER.instance_variable_get('@interp_thread')
    INTERP_THREAD_STATUS = INTERP_THREAD[:status]
    INTERP_MUTEX  = INTERP_THREAD[:mutex]
    INTERP_ROOT_CHECK = INTERP_THREAD[:root_check]
  end
  module TkCore
    INTERP_THREAD = MultiTkIp::INTERP_THREAD
    INTERP_THREAD_STATUS = MultiTkIp::INTERP_THREAD_STATUS
    INTERP_MUTEX  = MultiTkIp::INTERP_MUTEX
    INTERP_ROOT_CHECK = MultiTkIp::INTERP_ROOT_CHECK
  end
  class MultiTkIp
    remove_const(:INTERP_THREAD)
    remove_const(:INTERP_THREAD_STATUS)
    remove_const(:INTERP_MUTEX)
    remove_const(:INTERP_ROOT_CHECK)
  end
end

class MultiTkIp
  # undef_method :instance_eval
  undef_method :instance_variable_get
  undef_method :instance_variable_set
end
# end of MultiTkIp definition

# defend against modification
#MultiTkIp.freeze
#TclTkLib.freeze

########################################
#  start Tk which depends on MultiTkIp
module TkCore
  INTERP = MultiTkIp
end
require 'tk'
