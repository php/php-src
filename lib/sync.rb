#
#   sync.rb - 2 phase lock with counter
#       $Release Version: 1.0$
#       $Revision$
#       by Keiju ISHITSUKA(keiju@ishitsuka.com)
#
# --
#  Sync_m, Synchronizer_m
#  Usage:
#   obj.extend(Sync_m)
#   or
#   class Foo
#       include Sync_m
#       :
#   end
#
#   Sync_m#sync_mode
#   Sync_m#sync_locked?, locked?
#   Sync_m#sync_shared?, shared?
#   Sync_m#sync_exclusive?, sync_exclusive?
#   Sync_m#sync_try_lock, try_lock
#   Sync_m#sync_lock, lock
#   Sync_m#sync_unlock, unlock
#
#  Sync, Synchronizer:
#  Usage:
#   sync = Sync.new
#
#   Sync#mode
#   Sync#locked?
#   Sync#shared?
#   Sync#exclusive?
#   Sync#try_lock(mode) -- mode = :EX, :SH, :UN
#   Sync#lock(mode)     -- mode = :EX, :SH, :UN
#   Sync#unlock
#   Sync#synchronize(mode) {...}
#
#

unless defined? Thread
  raise "Thread not available for this ruby interpreter"
end

##
# A module that provides a two-phase lock with a counter.

module Sync_m
  RCS_ID='-$Id$-'

  # lock mode
  UN = :UN
  SH = :SH
  EX = :EX

  # exceptions
  class Err < StandardError
    def Err.Fail(*opt)
      fail self, sprintf(self::Message, *opt)
    end

    class UnknownLocker < Err
      Message = "Thread(%s) not locked."
      def UnknownLocker.Fail(th)
        super(th.inspect)
      end
    end

    class LockModeFailer < Err
      Message = "Unknown lock mode(%s)"
      def LockModeFailer.Fail(mode)
        if mode.id2name
          mode = id2name
        end
        super(mode)
      end
    end
  end

  def Sync_m.define_aliases(cl)
    cl.module_eval %q{
      alias locked? sync_locked?
      alias shared? sync_shared?
      alias exclusive? sync_exclusive?
      alias lock sync_lock
      alias unlock sync_unlock
      alias try_lock sync_try_lock
      alias synchronize sync_synchronize
    }
  end

  def Sync_m.append_features(cl)
    super
    # do nothing for Modules
    # make aliases for Classes.
    define_aliases(cl) unless cl.instance_of?(Module)
    self
  end

  def Sync_m.extend_object(obj)
    super
    obj.sync_extend
  end

  def sync_extend
    unless (defined? locked? and
            defined? shared? and
            defined? exclusive? and
            defined? lock and
            defined? unlock and
            defined? try_lock and
            defined? synchronize)
      Sync_m.define_aliases(singleton_class)
    end
    sync_initialize
  end

  # accessing
  def sync_locked?
    sync_mode != UN
  end

  def sync_shared?
    sync_mode == SH
  end

  def sync_exclusive?
    sync_mode == EX
  end

  # locking methods.
  def sync_try_lock(mode = EX)
    return unlock if mode == UN
    @sync_mutex.synchronize do
      sync_try_lock_sub(mode)
    end
  end

  def sync_lock(m = EX)
    return unlock if m == UN

    while true
      @sync_mutex.synchronize do
        if sync_try_lock_sub(m)
          return self
        else
          if sync_sh_locker[Thread.current]
            sync_upgrade_waiting.push [Thread.current, sync_sh_locker[Thread.current]]
            sync_sh_locker.delete(Thread.current)
          else
            sync_waiting.push Thread.current
          end
          @sync_mutex.sleep
        end
      end
    end
    self
  end

  def sync_unlock(m = EX)
    wakeup_threads = []
    @sync_mutex.synchronize do
      if sync_mode == UN
        Err::UnknownLocker.Fail(Thread.current)
      end

      m = sync_mode if m == EX and sync_mode == SH

      runnable = false
      case m
      when UN
        Err::UnknownLocker.Fail(Thread.current)

      when EX
        if sync_ex_locker == Thread.current
          if (self.sync_ex_count = sync_ex_count - 1) == 0
            self.sync_ex_locker = nil
            if sync_sh_locker.include?(Thread.current)
              self.sync_mode = SH
            else
              self.sync_mode = UN
            end
            runnable = true
          end
        else
          Err::UnknownLocker.Fail(Thread.current)
        end

      when SH
        if (count = sync_sh_locker[Thread.current]).nil?
          Err::UnknownLocker.Fail(Thread.current)
        else
          if (sync_sh_locker[Thread.current] = count - 1) == 0
            sync_sh_locker.delete(Thread.current)
            if sync_sh_locker.empty? and sync_ex_count == 0
              self.sync_mode = UN
              runnable = true
            end
          end
        end
      end

      if runnable
        if sync_upgrade_waiting.size > 0
          th, count = sync_upgrade_waiting.shift
          sync_sh_locker[th] = count
          th.wakeup
          wakeup_threads.push th
        else
          wait = sync_waiting
          self.sync_waiting = []
          for th in wait
            th.wakeup
            wakeup_threads.push th
          end
        end
      end
    end
    for th in wakeup_threads
      th.run
    end
    self
  end

  def sync_synchronize(mode = EX)
    sync_lock(mode)
    begin
      yield
    ensure
      sync_unlock
    end
  end

  attr_accessor :sync_mode

  attr_accessor :sync_waiting
  attr_accessor :sync_upgrade_waiting
  attr_accessor :sync_sh_locker
  attr_accessor :sync_ex_locker
  attr_accessor :sync_ex_count

  def sync_inspect
    sync_iv = instance_variables.select{|iv| /^@sync_/ =~ iv.id2name}.collect{|iv| iv.id2name + '=' + instance_eval(iv.id2name).inspect}.join(",")
    print "<#{self.class}.extend Sync_m: #{inspect}, <Sync_m: #{sync_iv}>"
  end

  private

  def sync_initialize
    @sync_mode = UN
    @sync_waiting = []
    @sync_upgrade_waiting = []
    @sync_sh_locker = Hash.new
    @sync_ex_locker = nil
    @sync_ex_count = 0

    @sync_mutex = Mutex.new
  end

  def initialize(*args)
    super
    sync_initialize
  end

  def sync_try_lock_sub(m)
    case m
    when SH
      case sync_mode
      when UN
        self.sync_mode = m
        sync_sh_locker[Thread.current] = 1
        ret = true
      when SH
        count = 0 unless count = sync_sh_locker[Thread.current]
        sync_sh_locker[Thread.current] = count + 1
        ret = true
      when EX
        # in EX mode, lock will upgrade to EX lock
        if sync_ex_locker == Thread.current
          self.sync_ex_count = sync_ex_count + 1
          ret = true
        else
          ret = false
        end
      end
    when EX
      if sync_mode == UN or
          sync_mode == SH && sync_sh_locker.size == 1 && sync_sh_locker.include?(Thread.current)
        self.sync_mode = m
        self.sync_ex_locker = Thread.current
        self.sync_ex_count = 1
        ret = true
      elsif sync_mode == EX && sync_ex_locker == Thread.current
        self.sync_ex_count = sync_ex_count + 1
        ret = true
      else
        ret = false
      end
    else
      Err::LockModeFailer.Fail mode
    end
    return ret
  end
end

##
# An alias for Sync_m from sync.rb

Synchronizer_m = Sync_m

##
# A class that providesa two-phase lock with a counter.  See Sync_m for
# details.

class Sync
  include Sync_m
end

##
# An alias for Sync from sync.rb.  See Sync_m for details.

Synchronizer = Sync
