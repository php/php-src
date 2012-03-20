#
# tk/itemconfig.rb : control item/tag configuration of widget
#
require 'tk'
require 'tkutil'
require 'tk/itemfont.rb'

module TkItemConfigOptkeys
  include TkUtil

  def __item_optkey_aliases(id)
    {}
  end
  private :__item_optkey_aliases

  def __item_numval_optkeys(id)
    []
  end
  private :__item_numval_optkeys

  def __item_numstrval_optkeys(id)
    []
  end
  private :__item_numstrval_optkeys

  def __item_boolval_optkeys(id)
    ['exportselection', 'jump', 'setgrid', 'takefocus']
  end
  private :__item_boolval_optkeys

  def __item_strval_optkeys(id)
    # maybe need to override
    [
      'text', 'label', 'show', 'data', 'file', 'maskdata', 'maskfile',
      'activebackground', 'activeforeground', 'background',
      'disabledforeground', 'disabledbackground', 'foreground',
      'highlightbackground', 'highlightcolor', 'insertbackground',
      'selectbackground', 'selectforeground', 'troughcolor'
    ]
  end
  private :__item_strval_optkeys

  def __item_listval_optkeys(id)
    []
  end
  private :__item_listval_optkeys

  def __item_numlistval_optkeys(id)
    # maybe need to override
    ['dash', 'activedash', 'disableddash']
  end
  private :__item_numlistval_optkeys

  def __item_tkvariable_optkeys(id)
    ['variable', 'textvariable']
  end
  private :__item_tkvariable_optkeys

  def __item_val2ruby_optkeys(id)  # { key=>method, ... }
    # The method is used to convert a opt-value to a ruby's object.
    # When get the value of the option "key", "method.call(id, val)" is called.
    {}
  end
  private :__item_val2ruby_optkeys

  def __item_ruby2val_optkeys(id)  # { key=>method, ... }
    # The method is used to convert a ruby's object to a opt-value.
    # When set the value of the option "key", "method.call(id, val)" is called.
    # That is, "-#{key} #{method.call(id, value)}".
    {}
  end
  private :__item_ruby2val_optkeys

  def __item_methodcall_optkeys(id)  # { key=>method, ... }
    # Use the method for both of get and set.
    # Usually, the 'key' will not be a widget option.
    #
    # maybe need to override
    # {'coords'=>'coords'}
    {}
  end
  private :__item_methodcall_optkeys

  ################################################

  def __item_keyonly_optkeys(id)  # { def_key=>(undef_key|nil), ... }
    # maybe need to override
    {}
  end
  private :__item_keyonly_optkeys


  def __conv_item_keyonly_opts(id, keys)
    return keys unless keys.kind_of?(Hash)
    keyonly = __item_keyonly_optkeys(id)
    keys2 = {}
    keys.each{|k, v|
      optkey = keyonly.find{|kk,vv| kk.to_s == k.to_s}
      if optkey
        defkey, undefkey = optkey
        if v
          keys2[defkey.to_s] = None
        else
          keys2[undefkey.to_s] = None
        end
      else
        keys2[k.to_s] = v
      end
    }
    keys2
  end

  def itemconfig_hash_kv(id, keys, enc_mode = nil, conf = nil)
    hash_kv(__conv_item_keyonly_opts(id, keys), enc_mode, conf)
  end
end

module TkItemConfigMethod
  include TkUtil
  include TkTreatItemFont
  include TkItemConfigOptkeys

  def TkItemConfigMethod.__IGNORE_UNKNOWN_CONFIGURE_OPTION__
    @mode || false
  end
  def TkItemConfigMethod.__set_IGNORE_UNKNOWN_CONFIGURE_OPTION__!(mode)
    fail SecurityError, "can't change the mode" if $SAFE>=4
    @mode = (mode)? true: false
  end

  def __item_cget_cmd(id)
    # maybe need to override
    [self.path, 'itemcget', id]
  end
  private :__item_cget_cmd

  def __item_config_cmd(id)
    # maybe need to override
    [self.path, 'itemconfigure', id]
  end
  private :__item_config_cmd

  def __item_confinfo_cmd(id)
    # maybe need to override
    __item_config_cmd(id)
  end
  private :__item_confinfo_cmd

  def __item_configinfo_struct(id)
    # maybe need to override
    {:key=>0, :alias=>1, :db_name=>1, :db_class=>2,
      :default_value=>3, :current_value=>4}
  end
  private :__item_configinfo_struct

  ################################################

  def tagid(tagOrId)
    # maybe need to override
    tagOrId
  end

  ################################################


  def itemcget_tkstring(tagOrId, option)
    opt = option.to_s
    fail ArgumentError, "Invalid option `#{option.inspect}'" if opt.length == 0
    tk_call_without_enc(*(__item_cget_cmd(tagid(tagOrId)) << "-#{opt}"))
  end

  def __itemcget_core(tagOrId, option)
    orig_opt = option
    option = option.to_s

    if option.length == 0
      fail ArgumentError, "Invalid option `#{orig_opt.inspect}'"
    end

    alias_name, real_name = __item_optkey_aliases(tagid(tagOrId)).find{|k, v| k.to_s == option}
    if real_name
      option = real_name.to_s
    end

    if ( method = _symbolkey2str(__item_val2ruby_optkeys(tagid(tagOrId)))[option] )
      optval = tk_call_without_enc(*(__item_cget_cmd(tagid(tagOrId)) << "-#{option}"))
      begin
        return method.call(tagOrId, optval)
      rescue => e
        warn("Warning:: #{e.message} (when #{method}.call(#{tagOrId.inspect}, #{optval.inspect})") if $DEBUG
        return optval
      end
    end

    if ( method = _symbolkey2str(__item_methodcall_optkeys(tagid(tagOrId)))[option] )
      return self.__send__(method, tagOrId)
    end

    case option
    when /^(#{__item_numval_optkeys(tagid(tagOrId)).join('|')})$/
      begin
        number(tk_call_without_enc(*(__item_cget_cmd(tagid(tagOrId)) << "-#{option}")))
      rescue
        nil
      end

    when /^(#{__item_numstrval_optkeys(tagid(tagOrId)).join('|')})$/
      num_or_str(tk_call_without_enc(*(__item_cget_cmd(tagid(tagOrId)) << "-#{option}")))

    when /^(#{__item_boolval_optkeys(tagid(tagOrId)).join('|')})$/
      begin
        bool(tk_call_without_enc(*(__item_cget_cmd(tagid(tagOrId)) << "-#{option}")))
      rescue
        nil
      end

    when /^(#{__item_listval_optkeys(tagid(tagOrId)).join('|')})$/
      simplelist(tk_call_without_enc(*(__item_cget_cmd(tagid(tagOrId)) << "-#{option}")))

    when /^(#{__item_numlistval_optkeys(tagid(tagOrId)).join('|')})$/
      conf = tk_call_without_enc(*(__item_cget_cmd(tagid(tagOrId)) << "-#{option}"))
      if conf =~ /^[0-9]/
        list(conf)
      else
        conf
      end

    when /^(#{__item_tkvariable_optkeys(tagid(tagOrId)).join('|')})$/
      v = tk_call_without_enc(*(__item_cget_cmd(tagid(tagOrId)) << "-#{option}"))
      (v.empty?)? nil: TkVarAccess.new(v)

    when /^(#{__item_strval_optkeys(tagid(tagOrId)).join('|')})$/
      _fromUTF8(tk_call_without_enc(*(__item_cget_cmd(tagid(tagOrId)) << "-#{option}")))

    when /^(|latin|ascii|kanji)(#{__item_font_optkeys(tagid(tagOrId)).join('|')})$/
      fontcode = $1
      fontkey  = $2
      fnt = tk_tcl2ruby(tk_call_without_enc(*(__item_cget_cmd(tagid(tagOrId)) << "-#{fontkey}")), true)
      unless fnt.kind_of?(TkFont)
        fnt = tagfontobj(tagid(tagOrId), fontkey)
      end
      if fontcode == 'kanji' && JAPANIZED_TK && TK_VERSION =~ /^4\.*/
        # obsolete; just for compatibility
        fnt.kanji_font
      else
        fnt
      end
    else
      tk_tcl2ruby(tk_call_without_enc(*(__item_cget_cmd(tagid(tagOrId)) << "-#{option}")), true)
    end
  end
  private :__itemcget_core

  def itemcget(tagOrId, option)
    unless TkItemConfigMethod.__IGNORE_UNKNOWN_CONFIGURE_OPTION__
      __itemcget_core(tagOrId, option)
    else
      begin
        __itemcget_core(tagOrId, option)
      rescue => e
        begin
          if __current_itemconfiginfo(tagOrId).has_key?(option.to_s)
            # not tag error & option is known -> error on known option
            fail e
          else
            # not tag error & option is unknown
            nil
          end
        rescue
          fail e  # tag error
        end
      end
    end
  end
  def itemcget_strict(tagOrId, option)
    # never use TkItemConfigMethod.__IGNORE_UNKNOWN_CONFIGURE_OPTION__
    __itemcget_core(tagOrId, option)
  end

  def __itemconfigure_core(tagOrId, slot, value=None)
    if slot.kind_of? Hash
      slot = _symbolkey2str(slot)

      __item_optkey_aliases(tagid(tagOrId)).each{|alias_name, real_name|
        alias_name = alias_name.to_s
        if slot.has_key?(alias_name)
          slot[real_name.to_s] = slot.delete(alias_name)
        end
      }

      __item_methodcall_optkeys(tagid(tagOrId)).each{|key, method|
        value = slot.delete(key.to_s)
        self.__send__(method, tagOrId, value) if value
      }

      __item_ruby2val_optkeys(tagid(tagOrId)).each{|key, method|
        key = key.to_s
        slot[key] = method.call(tagOrId, slot[key]) if slot.has_key?(key)
      }

      __item_keyonly_optkeys(tagid(tagOrId)).each{|defkey, undefkey|
        conf = slot.find{|kk, vv| kk == defkey.to_s}
        if conf
          k, v = conf
          if v
            slot[k] = None
          else
            slot[undefkey.to_s] = None if undefkey
            slot.delete(k)
          end
        end
      }

      if (slot.find{|k, v| k =~ /^(|latin|ascii|kanji)(#{__item_font_optkeys(tagid(tagOrId)).join('|')})$/})
        tagfont_configure(tagid(tagOrId), slot)
      elsif slot.size > 0
        tk_call(*(__item_config_cmd(tagid(tagOrId)).concat(hash_kv(slot))))
      end

    else
      orig_slot = slot
      slot = slot.to_s
      if slot.length == 0
        fail ArgumentError, "Invalid option `#{orig_slot.inspect}'"
      end

      alias_name, real_name = __item_optkey_aliases(tagid(tagOrId)).find{|k, v| k.to_s == slot}
      if real_name
        slot = real_name.to_s
      end

      if ( conf = __item_keyonly_optkeys(tagid(tagOrId)).find{|k, v| k.to_s == slot } )
        defkey, undefkey = conf
        if value
          tk_call(*(__item_config_cmd(tagid(tagOrId)) << "-#{defkey}"))
        elsif undefkey
          tk_call(*(__item_config_cmd(tagid(tagOrId)) << "-#{undefkey}"))
        end
      elsif ( method = _symbolkey2str(__item_ruby2val_optkeys(tagid(tagOrId)))[slot] )
        tk_call(*(__item_config_cmd(tagid(tagOrId)) << "-#{slot}" << method.call(tagOrId, value)))
      elsif ( method = _symbolkey2str(__item_methodcall_optkeys(tagid(tagOrId)))[slot] )
        self.__send__(method, tagOrId, value)
      elsif (slot =~ /^(|latin|ascii|kanji)(#{__item_font_optkeys(tagid(tagOrId)).join('|')})$/)
        if value == None
          tagfontobj(tagid(tagOrId), $2)
        else
          tagfont_configure(tagid(tagOrId), {slot=>value})
        end
      else
        tk_call(*(__item_config_cmd(tagid(tagOrId)) << "-#{slot}" << value))
      end
    end
    self
  end
  private :__itemconfigure_core

  def __check_available_itemconfigure_options(tagOrId, keys)
    id = tagid(tagOrId)

    availables = self.__current_itemconfiginfo(id).keys

    # add non-standard keys
    availables |= __font_optkeys.map{|k|
      [k.to_s, "latin#{k}", "ascii#{k}", "kanji#{k}"]
    }.flatten
    availables |= __item_methodcall_optkeys(id).keys.map{|k| k.to_s}
    availables |= __item_keyonly_optkeys(id).keys.map{|k| k.to_s}

    keys = _symbolkey2str(keys)

    keys.delete_if{|k, v| !(availables.include?(k))}
  end

  def itemconfigure(tagOrId, slot, value=None)
    unless TkItemConfigMethod.__IGNORE_UNKNOWN_CONFIGURE_OPTION__
      __itemconfigure_core(tagOrId, slot, value)
    else
      if slot.kind_of?(Hash)
        begin
          __itemconfigure_core(tagOrId, slot)
        rescue
          slot = __check_available_itemconfigure_options(tagOrId, slot)
          __itemconfigure_core(tagOrId, slot) unless slot.empty?
        end
      else
        begin
          __itemconfigure_core(tagOrId, slot, value)
        rescue => e
          begin
            if __current_itemconfiginfo(tagOrId).has_key?(slot.to_s)
              # not tag error & option is known -> error on known option
              fail e
            else
              # not tag error & option is unknown
              nil
            end
          rescue
            fail e  # tag error
          end
        end
      end
    end
    self
  end

  def __itemconfiginfo_core(tagOrId, slot = nil)
    if TkComm::GET_CONFIGINFO_AS_ARRAY
      if (slot && slot.to_s =~ /^(|latin|ascii|kanji)(#{__item_font_optkeys(tagid(tagOrId)).join('|')})$/)
        fontkey  = $2
        # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{fontkey}"))))
        conf = tk_split_simplelist(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{fontkey}")), false, true)
        conf[__item_configinfo_struct(tagid(tagOrId))[:key]] =
          conf[__item_configinfo_struct(tagid(tagOrId))[:key]][1..-1]
        if ( ! __item_configinfo_struct(tagid(tagOrId))[:alias] \
            || conf.size > __item_configinfo_struct(tagid(tagOrId))[:alias] + 1 )
          fnt = conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]]
          if TkFont.is_system_font?(fnt)
            conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] = TkNamedFont.new(fnt)
          end
          conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] = tagfontobj(tagid(tagOrId), fontkey)
        elsif ( __item_configinfo_struct(tagid(tagOrId))[:alias] \
               && conf.size == __item_configinfo_struct(tagid(tagOrId))[:alias] + 1 \
               && conf[__item_configinfo_struct(tagid(tagOrId))[:alias]][0] == ?- )
          conf[__item_configinfo_struct(tagid(tagOrId))[:alias]] =
            conf[__item_configinfo_struct(tagid(tagOrId))[:alias]][1..-1]
        end
        conf
      else
        if slot
          slot = slot.to_s

          alias_name, real_name = __item_optkey_aliases(tagid(tagOrId)).find{|k, v| k.to_s == slot}
          if real_name
            slot = real_name.to_s
          end

          case slot
          when /^(#{__item_val2ruby_optkeys(tagid(tagOrId)).keys.join('|')})$/
            method = _symbolkey2str(__item_val2ruby_optkeys(tagid(tagOrId)))[slot]
            conf = tk_split_simplelist(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}")), false, true)
            if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] )
              optval = conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]]
              begin
                val = method.call(tagOrId, optval)
              rescue => e
                warn("Warning:: #{e.message} (when #{method}.call(#{tagOrId.inspect}, #{optval.inspect})") if $DEBUG
                val = optval
              end
              conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] = val
            end
            if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] )
              optval = conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]]
              begin
                val = method.call(tagOrId, optval)
              rescue => e
                warn("Warning:: #{e.message} (when #{method}lcall(#{tagOrId.inspect}, #{optval.inspect})") if $DEBUG
                val = optval
              end
              conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] = val
            end

          when /^(#{__item_methodcall_optkeys(tagid(tagOrId)).keys.join('|')})$/
            method = _symbolkey2str(__item_methodcall_optkeys(tagid(tagOrId)))[slot]
            return [slot, '', '', '', self.__send__(method, tagOrId)]

          when /^(#{__item_numval_optkeys(tagid(tagOrId)).join('|')})$/
            # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}"))))
            conf = tk_split_simplelist(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}")), false, true)

            if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] )
              begin
                conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =
                  number(conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]])
              rescue
                conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] = nil
              end
            end
            if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] )
              begin
                conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =
                  number(conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]])
              rescue
                conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] = nil
              end
            end

          when /^(#{__item_numstrval_optkeys(tagid(tagOrId)).join('|')})$/
            # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}"))))
            conf = tk_split_simplelist(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}")), false, true)

            if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] )
              conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =
                num_or_str(conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]])
            end
            if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] )
              conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =
                num_or_str(conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]])
            end

          when /^(#{__item_boolval_optkeys(tagid(tagOrId)).join('|')})$/
            # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}"))))
            conf = tk_split_simplelist(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}")), false, true)

            if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] )
              begin
                conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =
                  bool(conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]])
              rescue
                conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] = nil
              end
            end
            if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] )
              begin
                conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =
                  bool(conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]])
              rescue
                conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] = nil
              end
            end

          when /^(#{__item_listval_optkeys(tagid(tagOrId)).join('|')})$/
            # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}"))))
            conf = tk_split_simplelist(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}")), false, true)

            if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] )
              conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =
                simplelist(conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]])
            end
            if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] )
              conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =
                simplelist(conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]])
            end

          when /^(#{__item_numlistval_optkeys(tagid(tagOrId)).join('|')})$/
            # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}"))))
            conf = tk_split_simplelist(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}")), false, true)

            if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] \
                && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =~ /^[0-9]/ )
              conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =
                list(conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]])
            end
            if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] \
                && conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =~ /^[0-9]/ )
              conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =
                list(conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]])
            end

          when /^(#{__item_strval_optkeys(tagid(tagOrId)).join('|')})$/
            # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}"))))
            conf = tk_split_simplelist(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}")), false, true)

          when /^(#{__item_tkvariable_optkeys(tagid(tagOrId)).join('|')})$/
            conf = tk_split_simplelist(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}")), false, true)

            if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] )
              v = conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]]
              if v.empty?
                conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] = nil
              else
                conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] = TkVarAccess.new(v)
              end
            end
            if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] )
              v = conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]]
              if v.empty?
                conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] = nil
              else
                conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] = TkVarAccess.new(v)
              end
            end

          else
            # conf = tk_split_list(_fromUTF8(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}"))))
            conf = tk_split_list(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}")), 0, false, true)
          end
          conf[__item_configinfo_struct(tagid(tagOrId))[:key]] =
            conf[__item_configinfo_struct(tagid(tagOrId))[:key]][1..-1]

          if ( __item_configinfo_struct(tagid(tagOrId))[:alias] \
              && conf.size == __item_configinfo_struct(tagid(tagOrId))[:alias] + 1 \
              && conf[__item_configinfo_struct(tagid(tagOrId))[:alias]][0] == ?- )
            conf[__item_configinfo_struct(tagid(tagOrId))[:alias]] =
              conf[__item_configinfo_struct(tagid(tagOrId))[:alias]][1..-1]
          end

          conf

        else
          # ret = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)))))).collect{|conflist|
          #   conf = tk_split_simplelist(conflist)
          ret = tk_split_simplelist(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)))), false, false).collect{|conflist|
            conf = tk_split_simplelist(conflist, false, true)
            conf[__item_configinfo_struct(tagid(tagOrId))[:key]] =
              conf[__item_configinfo_struct(tagid(tagOrId))[:key]][1..-1]

            optkey = conf[__item_configinfo_struct(tagid(tagOrId))[:key]]
            case optkey
            when /^(#{__item_val2ruby_optkeys(tagid(tagOrId)).keys.join('|')})$/
              method = _symbolkey2str(__item_val2ruby_optkeys(tagid(tagOrId)))[optkey]
              if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                  && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] )
                optval = conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]]
                begin
                  val = method(tagOrId, optval)
                rescue => e
                  warn("Warning:: #{e.message} (when #{method}.call(#{tagOrId.inspect}, #{optval.inspect})") if $DEBUG
                  val = optval
                end
                conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] = val
              end
              if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] )
                optval = conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]]
                begin
                  val = method.call(tagOrId, optval)
                rescue => e
                  warn("Warning:: #{e.message} (when #{method}.call(#{tagOrId.inspect}, #{optval.inspect})") if $DEBUG
                  val = optval
                end
                conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] = val
              end

            when /^(#{__item_strval_optkeys(tagid(tagOrId)).join('|')})$/
              # do nothing

            when /^(#{__item_numval_optkeys(tagid(tagOrId)).join('|')})$/
              if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                  && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] )
                begin
                  conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =
                    number(conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]])
                rescue
                  conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] = nil
                end
              end
              if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] )
                begin
                  conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =
                    number(conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]])
                rescue
                  conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] = nil
                end
              end

            when /^(#{__item_numstrval_optkeys(tagid(tagOrId)).join('|')})$/
              if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                  && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] )
                conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =
                  num_or_str(conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]])
              end
              if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] )
                conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =
                  num_or_str(conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]])
              end

            when /^(#{__item_boolval_optkeys(tagid(tagOrId)).join('|')})$/
              if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                  && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] )
                begin
                  conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =
                    bool(conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]])
                rescue
                  conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] = nil
                end
              end
              if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] )
                begin
                  conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =
                    bool(conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]])
                rescue
                  conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] = nil
                end
              end

            when /^(#{__item_listval_optkeys(tagid(tagOrId)).join('|')})$/
              if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                  && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] )
                conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =
                  simplelist(conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]])
              end
              if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] )
                conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =
                  simplelist(conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]])
              end

            when /^(#{__item_numlistval_optkeys(tagid(tagOrId)).join('|')})$/
              if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                  && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] \
                  && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =~ /^[0-9]/ )
                conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =
                  list(conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]])
              end
              if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] \
                  && conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =~ /^[0-9]/ )
                conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =
                  list(conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]])
              end

            when /^(#{__item_tkvariable_optkeys(tagid(tagOrId)).join('|')})$/
              if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                  && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] )
                v = conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]]
                if v.empty?
                  conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] = nil
                else
                  conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] = TkVarAccess.new(v)
                end
              end
              if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] )
                v = conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]]
                if v.empty?
                  conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] = nil
                else
                  conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] = TkVarAccess.new(v)
                end
              end

            else
              if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                  && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] )
                if conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]].index('{')
                  conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =
                    tk_split_list(conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]])
                else
                  conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =
                    tk_tcl2ruby(conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]])
                end
              end
              if conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]]
                if conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]].index('{')
                  conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =
                    tk_split_list(conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]])
                else
                  conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =
                    tk_tcl2ruby(conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]])
                end
              end
            end

            if ( __item_configinfo_struct(tagid(tagOrId))[:alias] \
                && conf.size == __item_configinfo_struct(tagid(tagOrId))[:alias] + 1 \
                && conf[__item_configinfo_struct(tagid(tagOrId))[:alias]][0] == ?- )
              conf[__item_configinfo_struct(tagid(tagOrId))[:alias]] =
                conf[__item_configinfo_struct(tagid(tagOrId))[:alias]][1..-1]
            end

            conf
          }

          __item_font_optkeys(tagid(tagOrId)).each{|optkey|
            optkey = optkey.to_s
            fontconf = ret.assoc(optkey)
            if fontconf && fontconf.size > 2
              ret.delete_if{|inf| inf[0] =~ /^(|latin|ascii|kanji)#{optkey}$/}
              fnt = fontconf[__item_configinfo_struct(tagid(tagOrId))[:default_value]]
              if TkFont.is_system_font?(fnt)
                fontconf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] = TkNamedFont.new(fnt)
              end
              fontconf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] = tagfontobj(tagid(tagOrId), optkey)
              ret.push(fontconf)
            end
          }

          __item_methodcall_optkeys(tagid(tagOrId)).each{|optkey, method|
            ret << [optkey.to_s, '', '', '', self.__send__(method, tagOrId)]
          }

          ret
        end
      end

    else # ! TkComm::GET_CONFIGINFO_AS_ARRAY
      if (slot && slot.to_s =~ /^(|latin|ascii|kanji)(#{__item_font_optkeys(tagid(tagOrId)).join('|')})$/)
        fontkey  = $2
        # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{fontkey}"))))
        conf = tk_split_simplelist(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{fontkey}")), false, true)
        conf[__item_configinfo_struct(tagid(tagOrId))[:key]] =
          conf[__item_configinfo_struct(tagid(tagOrId))[:key]][1..-1]

        if ( ! __item_configinfo_struct(tagid(tagOrId))[:alias] \
            || conf.size > __item_configinfo_struct(tagid(tagOrId))[:alias] + 1 )
          fnt = conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]]
          if TkFont.is_system_font?(fnt)
            conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] = TkNamedFont.new(fnt)
          end
          conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] = tagfontobj(tagid(tagOrId), fontkey)
          { conf.shift => conf }
        elsif ( __item_configinfo_struct(tagid(tagOrId))[:alias] \
               && conf.size == __item_configinfo_struct(tagid(tagOrId))[:alias] + 1 )
          if conf[__item_configinfo_struct(tagid(tagOrId))[:alias]][0] == ?-
            conf[__item_configinfo_struct(tagid(tagOrId))[:alias]] =
              conf[__item_configinfo_struct(tagid(tagOrId))[:alias]][1..-1]
          end
          { conf[0] => conf[1] }
        else
          { conf.shift => conf }
        end
      else
        if slot
          slot = slot.to_s

          alias_name, real_name = __item_optkey_aliases(tagid(tagOrId)).find{|k, v| k.to_s == slot}
          if real_name
            slot = real_name.to_s
          end

          case slot
          when /^(#{__item_val2ruby_optkeys(tagid(tagOrId)).keys.join('|')})$/
            method = _symbolkey2str(__item_val2ruby_optkeys(tagid(tagOrId)))[slot]
            conf = tk_split_simplelist(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}")), false, true)
            if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] )
              optval = conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]]
              begin
                val = method.call(tagOrId, optval)
              rescue => e
                warn("Warning:: #{e.message} (when #{method}.call(#{tagOrId.inspect}, #{optval.inspect})") if $DEBUG
                val = optval
              end
              conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] = val
            end
            if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] )
              optval = conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]]
              begin
                val = method.call(tagOrId, optval)
              rescue => e
                warn("Warning:: #{e.message} (when #{method}.call(#{tagOrId.inspect}, #{optval.inspect})") if $DEBUG
                val = optval
              end
              conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] = val
            end

          when /^(#{__item_methodcall_optkeys(tagid(tagOrId)).keys.join('|')})$/
            method = _symbolkey2str(__item_methodcall_optkeys(tagid(tagOrId)))[slot]
            return {slot => ['', '', '', self.__send__(method, tagOrId)]}

          when /^(#{__item_numval_optkeys(tagid(tagOrId)).join('|')})$/
            # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}"))))
            conf = tk_split_simplelist(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}")), false, true)

            if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] )
              begin
                conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =
                  number(conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]])
              rescue
                conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] = nil
              end
            end
            if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] )
              begin
                conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =
                  number(conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]])
              rescue
                conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] = nil
              end
            end

          when /^(#{__item_numstrval_optkeys(tagid(tagOrId)).join('|')})$/
            # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}"))))
            conf = tk_split_simplelist(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}")), false, true)

            if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] )
              conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =
                num_or_stre(conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]])
            end
            if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] )
              conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =
                num_or_str(conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]])
            end

          when /^(#{__item_boolval_optkeys(tagid(tagOrId)).join('|')})$/
            # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}"))))
            conf = tk_split_simplelist(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}")), false, true)

            if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] )
              begin
                conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =
                  bool(conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]])
              rescue
                conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] = nil
              end
            end
            if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] )
              begin
                conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =
                  bool(conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]])
              rescue
                conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] = nil
              end
            end

          when /^(#{__item_listval_optkeys(tagid(tagOrId)).join('|')})$/
            # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}"))))
            conf = tk_split_simplelist(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}")), false, true)

            if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] )
              conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =
                simplelist(conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]])
            end
            if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] )
              conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =
                simplelist(conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]])
            end

          when /^(#{__item_numlistval_optkeys(tagid(tagOrId)).join('|')})$/
            # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}"))))
            conf = tk_split_simplelist(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}")), false, true)

            if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] \
                && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =~ /^[0-9]/ )
              conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =
                list(conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]])
            end
            if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] \
                && conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =~ /^[0-9]/ )
              conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =
                list(conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]])
            end

          when /^(#{__item_strval_optkeys(tagid(tagOrId)).join('|')})$/
            # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}"))))
            conf = tk_split_simplelist(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}")), false, true)

          when /^(#{__item_tkvariable_optkeys(tagid(tagOrId)).join('|')})$/
            conf = tk_split_simplelist(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}")), false, true)

            if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] )
              v = conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]]
              if v.empty?
                conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] = nil
              else
                conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] = TkVarAccess.new(v)
              end
            end
            if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] )
              v = conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]]
              if v.empty?
                conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] = nil
              else
                conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] = TkVarAccess.new(v)
              end
            end

          else
            # conf = tk_split_list(_fromUTF8(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}"))))
            conf = tk_split_list(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)) << "-#{slot}")), 0, false, true)
          end
          conf[__item_configinfo_struct(tagid(tagOrId))[:key]] =
            conf[__item_configinfo_struct(tagid(tagOrId))[:key]][1..-1]

          if ( __item_configinfo_struct(tagid(tagOrId))[:alias] \
              && conf.size == __item_configinfo_struct(tagid(tagOrId))[:alias] + 1 )
            if conf[__item_configinfo_struct(tagid(tagOrId))[:alias]][0] == ?-
              conf[__item_configinfo_struct(tagid(tagOrId))[:alias]] =
                conf[__item_configinfo_struct(tagid(tagOrId))[:alias]][1..-1]
            end
            { conf[0] => conf[1] }
          else
            { conf.shift => conf }
          end

        else
          ret = {}
          # tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)))))).each{|conflist|
          #   conf = tk_split_simplelist(conflist)
          tk_split_simplelist(tk_call_without_enc(*(__item_confinfo_cmd(tagid(tagOrId)))), false, false).each{|conflist|
            conf = tk_split_simplelist(conflist, false, true)
            conf[__item_configinfo_struct(tagid(tagOrId))[:key]] =
              conf[__item_configinfo_struct(tagid(tagOrId))[:key]][1..-1]

            optkey = conf[__item_configinfo_struct(tagid(tagOrId))[:key]]
            case optkey
            when /^(#{__item_val2ruby_optkeys(tagid(tagOrId)).keys.join('|')})$/
              method = _symbolkey2str(__item_val2ruby_optkeys(tagid(tagOrId)))[optkey]
              if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                  && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] )
                optval = conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]]
                begin
                  val = method.call(tagOrId, optval)
                rescue => e
                  warn("Warning:: #{e.message} (when #{method}.call(#{tagOrId.inspect}, #{optval.inspect})") if $DEBUG
                  val = optval
                end
                conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] = val
              end
              if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] )
                optval = conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]]
                begin
                  val = method.call(tagOrId, optval)
                rescue => e
                  warn("Warning:: #{e.message} (when #{method}.call(#{tagOrId.inspect}, #{optval.inspect})") if $DEBUG
                  val = optval
                end
                conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] = val
              end

            when /^(#{__item_strval_optkeys(tagid(tagOrId)).join('|')})$/
              # do nothing

            when /^(#{__item_numval_optkeys(tagid(tagOrId)).join('|')})$/
              if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                  && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] )
                begin
                  conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =
                    number(conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]])
                rescue
                  conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] = nil
                end
              end
              if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] )
                begin
                  conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =
                    number(conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]])
                rescue
                  conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] = nil
                end
              end

            when /^(#{__item_numstrval_optkeys(tagid(tagOrId)).join('|')})$/
              if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                  && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] )
                conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =
                  num_or_str(conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]])
              end
              if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] )
                conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =
                  num_or_str(conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]])
              end

            when /^(#{__item_boolval_optkeys(tagid(tagOrId)).join('|')})$/
              if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                  && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] )
                begin
                  conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =
                    bool(conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]])
                rescue
                  conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] = nil
                end
              end
              if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] )
                begin
                  conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =
                    bool(conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]])
                rescue
                  conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] = nil
                end
              end

            when /^(#{__item_listval_optkeys(tagid(tagOrId)).join('|')})$/
              if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                  && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] )
                conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =
                  simplelist(conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]])
              end
              if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] )
                conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =
                  simplelist(conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]])
              end

            when /^(#{__item_numlistval_optkeys(tagid(tagOrId)).join('|')})$/
              if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                  && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] \
                  && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =~ /^[0-9]/ )
                conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =
                  list(conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]])
              end
              if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] \
                  && conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =~ /^[0-9]/ )
                conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =
                  list(conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]])
              end

            when /^(#{__item_tkvariable_optkeys(tagid(tagOrId)).join('|')})$/
              if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                  && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] )
                v = conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]]
                if v.empty?
                  conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] = nil
                else
                  conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] = TkVarAccess.new(v)
                end
              end
              if ( conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] )
                v = conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]]
                if v.empty?
                  conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] = nil
                else
                  conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] = TkVarAccess.new(v)
                end
              end

            else
              if ( __item_configinfo_struct(tagid(tagOrId))[:default_value] \
                  && conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] )
                if conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]].index('{')
                  conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =
                    tk_split_list(conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]])
                else
                  conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] =
                    tk_tcl2ruby(conf[__item_configinfo_struct(tagid(tagOrId))[:default_value]])
                end
              end
              if conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]]
                if conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]].index('{')
                  conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =
                    tk_split_list(conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]])
                else
                  conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] =
                    tk_tcl2ruby(conf[__item_configinfo_struct(tagid(tagOrId))[:current_value]])
                end
              end
            end

            if ( __item_configinfo_struct(tagid(tagOrId))[:alias] \
                && conf.size == __item_configinfo_struct(tagid(tagOrId))[:alias] + 1 )
              if conf[__item_configinfo_struct(tagid(tagOrId))[:alias]][0] == ?-
                conf[__item_configinfo_struct(tagid(tagOrId))[:alias]] =
                  conf[__item_configinfo_struct(tagid(tagOrId))[:alias]][1..-1]
              end
              ret[conf[0]] = conf[1]
            else
              ret[conf.shift] = conf
            end
          }

          __item_font_optkeys(tagid(tagOrId)).each{|optkey|
            optkey = optkey.to_s
            fontconf = ret[optkey]
            if fontconf.kind_of?(Array)
              ret.delete(optkey)
              ret.delete('latin' << optkey)
              ret.delete('ascii' << optkey)
              ret.delete('kanji' << optkey)
              fnt = fontconf[__item_configinfo_struct(tagid(tagOrId))[:default_value]]
              if TkFont.is_system_font?(fnt)
                fontconf[__item_configinfo_struct(tagid(tagOrId))[:default_value]] = TkNamedFont.new(fnt)
              end
              fontconf[__item_configinfo_struct(tagid(tagOrId))[:current_value]] = tagfontobj(tagid(tagOrId), optkey)
              ret[optkey] = fontconf
            end
          }

          __item_methodcall_optkeys(tagid(tagOrId)).each{|optkey, method|
            ret[optkey.to_s] = ['', '', '', self.__send__(method, tagOrId)]
          }

          ret
        end
      end
    end
  end
  private :__itemconfiginfo_core

  def itemconfiginfo(tagOrId, slot = nil)
    if slot && TkItemConfigMethod.__IGNORE_UNKNOWN_CONFIGURE_OPTION__
      begin
        __itemconfiginfo_core(tagOrId, slot)
      rescue => e
        begin
          __itemconfiginfo_core(tagOrId)
          # not tag error -> option is unknown
          Array.new(__item_configinfo_struct.values.max).unshift(slot.to_s)
        rescue
          fail e  # tag error
        end
      end
    else
      __itemconfiginfo_core(tagOrId, slot)
    end
  end

  def __current_itemconfiginfo(tagOrId, slot = nil)
    if TkComm::GET_CONFIGINFO_AS_ARRAY
      if slot
        org_slot = slot
        begin
          conf = __itemconfiginfo_core(tagOrId, slot)
          if ( ! __item_configinfo_struct(tagid(tagOrId))[:alias] \
              || conf.size > __item_configinfo_struct(tagid(tagOrId))[:alias] + 1 )
            return {conf[0] => conf[-1]}
          end
          slot = conf[__item_configinfo_struct(tagid(tagOrId))[:alias]]
        end while(org_slot != slot)
        fail RuntimeError,
          "there is a configure alias loop about '#{org_slot}'"
      else
        ret = {}
        __itemconfiginfo_core(tagOrId).each{|conf|
          if ( ! __item_configinfo_struct(tagid(tagOrId))[:alias] \
              || conf.size > __item_configinfo_struct(tagid(tagOrId))[:alias] + 1 )
            ret[conf[0]] = conf[-1]
          end
        }

        ret
      end
    else # ! TkComm::GET_CONFIGINFO_AS_ARRAY
      ret = {}
      __itemconfiginfo_core(tagOrId, slot).each{|key, conf|
        ret[key] = conf[-1] if conf.kind_of?(Array)
      }
      ret
    end
  end

  def current_itemconfiginfo(tagOrId, slot = nil)
    __current_itemconfiginfo(tagOrId, slot)
  end
end
