#
#   tk/winpkg.rb : methods for Tcl/Tk packages for Microsoft Windows
#                     2000/11/22 by Hidetoshi Nagai <nagai@ai.kyutech.ac.jp>
#
#     ATTENTION !!
#         This is NOT TESTED. Because I have no test-environment.
#
require 'tk'

module Tk::WinDDE
end
#TkWinDDE = Tk::WinDDE
#Tk.__set_toplevel_aliases__(:Tk, Tk::WinDDE, :TkWinDDE)
Tk.__set_loaded_toplevel_aliases__('tk/winpkg.rb', :Tk, Tk::WinDDE, :TkWinDDE)

module Tk::WinDDE
  extend Tk
  extend Tk::WinDDE

  TkCommandNames = ['dde'.freeze].freeze

  PACKAGE_NAME = 'dde'.freeze
  def self.package_name
    PACKAGE_NAME
  end

  if self.const_defined? :FORCE_VERSION
    tk_call_without_enc('package', 'require', 'dde', FORCE_VERSION)
  else
    tk_call_without_enc('package', 'require', 'dde')
  end

  #def servername(topic=None)
  #  tk_call('dde', 'servername', topic)
  #end
  def servername(*args)
    if args.size == 0
      tk_call('dde', 'servername')
    else
      if args[-1].kind_of?(Hash)  # dde 1.2 +
        keys = _symbolkey2str(args.pop)
        force = (keys.delete('force'))? '-force': None
        exact = (keys.delete('exact'))? '-exact': None
        if keys.size == 0
          tk_call('dde', 'servername', force, exact)
        elsif args.size == 0
          tk_call('dde', 'servername', force, exact, *hash_kv(keys))
        else
          tk_call('dde', 'servername', force, exact,
                  *((hash_kv(keys) << '--') + args))
        end
      else
        tk_call('dde', 'servername', *args)
      end
    end
  end

  def execute(service, topic, data)
    tk_call('dde', 'execute', service, topic, data)
  end

  def async_execute(service, topic, data)
    tk_call('dde', '-async', 'execute', service, topic, data)
  end

  def poke(service, topic, item, data)
    tk_call('dde', 'poke', service, topic, item, data)
  end

  def request(service, topic, item)
    tk_call('dde', 'request', service, topic, item)
  end

  def binary_request(service, topic, item)
    tk_call('dde', 'request', '-binary', service, topic, item)
  end

  def services(service, topic)
    tk_call('dde', 'services', service, topic)
  end

  def eval(topic, cmd, *args)
    tk_call('dde', 'eval', topic, cmd, *args)
  end

  def async_eval(topic, cmd, *args)
    tk_call('dde', 'eval', -async, topic, cmd, *args)
  end

  module_function :servername, :execute, :async_execute,
                  :poke, :request, :services, :eval
end

module Tk::WinRegistry
end
#TkWinRegistry = Tk::WinRegistry
#Tk.__set_toplevel_aliases__(:Tk, Tk::WinRegistry, :TkWinRegistry)
Tk.__set_loaded_toplevel_aliases__('tk/winpkg.rb', :Tk, Tk::WinRegistry,
                                   :TkWinRegistry)

module Tk::WinRegistry
  extend Tk
  extend Tk::WinRegistry

  TkCommandNames = ['registry'.freeze].freeze

  if self.const_defined? :FORCE_VERSION
    tk_call('package', 'require', 'registry', FORCE_VERSION)
  else
    tk_call('package', 'require', 'registry')
  end

  def broadcast(keynam, timeout=nil)
    if timeout
      tk_call('registry', 'broadcast', keynam, '-timeout', timeout)
    else
      tk_call('registry', 'broadcast', keynam)
    end
  end

  def delete(keynam, valnam=None)
    tk_call('registry', 'delete', keynam, valnam)
  end

  def get(keynam, valnam)
    tk_call('registry', 'get', keynam, valnam)
  end

  def keys(keynam, pattern=nil)
    lst = tk_split_simplelist(tk_call('registry', 'keys', keynam))
    if pattern
      lst.find_all{|key| key =~ pattern}
    else
      lst
    end
  end

  def set(keynam, valnam=None, data=None, dattype=None)
    tk_call('registry', 'set', keynam, valnam, data, dattype)
  end

  def type(keynam, valnam)
    tk_call('registry', 'type', keynam, valnam)
  end

  def values(keynam, pattern=nil)
    lst = tk_split_simplelist(tk_call('registry', 'values', keynam))
    if pattern
      lst.find_all{|val| val =~ pattern}
    else
      lst
    end
  end

  module_function :delete, :get, :keys, :set, :type, :values
end
