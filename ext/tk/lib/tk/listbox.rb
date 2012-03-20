#
# tk/listbox.rb : treat listbox widget
#
require 'tk'
require 'tk/itemconfig'
require 'tk/scrollable'
require 'tk/txtwin_abst'

module TkListItemConfig
  include TkItemConfigMethod

  def __item_listval_optkeys(id)
    []
  end
  private :__item_listval_optkeys
end

class Tk::Listbox<TkTextWin
  include TkListItemConfig
  include Scrollable

  TkCommandNames = ['listbox'.freeze].freeze
  WidgetClassName = 'Listbox'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  #def create_self(keys)
  #  if keys and keys != None
  #    tk_call_without_enc('listbox', @path, *hash_kv(keys, true))
  #  else
  #    tk_call_without_enc('listbox', @path)
  #  end
  #end
  #private :create_self

  def __tkvariable_optkeys
    super() << 'listvariable'
  end
  private :__tkvariable_optkeys

  def tagid(id)
    #id.to_s
    _get_eval_string(id)
  end

  def activate(y)
    tk_send_without_enc('activate', y)
    self
  end
  def curselection
    list(tk_send_without_enc('curselection'))
  end
  def get(first, last=nil)
    if last
      # tk_split_simplelist(_fromUTF8(tk_send_without_enc('get', first, last)))
      tk_split_simplelist(tk_send_without_enc('get', first, last), false, true)
    else
      _fromUTF8(tk_send_without_enc('get', first))
    end
  end
  def nearest(y)
    tk_send_without_enc('nearest', y).to_i
  end
  def size
    tk_send_without_enc('size').to_i
  end
  def selection_anchor(index)
    tk_send_without_enc('selection', 'anchor', index)
    self
  end
  def selection_clear(first, last=None)
    tk_send_without_enc('selection', 'clear', first, last)
    self
  end
  def selection_includes(index)
    bool(tk_send_without_enc('selection', 'includes', index))
  end
  def selection_set(first, last=None)
    tk_send_without_enc('selection', 'set', first, last)
    self
  end

  def index(idx)
    tk_send_without_enc('index', idx).to_i
  end

  def value
    get('0', 'end')
  end

  def value= (vals)
    unless vals.kind_of?(Array)
      fail ArgumentError, 'an Array is expected'
    end
    tk_send_without_enc('delete', '0', 'end')
    tk_send_without_enc('insert', '0',
                        *(vals.collect{|v| _get_eval_enc_str(v)}))
    vals
  end

  def clear
    tk_send_without_enc('delete', '0', 'end')
    self
  end
  alias erase clear

=begin
  def itemcget(index, key)
    case key.to_s
    when 'text', 'label', 'show'
      _fromUTF8(tk_send_without_enc('itemcget', index, "-#{key}"))
    when 'font', 'kanjifont'
      #fnt = tk_tcl2ruby(tk_send('itemcget', index, "-#{key}"))
      fnt = tk_tcl2ruby(_fromUTF8(tk_send_without_enc('itemcget', index,
                                                      '-font')))
      unless fnt.kind_of?(TkFont)
        fnt = tagfontobj(index, fnt)
      end
      if key.to_s == 'kanjifont' && JAPANIZED_TK && TK_VERSION =~ /^4\.*/
        # obsolete; just for compatibility
        fnt.kanji_font
      else
        fnt
      end
    else
      tk_tcl2ruby(_fromUTF8(tk_send_without_enc('itemcget', index, "-#{key}")))
    end
  end
  def itemconfigure(index, key, val=None)
    if key.kind_of? Hash
      if (key['font'] || key[:font] ||
          key['kanjifont'] || key[:kanjifont] ||
          key['latinfont'] || key[:latinfont] ||
          key['asciifont'] || key[:asciifont] )
        tagfont_configure(index, _symbolkey2str(key))
      else
        tk_send_without_enc('itemconfigure', index, *hash_kv(key, true))
      end

    else
      if (key == 'font' || key == :font ||
          key == 'kanjifont' || key == :kanjifont ||
          key == 'latinfont' || key == :latinfont ||
          key == 'asciifont' || key == :asciifont )
        if val == None
          tagfontobj(index)
        else
          tagfont_configure(index, {key=>val})
        end
      else
        tk_call('itemconfigure', index, "-#{key}", val)
      end
    end
    self
  end

  def itemconfiginfo(index, key=nil)
    if TkComm::GET_CONFIGINFO_AS_ARRAY
      if key
        case key.to_s
        when 'text', 'label', 'show'
          conf = tk_split_simplelist(_fromUTF8(tk_send_without_enc('itemconfigure',index,"-#{key}")))
        when 'font', 'kanjifont'
          conf = tk_split_simplelist(_fromUTF8(tk_send_without_enc('itemconfigure',index,"-#{key}")))
          conf[4] = tagfont_configinfo(index, conf[4])
        else
          conf = tk_split_list(_fromUTF8(tk_send_without_enc('itemconfigure',index,"-#{key}")))
        end
        conf[0] = conf[0][1..-1]
        conf
      else
        ret = tk_split_simplelist(_fromUTF8(tk_send_without_enc('itemconfigure', index))).collect{|conflist|
          conf = tk_split_simplelist(conflist)
          conf[0] = conf[0][1..-1]
          case conf[0]
          when 'text', 'label', 'show'
          else
            if conf[3]
              if conf[3].index('{')
                conf[3] = tk_split_list(conf[3])
              else
                conf[3] = tk_tcl2ruby(conf[3])
              end
            end
            if conf[4]
              if conf[4].index('{')
                conf[4] = tk_split_list(conf[4])
              else
                conf[4] = tk_tcl2ruby(conf[4])
              end
            end
          end
          conf[1] = conf[1][1..-1] if conf.size == 2 # alias info
          conf
        }
        fontconf = ret.assoc('font')
        if fontconf
          ret.delete_if{|item| item[0] == 'font' || item[0] == 'kanjifont'}
          fontconf[4] = tagfont_configinfo(index, fontconf[4])
          ret.push(fontconf)
        else
          ret
        end
      end
    else # ! TkComm::GET_CONFIGINFO_AS_ARRAY
      if key
        case key.to_s
        when 'text', 'label', 'show'
          conf = tk_split_simplelist(_fromUTF8(tk_send_without_enc('itemconfigure',index,"-#{key}")))
        when 'font', 'kanjifont'
          conf = tk_split_simplelist(_fromUTF8(tk_send_without_enc('itemconfigure',index,"-#{key}")))
          conf[4] = tagfont_configinfo(index, conf[4])
        else
          conf = tk_split_list(_fromUTF8(tk_send_without_enc('itemconfigure',index,"-#{key}")))
        end
        key = conf.shift[1..-1]
        { key => conf }
      else
        ret = {}
        tk_split_simplelist(_fromUTF8(tk_send_without_enc('itemconfigure', index))).each{|conflist|
          conf = tk_split_simplelist(conflist)
          key = conf.shift[1..-1]
          case key
          when 'text', 'label', 'show'
          else
            if conf[2]
              if conf[2].index('{')
                conf[2] = tk_split_list(conf[2])
              else
                conf[2] = tk_tcl2ruby(conf[2])
              end
            end
            if conf[3]
              if conf[3].index('{')
                conf[3] = tk_split_list(conf[3])
              else
                conf[3] = tk_tcl2ruby(conf[3])
              end
            end
          end
          if conf.size == 1
            ret[key] = conf[0][1..-1]  # alias info
          else
            ret[key] = conf
          end
        }
        fontconf = ret['font']
        if fontconf
          ret.delete('font')
          ret.delete('kanjifont')
          fontconf[3] = tagfont_configinfo(index, fontconf[3])
          ret['font'] = fontconf
        end
        ret
      end
    end
  end

  def current_itemconfiginfo(index, key=nil)
    if TkComm::GET_CONFIGINFO_AS_ARRAY
      if key
        conf = itemconfiginfo(index, key)
        {conf[0] => conf[4]}
      else
        ret = {}
        itemconfiginfo(index).each{|conf|
          ret[conf[0]] = conf[4] if conf.size > 2
        }
        ret
      end
    else # ! TkComm::GET_CONFIGINFO_AS_ARRAY
      ret = {}
      itemconfiginfo(index, key).each{|k, conf|
        ret[k] = conf[-1] if conf.kind_of?(Array)
      }
      ret
    end
  end
=end
end

#TkListbox = Tk::Listbox unless Object.const_defined? :TkListbox
#Tk.__set_toplevel_aliases__(:Tk, Tk::Listbox, :TkListbox)
Tk.__set_loaded_toplevel_aliases__('tk/listbox.rb', :Tk, Tk::Listbox,
                                   :TkListbox)
