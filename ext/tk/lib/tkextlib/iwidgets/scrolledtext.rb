#
#  tkextlib/iwidgets/scrolledtext.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tk/text'
require 'tkextlib/iwidgets.rb'

module Tk
  module Iwidgets
    class Scrolledtext < Tk::Iwidgets::Scrolledwidget
    end
  end
end

class Tk::Iwidgets::Scrolledtext
  TkCommandNames = ['::iwidgets::scrolledtext'.freeze].freeze
  WidgetClassName = 'Scrolledtext'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def __strval_optkeys
    super() << 'textbackground'
  end
  private :__strval_optkeys

  def __font_optkeys
    super() << 'textfont'
  end
  private :__font_optkeys

  ################################

  def initialize(*args)
    super(*args)
    @text = component_widget('text')
  end

  def method_missing(id, *args)
    if @text.respond_to?(id)
      @text.__send__(id, *args)
    else
      super(id, *args)
    end
  end

  ################################

  def child_site
    window(tk_call(@path, 'childsite'))
  end

  def clear
    tk_call(@path, 'clear')
    self
  end

  def import(file, idx=nil)
    if idx
      tk_call(@path, 'import', file, index(idx))
    else
      tk_call(@path, 'import', file)
    end
    self
  end

  def export(file)
    tk_call(@path, 'export', file)
    self
  end

  #####################################

  include TkTextTagConfig

  def tagid(tag)
    if tag.kind_of?(Tk::Itk::Component)
      tag.name
    else
      super(tag)
    end
  end
  private :tagid

  def bbox(index)
    list(tk_send('bbox', index))
  end
  def compare(idx1, op, idx2)
    bool(tk_send_without_enc('compare', _get_eval_enc_str(idx1),
                             op, _get_eval_enc_str(idx2)))
  end

  def debug
    bool(tk_send_without_enc('debug'))
  end
  def debug=(boolean)
    tk_send_without_enc('debug', boolean)
    #self
    boolean
  end

  def delete(first, last=None)
    tk_send_without_enc('delete', first, last)
    self
  end

  def dlineinfo(index)
    list(tk_send_without_enc('dlineinfo', _get_eval_enc_str(index)))
  end

  def get(*index)
    _fromUTF8(tk_send_without_enc('get', *index))
  end
  def get_displaychars(*index)
    # Tk8.5 feature
    get('-displaychars', *index)
  end

  def image_cget_tkstring(index, slot)
    _fromUTF8(tk_send_without_enc('image', 'cget',
                                  _get_eval_enc_str(index), "-#{slot.to_s}"))
  end
  def image_cget_strict(index, slot)
    case slot.to_s
    when 'text', 'label', 'show', 'data', 'file'
      _fromUTF8(tk_send_without_enc('image', 'cget',
                                    _get_eval_enc_str(index), "-#{slot}"))
    else
      tk_tcl2ruby(_fromUTF8(tk_send_without_enc('image', 'cget',
                                                _get_eval_enc_str(index),
                                                "-#{slot}")))
    end
  end
  def image_cget(index, slot)
    unless TkItemConfigMethod.__IGNORE_UNKNOWN_CONFIGURE_OPTION__
      image_cget_strict(index, slot)
    else
      begin
        image_cget_strict(index, slot)
      rescue => e
        begin
          if current_image_configinfo.has_key?(slot.to_s)
            # error on known option
            fail e
          else
            # unknown option
            nil
          end
        rescue
          fail e  # tag error
        end
      end
    end
  end

  def image_configure(index, slot, value=None)
    if slot.kind_of? Hash
      _fromUTF8(tk_send_without_enc('image', 'configure',
                                    _get_eval_enc_str(index),
                                    *hash_kv(slot, true)))
    else
      _fromUTF8(tk_send_without_enc('image', 'configure',
                                    _get_eval_enc_str(index),
                                    "-#{slot}",
                                    _get_eval_enc_str(value)))
    end
    self
  end

  def image_configinfo(index, slot = nil)
    if TkComm::GET_CONFIGINFO_AS_ARRAY
      if slot
        case slot.to_s
        when 'text', 'label', 'show', 'data', 'file'
          #conf = tk_split_simplelist(_fromUTF8(tk_send_without_enc('image', 'configure', _get_eval_enc_str(index), "-#{slot}")))
          conf = tk_split_simplelist(tk_send_without_enc('image', 'configure', _get_eval_enc_str(index), "-#{slot}"), false, true)
        else
          #conf = tk_split_list(_fromUTF8(tk_send_without_enc('image', 'configure', _get_eval_enc_str(index), "-#{slot}")))
          conf = tk_split_list(tk_send_without_enc('image', 'configure', _get_eval_enc_str(index), "-#{slot}"), 0, false, true)
        end
        conf[0] = conf[0][1..-1]
        conf
      else
        #tk_split_simplelist(_fromUTF8(tk_send_without_enc('image', 'configure', _get_eval_enc_str(index)))).collect{|conflist|
        #  conf = tk_split_simplelist(conflist)
        tk_split_simplelist(tk_send_without_enc('image', 'configure', _get_eval_enc_str(index)), false, false).collect{|conflist|
          conf = tk_split_simplelist(conflist, false, true)
          conf[0] = conf[0][1..-1]
          case conf[0]
          when 'text', 'label', 'show', 'data', 'file'
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
      end
    else # ! TkComm::GET_CONFIGINFO_AS_ARRAY
      if slot
        case slot.to_s
        when 'text', 'label', 'show', 'data', 'file'
          #conf = tk_split_simplelist(_fromUTF8(tk_send_without_enc('image', 'configure', _get_eval_enc_str(index), "-#{slot}")))
          conf = tk_split_simplelist(tk_send_without_enc('image', 'configure', _get_eval_enc_str(index), "-#{slot}"), false, true)
        else
          #conf = tk_split_list(_fromUTF8(tk_send_without_enc('image', 'configure', _get_eval_enc_str(index), "-#{slot}")))
          conf = tk_split_list(tk_send_without_enc('image', 'configure', _get_eval_enc_str(index), "-#{slot}"), 0, false, true)
        end
        key = conf.shift[1..-1]
        { key => conf }
      else
        ret = {}
        #tk_split_simplelist(_fromUTF8(tk_send_without_enc('image', 'configure', _get_eval_enc_str(index)))).each{|conflist|
        #  conf = tk_split_simplelist(conflist)
        tk_split_simplelist(tk_send_without_enc('image', 'configure', _get_eval_enc_str(index)), false, false).each{|conflist|
          conf = tk_split_simplelist(conflist, false, true)
          key = conf.shift[1..-1]
          case key
          when 'text', 'label', 'show', 'data', 'file'
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
        ret
      end
    end
  end

  def current_image_configinfo(index, slot = nil)
    if TkComm::GET_CONFIGINFO_AS_ARRAY
      if slot
        conf = image_configinfo(index, slot)
        {conf[0] => conf[4]}
      else
        ret = {}
        image_configinfo(index).each{|conf|
          ret[conf[0]] = conf[4] if conf.size > 2
        }
        ret
      end
    else # ! TkComm::GET_CONFIGINFO_AS_ARRAY
      ret = {}
      image_configinfo(index, slot).each{|k, conf|
        ret[k] = conf[-1] if conf.kind_of?(Array)
      }
      ret
    end
  end

  def image_names
    #tk_split_simplelist(_fromUTF8(tk_send_without_enc('image', 'names'))).collect{|elt|
    tk_split_simplelist(tk_send_without_enc('image', 'names'), false, true).collect{|elt|
      tagid2obj(elt)
    }
  end

  def index(idx)
    tk_send_without_enc('index', _get_eval_enc_str(idx))
  end

  def insert(index, *args)
    tk_send('insert', index, *args)
    self
  end

  def mark_names
    #tk_split_simplelist(_fromUTF8(tk_send_without_enc('mark', 'names'))).collect{|elt|
    tk_split_simplelist(tk_send_without_enc('mark', 'names'), false, true).collect{|elt|
      tagid2obj(elt)
    }
  end

  def mark_gravity(mark, direction=nil)
    if direction
      tk_send_without_enc('mark', 'gravity',
                          _get_eval_enc_str(mark), direction)
      self
    else
      tk_send_without_enc('mark', 'gravity', _get_eval_enc_str(mark))
    end
  end

  def mark_set(mark, index)
    tk_send_without_enc('mark', 'set', _get_eval_enc_str(mark),
                        _get_eval_enc_str(index))
    self
  end
  alias set_mark mark_set

  def mark_unset(*marks)
    tk_send_without_enc('mark', 'unset',
                        *(marks.collect{|mark| _get_eval_enc_str(mark)}))
    self
  end
  alias unset_mark mark_unset

  def mark_next(index)
    tagid2obj(_fromUTF8(tk_send_without_enc('mark', 'next',
                                            _get_eval_enc_str(index))))
  end
  alias next_mark mark_next

  def mark_previous(index)
    tagid2obj(_fromUTF8(tk_send_without_enc('mark', 'previous',
                                            _get_eval_enc_str(index))))
  end
  alias previous_mark mark_previous

  def scan_mark(x, y)
    tk_send_without_enc('scan', 'mark', x, y)
    self
  end
  def scan_dragto(x, y)
    tk_send_without_enc('scan', 'dragto', x, y)
    self
  end


  def _ktext_length(txt)
    if TkCore::WITH_ENCODING ### Ruby 1.9 !!!!!!!!!!!!!
      return txt.length
    end
    ###########################

    if $KCODE !~ /n/i
      return txt.gsub(/[^\Wa-zA-Z_\d]/, ' ').length
    end

    # $KCODE == 'NONE'
    if JAPANIZED_TK
      tk_call_without_enc('kstring', 'length',
                          _get_eval_enc_str(txt)).to_i
    else
      begin
        tk_call_without_enc('encoding', 'convertto', 'ascii',
                            _get_eval_enc_str(txt)).length
      rescue StandardError, NameError
        # sorry, I have no plan
        txt.length
      end
    end
  end
  private :_ktext_length

  def tksearch(*args)
    # call 'search' subcommand of text widget
    #   args ::= [<array_of_opts>] <pattern> <start_index> [<stop_index>]
    # If <pattern> is regexp, then it must be a regular expression of Tcl
    if args[0].kind_of?(Array)
      opts = args.shift.collect{|opt| '-' + opt.to_s }
    else
      opts = []
    end

    opts << '--'

    ret = tk_send('search', *(opts + args))
    if ret == ""
      nil
    else
      ret
    end
  end

  def tksearch_with_count(*args)
    # call 'search' subcommand of text widget
    #   args ::= [<array_of_opts>] <var> <pattern> <start_index> [<stop_index>]
    # If <pattern> is regexp, then it must be a regular expression of Tcl
    if args[0].kind_of?(Array)
      opts = args.shift.collect{|opt| '-' + opt.to_s }
    else
      opts = []
    end

    opts << '-count' << args.shift << '--'

    ret = tk_send('search', *(opts + args))
    if ret == ""
      nil
    else
      ret
    end
  end

  def search_with_length(pat,start,stop=None)
    pat = pat.chr if pat.kind_of? Integer
    if stop != None
      return ["", 0] if compare(start,'>=',stop)
      txt = get(start,stop)
      if (pos = txt.index(pat))
        match = $&
        #pos = txt[0..(pos-1)].split('').length if pos > 0
        pos = _ktext_length(txt[0..(pos-1)]) if pos > 0
        if pat.kind_of? String
          #return [index(start + " + #{pos} chars"), pat.split('').length]
          return [index(start + " + #{pos} chars"),
                  _ktext_length(pat), pat.dup]
        else
          #return [index(start + " + #{pos} chars"), $&.split('').length]
          return [index(start + " + #{pos} chars"),
                  _ktext_length(match), match]
        end
      else
        return ["", 0]
      end
    else
      txt = get(start,'end - 1 char')
      if (pos = txt.index(pat))
        match = $&
        #pos = txt[0..(pos-1)].split('').length if pos > 0
        pos = _ktext_length(txt[0..(pos-1)]) if pos > 0
        if pat.kind_of? String
          #return [index(start + " + #{pos} chars"), pat.split('').length]
          return [index(start + " + #{pos} chars"),
                  _ktext_length(pat), pat.dup]
        else
          #return [index(start + " + #{pos} chars"), $&.split('').length]
          return [index(start + " + #{pos} chars"),
                  _ktext_length(match), match]
        end
      else
        txt = get('1.0','end - 1 char')
        if (pos = txt.index(pat))
          match = $&
          #pos = txt[0..(pos-1)].split('').length if pos > 0
          pos = _ktext_length(txt[0..(pos-1)]) if pos > 0
          if pat.kind_of? String
            #return [index("1.0 + #{pos} chars"), pat.split('').length]
            return [index("1.0 + #{pos} chars"),
                    _ktext_length(pat), pat.dup]
          else
            #return [index("1.0 + #{pos} chars"), $&.split('').length]
            return [index("1.0 + #{pos} chars"), _ktext_length(match), match]
          end
        else
          return ["", 0]
        end
      end
    end
  end

  def search(pat,start,stop=None)
    search_with_length(pat,start,stop)[0]
  end

  def rsearch_with_length(pat,start,stop=None)
    pat = pat.chr if pat.kind_of? Integer
    if stop != None
      return ["", 0] if compare(start,'<=',stop)
      txt = get(stop,start)
      if (pos = txt.rindex(pat))
        match = $&
        #pos = txt[0..(pos-1)].split('').length if pos > 0
        pos = _ktext_length(txt[0..(pos-1)]) if pos > 0
        if pat.kind_of? String
          #return [index(stop + " + #{pos} chars"), pat.split('').length]
          return [index(stop + " + #{pos} chars"), _ktext_length(pat), pat.dup]
        else
          #return [index(stop + " + #{pos} chars"), $&.split('').length]
          return [index(stop + " + #{pos} chars"), _ktext_length(match), match]
        end
      else
        return ["", 0]
      end
    else
      txt = get('1.0',start)
      if (pos = txt.rindex(pat))
        match = $&
        #pos = txt[0..(pos-1)].split('').length if pos > 0
        pos = _ktext_length(txt[0..(pos-1)]) if pos > 0
        if pat.kind_of? String
          #return [index("1.0 + #{pos} chars"), pat.split('').length]
          return [index("1.0 + #{pos} chars"), _ktext_length(pat), pat.dup]
        else
          #return [index("1.0 + #{pos} chars"), $&.split('').length]
          return [index("1.0 + #{pos} chars"), _ktext_length(match), match]
        end
      else
        txt = get('1.0','end - 1 char')
        if (pos = txt.rindex(pat))
          match = $&
          #pos = txt[0..(pos-1)].split('').length if pos > 0
          pos = _ktext_length(txt[0..(pos-1)]) if pos > 0
          if pat.kind_of? String
            #return [index("1.0 + #{pos} chars"), pat.split('').length]
            return [index("1.0 + #{pos} chars"), _ktext_length(pat), pat.dup]
          else
            #return [index("1.0 + #{pos} chars"), $&.split('').length]
            return [index("1.0 + #{pos} chars"), _ktext_length(match), match]
          end
        else
          return ["", 0]
        end
      end
    end
  end

  def rsearch(pat,start,stop=None)
    rsearch_with_length(pat,start,stop)[0]
  end

  def see(index)
    tk_send_without_enc('see', index)
    self
  end

  ###############################

  def xview(*index)
    if index.size == 0
      list(tk_send_without_enc('xview'))
    else
      tk_send_without_enc('xview', *index)
      self
    end
  end
  def xview_moveto(*index)
    xview('moveto', *index)
  end
  def xview_scroll(*index)
    xview('scroll', *index)
  end

  def yview(*index)
    if index.size == 0
      list(tk_send_without_enc('yview'))
    else
      tk_send_without_enc('yview', *index)
      self
    end
  end
  def yview_moveto(*index)
    yview('moveto', *index)
  end
  def yview_scroll(*index)
    yview('scroll', *index)
  end
end
