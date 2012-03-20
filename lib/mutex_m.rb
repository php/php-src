#
#   mutex_m.rb -
#       $Release Version: 3.0$
#       $Revision: 1.7 $
#       Original from mutex.rb
#       by Keiju ISHITSUKA(keiju@ishitsuka.com)
#       modified by matz
#       patched by akira yamada
#
# --
#   Usage:
#       require "mutex_m.rb"
#       obj = Object.new
#       obj.extend Mutex_m
#       ...
#       extended object can be handled like Mutex
#       or
#       class Foo
#         include Mutex_m
#         ...
#       end
#       obj = Foo.new
#       this obj can be handled like Mutex
#

require 'thread'

module Mutex_m
  def Mutex_m.define_aliases(cl)
    cl.module_eval %q{
      alias locked? mu_locked?
      alias lock mu_lock
      alias unlock mu_unlock
      alias try_lock mu_try_lock
      alias synchronize mu_synchronize
    }
  end

  def Mutex_m.append_features(cl)
    super
    define_aliases(cl) unless cl.instance_of?(Module)
  end

  def Mutex_m.extend_object(obj)
    super
    obj.mu_extended
  end

  def mu_extended
    unless (defined? locked? and
            defined? lock and
            defined? unlock and
            defined? try_lock and
            defined? synchronize)
      Mutex_m.define_aliases(singleton_class)
    end
    mu_initialize
  end

  # locking
  def mu_synchronize(&block)
    @_mutex.synchronize(&block)
  end

  def mu_locked?
    @_mutex.locked?
  end

  def mu_try_lock
    @_mutex.try_lock
  end

  def mu_lock
    @_mutex.lock
  end

  def mu_unlock
    @_mutex.unlock
  end

  def sleep(timeout = nil)
    @_mutex.sleep(timeout)
  end

  private

  def mu_initialize
    @_mutex = Mutex.new
  end

  def initialize(*args)
    mu_initialize
    super
  end
end
