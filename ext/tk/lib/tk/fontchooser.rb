#
# tk/fontchooser.rb -- "tk fontchooser" support (Tcl/Tk8.6 or later)
#
require 'tk'
require 'tk/font'

module TkFont::Chooser
  extend TkCore
end

class << TkFont::Chooser
  def method_missing(id, *args)
    name = id.id2name
    case args.length
    when 1
      if name[-1] == ?=
        configure name[0..-2], args[0]
        args[0]
      else
        configure name, args[0]
        self
      end
    when 0
      begin
        cget(name)
      rescue
        super(id, *args)
      end
    else
      super(id, *args)
    end
  end

  def __configinfo_value(key, val)
    case key
    when 'parent'
      window(val)
    when 'title'
      val
    when 'font'
      if (lst = tk_split_simplelist(val)).size == 1
        lst[0]
      else
        lst.map{|elem| num_or_str(elem)}
      end
    when 'command'
      tk_tcl2ruby(val)
    when 'visible'
      bool(val)
    else # unkown
      val
    end
  end
  private :__configinfo_value

  def configinfo(option=nil)
    if !option && TkComm::GET_CONFIGINFOwoRES_AS_ARRAY
      lst = tk_split_simplelist(tk_call('tk', 'fontchooser', 'configure'))
      ret = []
      TkComm.slice_ary(lst, 2){|k, v|
        k = k[1..-1]
        ret << [k, __configinfo_value(k, v)]
      }
      ret
    else
      current_configinfo(option)
    end
  end

  def current_configinfo(option=nil)
    if option
      opt = option.to_s
      fail ArgumentError, "Invalid option `#{option.inspect}'" if opt.empty?
      __configinfo_value(option.to_s, tk_call('tk','fontchooser',
                                              'configure',"-#{opt}"))
    else
      lst = tk_split_simplelist(tk_call('tk', 'fontchooser', 'configure'))
      ret = {}
      TkComm.slice_ary(lst, 2){|k, v|
        k = k[1..-1]
        ret[k] = __configinfo_value(k, v)
      }
      ret
    end
  end

  def configure(option, value=None)
    if option.kind_of? Hash
      tk_call('tk', 'fontchooser', 'configure',
              *hash_kv(_symbolkey2str(option)))
    else
      opt = option.to_s
      fail ArgumentError, "Invalid option `#{option.inspect}'" if opt.empty?
      tk_call('tk', 'fontchooser', 'configure', "-#{opt}", value)
    end
    self
  end

  def configure_cmd(slot, value)
    configure(slot, install_cmd(value))
  end

  def command(cmd=nil, &b)
    if cmd
      configure_cmd('command', cmd)
    elsif b
      configure_cmd('command', Proc.new(&b))
    else
      cget('command')
    end
  end

  def cget(slot)
    configinfo slot
  end

  def [](slot)
    cget slot
  end

  def []=(slot, val)
    configure slot, val
    val
  end

  def show
    tk_call('tk', 'fontchooser', 'show')
    self
  end

  def hide
    tk_call('tk', 'fontchooser', 'hide')
    self
  end

  def toggle
    cget(:visible) ?  hide: show
    self
  end

  def set_for(target, title="Font")
    if target.kind_of? TkFont
      configs = {
        :font=>target.actual_hash,
        :command=>proc{|fnt, *args|
          target.configure(TkFont.actual_hash(fnt))
        }
      }
    elsif target.kind_of? Hash
      # key=>value list or OptionObj
      fnt = target[:font] rescue ''
      fnt = fnt.actual_hash if fnt.kind_of?(TkFont)
      configs = {
        :font => fnt,
        :command=>proc{|fnt, *args|
          target[:font] = TkFont.actual_hash(fnt)
        }
      }
    else
      configs = {
        :font=>target.cget_tkstring(:font),
        :command=>proc{|fnt, *args|
          target.font = TkFont.actual_hash_displayof(fnt, target)
        }
      }
    end

    configs[:title] = title if title
    configure(configs)
    target
  end

  def unset
    configure(:command, nil)
  end
end
