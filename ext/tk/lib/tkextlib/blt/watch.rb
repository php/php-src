#
#  tkextlib/blt/watch.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/blt.rb'

module Tk::BLT
  class Watch < TkObject
    extend TkCore

    TkCommandNames = ['::blt::watch'.freeze].freeze

    WATCH_ID_TBL = TkCore::INTERP.create_table

    (BLT_WATCH_ID = ['blt_watch_id'.freeze, TkUtil.untrust('00000')]).instance_eval{
      @mutex = Mutex.new
      def mutex; @mutex; end
      freeze
    }

    TkCore::INTERP.init_ip_env{
      WATCH_ID_TBL.mutex.synchronize{ WATCH_ID_TBL.clear }
    }

    def self.names(state = None)
      lst = tk_split_list(tk_call('::blt::watch', 'names', state))
      WATCH_ID_TBL.mutex.synchronize{
        lst.collect{|name|
          WATCH_ID_TBL[name] || name
        }
      }
    end

    def __numval_optkeys
      ['maxlevel']
    end
    private :__numval_optkeys

    def __boolval_optkeys
      ['active']
    end
    private :__boolval_optkeys

    def __config_cmd
      ['::blt::watch', 'configure', self.path]
    end
    private :__config_cmd

    def initialize(name = nil, keys = {})
      if name.kind_of?(Hash)
        keys = name
        name = nil
      end

      if name
        @id = name.to_s
      else
        BLT_WATCH_ID.mutex.synchronize{
          @id = BLT_WATCH_ID.join(TkCore::INTERP._ip_id_)
          BLT_WATCH_ID[1].succ!
        }
      end

      @path = @id

      WATCH_ID_TBL.mutex.synchronize{
        WATCH_ID_TBL[@id] = self
      }
      tk_call('::blt::watch', 'create', @id, *hash_kv(keys))
    end

    def activate
      tk_call('::blt::watch', 'activate', @id)
      self
    end
    def deactivate
      tk_call('::blt::watch', 'deactivate', @id)
      self
    end
    def delete
      tk_call('::blt::watch', 'delete', @id)
      self
    end
    def info
      ret = []
      lst = tk_split_simplelist(tk_call('::blt::watch', 'info', @id))
      until lst.empty?
        k, v, *lst = lst
        k = k[1..-1]
        case k
        when /^(#{__strval_optkeys.join('|')})$/
          # do nothing

        when /^(#{__numval_optkeys.join('|')})$/
          begin
            v = number(v)
          rescue
            v = nil
          end

        when /^(#{__numstrval_optkeys.join('|')})$/
          v = num_or_str(v)

        when /^(#{__boolval_optkeys.join('|')})$/
          begin
            v = bool(v)
          rescue
            v = nil
          end

        when /^(#{__listval_optkeys.join('|')})$/
          v = simplelist(v)

        when /^(#{__numlistval_optkeys.join('|')})$/
          v = list(v)

        else
          if v.index('{')
            v = tk_split_list(v)
          else
            v = tk_tcl2ruby(v)
          end
        end

        ret << [k, v]
      end

      ret
    end
    def configinfo(slot = nil)
      if slot
        slot = slot.to_s
        v = cget(slot)
        if TkComm::GET_CONFIGINFO_AS_ARRAY
          [slot, v]
        else
          {slot=>v}
        end
      else
        if TkComm::GET_CONFIGINFO_AS_ARRAY
          info
        else
          Hash[*(info.flatten)]
        end
      end
    end
    def cget_strict(key)
      key = key.to_s
      begin
        info.assoc(key)[1]
      rescue
        fail ArgumentError, "unknown option '#{key}'"
      end
    end
    def cget(key)
      unless TkConfigMethod.__IGNORE_UNKNOWN_CONFIGURE_OPTION__
        cget_strict(key)
      else
        begin
          cget_strict(key)
        rescue => e
          if current_configinfo.has_key?(key.to_s)
            # error on known option
            fail e
          else
            # unknown option
            nil
          end
        end
      end
    end
  end
end
