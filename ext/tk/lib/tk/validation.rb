#
#  tk/validation.rb - validation support module for entry, spinbox, and so on
#
require 'tk'

module Tk
  module ValidateConfigure
    def self.__def_validcmd(scope, klass, keys=nil)
      keys = klass._config_keys unless keys
      keys.each{|key|
        eval("def #{key}(*args, &b)
                 __validcmd_call(#{klass.name}, '#{key}', *args, &b)
              end", scope)
      }
    end

    def __validcmd_call(klass, key, *args, &b)
      return cget(key) if args.empty? && !b

      cmd = (b)? proc(&b) : args.shift

      if cmd.kind_of?(klass)
        configure(key, cmd)
      elsif !args.empty?
        configure(key, [cmd, args])
      else
        configure(key, cmd)
      end
    end

    def __validation_class_list
      # maybe need to override
      []
    end

    def __get_validate_key2class
      k2c = {}
      __validation_class_list.each{|klass|
        klass._config_keys.each{|key|
          k2c[key.to_s] = klass
        }
      }
      k2c
    end

    def __conv_vcmd_on_hash_kv(keys)
      key2class = __get_validate_key2class

      keys = _symbolkey2str(keys)
      key2class.each{|key, klass|
        if keys[key].kind_of?(Array)
          cmd, *args = keys[key]
          #keys[key] = klass.new(cmd, args.join(' '))
          keys[key] = klass.new(cmd, *args)
        # elsif keys[key].kind_of?(Proc) ||  keys[key].kind_of?(Method)
        elsif TkComm._callback_entry?(keys[key])
          keys[key] = klass.new(keys[key])
        end
      }
      keys
    end

    def create_self(keys)
      super(__conv_vcmd_on_hash_kv(keys))
    end
    private :create_self

    def configure(slot, value=TkComm::None)
      if slot.kind_of?(Hash)
        super(__conv_vcmd_on_hash_kv(slot))
      else
        super(__conv_vcmd_on_hash_kv(slot=>value))
      end
      self
    end
=begin
    def configure(slot, value=TkComm::None)
      key2class = __get_validate_key2class

      if slot.kind_of?(Hash)
        slot = _symbolkey2str(slot)
        key2class.each{|key, klass|
          if slot[key].kind_of?(Array)
            cmd, *args = slot[key]
            slot[key] = klass.new(cmd, args.join(' '))
          elsif slot[key].kind_of?(Proc) || slot[key].kind_of?(Method)
            slot[key] = klass.new(slot[key])
          end
        }
        super(slot)

      else
        slot = slot.to_s
        if (klass = key2class[slot])
          if value.kind_of?(Array)
            cmd, *args = value
            value = klass.new(cmd, args.join(' '))
          elsif value.kind_of?(Proc) || value.kind_of?(Method)
            value = klass.new(value)
          end
        end
        super(slot, value)
      end

      self
    end
=end
  end

  module ItemValidateConfigure
    def self.__def_validcmd(scope, klass, keys=nil)
      keys = klass._config_keys unless keys
      keys.each{|key|
        eval("def item_#{key}(id, *args, &b)
                 __item_validcmd_call(#{klass.name}, '#{key}', id, *args, &b)
              end", scope)
      }
    end

    def __item_validcmd_call(tagOrId, klass, key, *args, &b)
      return itemcget(tagid(tagOrId), key) if args.empty? && !b

      cmd = (b)? proc(&b) : args.shift

      if cmd.kind_of?(klass)
        itemconfigure(tagid(tagOrId), key, cmd)
      elsif !args.empty?
        itemconfigure(tagid(tagOrId), key, [cmd, args])
      else
        itemconfigure(tagid(tagOrId), key, cmd)
      end
    end

    def __item_validation_class_list(id)
      # maybe need to override
      []
    end

    def __get_item_validate_key2class(id)
      k2c = {}
      __item_validation_class_list(id).each{|klass|
        klass._config_keys.each{|key|
          k2c[key.to_s] = klass
        }
      }
    end

    def __conv_item_vcmd_on_hash_kv(keys)
      key2class = __get_item_validate_key2class(tagid(tagOrId))

      keys = _symbolkey2str(keys)
      key2class.each{|key, klass|
        if keys[key].kind_of?(Array)
          cmd, *args = keys[key]
          #keys[key] = klass.new(cmd, args.join(' '))
          keys[key] = klass.new(cmd, *args)
        # elsif keys[key].kind_of?(Proc) || keys[key].kind_of?(Method)
        elsif TkComm._callback_entry?(keys[key])
          keys[key] = klass.new(keys[key])
        end
      }
      keys
    end

    def itemconfigure(tagOrId, slot, value=TkComm::None)
      if slot.kind_of?(Hash)
        super(__conv_item_vcmd_on_hash_kv(slot))
      else
        super(__conv_item_vcmd_on_hash_kv(slot=>value))
      end
      self
    end
=begin
    def itemconfigure(tagOrId, slot, value=TkComm::None)
      key2class = __get_item_validate_key2class(tagid(tagOrId))

      if slot.kind_of?(Hash)
        slot = _symbolkey2str(slot)
        key2class.each{|key, klass|
          if slot[key].kind_of?(Array)
            cmd, *args = slot[key]
            slot[key] = klass.new(cmd, args.join(' '))
          elsif slot[key].kind_of?(Proc) ||  slot[key].kind_of?(Method)
            slot[key] = klass.new(slot[key])
          end
        }
        super(slot)

      else
        slot = slot.to_s
        if (klass = key2class[slot])
          if value.kind_of?(Array)
            cmd, *args = value
            value = klass.new(cmd, args.join(' '))
          elsif value.kind_of?(Proc) || value.kind_of?(Method)
            value = klass.new(value)
          end
        end
        super(slot, value)
      end

      self
    end
=end
  end
end

class TkValidateCommand
  include TkComm
  extend  TkComm

  class ValidateArgs < TkUtil::CallbackSubst
    KEY_TBL = [
      [ ?d, ?n, :action ],
      [ ?i, ?x, :index ],
      [ ?s, ?e, :current ],
      [ ?v, ?s, :type ],
      [ ?P, ?e, :value ],
      [ ?S, ?e, :string ],
      [ ?V, ?s, :triggered ],
      [ ?W, ?w, :widget ],
      nil
    ]

    PROC_TBL = [
      [ ?n, TkComm.method(:number) ],
      [ ?s, TkComm.method(:string) ],
      [ ?w, TkComm.method(:window) ],

      [ ?e, proc{|val|
          #enc = Tk.encoding
          enc = ((Tk.encoding)? Tk.encoding : Tk.encoding_system)
          if enc
            Tk.fromUTF8(TkComm::string(val), enc)
          else
            TkComm::string(val)
          end
        }
      ],

      [ ?x, proc{|val|
          idx = TkComm::number(val)
          if idx < 0
            nil
          else
            idx
          end
        }
      ],

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

    #
    # NOTE: The order of parameters which passed to callback procedure is
    #        <extra_arg>, <extra_arg>, ... , <subst_arg>, <subst_arg>, ...
    #

    #def self._get_extra_args_tbl
    #  # return an array of convert procs
    #  []
    #end

    def self.ret_val(val)
      (val)? '1': '0'
    end
  end

  ###############################################

  def self._config_keys
    # array of config-option key (string or symbol)
    ['vcmd', 'validatecommand', 'invcmd', 'invalidcommand']
  end

  def _initialize_for_cb_class(klass, cmd = Proc.new, *args)
    extra_args_tbl = klass._get_extra_args_tbl

    if args.compact.size > 0
      args.map!{|arg| klass._sym2subst(arg)}
      args = args.join(' ')
      keys = klass._get_subst_key(args)
      if cmd.kind_of?(String)
        id = cmd
      elsif cmd.kind_of?(TkCallbackEntry)
        @id = install_cmd(cmd)
      else
        @id = install_cmd(proc{|*arg|
             ex_args = []
             extra_args_tbl.reverse_each{|conv| ex_args << conv.call(arg.pop)}
             klass.ret_val(cmd.call(
               *(ex_args.concat(klass.scan_args(keys, arg)))
             ))
        }) + ' ' + args
      end
    else
      keys, args = klass._get_all_subst_keys
      if cmd.kind_of?(String)
        id = cmd
      elsif cmd.kind_of?(TkCallbackEntry)
        @id = install_cmd(cmd)
      else
        @id = install_cmd(proc{|*arg|
             ex_args = []
             extra_args_tbl.reverse_each{|conv| ex_args << conv.call(arg.pop)}
             klass.ret_val(cmd.call(
               *(ex_args << klass.new(*klass.scan_args(keys, arg)))
             ))
        }) + ' ' + args
      end
    end
  end

  def initialize(cmd = Proc.new, *args)
    _initialize_for_cb_class(self.class::ValidateArgs, cmd, *args)
  end

  def to_eval
    @id
  end
end

module TkValidation
  include Tk::ValidateConfigure

  class ValidateCmd < TkValidateCommand
    module Action
      Insert = 1
      Delete = 0
      Others = -1
      Focus  = -1
      Forced = -1
      Textvariable = -1
      TextVariable = -1
    end
  end

  #####################################

  def __validation_class_list
    super() << ValidateCmd
  end

  Tk::ValidateConfigure.__def_validcmd(binding, ValidateCmd)

=begin
  def validatecommand(cmd = Proc.new, args = nil)
    if cmd.kind_of?(ValidateCmd)
      configure('validatecommand', cmd)
    elsif args
      configure('validatecommand', [cmd, args])
    else
      configure('validatecommand', cmd)
    end
  end
=end
#  def validatecommand(*args, &b)
#    __validcmd_call(ValidateCmd, 'validatecommand', *args, &b)
#  end
#  alias vcmd validatecommand

=begin
  def invalidcommand(cmd = Proc.new, args = nil)
    if cmd.kind_of?(ValidateCmd)
      configure('invalidcommand', cmd)
    elsif args
      configure('invalidcommand', [cmd, args])
    else
      configure('invalidcommand', cmd)
    end
  end
=end
#  def invalidcommand(*args, &b)
#    __validcmd_call(ValidateCmd, 'invalidcommand', *args, &b)
#  end
#  alias invcmd invalidcommand
end
