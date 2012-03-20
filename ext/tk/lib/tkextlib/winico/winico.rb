#
#  tkextlib/winico/winico.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'

# call setup script for general 'tkextlib' libraries
require 'tkextlib/setup.rb'

# call setup script
require 'tkextlib/winico/setup.rb'

# TkPackage.require('winico', '0.5')
# TkPackage.require('winico', '0.6')
TkPackage.require('winico')

module Tk
  class Winico < TkObject
    PACKAGE_NAME = 'winico'.freeze
    def self.package_name
      PACKAGE_NAME
    end

    def self.package_version
      begin
        TkPackage.require('winico')
      rescue
        ''
      end
    end
  end
end

class Tk::Winico
  WinicoID_TBL = TkCore::INTERP.create_table

  TkCore::INTERP.init_ip_env{
    WinicoID_TBL.mutex.synchronize{ WinicoID_TBL.clear }
  }

  def self.id2obj(id)
    WinicoID_TBL.mutex.synchronize{
      (WinicoID_TBL.key?(id))? WinicoID_TBL[id] : id
    }
  end

  def self.info
    simplelist(Tk.tk_call('winico', 'info')).collect{|id|
      Tk::Winico.id2obj(id)
    }
  end

  def self.icon_info(id)
    simplelist(Tk.tk_call('winico', 'info', id)).collect{|inf|
      h = Hash[*list(inf)]
      h.keys.each{|k| h[k[1..-1]] = h.delete(k)}
    }
  end

  #################################

  def self.new_from_file(file_name)
    self.new(file_name)
  end

  def self.new_from_resource(resource_name, file_name = nil)
    self.new(file_name, resource_name)
  end

  def initialize(file_name, resource_name=nil, winico_id=nil)
    if resource_name
      # from resource
      if file_name
        @id = Tk.tk_call('winico', 'load', resource_name, file_name)
      else
        @id = Tk.tk_call('winico', 'load', resource_name)
      end
    elsif file_name
      # from .ico file
      @id = Tk.tk_call('winico', 'createfrom', file_name)
    elsif winico_id
      @id = winico_id
    else
      fail ArgumentError,
           "must be given proper information from where loading icons"
    end
    @path = @id
    WinicoID_TBL.mutex.synchronize{
      WinicoID_TBL[@id] = self
    }
  end

  def id
    @id
  end

  def set_window(win_id, *opts) # opts := ?'big'|'small'?, ?pos?
    # NOTE:: the window, which is denoted by win_id, MUST BE MAPPED.
    #        If not, then this may fail or crash.
    tk_call('winico', 'setwindow', win_id, @id, *opts)
  end

  def delete
    tk_call('winico', 'delete', @id)
    WinicoID_TBL.mutex.synchronize{
      WinicoID_TBL.delete(@id)
    }
    self
  end
  alias destroy delete

  def info
    Tk::Winico.icon_info(@id)
  end

  #################################

  class Winico_callback < TkValidateCommand
    class ValidateArgs < TkUtil::CallbackSubst
      KEY_TBL = [
        [ ?m, ?s, :message ],
        [ ?i, ?x, :icon ],
        [ ?x, ?n, :x ],
        [ ?y, ?n, :y ],
        [ ?X, ?n, :last_x ],
        [ ?Y, ?n, :last_y ],
        [ ?t, ?n, :tickcount ],
        [ ?w, ?n, :icon_idnum ],
        [ ?l, ?n, :msg_idnum ],
        nil
      ]

      PROC_TBL = [
        [ ?n, TkComm.method(:number) ],
        [ ?s, TkComm.method(:string) ],
        [ ?x, proc{|id|
            Tk::Winico::WinicoID_TBL.mutex.synchronize{
              if Tk::Winico::WinicoID_TBL.key?(id)
                obj = Tk::Winico::WinicoID_TBL[id]
              else
                # Tk::Winico.new(nil, nil, id)
                obj = Tk::Winico.allocate
                obj.instance_eval{ @path = @id = id }
                Tk::Winico::WinicoID_TBL[id] = obj
              end
              obj
            }
          } ],
        nil
      ]

=begin
      # for Ruby m17n :: ?x --> String --> char-code ( getbyte(0) )
      KEY_TBL.map!{|inf|
        if inf.kind_of?(Array)
          inf[0] = inf[0].getbyte(0) if inf[0].kind_of?(String)
          inf[1] = inf[1].getbyte(0) if inf[1].kind_of?(String)
        end
        inf
      }

      PROC_TBL.map!{|inf|
        if inf.kind_of?(Array)
          inf[0] = inf[0].getbyte(0) if inf[0].kind_of?(String)
        end
        inf
      }
=end

      _setup_subst_table(KEY_TBL, PROC_TBL);

      def self.ret_val(val)
        val
      end
    end

    def self._config_keys
      ['callback']
    end
  end

  #################################

  def add_to_taskbar(keys = {})
    keys = _symbolkey2str(keys)
    Winico_callback._config_keys.each{|k|
      if keys[k].kind_of?(Array)
        cmd, *args = keys[k]
        #keys[k] = Winico_callback.new(cmd, args.join(' '))
        keys[k] = Winico_callback.new(cmd, *args)
       # elsif keys[k].kind_of?(Proc)
      elsif TkComm._callback_entry?(keys[k])
        keys[k] = Winico_callback.new(keys[k])
      end
    }
    tk_call('winico', 'taskbar', 'add', @id, *(hash_kv(keys)))
    self
  end
  alias taskbar_add add_to_taskbar

  def modify_taskbar(keys = {})
    keys = _symbolkey2str(keys)
    Winico_callback._config_keys.each{|k|
      if keys[k].kind_of?(Array)
        cmd, *args = keys[k]
        #keys[k] = Winico_callback.new(cmd, args.join(' '))
        keys[k] = Winico_callback.new(cmd, *args)
      # elsif keys[k].kind_of?(Proc)
      elsif TkComm._callback_entry?(keys[k])
        keys[k] = Winico_callback.new(keys[k])
      end
    }
    tk_call('winico', 'taskbar', 'modify', @id, *(hash_kv(keys)))
    self
  end
  alias taskbar_modify modify_taskbar

  def delete_from_taskbar
    tk_call('winico', 'taskbar', 'delete', @id)
    self
  end
  alias taskbar_delete delete_from_taskbar
end
