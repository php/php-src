#
#               tk.rb - Tk interface module using tcltklib
#                       by Yukihiro Matsumoto <matz@netlab.jp>

# use Shigehiro's tcltklib
require 'tcltklib'
require 'tkutil'

# autoload
require 'tk/autoload'

# for Mutex
require 'thread'

class TclTkIp
  # backup original (without encoding) _eval and _invoke
  alias _eval_without_enc _eval
  alias __eval__ _eval
  alias _invoke_without_enc _invoke
  alias __invoke__ _invoke

  def _ip_id_
    # for RemoteTkIp
    ''
  end

  alias __initialize__ initialize
  private :__initialize__

  def initialize(*args)
    __initialize__(*args)

    @force_default_encoding ||= TkUtil.untrust([false])
    @encoding ||= TkUtil.untrust([nil])
    def @encoding.to_s; self.join(nil); end
  end
end

# define TkComm module (step 1: basic functions)
module TkComm
  include TkUtil
  extend TkUtil

  WidgetClassNames = TkUtil.untrust({})
  TkExtlibAutoloadModule = TkUtil.untrust([])

  # None = Object.new  ### --> definition is moved to TkUtil module
  # def None.to_s
  #   'None'
  # end
  # None.freeze

  #Tk_CMDTBL = {}
  #Tk_WINDOWS = {}
  Tk_IDs = [
    TkUtil.untrust("00000"), # [0]-cmdid
    TkUtil.untrust("00000")  # [1]-winid
  ]
  Tk_IDs.instance_eval{
    @mutex = Mutex.new
    def mutex; @mutex; end
    freeze
  }

  # for backward compatibility
  Tk_CMDTBL = Object.new
  def Tk_CMDTBL.method_missing(id, *args)
    TkCore::INTERP.tk_cmd_tbl.__send__(id, *args)
  end
  Tk_CMDTBL.freeze
  Tk_WINDOWS = Object.new
  def Tk_WINDOWS.method_missing(id, *args)
    TkCore::INTERP.tk_windows.__send__(id, *args)
  end
  Tk_WINDOWS.freeze

  self.instance_eval{
    @cmdtbl = TkUtil.untrust([])
  }

  unless const_defined?(:GET_CONFIGINFO_AS_ARRAY)
    # GET_CONFIGINFO_AS_ARRAY = false => returns a Hash { opt =>val, ... }
    #                           true  => returns an Array [[opt,val], ... ]
    # val is a list which includes resource info.
    GET_CONFIGINFO_AS_ARRAY = true
  end
  unless const_defined?(:GET_CONFIGINFOwoRES_AS_ARRAY)
    # for configinfo without resource info; list of [opt, value] pair
    #           false => returns a Hash { opt=>val, ... }
    #           true  => returns an Array [[opt,val], ... ]
    GET_CONFIGINFOwoRES_AS_ARRAY = true
  end
  #  *** ATTENTION ***
  # 'current_configinfo' method always returns a Hash under all cases of above.

  def error_at
    frames = caller()
    frames.delete_if do |c|
      c =~ %r!/tk(|core|thcore|canvas|text|entry|scrollbox)\.rb:\d+!
    end
    frames
  end
  private :error_at

  def _genobj_for_tkwidget(path)
    return TkRoot.new if path == '.'

    begin
      #tk_class = TkCore::INTERP._invoke('winfo', 'class', path)
      tk_class = Tk.ip_invoke_without_enc('winfo', 'class', path)
    rescue
      return path
    end

    if ruby_class = WidgetClassNames[tk_class]
      ruby_class_name = ruby_class.name
      # gen_class_name = ruby_class_name + 'GeneratedOnTk'
      gen_class_name = ruby_class_name
      classname_def = ''
    else # ruby_class == nil
      if Tk.const_defined?(tk_class)
        Tk.const_get(tk_class)  # auto_load
        ruby_class = WidgetClassNames[tk_class]
      end

      unless ruby_class
        mods = TkExtlibAutoloadModule.find_all{|m| m.const_defined?(tk_class)}
        mods.each{|mod|
          begin
            mod.const_get(tk_class)  # auto_load
            break if (ruby_class = WidgetClassNames[tk_class])
          rescue LoadError
            # ignore load error
          end
        }
      end

      unless ruby_class
        std_class = 'Tk' << tk_class
        if Object.const_defined?(std_class)
          Object.const_get(std_class)  # auto_load
          ruby_class = WidgetClassNames[tk_class]
        end
      end

      unless ruby_class
        if Tk.const_defined?('TOPLEVEL_ALIASES') &&
            Tk::TOPLEVEL_ALIASES.const_defined?(std_class)
          Tk::TOPLEVEL_ALIASES.const_get(std_class)  # auto_load
          ruby_class = WidgetClassNames[tk_class]
        end
      end

      if ruby_class
        # found
        ruby_class_name = ruby_class.name
        gen_class_name = ruby_class_name
        classname_def = ''
      else
        # unknown
        ruby_class_name = 'TkWindow'
        gen_class_name = 'TkWidget_' + tk_class
        classname_def = "WidgetClassName = '#{tk_class}'.freeze"
      end
    end

###################################
=begin
    if ruby_class = WidgetClassNames[tk_class]
      ruby_class_name = ruby_class.name
      # gen_class_name = ruby_class_name + 'GeneratedOnTk'
      gen_class_name = ruby_class_name
      classname_def = ''
    else
      mod = TkExtlibAutoloadModule.find{|m| m.const_defined?(tk_class)}
      if mod
        ruby_class_name = mod.name + '::' + tk_class
        gen_class_name = ruby_class_name
        classname_def = ''
      elsif Object.const_defined?('Tk' + tk_class)
        ruby_class_name = 'Tk' + tk_class
        # gen_class_name = ruby_class_name + 'GeneratedOnTk'
        gen_class_name = ruby_class_name
        classname_def = ''
      else
        ruby_class_name = 'TkWindow'
        # gen_class_name = ruby_class_name + tk_class + 'GeneratedOnTk'
        gen_class_name = 'TkWidget_' + tk_class
        classname_def = "WidgetClassName = '#{tk_class}'.freeze"
      end
    end
=end

=begin
    unless Object.const_defined? gen_class_name
      Object.class_eval "class #{gen_class_name}<#{ruby_class_name}
                           #{classname_def}
                         end"
    end
    Object.class_eval "#{gen_class_name}.new('widgetname'=>'#{path}',
                                             'without_creating'=>true)"
=end
    base = Object
    gen_class_name.split('::').each{|klass|
      next if klass == ''
      if base.const_defined?(klass)
        base = base.class_eval klass
      else
        base = base.class_eval "class #{klass}<#{ruby_class_name}
                                  #{classname_def}
                                end
                                #{klass}"
      end
    }
    base.class_eval "#{gen_class_name}.new('widgetname'=>'#{path}',
                                           'without_creating'=>true)"
  end
  private :_genobj_for_tkwidget
  module_function :_genobj_for_tkwidget

  def _at(x,y=nil)
    if y
      "@#{Integer(x)},#{Integer(y)}"
    else
      "@#{Integer(x)}"
    end
  end
  module_function :_at

  def tk_tcl2ruby(val, enc_mode = false, listobj = true)
=begin
    if val =~ /^rb_out\S* (c(_\d+_)?\d+)/
      #return Tk_CMDTBL[$1]
      return TkCore::INTERP.tk_cmd_tbl[$1]
      #cmd_obj = TkCore::INTERP.tk_cmd_tbl[$1]
      #if cmd_obj.kind_of?(Proc) || cmd_obj.kind_of?(Method)
      #  cmd_obj
      #else
      #  cmd_obj.cmd
      #end
    end
=end
    if val =~ /rb_out\S*(?:\s+(::\S*|[{](::.*)[}]|["](::.*)["]))? (c(_\d+_)?(\d+))/
      return TkCore::INTERP.tk_cmd_tbl[$4]
    end
    #if val.include? ?\s
    #  return val.split.collect{|v| tk_tcl2ruby(v)}
    #end
    case val
    when /\A@font\S+\z/
      TkFont.get_obj(val)
    when /\A-?\d+\z/
      val.to_i
    when /\A\.\S*\z/
      #Tk_WINDOWS[val] ? Tk_WINDOWS[val] : _genobj_for_tkwidget(val)
      TkCore::INTERP.tk_windows[val]?
           TkCore::INTERP.tk_windows[val] : _genobj_for_tkwidget(val)
    when /\Ai(_\d+_)?\d+\z/
      TkImage::Tk_IMGTBL.mutex.synchronize{
        TkImage::Tk_IMGTBL[val]? TkImage::Tk_IMGTBL[val] : val
      }
    when /\A-?\d+\.?\d*(e[-+]?\d+)?\z/
      val.to_f
    when /\\ /
      val.gsub(/\\ /, ' ')
    when /[^\\] /
      if listobj
        #tk_split_escstr(val).collect{|elt|
        #  tk_tcl2ruby(elt, enc_mode, listobj)
        #}
        val = _toUTF8(val) unless enc_mode
        tk_split_escstr(val, false, false).collect{|elt|
          tk_tcl2ruby(elt, true, listobj)
        }
      elsif enc_mode
        _fromUTF8(val)
      else
        val
      end
    else
      if enc_mode
        _fromUTF8(val)
      else
        val
      end
    end
  end

  private :tk_tcl2ruby
  module_function :tk_tcl2ruby
  #private_class_method :tk_tcl2ruby

unless const_defined?(:USE_TCLs_LIST_FUNCTIONS)
  USE_TCLs_LIST_FUNCTIONS = true
end

if USE_TCLs_LIST_FUNCTIONS
  ###########################################################################
  # use Tcl function version of split_list
  ###########################################################################

  def tk_split_escstr(str, src_enc=true, dst_enc=true)
    str = _toUTF8(str) if src_enc
    if dst_enc
      TkCore::INTERP._split_tklist(str).map!{|s| _fromUTF8(s)}
    else
      TkCore::INTERP._split_tklist(str)
    end
  end

  def tk_split_sublist(str, depth=-1, src_enc=true, dst_enc=true)
    # return [] if str == ""
    # list = TkCore::INTERP._split_tklist(str)
    str = _toUTF8(str) if src_enc

    if depth == 0
      return "" if str == ""
      list = [str]
    else
      return [] if str == ""
      list = TkCore::INTERP._split_tklist(str)
    end
    if list.size == 1
      # tk_tcl2ruby(list[0], nil, false)
      tk_tcl2ruby(list[0], dst_enc, false)
    else
      list.collect{|token| tk_split_sublist(token, depth - 1, false, dst_enc)}
    end
  end

  def tk_split_list(str, depth=0, src_enc=true, dst_enc=true)
    return [] if str == ""
    str = _toUTF8(str) if src_enc
    TkCore::INTERP._split_tklist(str).map!{|token|
      tk_split_sublist(token, depth - 1, false, dst_enc)
    }
  end

  def tk_split_simplelist(str, src_enc=true, dst_enc=true)
    #lst = TkCore::INTERP._split_tklist(str)
    #if (lst.size == 1 && lst =~ /^\{.*\}$/)
    #  TkCore::INTERP._split_tklist(str[1..-2])
    #else
    #  lst
    #end

    str = _toUTF8(str) if src_enc
    if dst_enc
      TkCore::INTERP._split_tklist(str).map!{|s| _fromUTF8(s)}
    else
      TkCore::INTERP._split_tklist(str)
    end
  end

  def array2tk_list(ary, enc=nil)
    return "" if ary.size == 0

    sys_enc = TkCore::INTERP.encoding
    sys_enc = TclTkLib.encoding_system unless sys_enc

    dst_enc = (enc == nil)? sys_enc: enc

    dst = ary.collect{|e|
      if e.kind_of? Array
        s = array2tk_list(e, enc)
      elsif e.kind_of? Hash
        tmp_ary = []
        #e.each{|k,v| tmp_ary << k << v }
        e.each{|k,v| tmp_ary << "-#{_get_eval_string(k)}" << v }
        s = array2tk_list(tmp_ary, enc)
      else
        s = _get_eval_string(e, enc)
      end

      if dst_enc != true && dst_enc != false
        if (s_enc = s.instance_variable_get(:@encoding))
          s_enc = s_enc.to_s
        elsif TkCore::WITH_ENCODING
          s_enc = s.encoding.name
        else
          s_enc = sys_enc
        end
        dst_enc = true if s_enc != dst_enc
      end

      s
    }

    if sys_enc && dst_enc
      dst.map!{|s| _toUTF8(s)}
      ret = TkCore::INTERP._merge_tklist(*dst)
      if TkCore::WITH_ENCODING
        if dst_enc.kind_of?(String)
          ret = _fromUTF8(ret, dst_enc)
          ret.force_encoding(dst_enc)
        else
          ret.force_encoding('utf-8')
        end
      else # without encoding
        if dst_enc.kind_of?(String)
          ret = _fromUTF8(ret, dst_enc)
          ret.instance_variable_set(:@encoding, dst_enc)
        else
          ret.instance_variable_set(:@encoding, 'utf-8')
        end
      end
      ret
    else
      TkCore::INTERP._merge_tklist(*dst)
    end
  end

else
  ###########################################################################
  # use Ruby script version of split_list (traditional methods)
  ###########################################################################

  def tk_split_escstr(str, src_enc=true, dst_enc=true)
    return [] if str == ""
    list = []
    token = nil
    escape = false
    brace = 0
    str.split('').each {|c|
      brace += 1 if c == '{' && !escape
      brace -= 1 if c == '}' && !escape
      if brace == 0 && c == ' ' && !escape
        list << token.gsub(/^\{(.*)\}$/, '\1') if token
        token = nil
      else
        token = (token || "") << c
      end
      escape = (c == '\\' && !escape)
    }
    list << token.gsub(/^\{(.*)\}$/, '\1') if token
    list
  end

  def tk_split_sublist(str, depth=-1, src_enc=true, dst_enc=true)
    #return [] if str == ""
    #return [tk_split_sublist(str[1..-2])] if str =~ /^\{.*\}$/
    #list = tk_split_escstr(str)
    if depth == 0
      return "" if str == ""
      str = str[1..-2] if str =~ /^\{.*\}$/
      list = [str]
    else
      return [] if str == []
      return [tk_split_sublist(str[1..-2], depth - 1)] if str =~ /^\{.*\}$/
      list = tk_split_escstr(str)
    end
    if list.size == 1
      tk_tcl2ruby(list[0], nil, false)
    else
      list.collect{|token| tk_split_sublist(token, depth - 1)}
    end
  end

  def tk_split_list(str, depth=0, src_enc=true, dst_enc=true)
    return [] if str == ""
    tk_split_escstr(str).collect{|token|
      tk_split_sublist(token, depth - 1)
    }
  end

  def tk_split_simplelist(str, src_enc=true, dst_enc=true)
    return [] if str == ""
    list = []
    token = nil
    escape = false
    brace = 0
    str.split('').each {|c|
      if c == '\\' && !escape
        escape = true
        token = (token || "") << c if brace > 0
        next
      end
      brace += 1 if c == '{' && !escape
      brace -= 1 if c == '}' && !escape
      if brace == 0 && c == ' ' && !escape
        list << token.gsub(/^\{(.*)\}$/, '\1') if token
        token = nil
      else
        token = (token || "") << c
      end
      escape = false
    }
    list << token.gsub(/^\{(.*)\}$/, '\1') if token
    list
  end

  def array2tk_list(ary, enc=nil)
    ary.collect{|e|
      if e.kind_of? Array
        "{#{array2tk_list(e, enc)}}"
      elsif e.kind_of? Hash
        # "{#{e.to_a.collect{|ee| array2tk_list(ee)}.join(' ')}}"
        e.each{|k,v| tmp_ary << "-#{_get_eval_string(k)}" << v }
        array2tk_list(tmp_ary, enc)
      else
        s = _get_eval_string(e, enc)
        (s.index(/\s/) || s.size == 0)? "{#{s}}": s
      end
    }.join(" ")
  end
end

  private :tk_split_escstr, :tk_split_sublist
  private :tk_split_list, :tk_split_simplelist
  private :array2tk_list

  module_function :tk_split_escstr, :tk_split_sublist
  module_function :tk_split_list, :tk_split_simplelist
  module_function :array2tk_list

  private_class_method :tk_split_escstr, :tk_split_sublist
  private_class_method :tk_split_list, :tk_split_simplelist
#  private_class_method :array2tk_list

=begin
  ### --> definition is moved to TkUtil module
  def _symbolkey2str(keys)
    h = {}
    keys.each{|key,value| h[key.to_s] = value}
    h
  end
  private :_symbolkey2str
  module_function :_symbolkey2str
=end

=begin
  ### --> definition is moved to TkUtil module
  # def hash_kv(keys, enc_mode = nil, conf = [], flat = false)
  def hash_kv(keys, enc_mode = nil, conf = nil)
    # Hash {key=>val, key=>val, ... } or Array [ [key, val], [key, val], ... ]
    #     ==> Array ['-key', val, '-key', val, ... ]
    dst = []
    if keys and keys != None
      keys.each{|k, v|
        #dst.push("-#{k}")
        dst.push('-' + k.to_s)
        if v != None
          # v = _get_eval_string(v, enc_mode) if (enc_mode || flat)
          v = _get_eval_string(v, enc_mode) if enc_mode
          dst.push(v)
        end
      }
    end
    if conf
      conf + dst
    else
      dst
    end
  end
  private :hash_kv
  module_function :hash_kv
=end

=begin
  ### --> definition is moved to TkUtil module
  def bool(val)
    case val
    when "1", 1, 'yes', 'true'
      true
    else
      false
    end
  end

  def number(val)
    case val
    when /^-?\d+$/
      val.to_i
    when /^-?\d+\.?\d*(e[-+]?\d+)?$/
      val.to_f
    else
      fail(ArgumentError, "invalid value for Number:'#{val}'")
    end
  end
  def string(val)
    if val == "{}"
      ''
    elsif val[0] == ?{ && val[-1] == ?}
      val[1..-2]
    else
      val
    end
  end
  def num_or_str(val)
    begin
      number(val)
    rescue ArgumentError
      string(val)
    end
  end
=end

  def list(val, depth=0, enc=true)
    tk_split_list(val, depth, enc, enc)
  end
  def simplelist(val, src_enc=true, dst_enc=true)
    tk_split_simplelist(val, src_enc, dst_enc)
  end
  def window(val)
    if val =~ /^\./
      #Tk_WINDOWS[val]? Tk_WINDOWS[val] : _genobj_for_tkwidget(val)
      TkCore::INTERP.tk_windows[val]?
           TkCore::INTERP.tk_windows[val] : _genobj_for_tkwidget(val)
    else
      nil
    end
  end
  def image_obj(val)
    if val =~ /^i(_\d+_)?\d+$/
      TkImage::Tk_IMGTBL.mutex.synchronize{
        TkImage::Tk_IMGTBL[val]? TkImage::Tk_IMGTBL[val] : val
      }
    else
      val
    end
  end
  def procedure(val)
=begin
    if val =~ /^rb_out\S* (c(_\d+_)?\d+)/
      #Tk_CMDTBL[$1]
      #TkCore::INTERP.tk_cmd_tbl[$1]
      TkCore::INTERP.tk_cmd_tbl[$1].cmd
=end
    if val =~ /rb_out\S*(?:\s+(::\S*|[{](::.*)[}]|["](::.*)["]))? (c(_\d+_)?(\d+))/
      return TkCore::INTERP.tk_cmd_tbl[$4].cmd
    else
      #nil
      val
    end
  end
  private :bool, :number, :num_or_str, :num_or_nil, :string
  private :list, :simplelist, :window, :image_obj, :procedure
  module_function :bool, :number, :num_or_str, :num_or_nil, :string
  module_function :list, :simplelist, :window, :image_obj, :procedure

  if (RUBY_VERSION.split('.').map{|n| n.to_i} <=> [1,8,7]) < 0
    def slice_ary(ary, size)
      sliced = []
      wk_ary = ary.dup
      until wk_ary.size.zero?
        sub_ary = []
        size.times{ sub_ary << wk_ary.shift }
        yield(sub_ary) if block_given?
        sliced << sub_ary
      end
      (block_given?)? ary: sliced
    end
  else
    def slice_ary(ary, size, &b)
      if b
        ary.each_slice(size, &b)
      else
        ary.each_slice(size).to_a
      end
    end
  end
  private :slice_ary
  module_function :slice_ary

  def subst(str, *opts)
    # opts := :nobackslashes | :nocommands | novariables
    tk_call('subst',
            *(opts.collect{|opt|
                opt = opt.to_s
                (opt[0] == ?-)? opt: '-' << opt
              } << str))
  end

  def _toUTF8(str, encoding = nil)
    TkCore::INTERP._toUTF8(str, encoding)
  end
  def _fromUTF8(str, encoding = nil)
    TkCore::INTERP._fromUTF8(str, encoding)
  end
  private :_toUTF8, :_fromUTF8
  module_function :_toUTF8, :_fromUTF8

  def _callback_entry_class?(cls)
    cls <= Proc || cls <= Method || cls <= TkCallbackEntry
  end
  private :_callback_entry_class?
  module_function :_callback_entry_class?

  def _callback_entry?(obj)
    obj.kind_of?(Proc) || obj.kind_of?(Method) || obj.kind_of?(TkCallbackEntry)
  end
  private :_callback_entry?
  module_function :_callback_entry?

=begin
  ### --> definition is moved to TkUtil module
  def _get_eval_string(str, enc_mode = nil)
    return nil if str == None
    if str.kind_of?(TkObject)
      str = str.path
    elsif str.kind_of?(String)
      str = _toUTF8(str) if enc_mode
    elsif str.kind_of?(Symbol)
      str = str.id2name
      str = _toUTF8(str) if enc_mode
    elsif str.kind_of?(Hash)
      str = hash_kv(str, enc_mode).join(" ")
    elsif str.kind_of?(Array)
      str = array2tk_list(str)
      str = _toUTF8(str) if enc_mode
    elsif str.kind_of?(Proc)
      str = install_cmd(str)
    elsif str == nil
      str = ""
    elsif str == false
      str = "0"
    elsif str == true
      str = "1"
    elsif (str.respond_to?(:to_eval))
      str = str.to_eval()
      str = _toUTF8(str) if enc_mode
    else
      str = str.to_s() || ''
      unless str.kind_of? String
        fail RuntimeError, "fail to convert the object to a string"
      end
      str = _toUTF8(str) if enc_mode
    end
    return str
  end
=end
=begin
  def _get_eval_string(obj, enc_mode = nil)
    case obj
    when Numeric
      obj.to_s
    when String
      (enc_mode)? _toUTF8(obj): obj
    when Symbol
      (enc_mode)? _toUTF8(obj.id2name): obj.id2name
    when TkObject
      obj.path
    when Hash
      hash_kv(obj, enc_mode).join(' ')
    when Array
      (enc_mode)? _toUTF8(array2tk_list(obj)): array2tk_list(obj)
    when Proc, Method, TkCallbackEntry
      install_cmd(obj)
    when false
      '0'
    when true
      '1'
    when nil
      ''
    when None
      nil
    else
      if (obj.respond_to?(:to_eval))
        (enc_mode)? _toUTF8(obj.to_eval): obj.to_eval
      else
        begin
          obj = obj.to_s || ''
        rescue
          fail RuntimeError, "fail to convert object '#{obj}' to string"
        end
        (enc_mode)? _toUTF8(obj): obj
      end
    end
  end
  private :_get_eval_string
  module_function :_get_eval_string
=end

=begin
  ### --> definition is moved to TkUtil module
  def _get_eval_enc_str(obj)
    return obj if obj == None
    _get_eval_string(obj, true)
  end
  private :_get_eval_enc_str
  module_function :_get_eval_enc_str
=end

=begin
  ### --> obsolete
  def ruby2tcl(v, enc_mode = nil)
    if v.kind_of?(Hash)
      v = hash_kv(v)
      v.flatten!
      v.collect{|e|ruby2tcl(e, enc_mode)}
    else
      _get_eval_string(v, enc_mode)
    end
  end
  private :ruby2tcl
=end

=begin
  ### --> definition is moved to TkUtil module
  def _conv_args(args, enc_mode, *src_args)
    conv_args = []
    src_args.each{|arg|
      conv_args << _get_eval_string(arg, enc_mode) unless arg == None
      # if arg.kind_of?(Hash)
      # arg.each{|k, v|
      #   args << '-' + k.to_s
      #   args << _get_eval_string(v, enc_mode)
      # }
      # elsif arg != None
      #   args << _get_eval_string(arg, enc_mode)
      # end
    }
    args + conv_args
  end
  private :_conv_args
=end

  def _curr_cmd_id
    #id = format("c%.4d", Tk_IDs[0])
    id = "c" + TkCore::INTERP._ip_id_ + TkComm::Tk_IDs[0]
  end
  def _next_cmd_id
    TkComm::Tk_IDs.mutex.synchronize{
      id = _curr_cmd_id
      #Tk_IDs[0] += 1
      TkComm::Tk_IDs[0].succ!
      id
    }
  end
  private :_curr_cmd_id, :_next_cmd_id
  module_function :_curr_cmd_id, :_next_cmd_id

  def TkComm.install_cmd(cmd, local_cmdtbl=nil)
    return '' if cmd == ''
    begin
      ns = TkCore::INTERP._invoke_without_enc('namespace', 'current')
      ns = nil if ns == '::' # for backward compatibility
    rescue
      # probably, Tcl7.6
      ns = nil
    end
    id = _next_cmd_id
    #Tk_CMDTBL[id] = cmd
    if cmd.kind_of?(TkCallbackEntry)
      TkCore::INTERP.tk_cmd_tbl[id] = cmd
    else
      TkCore::INTERP.tk_cmd_tbl[id] = TkCore::INTERP.get_cb_entry(cmd)
    end
    @cmdtbl = [] unless defined? @cmdtbl
    TkUtil.untrust(@cmdtbl) unless @cmdtbl.tainted?
    @cmdtbl.push id

    if local_cmdtbl && local_cmdtbl.kind_of?(Array)
      begin
        local_cmdtbl << id
      rescue Exception
        # ignore
      end
    end

    #return Kernel.format("rb_out %s", id);
    if ns
      'rb_out' << TkCore::INTERP._ip_id_ << ' ' << ns << ' ' << id
    else
      'rb_out' << TkCore::INTERP._ip_id_ << ' ' << id
    end
  end
  def TkComm.uninstall_cmd(id, local_cmdtbl=nil)
    #id = $1 if /rb_out\S* (c(_\d+_)?\d+)/ =~ id
    id = $4 if id =~ /rb_out\S*(?:\s+(::\S*|[{](::.*)[}]|["](::.*)["]))? (c(_\d+_)?(\d+))/

    if local_cmdtbl && local_cmdtbl.kind_of?(Array)
      begin
        local_cmdtbl.delete(id)
      rescue Exception
        # ignore
      end
    end
    @cmdtbl.delete(id)

    #Tk_CMDTBL.delete(id)
    TkCore::INTERP.tk_cmd_tbl.delete(id)
  end
  # private :install_cmd, :uninstall_cmd
  # module_function :install_cmd, :uninstall_cmd
  def install_cmd(cmd)
    TkComm.install_cmd(cmd, @cmdtbl)
  end
  def uninstall_cmd(id)
    TkComm.uninstall_cmd(id, @cmdtbl)
  end

=begin
  def install_win(ppath,name=nil)
    if !name or name == ''
      #name = format("w%.4d", Tk_IDs[1])
      #Tk_IDs[1] += 1
      name = "w" + Tk_IDs[1]
      Tk_IDs[1].succ!
    end
    if name[0] == ?.
      @path = name.dup
    elsif !ppath or ppath == "."
      @path = Kernel.format(".%s", name);
    else
      @path = Kernel.format("%s.%s", ppath, name)
    end
    #Tk_WINDOWS[@path] = self
    TkCore::INTERP.tk_windows[@path] = self
  end
=end
  def install_win(ppath,name=nil)
    if name
      if name == ''
        raise ArgumentError, "invalid wiget-name '#{name}'"
      end
      if name[0] == ?.
        @path = '' + name
        @path.freeze
        return TkCore::INTERP.tk_windows[@path] = self
      end
    else
      Tk_IDs.mutex.synchronize{
        name = "w" + TkCore::INTERP._ip_id_ + Tk_IDs[1]
        Tk_IDs[1].succ!
      }
    end
    if !ppath or ppath == '.'
      @path = '.' + name
    else
      @path = ppath + '.' + name
    end
    @path.freeze
    TkCore::INTERP.tk_windows[@path] = self
  end

  def uninstall_win()
    #Tk_WINDOWS.delete(@path)
    TkCore::INTERP.tk_windows.delete(@path)
  end
  private :install_win, :uninstall_win

  def _epath(win)
    if win.kind_of?(TkObject)
      win.epath
    elsif win.respond_to?(:epath)
      win.epath
    else
      win
    end
  end
  private :_epath
end

# define TkComm module (step 2: event binding)
module TkComm
  include TkEvent
  extend TkEvent

  def tk_event_sequence(context)
    if context.kind_of? TkVirtualEvent
      context = context.path
    end
    if context.kind_of? Array
      context = context.collect{|ev|
        if ev.kind_of? TkVirtualEvent
          ev.path
        else
          ev
        end
      }.join("><")
    end
    if /,/ =~ context
      context = context.split(/\s*,\s*/).join("><")
    else
      context
    end
  end

  def _bind_core(mode, what, context, cmd, *args)
    id = install_bind(cmd, *args) if cmd
    begin
      tk_call_without_enc(*(what + ["<#{tk_event_sequence(context)}>",
                              mode + id]))
    rescue
      uninstall_cmd(id) if cmd
      fail
    end
  end

  def _bind(what, context, cmd, *args)
    _bind_core('', what, context, cmd, *args)
  end

  def _bind_append(what, context, cmd, *args)
    _bind_core('+', what, context, cmd, *args)
  end

  def _bind_remove(what, context)
    tk_call_without_enc(*(what + ["<#{tk_event_sequence(context)}>", '']))
  end

  def _bindinfo(what, context=nil)
    if context
      if TkCore::WITH_RUBY_VM  ### Ruby 1.9 !!!!
        enum_obj = tk_call_without_enc(*what+["<#{tk_event_sequence(context)}>"]).each_line
      else
        enum_obj = tk_call_without_enc(*what+["<#{tk_event_sequence(context)}>"])
      end
      enum_obj.collect {|cmdline|
=begin
        if cmdline =~ /^rb_out\S* (c(?:_\d+_)?\d+)\s+(.*)$/
          #[Tk_CMDTBL[$1], $2]
          [TkCore::INTERP.tk_cmd_tbl[$1], $2]
=end
        if cmdline =~ /rb_out\S*(?:\s+(::\S*|[{](::.*)[}]|["](::.*)["]))? (c(_\d+_)?(\d+))/
          [TkCore::INTERP.tk_cmd_tbl[$4], $5]
        else
          cmdline
        end
      }
    else
      tk_split_simplelist(tk_call_without_enc(*what)).collect!{|seq|
        l = seq.scan(/<*[^<>]+>*/).collect!{|subseq|
          case (subseq)
          when /^<<[^<>]+>>$/
            TkVirtualEvent.getobj(subseq[1..-2])
          when /^<[^<>]+>$/
            subseq[1..-2]
          else
            subseq.split('')
          end
        }.flatten
        (l.size == 1) ? l[0] : l
      }
    end
  end

  def _bind_core_for_event_class(klass, mode, what, context, cmd, *args)
    id = install_bind_for_event_class(klass, cmd, *args) if cmd
    begin
      tk_call_without_enc(*(what + ["<#{tk_event_sequence(context)}>",
                              mode + id]))
    rescue
      uninstall_cmd(id) if cmd
      fail
    end
  end

  def _bind_for_event_class(klass, what, context, cmd, *args)
    _bind_core_for_event_class(klass, '', what, context, cmd, *args)
  end

  def _bind_append_for_event_class(klass, what, context, cmd, *args)
    _bind_core_for_event_class(klass, '+', what, context, cmd, *args)
  end

  def _bind_remove_for_event_class(klass, what, context)
    _bind_remove(what, context)
  end

  def _bindinfo_for_event_class(klass, what, context=nil)
    _bindinfo(what, context)
  end

  private :tk_event_sequence
  private :_bind_core, :_bind, :_bind_append, :_bind_remove, :_bindinfo
  private :_bind_core_for_event_class, :_bind_for_event_class,
          :_bind_append_for_event_class, :_bind_remove_for_event_class,
          :_bindinfo_for_event_class

  #def bind(tagOrClass, context, cmd=Proc.new, *args)
  #  _bind(["bind", tagOrClass], context, cmd, *args)
  #  tagOrClass
  #end
  def bind(tagOrClass, context, *args)
    # if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
    if TkComm._callback_entry?(args[0]) || !block_given?
      cmd = args.shift
    else
      cmd = Proc.new
    end
    _bind(["bind", tagOrClass], context, cmd, *args)
    tagOrClass
  end

  #def bind_append(tagOrClass, context, cmd=Proc.new, *args)
  #  _bind_append(["bind", tagOrClass], context, cmd, *args)
  #  tagOrClass
  #end
  def bind_append(tagOrClass, context, *args)
    # if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
    if TkComm._callback_entry?(args[0]) || !block_given?
      cmd = args.shift
    else
      cmd = Proc.new
    end
    _bind_append(["bind", tagOrClass], context, cmd, *args)
    tagOrClass
  end

  def bind_remove(tagOrClass, context)
    _bind_remove(['bind', tagOrClass], context)
    tagOrClass
  end

  def bindinfo(tagOrClass, context=nil)
    _bindinfo(['bind', tagOrClass], context)
  end

  #def bind_all(context, cmd=Proc.new, *args)
  #  _bind(['bind', 'all'], context, cmd, *args)
  #  TkBindTag::ALL
  #end
  def bind_all(context, *args)
    # if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
    if TkComm._callback_entry?(args[0]) || !block_given?
      cmd = args.shift
    else
      cmd = Proc.new
    end
    _bind(['bind', 'all'], context, cmd, *args)
    TkBindTag::ALL
  end

  #def bind_append_all(context, cmd=Proc.new, *args)
  #  _bind_append(['bind', 'all'], context, cmd, *args)
  #  TkBindTag::ALL
  #end
  def bind_append_all(context, *args)
    # if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
    if TkComm._callback_entry?(args[0]) || !block_given?
      cmd = args.shift
    else
      cmd = Proc.new
    end
    _bind_append(['bind', 'all'], context, cmd, *args)
    TkBindTag::ALL
  end

  def bind_remove_all(context)
    _bind_remove(['bind', 'all'], context)
    TkBindTag::ALL
  end

  def bindinfo_all(context=nil)
    _bindinfo(['bind', 'all'], context)
  end
end


module TkCore
  include TkComm
  extend TkComm

  WITH_RUBY_VM  = Object.const_defined?(:RubyVM) && ::RubyVM.class == Class
  WITH_ENCODING = defined?(::Encoding.default_external) && true
  #WITH_ENCODING = Object.const_defined?(:Encoding) && ::Encoding.class == Class

  unless self.const_defined? :INTERP
    if self.const_defined? :IP_NAME
      name = IP_NAME.to_s
    else
      #name = nil
      name = $0
    end
    if self.const_defined? :IP_OPTS
      if IP_OPTS.kind_of?(Hash)
        opts = hash_kv(IP_OPTS).join(' ')
      else
        opts = IP_OPTS.to_s
      end
    else
      opts = ''
    end

    # RUN_EVENTLOOP_ON_MAIN_THREAD = true

    unless self.const_defined? :RUN_EVENTLOOP_ON_MAIN_THREAD
      if WITH_RUBY_VM ### check Ruby 1.9 !!!!!!!
        # *** NEED TO FIX ***
        case RUBY_PLATFORM
        when /cygwin/
          RUN_EVENTLOOP_ON_MAIN_THREAD = true
        when /darwin/ # MacOS X
=begin
          ip = TclTkIp.new(name, opts)
          if ip._invoke_without_enc('tk', 'windowingsystem') == 'aqua' &&
             (TclTkLib.get_version<=>[8,4,TclTkLib::RELEASE_TYPE::FINAL,6]) > 0
=end
          if TclTkLib::WINDOWING_SYSTEM == 'aqua' &&
             (TclTkLib.get_version<=>[8,4,TclTkLib::RELEASE_TYPE::FINAL,6]) > 0
            # *** KNOWN BUG ***
            #  Main event loop thread of TkAqua (> Tk8.4.9) must be the main
            #  application thread. So, ruby1.9 users must call Tk.mainloop on
            #  the main application thread.
            #
            # *** ADD (2009/05/10) ***
            #  In some cases (I don't know the description of conditions),
            #  TkAqua 8.4.7 has a same kind of hang-up trouble.
            #  So, if 8.4.7 or later, set RUN_EVENTLOOP_ON_MAIN_THREAD to true.
            #  When you want to control this mode, please call the following
            #  (set true/false as you want) before "require 'tk'".
            #  ----------------------------------------------------------
            #  module TkCore; RUN_EVENTLOOP_ON_MAIN_THREAD = true; end
            #  ----------------------------------------------------------
            #
            # *** ADD (2010/07/05) ***
            #  The value of TclTkLib::WINDOWING_SYSTEM is defined at compiling.
            #  If it is inconsistent with linked DLL, please call the following
            #  before "require 'tk'".
            #  ----------------------------------------------------------
            #  require 'tcltklib'
            #  module TclTkLib
            #    remove_const :WINDOWING_SYSTEM
            #    WINDOWING_SYSTEM = 'x11' # or 'aqua'
            #  end
            #  ----------------------------------------------------------
            #
            RUN_EVENTLOOP_ON_MAIN_THREAD = true
          else
            RUN_EVENTLOOP_ON_MAIN_THREAD = false
=begin
            ip.delete
            ip = nil
=end
          end
        else
          RUN_EVENTLOOP_ON_MAIN_THREAD = false
        end

      else # Ruby 1.8.x
        RUN_EVENTLOOP_ON_MAIN_THREAD = false
      end
    end

    if !WITH_RUBY_VM || RUN_EVENTLOOP_ON_MAIN_THREAD ### check Ruby 1.9 !!!!!!!
      INTERP = TclTkIp.new(name, opts) unless self.const_defined? :INTERP
    else
      INTERP_MUTEX = Mutex.new
      INTERP_ROOT_CHECK = ConditionVariable.new
      INTERP_THREAD = Thread.new{
        begin
          #Thread.current[:interp] = interp = TclTkIp.new(name, opts)
          interp = TclTkIp.new(name, opts)
        rescue => e
          Thread.current[:interp] = e
          raise e
        end

        interp.mainloop_abort_on_exception = true
        Thread.current.instance_variable_set("@interp", interp)

        status = [nil]
        def status.value
          self[0]
        end
        def status.value=(val)
          self[0] = val
        end

        Thread.current[:status] = status
        #sleep

        # like as 1.8, withdraw a root widget before calling Tk.mainloop
        interp._eval <<EOS
wm withdraw .
rename wm __wm_orig__
proc wm {subcmd win args} {
  set val [eval [list __wm_orig__ $subcmd $win] $args]
  if {[string equal $subcmd withdraw] && [string equal $win .]} {
    rename wm {}
    rename __wm_orig__ wm
  }
  return $val
}
proc __startup_rbtk_mainloop__ {args} {
  rename __startup_rbtk_mainloop__ {}
  if {[info command __wm_orig__] == "__wm_orig__"} {
    rename wm {}
    rename __wm_orig__ wm
    if [string equal [wm state .] withdrawn] {
      wm deiconify .
    }
  }
}
set __initial_state_of_rubytk__ 1
trace add variable __initial_state_of_rubytk__ unset __startup_rbtk_mainloop__

# complete initializing
ruby {TkCore::INTERP_THREAD[:interp] = TkCore::INTERP_THREAD.instance_variable_get('@interp')}
EOS

        begin
          begin
            #TclTkLib.mainloop_abort_on_exception = false
            #interp.mainloop_abort_on_exception = true
            #Thread.current[:interp] = interp
            #Thread.current[:status].value = TclTkLib.mainloop(true)
            Thread.current[:status].value = interp.mainloop(true)
          rescue SystemExit=>e
            Thread.current[:status].value = e
          rescue Exception=>e
            Thread.current[:status].value = e
            p e if $DEBUG
            retry if interp.has_mainwindow?
          ensure
            INTERP_MUTEX.synchronize{ INTERP_ROOT_CHECK.broadcast }
          end

          unless interp.deleted?
            #Thread.current[:status].value = TclTkLib.mainloop(false)
            Thread.current[:status].value = interp.mainloop(false)
          end

        ensure
          # interp must be deleted before the thread for interp is dead.
          # If not, raise Tcl_Panic on Tcl_AsyncDelete because async handler
          # deleted by the wrong thread.
          interp.delete
        end
      }

      # check a Tcl/Tk interpreter is initialized
      until INTERP_THREAD[:interp]
        # Thread.pass
        INTERP_THREAD.run
      end

      # INTERP_THREAD.run
      raise INTERP_THREAD[:interp] if INTERP_THREAD[:interp].kind_of? Exception

      # check an eventloop is running
      while INTERP_THREAD.alive? && TclTkLib.mainloop_thread?.nil?
        INTERP_THREAD.run
      end

      INTERP = INTERP_THREAD[:interp]
      INTERP_THREAD_STATUS = INTERP_THREAD[:status]

      # delete the interpreter and kill the eventloop thread at exit
      END{
        if INTERP_THREAD.alive?
          INTERP.delete
          INTERP_THREAD.kill
        end
      }

      # (for safety's sake) force the eventloop to run
      INTERP_THREAD.run
    end

    def INTERP.__getip
      self
    end
    def INTERP.default_master?
      true
    end

    INTERP.instance_eval{
      # @tk_cmd_tbl = TkUtil.untrust({})
      @tk_cmd_tbl =
        TkUtil.untrust(Hash.new{|hash, key|
                         fail IndexError, "unknown command ID '#{key}'"
                       })
      def @tk_cmd_tbl.[]=(idx,val)
        if self.has_key?(idx) && Thread.current.group != ThreadGroup::Default
          fail SecurityError,"cannot change the entried command"
        end
        super(idx,val)
      end

      @tk_windows = TkUtil.untrust({})

      @tk_table_list = TkUtil.untrust([])

      @init_ip_env  = TkUtil.untrust([])  # table of Procs
      @add_tk_procs = TkUtil.untrust([])  # table of [name, args, body]

      @force_default_encoding ||= TkUtil.untrust([false])
      @encoding ||= TkUtil.untrust([nil])
      def @encoding.to_s; self.join(nil); end

      @cb_entry_class = Class.new(TkCallbackEntry){
        class << self
          def inspect
            sprintf("#<Class(TkCallbackEntry):%0x>", self.__id__)
          end
          alias to_s inspect
        end

        def initialize(ip, cmd)
          @ip = ip
          @cmd = cmd
        end
        attr_reader :ip, :cmd
        def call(*args)
          @ip.cb_eval(@cmd, *args)
        end
        def inspect
          sprintf("#<cb_entry:%0x>", self.__id__)
        end
        alias to_s inspect
      }.freeze
    }

    def INTERP.cb_entry_class
      @cb_entry_class
    end
    def INTERP.tk_cmd_tbl
      @tk_cmd_tbl
    end
    def INTERP.tk_windows
      @tk_windows
    end

    class Tk_OBJECT_TABLE
      def initialize(id)
        @id = id
        @mutex = Mutex.new
      end
      def mutex
        @mutex
      end
      def method_missing(m, *args, &b)
        TkCore::INTERP.tk_object_table(@id).__send__(m, *args, &b)
      end
    end

    def INTERP.tk_object_table(id)
      @tk_table_list[id]
    end
    def INTERP.create_table
      id = @tk_table_list.size
      (tbl = {}).tainted? || TkUtil.untrust(tbl)
      @tk_table_list << tbl
#      obj = Object.new
#      obj.instance_eval <<-EOD
#        def self.method_missing(m, *args)
#         TkCore::INTERP.tk_object_table(#{id}).send(m, *args)
#        end
#      EOD
#      return obj
      Tk_OBJECT_TABLE.new(id)
    end

    def INTERP.get_cb_entry(cmd)
      @cb_entry_class.new(__getip, cmd).freeze
    end
    def INTERP.cb_eval(cmd, *args)
      TkUtil._get_eval_string(TkUtil.eval_cmd(cmd, *args))
    end

    def INTERP.init_ip_env(script = Proc.new)
      @init_ip_env << script
      script.call(self)
    end
    def INTERP.add_tk_procs(name, args = nil, body = nil)
      if name.kind_of?(Array)
        name.each{|param| self.add_tk_procs(*param)}
      else
        name = name.to_s
        @add_tk_procs << [name, args, body]
        self._invoke('proc', name, args, body) if args && body
      end
    end
    def INTERP.remove_tk_procs(*names)
      names.each{|name|
        name = name.to_s
        @add_tk_procs.delete_if{|elem|
          elem.kind_of?(Array) && elem[0].to_s == name
        }
        #self._invoke('rename', name, '')
        self.__invoke__('rename', name, '')
      }
    end
    def INTERP.init_ip_internal
      ip = self
      @init_ip_env.each{|script| script.call(ip)}
      @add_tk_procs.each{|name,args,body| ip._invoke('proc',name,args,body)}
    end
  end

  unless self.const_defined? :RUN_EVENTLOOP_ON_MAIN_THREAD
    ### Ruby 1.9 !!!!!!!!!!!!!!!!!!!!!!!!!!
    RUN_EVENTLOOP_ON_MAIN_THREAD = false
  end

  WIDGET_DESTROY_HOOK = '<WIDGET_DESTROY_HOOK>'
  INTERP._invoke_without_enc('event', 'add',
                             "<#{WIDGET_DESTROY_HOOK}>", '<Destroy>')
  INTERP._invoke_without_enc('bind', 'all', "<#{WIDGET_DESTROY_HOOK}>",
                             install_cmd(proc{|path|
                                unless TkCore::INTERP.deleted?
                                  begin
                                    if (widget=TkCore::INTERP.tk_windows[path])
                                      if widget.respond_to?(:__destroy_hook__)
                                        widget.__destroy_hook__
                                      end
                                    end
                                  rescue Exception=>e
                                      p e if $DEBUG
                                  end
                                end
                             }) << ' %W')

  INTERP.add_tk_procs(TclTkLib::FINALIZE_PROC_NAME, '',
                      "catch { bind all <#{WIDGET_DESTROY_HOOK}> {} }")

  INTERP.add_tk_procs('rb_out', 'ns args', <<-'EOL')
    if [regexp {^::} $ns] {
      set cmd {namespace eval $ns {ruby_cmd TkCore callback} $args}
    } else {
      set cmd {eval {ruby_cmd TkCore callback} $ns $args}
    }
    if {[set st [catch $cmd ret]] != 0} {
       #return -code $st $ret
       set idx [string first "\n\n" $ret]
       if {$idx > 0} {
          return -code $st \
                 -errorinfo [string range $ret [expr $idx + 2] \
                                               [string length $ret]] \
                 [string range $ret 0 [expr $idx - 1]]
       } else {
          return -code $st $ret
       }
    } else {
        return $ret
    }
  EOL
=begin
  INTERP.add_tk_procs('rb_out', 'args', <<-'EOL')
    if {[set st [catch {eval {ruby_cmd TkCore callback} $args} ret]] != 0} {
       #return -code $st $ret
       set idx [string first "\n\n" $ret]
       if {$idx > 0} {
          return -code $st \
                 -errorinfo [string range $ret [expr $idx + 2] \
                                               [string length $ret]] \
                 [string range $ret 0 [expr $idx - 1]]
       } else {
          return -code $st $ret
       }
    } else {
        return $ret
    }
  EOL
=end
=begin
  INTERP.add_tk_procs('rb_out', 'args', <<-'EOL')
    #regsub -all {\\} $args {\\\\} args
    #regsub -all {!} $args {\\!} args
    #regsub -all "{" $args "\\{" args
    regsub -all {(\\|!|\{|\})} $args {\\\1} args
    if {[set st [catch {ruby [format "TkCore.callback %%Q!%s!" $args]} ret]] != 0} {
       #return -code $st $ret
       set idx [string first "\n\n" $ret]
       if {$idx > 0} {
          return -code $st \
                 -errorinfo [string range $ret [expr $idx + 2] \
                                               [string length $ret]] \
                 [string range $ret 0 [expr $idx - 1]]
       } else {
          return -code $st $ret
       }
    } else {
        return $ret
    }
  EOL
=end

  at_exit{ INTERP.remove_tk_procs(TclTkLib::FINALIZE_PROC_NAME) }

  EventFlag = TclTkLib::EventFlag

  def callback_break
    fail TkCallbackBreak, "Tk callback returns 'break' status"
  end

  def callback_continue
    fail TkCallbackContinue, "Tk callback returns 'continue' status"
  end

  def callback_return
    fail TkCallbackReturn, "Tk callback returns 'return' status"
  end

  def TkCore.callback(*arg)
    begin
      if TkCore::INTERP.tk_cmd_tbl.kind_of?(Hash)
        #TkCore::INTERP.tk_cmd_tbl[arg.shift].call(*arg)
        normal_ret = false
        ret = catch(:IRB_EXIT) do  # IRB hack
          retval = TkCore::INTERP.tk_cmd_tbl[arg.shift].call(*arg)
          normal_ret = true
          retval
        end
        unless normal_ret
          # catch IRB_EXIT
          exit(ret)
        end
        ret
      end
    rescue SystemExit=>e
      exit(e.status)
    rescue Interrupt=>e
      fail(e)
    rescue Exception => e
      begin
        msg = _toUTF8(e.class.inspect) + ': ' +
              _toUTF8(e.message) + "\n" +
              "\n---< backtrace of Ruby side >-----\n" +
              _toUTF8(e.backtrace.join("\n")) +
              "\n---< backtrace of Tk side >-------"
        if TkCore::WITH_ENCODING
          msg.force_encoding('utf-8')
        else
          msg.instance_variable_set(:@encoding, 'utf-8')
        end
      rescue Exception
        msg = e.class.inspect + ': ' + e.message + "\n" +
              "\n---< backtrace of Ruby side >-----\n" +
              e.backtrace.join("\n") +
              "\n---< backtrace of Tk side >-------"
      end
      # TkCore::INTERP._set_global_var('errorInfo', msg)
      # fail(e)
      fail(e, msg)
    end
  end
=begin
  def TkCore.callback(arg_str)
    # arg = tk_split_list(arg_str)
    arg = tk_split_simplelist(arg_str)
    #_get_eval_string(TkUtil.eval_cmd(Tk_CMDTBL[arg.shift], *arg))
    #_get_eval_string(TkUtil.eval_cmd(TkCore::INTERP.tk_cmd_tbl[arg.shift],
    #                        *arg))
    # TkCore::INTERP.tk_cmd_tbl[arg.shift].call(*arg)
    begin
      TkCore::INTERP.tk_cmd_tbl[arg.shift].call(*arg)
    rescue Exception => e
      raise(e, e.class.inspect + ': ' + e.message + "\n" +
               "\n---< backtrace of Ruby side >-----\n" +
               e.backtrace.join("\n") +
               "\n---< backtrace of Tk side >-------")
    end
#=begin
#    cb_obj = TkCore::INTERP.tk_cmd_tbl[arg.shift]
#    unless $DEBUG
#      cb_obj.call(*arg)
#    else
#      begin
#       raise 'check backtrace'
#      rescue
#       # ignore backtrace before 'callback'
#       pos = -($!.backtrace.size)
#      end
#      begin
#       cb_obj.call(*arg)
#      rescue
#       trace = $!.backtrace
#       raise $!, "\n#{trace[0]}: #{$!.message} (#{$!.class})\n" +
#                 "\tfrom #{trace[1..pos].join("\n\tfrom ")}"
#      end
#    end
#=end
  end
=end

  def load_cmd_on_ip(tk_cmd)
    bool(tk_call('auto_load', tk_cmd))
  end

  def after(ms, cmd=Proc.new)
    cmdid = install_cmd(proc{ret = cmd.call;uninstall_cmd(cmdid); ret})
    after_id = tk_call_without_enc("after",ms,cmdid)
    after_id.instance_variable_set('@cmdid', cmdid)
    after_id
  end
=begin
  def after(ms, cmd=Proc.new)
    crit_bup = Thread.critical
    Thread.critical = true

    myid = _curr_cmd_id
    cmdid = install_cmd(proc{ret = cmd.call;uninstall_cmd(myid); ret})

    Thread.critical = crit_bup

    tk_call_without_enc("after",ms,cmdid)  # return id
#    return
#    if false #defined? Thread
#      Thread.start do
#       ms = Float(ms)/1000
#       ms = 10 if ms == 0
#       sleep ms/1000
#       cmd.call
#      end
#    else
#      cmdid = install_cmd(cmd)
#      tk_call("after",ms,cmdid)
#    end
  end
=end

  def after_idle(cmd=Proc.new)
    cmdid = install_cmd(proc{ret = cmd.call;uninstall_cmd(cmdid); ret})
    after_id = tk_call_without_enc('after','idle',cmdid)
    after_id.instance_variable_set('@cmdid', cmdid)
    after_id
  end
=begin
  def after_idle(cmd=Proc.new)
    crit_bup = Thread.critical
    Thread.critical = true

    myid = _curr_cmd_id
    cmdid = install_cmd(proc{ret = cmd.call;uninstall_cmd(myid); ret})

    Thread.critical = crit_bup

    tk_call_without_enc('after','idle',cmdid)
  end
=end

  def after_cancel(afterId)
    tk_call_without_enc('after','cancel',afterId)
    if (cmdid = afterId.instance_variable_get('@cmdid'))
      afterId.instance_variable_set('@cmdid', nil)
      uninstall_cmd(cmdid)
    end
    afterId
  end

  def windowingsystem
    tk_call_without_enc('tk', 'windowingsystem')
  end

  def scaling(scale=nil)
    if scale
      tk_call_without_enc('tk', 'scaling', scale)
    else
      Float(number(tk_call_without_enc('tk', 'scaling')))
    end
  end
  def scaling_displayof(win, scale=nil)
    if scale
      tk_call_without_enc('tk', 'scaling', '-displayof', win, scale)
    else
      Float(number(tk_call_without_enc('tk', '-displayof', win, 'scaling')))
    end
  end

  def inactive
    Integer(tk_call_without_enc('tk', 'inactive'))
  end
  def inactive_displayof(win)
    Integer(tk_call_without_enc('tk', 'inactive', '-displayof', win))
  end
  def reset_inactive
    tk_call_without_enc('tk', 'inactive', 'reset')
  end
  def reset_inactive_displayof(win)
    tk_call_without_enc('tk', 'inactive', '-displayof', win, 'reset')
  end

  def appname(name=None)
    tk_call('tk', 'appname', name)
  end

  def appsend_deny
    tk_call('rename', 'send', '')
  end

  def appsend(interp, async, *args)
    if $SAFE >= 4
      fail SecurityError, "cannot send Tk commands at level 4"
    elsif $SAFE >= 1 && args.find{|obj| obj.tainted?}
      fail SecurityError, "cannot send tainted Tk commands at level #{$SAFE}"
    end
    if async != true && async != false && async != nil
      args.unshift(async)
      async = false
    end
    if async
      tk_call('send', '-async', '--', interp, *args)
    else
      tk_call('send', '--', interp, *args)
    end
  end

  def rb_appsend(interp, async, *args)
    if $SAFE >= 4
      fail SecurityError, "cannot send Ruby commands at level 4"
    elsif $SAFE >= 1 && args.find{|obj| obj.tainted?}
      fail SecurityError, "cannot send tainted Ruby commands at level #{$SAFE}"
    end
    if async != true && async != false && async != nil
      args.unshift(async)
      async = false
    end
    #args = args.collect!{|c| _get_eval_string(c).gsub(/[\[\]$"]/, '\\\\\&')}
    args = args.collect!{|c| _get_eval_string(c).gsub(/[\[\]$"\\]/, '\\\\\&')}
    # args.push(').to_s"')
    # appsend(interp, async, 'ruby "(', *args)
    args.push('}.call)"')
    appsend(interp, async, 'ruby "TkComm._get_eval_string(proc{', *args)
  end

  def appsend_displayof(interp, win, async, *args)
    if $SAFE >= 4
      fail SecurityError, "cannot send Tk commands at level 4"
    elsif $SAFE >= 1 && args.find{|obj| obj.tainted?}
      fail SecurityError, "cannot send tainted Tk commands at level #{$SAFE}"
    end
    win = '.' if win == nil
    if async != true && async != false && async != nil
      args.unshift(async)
      async = false
    end
    if async
      tk_call('send', '-async', '-displayof', win, '--', interp, *args)
    else
      tk_call('send', '-displayor', win, '--', interp, *args)
    end
  end

  def rb_appsend_displayof(interp, win, async, *args)
    if $SAFE >= 4
      fail SecurityError, "cannot send Ruby commands at level 4"
    elsif $SAFE >= 1 && args.find{|obj| obj.tainted?}
      fail SecurityError, "cannot send tainted Ruby commands at level #{$SAFE}"
    end
    win = '.' if win == nil
    if async != true && async != false && async != nil
      args.unshift(async)
      async = false
    end
    #args = args.collect!{|c| _get_eval_string(c).gsub(/[\[\]$"]/, '\\\\\&')}
    args = args.collect!{|c| _get_eval_string(c).gsub(/[\[\]$"\\]/, '\\\\\&')}
    # args.push(').to_s"')
    # appsend_displayof(interp, win, async, 'ruby "(', *args)
    args.push('}.call)"')
    appsend(interp, win, async, 'ruby "TkComm._get_eval_string(proc{', *args)
  end

  def info(*args)
    tk_call('info', *args)
  end

  def mainloop(check_root = true)
    if !TkCore::WITH_RUBY_VM
      TclTkLib.mainloop(check_root)

    elsif TkCore::RUN_EVENTLOOP_ON_MAIN_THREAD
      # if TclTkLib::WINDOWING_SYSTEM == 'aqua' &&
      #if TkCore::INTERP._invoke_without_enc('tk','windowingsystem')=='aqua' &&
      #    Thread.current != Thread.main &&
      #    (TclTkLib.get_version <=> [8,4,TclTkLib::RELEASE_TYPE::FINAL,9]) > 0
      #  raise RuntimeError,
      #       "eventloop on TkAqua ( > Tk8.4.9 ) works on the main thread only"
      #end
      if Thread.current != Thread.main
        raise RuntimeError, "Tk.mainloop is allowed on the main thread only"
      end
      TclTkLib.mainloop(check_root)

    else ### Ruby 1.9 !!!!!
      unless TkCore::INTERP.default_master?
        # [MultiTkIp] slave interp ?
        return TkCore::INTERP._thread_tkwait('window', '.') if check_root
      end

      # like as 1.8, withdraw a root widget before calling Tk.mainloop
      TkCore::INTERP._eval_without_enc('catch {unset __initial_state_of_rubytk__}')
      INTERP_THREAD.run

      begin
        TclTkLib.set_eventloop_window_mode(true)

        # force run the eventloop
        TkCore::INTERP._eval_without_enc('update')
        TkCore::INTERP._eval_without_enc('catch {set __initial_state_of_rubytk__}')
        INTERP_THREAD.run
        if check_root
          INTERP_MUTEX.synchronize{
            INTERP_ROOT_CHECK.wait(INTERP_MUTEX)
            status = INTERP_THREAD_STATUS.value
            if status && TkCore::INTERP.default_master?
              INTERP_THREAD_STATUS.value = nil if $SAFE < 4
              raise status if status.kind_of?(Exception)
            end
          }
        else
          # INTERP_THREAD.value
          begin
            INTERP_THREAD.value
          rescue Exception => e
            raise e
          end
        end
      rescue Exception => e
        raise e
      ensure
        TclTkLib.set_eventloop_window_mode(false)
      end
    end
  end

  def mainloop_thread?
    # true  : current thread is mainloop
    # nil   : there is no mainloop
    # false : mainloop is running on the other thread
    #         ( At then, it is dangerous to call Tk interpreter directly. )
    if !TkCore::WITH_RUBY_VM || TkCore::RUN_EVENTLOOP_ON_MAIN_THREAD
      ### Ruby 1.9 !!!!!!!!!!!
      TclTkLib.mainloop_thread?
    else
      Thread.current == INTERP_THREAD
    end
  end

  def mainloop_exist?
    TclTkLib.mainloop_thread? != nil
  end

  def is_mainloop?
    TclTkLib.mainloop_thread? == true
  end

  def mainloop_watchdog(check_root = true)
    # watchdog restarts mainloop when mainloop is dead
    TclTkLib.mainloop_watchdog(check_root)
  end

  def do_one_event(flag = TclTkLib::EventFlag::ALL)
    TclTkLib.do_one_event(flag)
  end

  def set_eventloop_tick(timer_tick)
    TclTkLib.set_eventloop_tick(timer_tick)
  end

  def get_eventloop_tick()
    TclTkLib.get_eventloop_tick
  end

  def set_no_event_wait(wait)
    TclTkLib.set_no_even_wait(wait)
  end

  def get_no_event_wait()
    TclTkLib.get_no_eventloop_wait
  end

  def set_eventloop_weight(loop_max, no_event_tick)
    TclTkLib.set_eventloop_weight(loop_max, no_event_tick)
  end

  def get_eventloop_weight()
    TclTkLib.get_eventloop_weight
  end

  def restart(app_name = nil, keys = {})
    TkCore::INTERP.init_ip_internal

    tk_call('set', 'argv0', app_name) if app_name
    if keys.kind_of?(Hash)
      # tk_call('set', 'argc', keys.size * 2)
      tk_call('set', 'argv', hash_kv(keys).join(' '))
    end

    INTERP.restart
    nil
  end

  def event_generate(win, context, keys=nil)
    #win = win.path if win.kind_of?(TkObject)
    if context.kind_of?(TkEvent::Event)
      context.generate(win, ((keys)? keys: {}))
    elsif keys
      tk_call_without_enc('event', 'generate', win,
                          "<#{tk_event_sequence(context)}>",
                          *hash_kv(keys, true))
    else
      tk_call_without_enc('event', 'generate', win,
                          "<#{tk_event_sequence(context)}>")
    end
    nil
  end

  def messageBox(keys)
    tk_call('tk_messageBox', *hash_kv(keys))
  end

  def getOpenFile(keys = nil)
    tk_call('tk_getOpenFile', *hash_kv(keys))
  end
  def getMultipleOpenFile(keys = nil)
    simplelist(tk_call('tk_getOpenFile', '-multiple', '1', *hash_kv(keys)))
  end

  def getSaveFile(keys = nil)
    tk_call('tk_getSaveFile', *hash_kv(keys))
  end
  def getMultipleSaveFile(keys = nil)
    simplelist(tk_call('tk_getSaveFile', '-multiple', '1', *hash_kv(keys)))
  end

  def chooseColor(keys = nil)
    tk_call('tk_chooseColor', *hash_kv(keys))
  end

  def chooseDirectory(keys = nil)
    tk_call('tk_chooseDirectory', *hash_kv(keys))
  end

  def _ip_eval_core(enc_mode, cmd_string)
    case enc_mode
    when nil
      res = INTERP._eval(cmd_string)
    when false
      res = INTERP._eval_without_enc(cmd_string)
    when true
      res = INTERP._eval_with_enc(cmd_string)
    end
    if  INTERP._return_value() != 0
      fail RuntimeError, res, error_at
    end
    return res
  end
  private :_ip_eval_core

  def ip_eval(cmd_string)
    _ip_eval_core(nil, cmd_string)
  end

  def ip_eval_without_enc(cmd_string)
    _ip_eval_core(false, cmd_string)
  end

  def ip_eval_with_enc(cmd_string)
    _ip_eval_core(true, cmd_string)
  end

  def _ip_invoke_core(enc_mode, *args)
    case enc_mode
    when false
      res = INTERP._invoke_without_enc(*args)
    when nil
      res = INTERP._invoke(*args)
    when true
      res = INTERP._invoke_with_enc(*args)
    end
    if  INTERP._return_value() != 0
      fail RuntimeError, res, error_at
    end
    return res
  end
  private :_ip_invoke_core

  def ip_invoke(*args)
    _ip_invoke_core(nil, *args)
  end

  def ip_invoke_without_enc(*args)
    _ip_invoke_core(false, *args)
  end

  def ip_invoke_with_enc(*args)
    _ip_invoke_core(true, *args)
  end

  def _tk_call_core(enc_mode, *args)
    ### puts args.inspect if $DEBUG
    #args.collect! {|x|ruby2tcl(x, enc_mode)}
    #args.compact!
    #args.flatten!
    args = _conv_args([], enc_mode, *args)
    puts 'invoke args => ' + args.inspect if $DEBUG
    ### print "=> ", args.join(" ").inspect, "\n" if $DEBUG
    begin
      # res = TkUtil.untrust(INTERP._invoke(*args))
      # res = INTERP._invoke(enc_mode, *args)
      res = _ip_invoke_core(enc_mode, *args)
      # >>>>>  _invoke returns a TAINTED string  <<<<<
    rescue NameError => err
      # err = $!
      begin
        args.unshift "unknown"
        #res = TkUtil.untrust(INTERP._invoke(*args))
        #res = INTERP._invoke(enc_mode, *args)
        res = _ip_invoke_core(enc_mode, *args)
        # >>>>>  _invoke returns a TAINTED string  <<<<<
      rescue StandardError => err2
        fail err2 unless /^invalid command/ =~ err2.message
        fail err
      end
    end
    if  INTERP._return_value() != 0
      fail RuntimeError, res, error_at
    end
    ### print "==> ", res.inspect, "\n" if $DEBUG
    return res
  end
  private :_tk_call_core

  def tk_call(*args)
    _tk_call_core(nil, *args)
  end

  def tk_call_without_enc(*args)
    _tk_call_core(false, *args)
  end

  def tk_call_with_enc(*args)
    _tk_call_core(true, *args)
  end

  def _tk_call_to_list_core(depth, arg_enc, val_enc, *args)
    args = _conv_args([], arg_enc, *args)
    val = _tk_call_core(false, *args)
    if !depth.kind_of?(Integer) || depth == 0
      tk_split_simplelist(val, false, val_enc)
    else
      tk_split_list(val, depth, false, val_enc)
    end
  end
  #private :_tk_call_to_list_core

  def tk_call_to_list(*args)
    _tk_call_to_list_core(-1, nil, true, *args)
  end

  def tk_call_to_list_without_enc(*args)
    _tk_call_to_list_core(-1, false, false, *args)
  end

  def tk_call_to_list_with_enc(*args)
    _tk_call_to_list_core(-1, true, true, *args)
  end

  def tk_call_to_simplelist(*args)
    _tk_call_to_list_core(0, nil, true, *args)
  end

  def tk_call_to_simplelist_without_enc(*args)
    _tk_call_to_list_core(0, false, false, *args)
  end

  def tk_call_to_simplelist_with_enc(*args)
    _tk_call_to_list_core(0, true, true, *args)
  end
end


module Tk
  include TkCore
  extend Tk

  TCL_VERSION = INTERP._invoke_without_enc("info", "tclversion").freeze
  TCL_PATCHLEVEL = INTERP._invoke_without_enc("info", "patchlevel").freeze

  major, minor = TCL_VERSION.split('.')
  TCL_MAJOR_VERSION = major.to_i
  TCL_MINOR_VERSION = minor.to_i

  TK_VERSION  = INTERP._invoke_without_enc("set", "tk_version").freeze
  TK_PATCHLEVEL  = INTERP._invoke_without_enc("set", "tk_patchLevel").freeze

  major, minor = TK_VERSION.split('.')
  TK_MAJOR_VERSION = major.to_i
  TK_MINOR_VERSION = minor.to_i

  JAPANIZED_TK = (INTERP._invoke_without_enc("info", "commands",
                                             "kanji") != "").freeze

  def Tk.const_missing(sym)
    case(sym)
    when :TCL_LIBRARY
      INTERP._invoke_without_enc('global', 'tcl_library')
      INTERP._invoke("set", "tcl_library").freeze

    when :TK_LIBRARY
      INTERP._invoke_without_enc('global', 'tk_library')
      INTERP._invoke("set", "tk_library").freeze

    when :LIBRARY
      INTERP._invoke("info", "library").freeze

    #when :PKG_PATH, :PACKAGE_PATH, :TCL_PACKAGE_PATH
    #  INTERP._invoke_without_enc('global', 'tcl_pkgPath')
    #  tk_split_simplelist(INTERP._invoke('set', 'tcl_pkgPath'))

    #when :LIB_PATH, :LIBRARY_PATH, :TCL_LIBRARY_PATH
    #  INTERP._invoke_without_enc('global', 'tcl_libPath')
    #  tk_split_simplelist(INTERP._invoke('set', 'tcl_libPath'))

    when :PLATFORM, :TCL_PLATFORM
      if $SAFE >= 4
        fail SecurityError, "can't get #{sym} when $SAFE >= 4"
      end
      INTERP._invoke_without_enc('global', 'tcl_platform')
      Hash[*tk_split_simplelist(INTERP._invoke_without_enc('array', 'get',
                                                           'tcl_platform'))]

    when :ENV
      INTERP._invoke_without_enc('global', 'env')
      Hash[*tk_split_simplelist(INTERP._invoke('array', 'get', 'env'))]

    #when :AUTO_PATH   #<===
    #  tk_split_simplelist(INTERP._invoke('set', 'auto_path'))

    #when :AUTO_OLDPATH
    #  tk_split_simplelist(INTERP._invoke('set', 'auto_oldpath'))

    when :AUTO_INDEX
      INTERP._invoke_without_enc('global', 'auto_index')
      Hash[*tk_split_simplelist(INTERP._invoke('array', 'get', 'auto_index'))]

    when :PRIV, :PRIVATE, :TK_PRIV
      priv = {}
      if INTERP._invoke_without_enc('info', 'vars', 'tk::Priv') != ""
        var_nam = 'tk::Priv'
      else
        var_nam = 'tkPriv'
      end
      INTERP._invoke_without_enc('global', var_nam)
      Hash[*tk_split_simplelist(INTERP._invoke('array', 'get',
                                               var_nam))].each{|k,v|
        k.freeze
        case v
        when /^-?\d+$/
          priv[k] = v.to_i
        when /^-?\d+\.?\d*(e[-+]?\d+)?$/
          priv[k] = v.to_f
        else
          priv[k] = v.freeze
        end
      }
      priv

    else
      raise NameError, 'uninitialized constant Tk::' + sym.id2name
    end
  end

  def Tk.errorInfo
    INTERP._invoke_without_enc('global', 'errorInfo')
    INTERP._invoke_without_enc('set', 'errorInfo')
  end

  def Tk.errorCode
    INTERP._invoke_without_enc('global', 'errorCode')
    code = tk_split_simplelist(INTERP._invoke_without_enc('set', 'errorCode'))
    case code[0]
    when 'CHILDKILLED', 'CHILDSTATUS', 'CHILDSUSP'
      begin
        pid = Integer(code[1])
        code[1] = pid
      rescue
      end
    end
    code
  end

  def Tk.has_mainwindow?
    INTERP.has_mainwindow?
  end

  def root
    Tk::Root.new
  end

  def Tk.load_tclscript(file, enc=nil)
    if enc
      # TCL_VERSION >= 8.5
      tk_call('source', '-encoding', enc, file)
    else
      tk_call('source', file)
    end
  end

  def Tk.load_tcllibrary(file, pkg_name=None, interp=None)
    tk_call('load', file, pkg_name, interp)
  end

  def Tk.unload_tcllibrary(*args)
    if args[-1].kind_of?(Hash)
      keys = _symbolkey2str(args.pop)
      nocomp = (keys['nocomplain'])? '-nocomplain': None
      keeplib = (keys['keeplibrary'])? '-keeplibrary': None
      tk_call('unload', nocomp, keeplib, '--', *args)
    else
      tk_call('unload', *args)
    end
  end

  def Tk.pkgconfig_list(mod)
    # Tk8.5 feature
    if mod.kind_of?(Module)
      if mod.respond_to?(:package_name)
        pkgname = mod.package_name
      elsif mod.const_defined?(:PACKAGE_NAME)
        pkgname = mod::PACKAGE_NAME
      else
        fail NotImplementedError, 'may not be a module for a Tcl extension'
      end
    else
      pkgname = mod.to_s
    end

    pkgname = '::' << pkgname unless pkgname =~ /^::/

    tk_split_list(tk_call(pkgname + '::pkgconfig', 'list'))
  end

  def Tk.pkgconfig_get(mod, key)
    # Tk8.5 feature
    if mod.kind_of?(Module)
      if mod.respond_to?(:package_name)
        pkgname = mod.package_name
      else
        fail NotImplementedError, 'may not be a module for a Tcl extension'
      end
    else
      pkgname = mod.to_s
    end

    pkgname = '::' << pkgname unless pkgname =~ /^::/

    tk_call(pkgname + '::pkgconfig', 'get', key)
  end

  def Tk.tcl_pkgconfig_list
    # Tk8.5 feature
    Tk.pkgconfig_list('::tcl')
  end

  def Tk.tcl_pkgconfig_get(key)
    # Tk8.5 feature
    Tk.pkgconfig_get('::tcl', key)
  end

  def Tk.tk_pkgconfig_list
    # Tk8.5 feature
    Tk.pkgconfig_list('::tk')
  end

  def Tk.tk_pkgconfig_get(key)
    # Tk8.5 feature
    Tk.pkgconfig_get('::tk', key)
  end

  def Tk.bell(nice = false)
    if nice
      tk_call_without_enc('bell', '-nice')
    else
      tk_call_without_enc('bell')
    end
    nil
  end

  def Tk.bell_on_display(win, nice = false)
    if nice
      tk_call_without_enc('bell', '-displayof', win, '-nice')
    else
      tk_call_without_enc('bell', '-displayof', win)
    end
    nil
  end

  def Tk.destroy(*wins)
    #tk_call_without_enc('destroy', *wins)
    tk_call_without_enc('destroy', *(wins.collect{|win|
                                       if win.kind_of?(TkWindow)
                                         win.epath
                                       else
                                         win
                                       end
                                     }))
  end

  def Tk.exit
    TkCore::INTERP.has_mainwindow? && tk_call_without_enc('destroy', '.')
  end

  ################################################

  def Tk.sleep(ms = nil, id = nil)
    if id
      var = (id.kind_of?(TkVariable))? id: TkVarAccess.new(id.to_s)
    else
      var = TkVariable.new
    end

    var.value = tk_call_without_enc('after', ms, proc{ var.value = 0 }) if ms
    var.thread_wait
    ms
  end

  def Tk.wakeup(id)
    ((id.kind_of?(TkVariable))? id: TkVarAccess.new(id.to_s)).value = 0
    nil
  end

  ################################################

  def Tk.pack(*args)
    TkPack.configure(*args)
  end
  def Tk.pack_forget(*args)
    TkPack.forget(*args)
  end
  def Tk.unpack(*args)
    TkPack.forget(*args)
  end

  def Tk.grid(*args)
    TkGrid.configure(*args)
  end
  def Tk.grid_forget(*args)
    TkGrid.forget(*args)
  end
  def Tk.ungrid(*args)
    TkGrid.forget(*args)
  end

  def Tk.place(*args)
    TkPlace.configure(*args)
  end
  def Tk.place_forget(*args)
    TkPlace.forget(*args)
  end
  def Tk.unplace(*args)
    TkPlace.forget(*args)
  end

  def Tk.update(idle=nil)
    if idle
      tk_call_without_enc('update', 'idletasks')
    else
      tk_call_without_enc('update')
    end
  end
  def Tk.update_idletasks
    update(true)
  end
  def update(idle=nil)
    # only for backward compatibility (This never be recommended to use)
    Tk.update(idle)
    self
  end

  # NOTE::
  #   If no eventloop-thread is running, "thread_update" method is same
  #   to "update" method. Else, "thread_update" method waits to complete
  #   idletask operation on the eventloop-thread.
  def Tk.thread_update(idle=nil)
    if idle
      tk_call_without_enc('thread_update', 'idletasks')
    else
      tk_call_without_enc('thread_update')
    end
  end
  def Tk.thread_update_idletasks
    thread_update(true)
  end

  def Tk.lower_window(win, below=None)
    tk_call('lower', _epath(win), _epath(below))
    nil
  end
  def Tk.raise_window(win, above=None)
    tk_call('raise', _epath(win), _epath(above))
    nil
  end

  def Tk.current_grabs(win = nil)
    if win
      window(tk_call_without_enc('grab', 'current', win))
    else
      tk_split_list(tk_call_without_enc('grab', 'current'))
    end
  end

  def Tk.focus(display=nil)
    if display == nil
      window(tk_call_without_enc('focus'))
    else
      window(tk_call_without_enc('focus', '-displayof', display))
    end
  end

  def Tk.focus_to(win, force=false)
    if force
      tk_call_without_enc('focus', '-force', win)
    else
      tk_call_without_enc('focus', win)
    end
  end

  def Tk.focus_lastfor(win)
    window(tk_call_without_enc('focus', '-lastfor', win))
  end

  def Tk.focus_next(win)
    TkManageFocus.next(win)
  end

  def Tk.focus_prev(win)
    TkManageFocus.prev(win)
  end

  def Tk.strictMotif(mode=None)
    bool(tk_call_without_enc('set', 'tk_strictMotif', mode))
  end

  def Tk.show_kinsoku(mode='both')
    begin
      if /^8\.*/ === TK_VERSION  && JAPANIZED_TK
        tk_split_simplelist(tk_call('kinsoku', 'show', mode))
      end
    rescue
    end
  end
  def Tk.add_kinsoku(chars, mode='both')
    begin
      if /^8\.*/ === TK_VERSION  && JAPANIZED_TK
        tk_split_simplelist(tk_call('kinsoku', 'add', mode,
                                    *(chars.split(''))))
      else
        []
      end
    rescue
      []
    end
  end
  def Tk.delete_kinsoku(chars, mode='both')
    begin
      if /^8\.*/ === TK_VERSION  && JAPANIZED_TK
        tk_split_simplelist(tk_call('kinsoku', 'delete', mode,
                            *(chars.split(''))))
      end
    rescue
    end
  end

  def Tk.toUTF8(str, encoding = nil)
    _toUTF8(str, encoding)
  end

  def Tk.fromUTF8(str, encoding = nil)
    _fromUTF8(str, encoding)
  end
end

###########################################
#  string with Tcl's encoding
###########################################
module Tk
  def Tk.subst_utf_backslash(str)
    Tk::EncodedString.subst_utf_backslash(str)
  end
  def Tk.subst_tk_backslash(str)
    Tk::EncodedString.subst_tk_backslash(str)
  end
  def Tk.utf_to_backslash_sequence(str)
    Tk::EncodedString.utf_to_backslash_sequence(str)
  end
  def Tk.utf_to_backslash(str)
    Tk::EncodedString.utf_to_backslash_sequence(str)
  end
  def Tk.to_backslash_sequence(str)
    Tk::EncodedString.to_backslash_sequence(str)
  end
end


###########################################
#  convert kanji string to/from utf-8
###########################################
if (/^(8\.[1-9]|9\.|[1-9][0-9])/ =~ Tk::TCL_VERSION && !Tk::JAPANIZED_TK)
  module Tk
    module Encoding
      extend Encoding

      TkCommandNames = ['encoding'.freeze].freeze

      #############################################

      if TkCore::WITH_ENCODING ### Ruby 1.9
        RubyEncoding = ::Encoding

        # for saving GC cost
        #ENCNAMES_CMD = ['encoding'.freeze, 'names'.freeze]
        BINARY_NAME  = 'binary'.freeze
        UTF8_NAME    = 'utf-8'.freeze
        DEFAULT_EXTERNAL_NAME = RubyEncoding.default_external.name.freeze
        DEFAULT_INTERNAL_NAME = RubyEncoding.default_internal.name.freeze rescue nil

        BINARY  = RubyEncoding.find(BINARY_NAME)
        UNKNOWN = RubyEncoding.find('ASCII-8BIT')

        ### start of creating ENCODING_TABLE
        ENCODING_TABLE = TkCore::INTERP.encoding_table
=begin
        ENCODING_TABLE = {
          'binary'       => BINARY,
          # 'UNKNOWN-8BIT' => UNKNOWN,
        }

        list = TkCore::INTERP._invoke_without_enc(ENCNAMES_CMD[0],
                                                  ENCNAMES_CMD[1])
        TkCore::INTERP._split_tklist(list).each{|name|
          begin
            enc = RubyEncoding.find(name)
          rescue ArgumentError
            case name
            when 'identity'
              enc = BINARY
            when 'shiftjis'
              enc = RubyEncoding.find('Shift_JIS')
            when 'unicode'
              enc = RubyEncoding.find('UTF-8')
              #if Tk.tk_call('set', 'tcl_platform(byteOrder)') =='littleEndian'
              #  enc = RubyEncoding.find('UTF-16LE')
              #else
              #  enc = RubyEncoding.find('UTF-16BE')
              #end
            when 'symbol'
              # single byte data
              enc = RubyEncoding.find('ASCII-8BIT') ### ???
            else
              # unsupported on Ruby, but supported on Tk
              enc = TkCore::INTERP.create_dummy_encoding_for_tk(name)
            end
          end
          ENCODING_TABLE[name.freeze] = enc
        }
=end
=begin
        def ENCODING_TABLE.get_name(enc)
          orig_enc = enc

          # unles enc, use system default
          #  1st: Ruby/Tk default encoding
          #  2nd: Tcl/Tk default encoding
          #  3rd: Ruby's default_external
          enc ||= TkCore::INTERP.encoding
          enc ||= TclTkLib.encoding_system
          enc ||= DEFAULT_EXTERNAL_NAME

          if enc.kind_of?(RubyEncoding)
            # Ruby's Encoding object
            if (name = self.key(enc))
              return name
            end

            # Is it new ?
            list = TkCore::INTERP._invoke_without_enc(ENCNAMES_CMD[0],
                                                      ENCNAMES_CMD[1])
            TkComm.simplelist(list).each{|name|
              if ((enc == RubyEncoding.find(name)) rescue false)
                # new relation!! update table
                self[name.freeze] = enc
                return name
              end
            }
          else
            # String or Symbol ?
            if self[name = enc.to_s]
              return name
            end

            # Is it new ?
            if (enc_obj = (RubyEncoding.find(name) rescue false))
              list = TkCore::INTERP._invoke_without_enc(ENCNAMES_CMD[0],
                                                        ENCNAMES_CMD[1])
              if TkComm.simplelist(list).index(name)
                # Tk's encoding name ?
                self[name.freeze] = enc_obj  # new relation!! update table
                return name
              else
                # Ruby's encoding name ?
                if (name = self.key(enc_obj))
                  return name
                end
              end
            end
          end

          fail ArgumentError, "unsupported Tk encoding '#{orig_enc}'"
        end

        def ENCODING_TABLE.get_obj(enc)
          # returns the encoding object.
          # If 'enc' is the encoding name on Tk only, it returns nil.
          ((obj = self[self.get_name(enc)]).kind_of?(RubyEncoding))? obj: nil
        end
=end
        ### end of creating ENCODING_TABLE

      end

      #############################################

      if TkCore::WITH_ENCODING
        ################################
        ### Ruby 1.9
        ################################
        def force_default_encoding(mode)
          TkCore::INTERP.force_default_encoding = mode
        end

        def force_default_encoding?
          TkCore::INTERP.force_default_encoding?
        end

        def default_encoding=(enc)
          TkCore::INTERP.default_encoding = Tk::Encoding::ENCODING_TABLE.get_name(enc)
        end

        def encoding=(enc)
          TkCore::INTERP.encoding = Tk::Encoding::ENCODING_TABLE.get_name(enc)
        end

        def encoding_name
          Tk::Encoding::ENCODING_TABLE.get_name(TkCore::INTERP.encoding)
        end
        def encoding_obj
          Tk::Encoding::ENCODING_TABLE.get_obj(TkCore::INTERP.encoding)
        end
        alias encoding encoding_name
        alias default_encoding encoding_name

        def tk_encoding_names
          #TkComm.simplelist(TkCore::INTERP._invoke_without_enc(Tk::Encoding::ENCNAMES_CMD[0], Tk::Encoding::ENCNAMES_CMD[1]))
          TkComm.simplelist(TkCore::INTERP._invoke_without_enc('encoding', 'names'))
        end
        def encoding_names
          self.tk_encoding_names.find_all{|name|
            Tk::Encoding::ENCODING_TABLE.get_name(name) rescue false
          }
        end
        def encoding_objs
          self.tk_encoding_names.map!{|name|
            Tk::Encoding::ENCODING_TABLE.get_obj(name) rescue nil
          }.compact
        end

        def encoding_system=(enc)
          TclTkLib.encoding_system = Tk::Encoding::ENCODING_TABLE.get_name(enc)
        end

        def encoding_system_name
          Tk::Encoding::ENCODING_TABLE.get_name(TclTkLib.encoding_system)
        end
        def encoding_system_obj
          Tk::Encoding::ENCODING_TABLE.get_obj(TclTkLib.encoding_system)
        end
        alias encoding_system encoding_system_name

        ################################
      else
        ################################
        ### Ruby 1.8-
        ################################
        def force_default_encoding=(mode)
          true
        end

        def force_default_encoding?
          true
        end

        def default_encoding=(enc)
          TkCore::INTERP.default_encoding = enc
        end

        def encoding=(enc)
          TkCore::INTERP.encoding = enc
        end

        def encoding_obj
          TkCore::INTERP.encoding
        end
        def encoding_name
          TkCore::INTERP.encoding
        end
        alias encoding encoding_name
        alias default_encoding encoding_name

        def tk_encoding_names
          TkComm.simplelist(Tk.tk_call('encoding', 'names'))
        end
        def encoding_objs
          self.tk_encoding_names
        end
        def encoding_names
          self.tk_encoding_names
        end

        def encoding_system=(enc)
          TclTkLib.encoding_system = enc
        end

        def encoding_system_name
          TclTkLib.encoding_system
        end
        def encoding_system_obj
          TclTkLib.encoding_system
        end
        alias encoding_system encoding_system_name

        ################################
      end

      def encoding_convertfrom(str, enc=nil)
        enc = encoding_system_name unless enc
        str = str.dup
        if TkCore::WITH_ENCODING
          if str.kind_of?(Tk::EncodedString)
            str.__instance_variable_set('@encoding', nil)
          else
            str.instance_variable_set('@encoding', nil)
          end
          str.force_encoding('binary')
        else
          str.instance_variable_set('@encoding', 'binary')
        end
        ret = TkCore::INTERP._invoke_without_enc('encoding', 'convertfrom',
                                                 enc, str)
        if TkCore::WITH_ENCODING
          ret.force_encoding('utf-8')
        else
          Tk::UTF8_String.new(ret)
        end
        ret
      end
      alias encoding_convert_from encoding_convertfrom

      def encoding_convertto(str, enc=nil)
        # str must be a UTF-8 string
        enc = encoding_system_name unless enc
        ret = TkCore::INTERP._invoke_without_enc('encoding', 'convertto',
                                                 enc, str)
        #ret.instance_variable_set('@encoding', 'binary')
        if TkCore::WITH_ENCODING
          #ret.force_encoding(Tk::Encoding::ENCODING_TABLE.get_obj('binary'))
          ret.force_encoding(Tk::Encoding::ENCODING_TABLE.get_obj(enc))
        end
        ret
      end
      alias encoding_convert_to encoding_convertto

      def encoding_dirs
        # Tcl8.5 feature
        TkComm.simplelist(Tk.tk_call_without_enc('encoding', 'dirs'))
      end

      def encoding_dirs=(dir_list) # an array or a Tcl's list string
        # Tcl8.5 feature
        Tk.tk_call_without_enc('encoding', 'dirs', dir_list)
      end
    end

    extend Encoding
  end

  class TclTkIp
    def force_default_encoding=(mode)
      @force_default_encoding[0] = (mode)? true: false
    end

    def force_default_encoding?
      @force_default_encoding[0] ||= false
    end

    def default_encoding=(name)
      name = name.name if Tk::WITH_ENCODING && name.kind_of?(::Encoding)
      @encoding[0] = name.to_s.dup
    end

    # from tkencoding.rb by ttate@jaist.ac.jp
    #attr_accessor :encoding
    def encoding=(name)
      self.force_default_encoding = true  # for comaptibility
      self.default_encoding = name
    end

    def encoding_name
      (@encoding[0])? @encoding[0].dup: nil
    end
    alias encoding encoding_name
    alias default_encoding encoding_name

    def encoding_obj
      if Tk::WITH_ENCODING
        Tk::Encoding.tcl2rb_encoding(@encoding[0])
      else
        (@encoding[0])? @encoding[0].dup: nil
      end
    end

    alias __toUTF8 _toUTF8
    alias __fromUTF8 _fromUTF8

    if Object.const_defined?(:Encoding) && ::Encoding.class == Class
      # with Encoding (Ruby 1.9+)
      #
      # use functions on Tcl as default.
      # but when unsupported encoding on Tcl, use methods on Ruby.
      #
      def _toUTF8(str, enc = nil)
        if enc
          # use given encoding
          begin
            enc_name = Tk::Encoding::ENCODING_TABLE.get_name(enc)
          rescue
            # unknown encoding for Tk -> try to convert encoding on Ruby
            str = str.dup.force_encoding(enc)
            str.encode!(Tk::Encoding::UTF8_NAME) # modify self !!
            return str  # if no error, probably succeed converting
          end
        end

        enc_name ||= str.instance_variable_get(:@encoding)

        enc_name ||=
          Tk::Encoding::ENCODING_TABLE.get_name(str.encoding) rescue nil

        if enc_name
          # str has its encoding information
          encstr = __toUTF8(str, enc_name)
          encstr.force_encoding(Tk::Encoding::UTF8_NAME)
          return encstr
        else
          # str.encoding isn't supported by Tk -> try to convert on Ruby
          begin
            return str.encode(Tk::Encoding::UTF8_NAME) # new string
          rescue
            # error -> ignore, try to use default encoding of Ruby/Tk
          end
        end

        #enc_name ||=
        #  Tk::Encoding::ENCODING_TABLE.get_name(Tk.encoding) rescue nil
        enc_name ||= Tk::Encoding::ENCODING_TABLE.get_name(nil)

        # is 'binary' encoding?
        if enc_name == Tk::Encoding::BINARY_NAME
          return str.dup.force_encoding(Tk::Encoding::BINARY_NAME)
        end

        # force default encoding?
        if ! str.kind_of?(Tk::EncodedString) && self.force_default_encoding?
          enc_name = Tk::Encoding::ENCODING_TABLE.get_name(Tk.default_encoding)
        end

        encstr = __toUTF8(str, enc_name)
        encstr.force_encoding(Tk::Encoding::UTF8_NAME)
        encstr
      end
      def _fromUTF8(str, enc = nil)
        # str must be UTF-8 or binary.
        enc_name = str.instance_variable_get(:@encoding)
        enc_name ||=
          Tk::Encoding::ENCODING_TABLE.get_name(str.encoding) rescue nil

        # is 'binary' encoding?
        if enc_name == Tk::Encoding::BINARY_NAME
          return str.dup.force_encoding(Tk::Encoding::BINARY_NAME)
        end

        # get target encoding name (if enc == nil, use default encoding)
        begin
          enc_name = Tk::Encoding::ENCODING_TABLE.get_name(enc)
        rescue
          # then, enc != nil
          # unknown encoding for Tk -> try to convert encoding on Ruby
          str = str.dup.force_encoding(Tk::Encoding::UTF8_NAME)
          str.encode!(enc) # modify self !!
          return str  # if no error, probably succeed converting
        end

        encstr = __fromUTF8(str, enc_name)
        encstr.force_encoding(Tk::Encoding::ENCODING_TABLE.get_obj(enc_name))
        encstr
      end
      ###
    else
      # without Encoding (Ruby 1.8)
      def _toUTF8(str, encoding = nil)
        __toUTF8(str, encoding)
      end
      def _fromUTF8(str, encoding = nil)
        __fromUTF8(str, encoding)
      end
      ###
    end

    alias __eval _eval
    alias __invoke _invoke

    def _eval(cmd)
      _fromUTF8(__eval(_toUTF8(cmd)))
    end

    def _invoke(*cmds)
      _fromUTF8(__invoke(*(cmds.collect{|cmd| _toUTF8(cmd)})))
    end

    alias _eval_with_enc _eval
    alias _invoke_with_enc _invoke

=begin
    #### --> definition is moved to TclTkIp module

    def _toUTF8(str, encoding = nil)
      # decide encoding
      if encoding
        encoding = encoding.to_s
      elsif str.kind_of?(Tk::EncodedString) && str.encoding != nil
        encoding = str.encoding.to_s
      elsif str.instance_variable_get(:@encoding)
        encoding = str.instance_variable_get(:@encoding).to_s
      elsif defined?(@encoding) && @encoding != nil
        encoding = @encoding.to_s
      else
        encoding = __invoke('encoding', 'system')
      end

      # convert
      case encoding
      when 'utf-8', 'binary'
        str
      else
        __toUTF8(str, encoding)
      end
    end

    def _fromUTF8(str, encoding = nil)
      unless encoding
        if defined?(@encoding) && @encoding != nil
          encoding = @encoding.to_s
        else
          encoding = __invoke('encoding', 'system')
        end
      end

      if str.kind_of?(Tk::EncodedString)
        if str.encoding == 'binary'
          str
        else
          __fromUTF8(str, encoding)
        end
      elsif str.instance_variable_get(:@encoding).to_s == 'binary'
        str
      else
        __fromUTF8(str, encoding)
      end
    end
=end

=begin
    def _eval(cmd)
      if defined?(@encoding) && @encoding != 'utf-8'
        ret = if cmd.kind_of?(Tk::EncodedString)
                case cmd.encoding
                when 'utf-8', 'binary'
                  __eval(cmd)
                else
                  __eval(_toUTF8(cmd, cmd.encoding))
                end
              elsif cmd.instance_variable_get(:@encoding) == 'binary'
                __eval(cmd)
              else
                __eval(_toUTF8(cmd, @encoding))
              end
        if ret.kind_of?(String) && ret.instance_variable_get(:@encoding) == 'binary'
          ret
        else
          _fromUTF8(ret, @encoding)
        end
      else
        __eval(cmd)
      end
    end

    def _invoke(*cmds)
      if defined?(@encoding) && @encoding != 'utf-8'
        cmds = cmds.collect{|cmd|
          if cmd.kind_of?(Tk::EncodedString)
            case cmd.encoding
            when 'utf-8', 'binary'
              cmd
            else
              _toUTF8(cmd, cmd.encoding)
            end
          elsif cmd.instance_variable_get(:@encoding) == 'binary'
            cmd
          else
            _toUTF8(cmd, @encoding)
          end
        }
        ret = __invoke(*cmds)
        if ret.kind_of?(String) && ret.instance_variable_get(:@encoding) == 'binary'
          ret
        else
          _fromUTF8(ret, @encoding)
        end
      else
        __invoke(*cmds)
        end
    end
=end
  end

  module TclTkLib
    class << self
      def force_default_encoding=(mode)
        TkCore::INTERP.force_default_encoding = mode
      end

      def force_default_encoding?
        TkCore::INTERP.force_default_encoding?
      end

      def default_encoding=(name)
        TkCore::INTERP.default_encoding = name
      end

      alias _encoding encoding
      alias _encoding= encoding=
      def encoding=(name)
        name = name.name if name.kind_of?(::Encoding) if Tk::WITH_ENCODING
        TkCore::INTERP.encoding = name
      end

      def encoding_name
        TkCore::INTERP.encoding
      end
      alias encoding encoding_name
      alias default_encoding encoding_name

      def encoding_obj
        if Tk::WITH_ENCODING
          Tk::Encoding.tcl2rb_encoding(TkCore::INTERP.encoding)
        else
          TkCore::INTERP.encoding
        end
      end
    end
  end

  # estimate encoding
  unless TkCore::WITH_ENCODING
    case $KCODE
    when /^e/i  # EUC
      Tk.encoding = 'euc-jp'
      Tk.encoding_system = 'euc-jp'
    when /^s/i  # SJIS
      begin
        if Tk.encoding_system == 'cp932'
          Tk.encoding = 'cp932'
        else
          Tk.encoding = 'shiftjis'
          Tk.encoding_system = 'shiftjis'
        end
      rescue StandardError, NameError
        Tk.encoding = 'shiftjis'
        Tk.encoding_system = 'shiftjis'
      end
    when /^u/i  # UTF8
      Tk.encoding = 'utf-8'
      Tk.encoding_system = 'utf-8'
    else        # NONE
      if defined? DEFAULT_TK_ENCODING
        Tk.encoding_system = DEFAULT_TK_ENCODING
      end
      begin
        Tk.encoding = Tk.encoding_system
      rescue StandardError, NameError
        Tk.encoding = 'utf-8'
        Tk.encoding_system = 'utf-8'
      end
    end

  else ### Ruby 1.9 !!!!!!!!!!!!
    # loc_enc_obj = (::Encoding.find(::Encoding.locale_charmap) rescue Tk::Encoding::UNKNOWN)
    loc_enc_obj = ::Encoding.find("locale")
    ext_enc_obj = ::Encoding.default_external
    int_enc_obj = ::Encoding.default_internal || ext_enc_obj
    tksys_enc_name = Tk::Encoding::ENCODING_TABLE.get_name(Tk.encoding_system)
    # p [Tk.encoding, Tk.encoding_system, loc_enc_obj, ext_enc_obj]

=begin
    if ext_enc_obj == Tk::Encoding::UNKNOWN
      if defined? DEFAULT_TK_ENCODING
        if DEFAULT_TK_ENCODING.kind_of?(::Encoding)
          tk_enc_name    = DEFAULT_TK_ENCODING.name
          tksys_enc_name = DEFAULT_TK_ENCODING.name
        else
          tk_enc_name    = DEFAULT_TK_ENCODING
          tksys_enc_name = DEFAULT_TK_ENCODING
        end
      else
        tk_enc_name    = loc_enc_obj.name
        tksys_enc_name = loc_enc_obj.name
      end
    else
      tk_enc_name    = ext_enc_obj.name
      tksys_enc_name = ext_enc_obj.name
    end

    # Tk.encoding = tk_enc_name
    Tk.default_encoding = tk_enc_name
    Tk.encoding_system = tksys_enc_name
=end

    if ext_enc_obj == Tk::Encoding::UNKNOWN
      if loc_enc_obj == Tk::Encoding::UNKNOWN
        # use Tk.encoding_system
      else
        # use locale_charmap
        begin
          loc_enc_name = Tk::Encoding::ENCODING_TABLE.get_name(loc_enc_obj)
          if loc_enc_name && loc_enc_name != tksys_enc_name
            # use locale_charmap
            Tk.encoding_system = loc_enc_name
          else
            # use Tk.encoding_system
          end
        rescue ArgumentError
          # unsupported encoding on Tk -> use Tk.encoding_system
        end
      end
    else
      begin
        ext_enc_name = Tk::Encoding::ENCODING_TABLE.get_name(ext_enc_obj)
        if ext_enc_name && ext_enc_name != tksys_enc_name
          # use default_external
          Tk.encoding_system = ext_enc_name
        else
          # use Tk.encoding_system
        end
      rescue ArgumentError
        # unsupported encoding on Tk -> use Tk.encoding_system
      end
    end

    # setup Tk.encoding
    enc_name = nil

    begin
      default_def = DEFAULT_TK_ENCODING
      if ::Encoding.find(default_def.to_s) != Tk::Encoding::UNKNOWN
        enc_name = Tk::Encoding::ENCODING_TABLE.get_name(default_def)
      end
    rescue NameError
      # ignore
      enc_name = nil
    rescue ArgumentError
      enc_name = nil
      fail ArgumentError,
           "DEFAULT_TK_ENCODING has an unknown encoding #{default_def}"
    end

    unless enc_name
      #if ext_enc_obj == Tk::Encoding::UNKNOWN
      if int_enc_obj == Tk::Encoding::UNKNOWN
        if loc_enc_obj == Tk::Encoding::UNKNOWN
          # use Tk.encoding_system
          enc_name = tksys_enc_name
        else
          # use locale_charmap
          begin
            loc_enc_name = Tk::Encoding::ENCODING_TABLE.get_name(loc_enc_obj)
            if loc_enc_name
              # use locale_charmap
              enc_name = loc_enc_name
            else
              # use Tk.encoding_system
              enc_name = tksys_enc_name
            end
          rescue ArgumentError
            # unsupported encoding on Tk -> use Tk.encoding_system
            enc_name = tksys_enc_name
          end
        end
      else
        begin
          #ext_enc_name = Tk::Encoding::ENCODING_TABLE.get_name(ext_enc_obj)
          #if ext_enc_name && ext_enc_name != tksys_enc_name
          int_enc_name = Tk::Encoding::ENCODING_TABLE.get_name(int_enc_obj)
          if int_enc_name
            # use default_internal
            enc_name = int_enc_name
          else
            # use Tk.encoding_system
            enc_name = tksys_enc_name
          end
        rescue ArgumentError
          # unsupported encoding on Tk -> use Tk.encoding_system
          enc_name = tksys_enc_name
        end
      end
    end

    Tk.default_encoding = (enc_name)? enc_name: tksys_enc_name
  end

else
  # dummy methods
  module Tk
    module Encoding
      extend Encoding

      def force_default_encoding=(mode)
        nil
      end

      def force_default_encoding?
        nil
      end

      def default_encoding=(enc)
        nil
      end
      def default_encoding
        nil
      end

      def encoding=(name)
        nil
      end
      def encoding
        nil
      end
      def encoding_names
        nil
      end
      def encoding_system
        nil
      end
      def encoding_system=(enc)
        nil
      end

      def encoding_convertfrom(str, enc=None)
        str
      end
      alias encoding_convert_from encoding_convertfrom

      def encoding_convertto(str, enc=None)
        str
      end
      alias encoding_convert_to encoding_convertto
      def encoding_dirs
        nil
      end
      def encoding_dirs=(dir_array)
        nil
      end
    end

    extend Encoding
  end

  class TclTkIp
    attr_accessor :encoding

    alias __eval _eval
    alias __invoke _invoke

    alias _eval_with_enc _eval
    alias _invoke_with_enc _invoke
  end
end


module TkBindCore
  #def bind(context, cmd=Proc.new, *args)
  #  Tk.bind(self, context, cmd, *args)
  #end
  def bind(context, *args)
    # if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
    if TkComm._callback_entry?(args[0]) || !block_given?
      cmd = args.shift
    else
      cmd = Proc.new
    end
    Tk.bind(self, context, cmd, *args)
  end

  #def bind_append(context, cmd=Proc.new, *args)
  #  Tk.bind_append(self, context, cmd, *args)
  #end
  def bind_append(context, *args)
    # if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
    if TkComm._callback_entry?(args[0]) || !block_given?
      cmd = args.shift
    else
      cmd = Proc.new
    end
    Tk.bind_append(self, context, cmd, *args)
  end

  def bind_remove(context)
    Tk.bind_remove(self, context)
  end

  def bindinfo(context=nil)
    Tk.bindinfo(self, context)
  end
end


module TkTreatFont
  def __font_optkeys
    ['font']
  end
  private :__font_optkeys

  def __pathname
    self.path
  end
  private :__pathname

  ################################

  def font_configinfo(key = nil)
    optkeys = __font_optkeys
    if key && !optkeys.find{|opt| opt.to_s == key.to_s}
      fail ArgumentError, "unknown font option name `#{key}'"
    end

    win, tag = __pathname.split(':')

    if key
      pathname = [win, tag, key].join(';')
      TkFont.used_on(pathname) ||
        TkFont.init_widget_font(pathname, *__confinfo_cmd)
    elsif optkeys.size == 1
      pathname = [win, tag, optkeys[0]].join(';')
      TkFont.used_on(pathname) ||
        TkFont.init_widget_font(pathname, *__confinfo_cmd)
    else
      fonts = {}
      optkeys.each{|k|
        k = k.to_s
        pathname = [win, tag, k].join(';')
        fonts[k] =
          TkFont.used_on(pathname) ||
          TkFont.init_widget_font(pathname, *__confinfo_cmd)
      }
      fonts
    end
  end
  alias fontobj font_configinfo

  def font_configure(slot)
    pathname = __pathname

    slot = _symbolkey2str(slot)

    __font_optkeys.each{|optkey|
      optkey = optkey.to_s
      l_optkey = 'latin' << optkey
      a_optkey = 'ascii' << optkey
      k_optkey = 'kanji' << optkey

      if slot.key?(optkey)
        fnt = slot.delete(optkey)
        if fnt.kind_of?(TkFont)
          slot.delete(l_optkey)
          slot.delete(a_optkey)
          slot.delete(k_optkey)

          fnt.call_font_configure([pathname, optkey], *(__config_cmd << {}))
          next
        else
          if fnt
            if (slot.key?(l_optkey) ||
                slot.key?(a_optkey) ||
                slot.key?(k_optkey))
              fnt = TkFont.new(fnt)

              lfnt = slot.delete(l_optkey)
              lfnt = slot.delete(a_optkey) if slot.key?(a_optkey)
              kfnt = slot.delete(k_optkey)

              fnt.latin_replace(lfnt) if lfnt
              fnt.kanji_replace(kfnt) if kfnt

              fnt.call_font_configure([pathname, optkey],
                                      *(__config_cmd << {}))
              next
            else
              fnt = hash_kv(fnt) if fnt.kind_of?(Hash)
              unless TkConfigMethod.__IGNORE_UNKNOWN_CONFIGURE_OPTION__
                tk_call(*(__config_cmd << "-#{optkey}" << fnt))
              else
                begin
                  tk_call(*(__config_cmd << "-#{optkey}" << fnt))
                rescue
                  # ignore
                end
              end
            end
          end
          next
        end
      end

      lfnt = slot.delete(l_optkey)
      lfnt = slot.delete(a_optkey) if slot.key?(a_optkey)
      kfnt = slot.delete(k_optkey)

      if lfnt && kfnt
        TkFont.new(lfnt, kfnt).call_font_configure([pathname, optkey],
                                                   *(__config_cmd << {}))
      elsif lfnt
        latinfont_configure([lfnt, optkey])
      elsif kfnt
        kanjifont_configure([kfnt, optkey])
      end
    }

    # configure other (without font) options
    tk_call(*(__config_cmd.concat(hash_kv(slot)))) if slot != {}
    self
  end

  def latinfont_configure(ltn, keys=nil)
    if ltn.kind_of?(Array)
      key = ltn[1]
      ltn = ltn[0]
    else
      key = nil
    end

    optkeys = __font_optkeys
    if key && !optkeys.find{|opt| opt.to_s == key.to_s}
      fail ArgumentError, "unknown font option name `#{key}'"
    end

    win, tag = __pathname.split(':')

    optkeys = [key] if key

    optkeys.each{|optkey|
      optkey = optkey.to_s

      pathname = [win, tag, optkey].join(';')

      if (fobj = TkFont.used_on(pathname))
        fobj = TkFont.new(fobj) # create a new TkFont object
      elsif Tk::JAPANIZED_TK
        fobj = fontobj          # create a new TkFont object
      else
        ltn = hash_kv(ltn) if ltn.kind_of?(Hash)
        unless TkConfigMethod.__IGNORE_UNKNOWN_CONFIGURE_OPTION__
          tk_call(*(__config_cmd << "-#{optkey}" << ltn))
        else
          begin
            tk_call(*(__config_cmd << "-#{optkey}" << ltn))
          rescue => e
            # ignore
          end
        end
        next
      end

      if fobj.kind_of?(TkFont)
        if ltn.kind_of?(TkFont)
          conf = {}
          ltn.latin_configinfo.each{|k,val| conf[k] = val}
          if keys
            fobj.latin_configure(conf.update(keys))
          else
            fobj.latin_configure(conf)
          end
        else
          fobj.latin_replace(ltn)
        end
      end

      fobj.call_font_configure([pathname, optkey], *(__config_cmd << {}))
    }
    self
  end
  alias asciifont_configure latinfont_configure

  def kanjifont_configure(knj, keys=nil)
    if knj.kind_of?(Array)
      key = knj[1]
      knj = knj[0]
    else
      key = nil
    end

    optkeys = __font_optkeys
    if key && !optkeys.find{|opt| opt.to_s == key.to_s}
      fail ArgumentError, "unknown font option name `#{key}'"
    end

    win, tag = __pathname.split(':')

    optkeys = [key] if key

    optkeys.each{|optkey|
      optkey = optkey.to_s

      pathname = [win, tag, optkey].join(';')

      if (fobj = TkFont.used_on(pathname))
        fobj = TkFont.new(fobj) # create a new TkFont object
      elsif Tk::JAPANIZED_TK
        fobj = fontobj          # create a new TkFont object
      else
        knj = hash_kv(knj) if knj.kind_of?(Hash)
        unless TkConfigMethod.__IGNORE_UNKNOWN_CONFIGURE_OPTION__
          tk_call(*(__config_cmd << "-#{optkey}" << knj))
        else
          begin
            tk_call(*(__config_cmd << "-#{optkey}" << knj))
          rescue => e
            # ignore
          end
        end
        next
      end

      if fobj.kind_of?(TkFont)
        if knj.kind_of?(TkFont)
          conf = {}
          knj.kanji_configinfo.each{|k,val| conf[k] = val}
          if keys
            fobj.kanji_configure(conf.update(keys))
          else
            fobj.kanji_configure(conf)
          end
        else
          fobj.kanji_replace(knj)
        end
      end

      fobj.call_font_configure([pathname, optkey], *(__config_cmd << {}))
    }
    self
  end

  def font_copy(win, wintag=nil, winkey=nil, targetkey=nil)
    if wintag
      if winkey
        fnt = win.tagfontobj(wintag, winkey).dup
      else
        fnt = win.tagfontobj(wintag).dup
      end
    else
      if winkey
        fnt = win.fontobj(winkey).dup
      else
        fnt = win.fontobj.dup
      end
    end

    if targetkey
      fnt.call_font_configure([__pathname, targetkey], *(__config_cmd << {}))
    else
      fnt.call_font_configure(__pathname, *(__config_cmd << {}))
    end
    self
  end

  def latinfont_copy(win, wintag=nil, winkey=nil, targetkey=nil)
    if targetkey
      fontobj(targetkey).dup.call_font_configure([__pathname, targetkey],
                                                 *(__config_cmd << {}))
    else
      fontobj.dup.call_font_configure(__pathname, *(__config_cmd << {}))
    end

    if wintag
      if winkey
        fontobj.latin_replace(win.tagfontobj(wintag, winkey).latin_font_id)
      else
        fontobj.latin_replace(win.tagfontobj(wintag).latin_font_id)
      end
    else
      if winkey
        fontobj.latin_replace(win.fontobj(winkey).latin_font_id)
      else
        fontobj.latin_replace(win.fontobj.latin_font_id)
      end
    end
    self
  end
  alias asciifont_copy latinfont_copy

  def kanjifont_copy(win, wintag=nil, winkey=nil, targetkey=nil)
    if targetkey
      fontobj(targetkey).dup.call_font_configure([__pathname, targetkey],
                                                 *(__config_cmd << {}))
    else
        fontobj.dup.call_font_configure(__pathname, *(__config_cmd << {}))
    end

    if wintag
      if winkey
        fontobj.kanji_replace(win.tagfontobj(wintag, winkey).kanji_font_id)
      else
        fontobj.kanji_replace(win.tagfontobj(wintag).kanji_font_id)
      end
    else
      if winkey
        fontobj.kanji_replace(win.fontobj(winkey).kanji_font_id)
      else
        fontobj.kanji_replace(win.fontobj.kanji_font_id)
      end
    end
    self
  end
end


module TkConfigMethod
  include TkUtil
  include TkTreatFont

  def TkConfigMethod.__IGNORE_UNKNOWN_CONFIGURE_OPTION__
    @mode || false
  end
  def TkConfigMethod.__set_IGNORE_UNKNOWN_CONFIGURE_OPTION__!(mode)
    fail SecurityError, "can't change the mode" if $SAFE>=4
    @mode = (mode)? true: false
  end

  def __cget_cmd
    [self.path, 'cget']
  end
  private :__cget_cmd

  def __config_cmd
    [self.path, 'configure']
  end
  private :__config_cmd

  def __confinfo_cmd
    __config_cmd
  end
  private :__confinfo_cmd

  def __configinfo_struct
    {:key=>0, :alias=>1, :db_name=>1, :db_class=>2,
      :default_value=>3, :current_value=>4}
  end
  private :__configinfo_struct

  def __optkey_aliases
    {}
  end
  private :__optkey_aliases

  def __numval_optkeys
    []
  end
  private :__numval_optkeys

  def __numstrval_optkeys
    []
  end
  private :__numstrval_optkeys

  def __boolval_optkeys
    ['exportselection', 'jump', 'setgrid', 'takefocus']
  end
  private :__boolval_optkeys

  def __strval_optkeys
    [
      'text', 'label', 'show', 'data', 'file',
      'activebackground', 'activeforeground', 'background',
      'disabledforeground', 'disabledbackground', 'foreground',
      'highlightbackground', 'highlightcolor', 'insertbackground',
      'selectbackground', 'selectforeground', 'troughcolor'
    ]
  end
  private :__strval_optkeys

  def __listval_optkeys
    []
  end
  private :__listval_optkeys

  def __numlistval_optkeys
    []
  end
  private :__numlistval_optkeys

  def __tkvariable_optkeys
    ['variable', 'textvariable']
  end
  private :__tkvariable_optkeys

  def __val2ruby_optkeys  # { key=>proc, ... }
    # The method is used to convert a opt-value to a ruby's object.
    # When get the value of the option "key", "proc.call(value)" is called.
    {}
  end
  private :__val2ruby_optkeys

  def __ruby2val_optkeys  # { key=>proc, ... }
    # The method is used to convert a ruby's object to a opt-value.
    # When set the value of the option "key", "proc.call(value)" is called.
    # That is, "-#{key} #{proc.call(value)}".
    {}
  end
  private :__ruby2val_optkeys

  def __methodcall_optkeys  # { key=>method, ... }
    # The method is used to both of get and set.
    # Usually, the 'key' will not be a widget option.
    {}
  end
  private :__methodcall_optkeys

  def __keyonly_optkeys  # { def_key=>undef_key or nil, ... }
    {}
  end
  private :__keyonly_optkeys

  def __conv_keyonly_opts(keys)
    return keys unless keys.kind_of?(Hash)
    keyonly = __keyonly_optkeys
    keys2 = {}
    keys.each{|k, v|
      optkey = keyonly.find{|kk,vv| kk.to_s == k.to_s}
      if optkey
        defkey, undefkey = optkey
        if v
          keys2[defkey.to_s] = None
        elsif undefkey
          keys2[undefkey.to_s] = None
        else
          # remove key
        end
      else
        keys2[k.to_s] = v
      end
    }
    keys2
  end
  private :__conv_keyonly_opts

  def config_hash_kv(keys, enc_mode = nil, conf = nil)
    hash_kv(__conv_keyonly_opts(keys), enc_mode, conf)
  end

  ################################

  def [](id)
    cget(id)
  end

  def []=(id, val)
    configure(id, val)
    val
  end

  def cget_tkstring(option)
    opt = option.to_s
    fail ArgumentError, "Invalid option `#{option.inspect}'" if opt.length == 0
    tk_call_without_enc(*(__cget_cmd << "-#{opt}"))
  end

  def __cget_core(slot)
    orig_slot = slot
    slot = slot.to_s

    if slot.length == 0
      fail ArgumentError, "Invalid option `#{orig_slot.inspect}'"
    end

    alias_name, real_name = __optkey_aliases.find{|k, v| k.to_s == slot}
    if real_name
      slot = real_name.to_s
    end

    if ( method = _symbolkey2str(__val2ruby_optkeys())[slot] )
      optval = tk_call_without_enc(*(__cget_cmd << "-#{slot}"))
      begin
        return method.call(optval)
      rescue => e
        warn("Warning:: #{e.message} (when #{method}.call(#{optval.inspect})") if $DEBUG
        return optval
      end
    end

    if ( method = _symbolkey2str(__methodcall_optkeys)[slot] )
      return self.__send__(method)
    end

    case slot
    when /^(#{__numval_optkeys.join('|')})$/
      begin
        number(tk_call_without_enc(*(__cget_cmd << "-#{slot}")))
      rescue
        nil
      end

    when /^(#{__numstrval_optkeys.join('|')})$/
      num_or_str(tk_call_without_enc(*(__cget_cmd << "-#{slot}")))

    when /^(#{__boolval_optkeys.join('|')})$/
      begin
        bool(tk_call_without_enc(*(__cget_cmd << "-#{slot}")))
      rescue
        nil
      end

    when /^(#{__listval_optkeys.join('|')})$/
      simplelist(tk_call_without_enc(*(__cget_cmd << "-#{slot}")))

    when /^(#{__numlistval_optkeys.join('|')})$/
      conf = tk_call_without_enc(*(__cget_cmd << "-#{slot}"))
      if conf =~ /^[0-9+-]/
        list(conf)
      else
        conf
      end

    when /^(#{__strval_optkeys.join('|')})$/
      _fromUTF8(tk_call_without_enc(*(__cget_cmd << "-#{slot}")))

    when /^(|latin|ascii|kanji)(#{__font_optkeys.join('|')})$/
      fontcode = $1
      fontkey  = $2
      fnt = tk_tcl2ruby(tk_call_without_enc(*(__cget_cmd << "-#{fontkey}")), true)
      unless fnt.kind_of?(TkFont)
        fnt = fontobj(fontkey)
      end
      if fontcode == 'kanji' && JAPANIZED_TK && TK_VERSION =~ /^4\.*/
        # obsolete; just for compatibility
        fnt.kanji_font
      else
        fnt
      end

    when /^(#{__tkvariable_optkeys.join('|')})$/
      v = tk_call_without_enc(*(__cget_cmd << "-#{slot}"))
      (v.empty?)? nil: TkVarAccess.new(v)

    else
      tk_tcl2ruby(tk_call_without_enc(*(__cget_cmd << "-#{slot}")), true)
    end
  end
  private :__cget_core

  def cget(slot)
    unless TkConfigMethod.__IGNORE_UNKNOWN_CONFIGURE_OPTION__
      __cget_core(slot)
    else
      begin
        __cget_core(slot)
      rescue => e
        if current_configinfo.has_key?(slot.to_s)
          # error on known option
          fail e
        else
          # unknown option
          nil
        end
      end
    end
  end
  def cget_strict(slot)
    # never use TkConfigMethod.__IGNORE_UNKNOWN_CONFIGURE_OPTION__
    __cget_core(slot)
  end

  def __configure_core(slot, value=None)
    if slot.kind_of? Hash
      slot = _symbolkey2str(slot)

      __optkey_aliases.each{|alias_name, real_name|
        alias_name = alias_name.to_s
        if slot.has_key?(alias_name)
          slot[real_name.to_s] = slot.delete(alias_name)
        end
      }

      __methodcall_optkeys.each{|key, method|
        value = slot.delete(key.to_s)
        self.__send__(method, value) if value
      }

      __ruby2val_optkeys.each{|key, method|
        key = key.to_s
        slot[key] = method.call(slot[key]) if slot.has_key?(key)
      }

      __keyonly_optkeys.each{|defkey, undefkey|
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

      if (slot.find{|k, v| k =~ /^(|latin|ascii|kanji)(#{__font_optkeys.join('|')})$/})
        font_configure(slot)
      elsif slot.size > 0
        tk_call(*(__config_cmd.concat(hash_kv(slot))))
      end

    else
      orig_slot = slot
      slot = slot.to_s
      if slot.length == 0
        fail ArgumentError, "Invalid option `#{orig_slot.inspect}'"
      end

      alias_name, real_name = __optkey_aliases.find{|k, v| k.to_s == slot}
      if real_name
        slot = real_name.to_s
      end

      if ( conf = __keyonly_optkeys.find{|k, v| k.to_s == slot} )
        defkey, undefkey = conf
        if value
          tk_call(*(__config_cmd << "-#{defkey}"))
        elsif undefkey
          tk_call(*(__config_cmd << "-#{undefkey}"))
        end
      elsif ( method = _symbolkey2str(__ruby2val_optkeys)[slot] )
        tk_call(*(__config_cmd << "-#{slot}" << method.call(value)))
      elsif ( method = _symbolkey2str(__methodcall_optkeys)[slot] )
        self.__send__(method, value)
      elsif (slot =~ /^(|latin|ascii|kanji)(#{__font_optkeys.join('|')})$/)
        if value == None
          fontobj($2)
        else
          font_configure({slot=>value})
        end
      else
        tk_call(*(__config_cmd << "-#{slot}" << value))
      end
    end
    self
  end
  private :__configure_core

  def __check_available_configure_options(keys)
    availables = self.current_configinfo.keys

    # add non-standard keys
    availables |= __font_optkeys.map{|k|
      [k.to_s, "latin#{k}", "ascii#{k}", "kanji#{k}"]
    }.flatten
    availables |= __methodcall_optkeys.keys.map{|k| k.to_s}
    availables |= __keyonly_optkeys.keys.map{|k| k.to_s}

    keys = _symbolkey2str(keys)
    keys.delete_if{|k, v| !(availables.include?(k))}
  end

  def configure(slot, value=None)
    unless TkConfigMethod.__IGNORE_UNKNOWN_CONFIGURE_OPTION__
      __configure_core(slot, value)
    else
      if slot.kind_of?(Hash)
        begin
          __configure_core(slot)
        rescue
          slot = __check_available_configure_options(slot)
          __configure_core(slot) unless slot.empty?
        end
      else
        begin
          __configure_core(slot, value)
        rescue => e
          if current_configinfo.has_key?(slot.to_s)
            # error on known option
            fail e
          else
            # unknown option
            nil
          end
        end
      end
    end
    self
  end

  def configure_cmd(slot, value)
    configure(slot, install_cmd(value))
  end

  def __configinfo_core(slot = nil)
    if TkComm::GET_CONFIGINFO_AS_ARRAY
      if (slot &&
          slot.to_s =~ /^(|latin|ascii|kanji)(#{__font_optkeys.join('|')})$/)
        fontkey  = $2
        # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__confinfo_cmd << "-#{fontkey}"))))
        conf = tk_split_simplelist(tk_call_without_enc(*(__confinfo_cmd << "-#{fontkey}")), false, true)
        conf[__configinfo_struct[:key]] =
          conf[__configinfo_struct[:key]][1..-1]
        if ( ! __configinfo_struct[:alias] \
            || conf.size > __configinfo_struct[:alias] + 1 )
          fnt = conf[__configinfo_struct[:default_value]]
          if TkFont.is_system_font?(fnt)
            conf[__configinfo_struct[:default_value]] = TkNamedFont.new(fnt)
          end
          conf[__configinfo_struct[:current_value]] = fontobj(fontkey)
        elsif ( __configinfo_struct[:alias] \
               && conf.size == __configinfo_struct[:alias] + 1 \
               && conf[__configinfo_struct[:alias]][0] == ?- )
          conf[__configinfo_struct[:alias]] =
            conf[__configinfo_struct[:alias]][1..-1]
        end
        conf
      else
        if slot
          slot = slot.to_s

          alias_name, real_name = __optkey_aliases.find{|k, v| k.to_s == slot}
          if real_name
            slot = real_name.to_s
          end

          case slot
          when /^(#{__val2ruby_optkeys().keys.join('|')})$/
            method = _symbolkey2str(__val2ruby_optkeys())[slot]
            conf = tk_split_simplelist(tk_call_without_enc(*(__confinfo_cmd() << "-#{slot}")), false, true)
            if ( __configinfo_struct[:default_value] \
                && conf[__configinfo_struct[:default_value]] )
              optval = conf[__configinfo_struct[:default_value]]
              begin
                val = method.call(optval)
              rescue => e
                warn("Warning:: #{e.message} (when #{method}.call(#{optval.inspect})") if $DEBUG
                val = optval
              end
              conf[__configinfo_struct[:default_value]] = val
            end
            if ( conf[__configinfo_struct[:current_value]] )
              optval = conf[__configinfo_struct[:current_value]]
              begin
                val = method.call(optval)
              rescue => e
                warn("Warning:: #{e.message} (when #{method}.call(#{optval.inspect})") if $DEBUG
                val = optval
              end
              conf[__configinfo_struct[:current_value]] = val
            end

          when /^(#{__methodcall_optkeys.keys.join('|')})$/
            method = _symbolkey2str(__methodcall_optkeys)[slot]
            return [slot, '', '', '', self.__send__(method)]

          when /^(#{__numval_optkeys.join('|')})$/
            # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}"))))
            conf = tk_split_simplelist(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}")), false, true)

            if ( __configinfo_struct[:default_value] \
                && conf[__configinfo_struct[:default_value]])
              begin
                conf[__configinfo_struct[:default_value]] =
                  number(conf[__configinfo_struct[:default_value]])
              rescue
                conf[__configinfo_struct[:default_value]] = nil
              end
            end
            if ( conf[__configinfo_struct[:current_value]] )
              begin
                conf[__configinfo_struct[:current_value]] =
                  number(conf[__configinfo_struct[:current_value]])
              rescue
                conf[__configinfo_struct[:current_value]] = nil
              end
            end

          when /^(#{__numstrval_optkeys.join('|')})$/
            # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}"))))
            conf = tk_split_simplelist(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}")), false, true)

            if ( __configinfo_struct[:default_value] \
                && conf[__configinfo_struct[:default_value]])
              conf[__configinfo_struct[:default_value]] =
                num_or_str(conf[__configinfo_struct[:default_value]])
            end
            if ( conf[__configinfo_struct[:current_value]] )
              conf[__configinfo_struct[:current_value]] =
                num_or_str(conf[__configinfo_struct[:current_value]])
            end

          when /^(#{__boolval_optkeys.join('|')})$/
            # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}"))))
            conf = tk_split_simplelist(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}")), false, true)

            if ( __configinfo_struct[:default_value] \
                && conf[__configinfo_struct[:default_value]])
              begin
                conf[__configinfo_struct[:default_value]] =
                  bool(conf[__configinfo_struct[:default_value]])
              rescue
                conf[__configinfo_struct[:default_value]] = nil
              end
            end
            if ( conf[__configinfo_struct[:current_value]] )
              begin
                conf[__configinfo_struct[:current_value]] =
                  bool(conf[__configinfo_struct[:current_value]])
              rescue
                conf[__configinfo_struct[:current_value]] = nil
              end
            end

          when /^(#{__listval_optkeys.join('|')})$/
            # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}"))))
            conf = tk_split_simplelist(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}")), false, true)

            if ( __configinfo_struct[:default_value] \
                && conf[__configinfo_struct[:default_value]])
              conf[__configinfo_struct[:default_value]] =
                simplelist(conf[__configinfo_struct[:default_value]])
            end
            if ( conf[__configinfo_struct[:current_value]] )
              conf[__configinfo_struct[:current_value]] =
                simplelist(conf[__configinfo_struct[:current_value]])
            end

          when /^(#{__numlistval_optkeys.join('|')})$/
            # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}"))))
            conf = tk_split_simplelist(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}")), false, true)

            if ( __configinfo_struct[:default_value] \
                && conf[__configinfo_struct[:default_value]] \
                && conf[__configinfo_struct[:default_value]] =~ /^[0-9]/ )
              conf[__configinfo_struct[:default_value]] =
                list(conf[__configinfo_struct[:default_value]])
            end
            if ( conf[__configinfo_struct[:current_value]] \
                && conf[__configinfo_struct[:current_value]] =~ /^[0-9]/ )
              conf[__configinfo_struct[:current_value]] =
                list(conf[__configinfo_struct[:current_value]])
            end

          when /^(#{__strval_optkeys.join('|')})$/
            # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}"))))
            conf = tk_split_simplelist(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}")), false, true)

          when /^(#{__tkvariable_optkeys.join('|')})$/
            conf = tk_split_simplelist(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}")), false, true)

            if ( __configinfo_struct[:default_value] \
                && conf[__configinfo_struct[:default_value]])
              v = conf[__configinfo_struct[:default_value]]
              if v.empty?
                conf[__configinfo_struct[:default_value]] = nil
              else
                conf[__configinfo_struct[:default_value]] = TkVarAccess.new(v)
              end
            end
            if ( conf[__configinfo_struct[:current_value]] )
              v = conf[__configinfo_struct[:current_value]]
              if v.empty?
                conf[__configinfo_struct[:current_value]] = nil
              else
                conf[__configinfo_struct[:current_value]] = TkVarAccess.new(v)
              end
            end

          else
            # conf = tk_split_list(_fromUTF8(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}"))))
            # conf = tk_split_list(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}")), 0, false, true)
            conf = tk_split_list(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}")), 1, false, true)
          end
          conf[__configinfo_struct[:key]] =
            conf[__configinfo_struct[:key]][1..-1]

          if ( __configinfo_struct[:alias] \
              && conf.size == __configinfo_struct[:alias] + 1 \
              && conf[__configinfo_struct[:alias]][0] == ?- )
            conf[__configinfo_struct[:alias]] =
              conf[__configinfo_struct[:alias]][1..-1]
          end

          conf

        else
          # ret = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*__confinfo_cmd))).collect{|conflist|
          #  conf = tk_split_simplelist(conflist)
          ret = tk_split_simplelist(tk_call_without_enc(*__confinfo_cmd), false, false).collect{|conflist|
            conf = tk_split_simplelist(conflist, false, true)
            conf[__configinfo_struct[:key]] =
              conf[__configinfo_struct[:key]][1..-1]

            optkey = conf[__configinfo_struct[:key]]
            case optkey
            when /^(#{__val2ruby_optkeys().keys.join('|')})$/
              method = _symbolkey2str(__val2ruby_optkeys())[optkey]
              if ( __configinfo_struct[:default_value] \
                  && conf[__configinfo_struct[:default_value]] )
                optval = conf[__configinfo_struct[:default_value]]
                begin
                  val = method.call(optval)
                rescue => e
                  warn("Warning:: #{e.message} (when #{method}.call(#{optval.inspect})") if $DEBUG
                  val = optval
                end
                conf[__configinfo_struct[:default_value]] = val
              end
              if ( conf[__configinfo_struct[:current_value]] )
                optval = conf[__configinfo_struct[:current_value]]
                begin
                  val = method.call(optval)
                rescue => e
                  warn("Warning:: #{e.message} (when #{method}.call(#{optval.inspect})") if $DEBUG
                  val = optval
                end
                conf[__configinfo_struct[:current_value]] = val
              end

            when /^(#{__strval_optkeys.join('|')})$/
              # do nothing

            when /^(#{__numval_optkeys.join('|')})$/
              if ( __configinfo_struct[:default_value] \
                  && conf[__configinfo_struct[:default_value]] )
                begin
                  conf[__configinfo_struct[:default_value]] =
                    number(conf[__configinfo_struct[:default_value]])
                rescue
                  conf[__configinfo_struct[:default_value]] = nil
                end
              end
              if ( conf[__configinfo_struct[:current_value]] )
                begin
                  conf[__configinfo_struct[:current_value]] =
                    number(conf[__configinfo_struct[:current_value]])
                rescue
                  conf[__configinfo_struct[:current_value]] = nil
                end
              end

            when /^(#{__numstrval_optkeys.join('|')})$/
              if ( __configinfo_struct[:default_value] \
                  && conf[__configinfo_struct[:default_value]] )
                conf[__configinfo_struct[:default_value]] =
                  num_or_str(conf[__configinfo_struct[:default_value]])
              end
              if ( conf[__configinfo_struct[:current_value]] )
                conf[__configinfo_struct[:current_value]] =
                  num_or_str(conf[__configinfo_struct[:current_value]])
              end

            when /^(#{__boolval_optkeys.join('|')})$/
              if ( __configinfo_struct[:default_value] \
                  && conf[__configinfo_struct[:default_value]] )
                begin
                  conf[__configinfo_struct[:default_value]] =
                    bool(conf[__configinfo_struct[:default_value]])
                rescue
                  conf[__configinfo_struct[:default_value]] = nil
                end
              end
              if ( conf[__configinfo_struct[:current_value]] )
                begin
                  conf[__configinfo_struct[:current_value]] =
                    bool(conf[__configinfo_struct[:current_value]])
                rescue
                  conf[__configinfo_struct[:current_value]] = nil
                end
              end

            when /^(#{__listval_optkeys.join('|')})$/
              if ( __configinfo_struct[:default_value] \
                  && conf[__configinfo_struct[:default_value]] )
                conf[__configinfo_struct[:default_value]] =
                  simplelist(conf[__configinfo_struct[:default_value]])
              end
              if ( conf[__configinfo_struct[:current_value]] )
                conf[__configinfo_struct[:current_value]] =
                  simplelist(conf[__configinfo_struct[:current_value]])
              end

            when /^(#{__numlistval_optkeys.join('|')})$/
              if ( __configinfo_struct[:default_value] \
                  && conf[__configinfo_struct[:default_value]] \
                  && conf[__configinfo_struct[:default_value]] =~ /^[0-9]/ )
                conf[__configinfo_struct[:default_value]] =
                  list(conf[__configinfo_struct[:default_value]])
              end
              if ( conf[__configinfo_struct[:current_value]] \
                  && conf[__configinfo_struct[:current_value]] =~ /^[0-9]/ )
                conf[__configinfo_struct[:current_value]] =
                  list(conf[__configinfo_struct[:current_value]])
              end

            when /^(#{__tkvariable_optkeys.join('|')})$/
              if ( __configinfo_struct[:default_value] \
                  && conf[__configinfo_struct[:default_value]] )
                v = conf[__configinfo_struct[:default_value]]
                if v.empty?
                  conf[__configinfo_struct[:default_value]] = nil
                else
                  conf[__configinfo_struct[:default_value]] = TkVarAccess.new(v)
                end
              end
              if ( conf[__configinfo_struct[:current_value]] )
                v = conf[__configinfo_struct[:current_value]]
                if v.empty?
                  conf[__configinfo_struct[:current_value]] = nil
                else
                  conf[__configinfo_struct[:current_value]] = TkVarAccess.new(v)
                end
              end

            else
              if ( __configinfo_struct[:default_value] \
                  && conf[__configinfo_struct[:default_value]] )
                if conf[__configinfo_struct[:default_value]].index('{')
                  conf[__configinfo_struct[:default_value]] =
                    tk_split_list(conf[__configinfo_struct[:default_value]])
                else
                  conf[__configinfo_struct[:default_value]] =
                    tk_tcl2ruby(conf[__configinfo_struct[:default_value]])
                end
              end
              if conf[__configinfo_struct[:current_value]]
                if conf[__configinfo_struct[:current_value]].index('{')
                  conf[__configinfo_struct[:current_value]] =
                    tk_split_list(conf[__configinfo_struct[:current_value]])
                else
                  conf[__configinfo_struct[:current_value]] =
                    tk_tcl2ruby(conf[__configinfo_struct[:current_value]])
                end
              end
            end

            if ( __configinfo_struct[:alias] \
                && conf.size == __configinfo_struct[:alias] + 1 \
                && conf[__configinfo_struct[:alias]][0] == ?- )
              conf[__configinfo_struct[:alias]] =
                conf[__configinfo_struct[:alias]][1..-1]
            end

            conf
          }

          __font_optkeys.each{|optkey|
            optkey = optkey.to_s
            fontconf = ret.assoc(optkey)
            if fontconf && fontconf.size > 2
              ret.delete_if{|inf| inf[0] =~ /^(|latin|ascii|kanji)#{optkey}$/}
              fnt = fontconf[__configinfo_struct[:default_value]]
              if TkFont.is_system_font?(fnt)
                fontconf[__configinfo_struct[:default_value]] \
                  = TkNamedFont.new(fnt)
              end
              fontconf[__configinfo_struct[:current_value]] = fontobj(optkey)
              ret.push(fontconf)
            end
          }

          __methodcall_optkeys.each{|optkey, m|
            ret << [optkey.to_s, '', '', '', self.__send__(m)]
          }

          ret
        end
      end

    else # ! TkComm::GET_CONFIGINFO_AS_ARRAY
      if (slot &&
          slot.to_s =~ /^(|latin|ascii|kanji)(#{__font_optkeys.join('|')})$/)
        fontkey  = $2
        # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__confinfo_cmd << "-#{fontkey}"))))
        conf = tk_split_simplelist(tk_call_without_enc(*(__confinfo_cmd << "-#{fontkey}")), false, true)
        conf[__configinfo_struct[:key]] =
          conf[__configinfo_struct[:key]][1..-1]

        if ( ! __configinfo_struct[:alias] \
            || conf.size > __configinfo_struct[:alias] + 1 )
          fnt = conf[__configinfo_struct[:default_value]]
          if TkFont.is_system_font?(fnt)
            conf[__configinfo_struct[:default_value]] = TkNamedFont.new(fnt)
          end
          conf[__configinfo_struct[:current_value]] = fontobj(fontkey)
          { conf.shift => conf }
        elsif ( __configinfo_struct[:alias] \
               && conf.size == __configinfo_struct[:alias] + 1 )
          if conf[__configinfo_struct[:alias]][0] == ?-
            conf[__configinfo_struct[:alias]] =
              conf[__configinfo_struct[:alias]][1..-1]
          end
          { conf[0] => conf[1] }
        else
          { conf.shift => conf }
        end
      else
        if slot
          slot = slot.to_s

          alias_name, real_name = __optkey_aliases.find{|k,var| k.to_s == slot}
          if real_name
            slot = real_name.to_s
          end

          case slot
          when /^(#{__val2ruby_optkeys().keys.join('|')})$/
            method = _symbolkey2str(__val2ruby_optkeys())[slot]
            conf = tk_split_simplelist(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}")), false, true)
            if ( __configinfo_struct[:default_value] \
                && conf[__configinfo_struct[:default_value]] )
              optval = conf[__configinfo_struct[:default_value]]
              begin
                val = method.call(optval)
              rescue => e
                warn("Warning:: #{e.message} (when #{method}.call(#{optval.inspect})") if $DEBUG
                val = optval
              end
              conf[__configinfo_struct[:default_value]] = val
            end
            if ( conf[__configinfo_struct[:current_value]] )
              optval = conf[__configinfo_struct[:current_value]]
              begin
                val = method.call(optval)
              rescue => e
                warn("Warning:: #{e.message} (when #{method}.call(#{optval.inspect})") if $DEBUG
                val = optval
              end
              conf[__configinfo_struct[:current_value]] = val
            end

          when /^(#{__methodcall_optkeys.keys.join('|')})$/
            method = _symbolkey2str(__methodcall_optkeys)[slot]
            return {slot => ['', '', '', self.__send__(method)]}

          when /^(#{__numval_optkeys.join('|')})$/
            # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}"))))
            conf = tk_split_simplelist(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}")), false, true)

            if ( __configinfo_struct[:default_value] \
                && conf[__configinfo_struct[:default_value]] )
              begin
                conf[__configinfo_struct[:default_value]] =
                  number(conf[__configinfo_struct[:default_value]])
              rescue
                conf[__configinfo_struct[:default_value]] = nil
              end
            end
            if ( conf[__configinfo_struct[:current_value]] )
              begin
                conf[__configinfo_struct[:current_value]] =
                  number(conf[__configinfo_struct[:current_value]])
              rescue
                conf[__configinfo_struct[:current_value]] = nil
              end
            end

          when /^(#{__numstrval_optkeys.join('|')})$/
            # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}"))))
            conf = tk_split_simplelist(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}")), false, true)

            if ( __configinfo_struct[:default_value] \
                && conf[__configinfo_struct[:default_value]] )
              conf[__configinfo_struct[:default_value]] =
                num_or_str(conf[__configinfo_struct[:default_value]])
            end
            if ( conf[__configinfo_struct[:current_value]] )
              conf[__configinfo_struct[:current_value]] =
                num_or_str(conf[__configinfo_struct[:current_value]])
            end

          when /^(#{__boolval_optkeys.join('|')})$/
            # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}"))))
            conf = tk_split_simplelist(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}")), false, true)

            if ( __configinfo_struct[:default_value] \
                && conf[__configinfo_struct[:default_value]] )
              begin
                conf[__configinfo_struct[:default_value]] =
                  bool(conf[__configinfo_struct[:default_value]])
              rescue
                conf[__configinfo_struct[:default_value]] = nil
              end
            end
            if ( conf[__configinfo_struct[:current_value]] )
              begin
                conf[__configinfo_struct[:current_value]] =
                  bool(conf[__configinfo_struct[:current_value]])
              rescue
                conf[__configinfo_struct[:current_value]] = nil
              end
            end

          when /^(#{__listval_optkeys.join('|')})$/
            # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}"))))
            conf = tk_split_simplelist(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}")), false, true)

            if ( __configinfo_struct[:default_value] \
                && conf[__configinfo_struct[:default_value]] )
              conf[__configinfo_struct[:default_value]] =
                simplelist(conf[__configinfo_struct[:default_value]])
            end
            if ( conf[__configinfo_struct[:current_value]] )
              conf[__configinfo_struct[:current_value]] =
                simplelist(conf[__configinfo_struct[:current_value]])
            end

          when /^(#{__numlistval_optkeys.join('|')})$/
            # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}"))))
            conf = tk_split_simplelist(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}")), false, true)

            if ( __configinfo_struct[:default_value] \
                && conf[__configinfo_struct[:default_value]] \
                && conf[__configinfo_struct[:default_value]] =~ /^[0-9]/ )
              conf[__configinfo_struct[:default_value]] =
                list(conf[__configinfo_struct[:default_value]])
            end
            if ( conf[__configinfo_struct[:current_value]] \
                && conf[__configinfo_struct[:current_value]] =~ /^[0-9]/ )
              conf[__configinfo_struct[:current_value]] =
                list(conf[__configinfo_struct[:current_value]])
            end

          when /^(#{__tkvariable_optkeys.join('|')})$/
            conf = tk_split_simplelist(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}")), false, true)

            if ( __configinfo_struct[:default_value] \
                && conf[__configinfo_struct[:default_value]] )
              v = conf[__configinfo_struct[:default_value]]
              if v.empty?
                conf[__configinfo_struct[:default_value]] = nil
              else
                conf[__configinfo_struct[:default_value]] = TkVarAccess.new(v)
              end
            end
            if ( conf[__configinfo_struct[:current_value]] )
              v = conf[__configinfo_struct[:current_value]]
              if v.empty?
                conf[__configinfo_struct[:current_value]] = nil
              else
                conf[__configinfo_struct[:current_value]] = TkVarAccess.new(v)
              end
            end

          when /^(#{__strval_optkeys.join('|')})$/
            # conf = tk_split_simplelist(_fromUTF8(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}"))))
            conf = tk_split_simplelist(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}")), false, true)
          else
            # conf = tk_split_list(_fromUTF8(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}"))))
            conf = tk_split_list(tk_call_without_enc(*(__confinfo_cmd << "-#{slot}")), 0, false, true)
          end
          conf[__configinfo_struct[:key]] =
            conf[__configinfo_struct[:key]][1..-1]

          if ( __configinfo_struct[:alias] \
              && conf.size == __configinfo_struct[:alias] + 1 )
            if conf[__configinfo_struct[:alias]][0] == ?-
              conf[__configinfo_struct[:alias]] =
                conf[__configinfo_struct[:alias]][1..-1]
            end
            { conf[0] => conf[1] }
          else
            { conf.shift => conf }
          end

        else
          ret = {}
          # tk_split_simplelist(_fromUTF8(tk_call_without_enc(*__confinfo_cmd))).each{|conflist|
          #  conf = tk_split_simplelist(conflist)
          tk_split_simplelist(tk_call_without_enc(*__confinfo_cmd), false, false).each{|conflist|
            conf = tk_split_simplelist(conflist, false, true)
            conf[__configinfo_struct[:key]] =
              conf[__configinfo_struct[:key]][1..-1]

            optkey = conf[__configinfo_struct[:key]]
            case optkey
            when /^(#{__val2ruby_optkeys().keys.join('|')})$/
              method = _symbolkey2str(__val2ruby_optkeys())[optkey]
              if ( __configinfo_struct[:default_value] \
                  && conf[__configinfo_struct[:default_value]] )
                optval = conf[__configinfo_struct[:default_value]]
                begin
                  val = method.call(optval)
                rescue => e
                  warn("Warning:: #{e.message} (when #{method}.call(#{optval.inspect})") if $DEBUG
                  val = optval
                end
                conf[__configinfo_struct[:default_value]] = val
              end
              if ( conf[__configinfo_struct[:current_value]] )
                optval = conf[__configinfo_struct[:current_value]]
                begin
                  val = method.call(optval)
                rescue => e
                  warn("Warning:: #{e.message} (when #{method}.call(#{optval.inspect})") if $DEBUG
                  val = optval
                end
                conf[__configinfo_struct[:current_value]] = val
              end

            when /^(#{__strval_optkeys.join('|')})$/
              # do nothing

            when /^(#{__numval_optkeys.join('|')})$/
              if ( __configinfo_struct[:default_value] \
                  && conf[__configinfo_struct[:default_value]] )
                begin
                  conf[__configinfo_struct[:default_value]] =
                    number(conf[__configinfo_struct[:default_value]])
                rescue
                  conf[__configinfo_struct[:default_value]] = nil
                end
              end
              if ( conf[__configinfo_struct[:current_value]] )
                begin
                  conf[__configinfo_struct[:current_value]] =
                    number(conf[__configinfo_struct[:current_value]])
                rescue
                  conf[__configinfo_struct[:current_value]] = nil
                end
              end

            when /^(#{__numstrval_optkeys.join('|')})$/
              if ( __configinfo_struct[:default_value] \
                  && conf[__configinfo_struct[:default_value]] )
                conf[__configinfo_struct[:default_value]] =
                  num_or_str(conf[__configinfo_struct[:default_value]])
              end
              if ( conf[__configinfo_struct[:current_value]] )
                conf[__configinfo_struct[:current_value]] =
                  num_or_str(conf[__configinfo_struct[:current_value]])
              end

            when /^(#{__boolval_optkeys.join('|')})$/
              if ( __configinfo_struct[:default_value] \
                  && conf[__configinfo_struct[:default_value]] )
                begin
                  conf[__configinfo_struct[:default_value]] =
                    bool(conf[__configinfo_struct[:default_value]])
                rescue
                  conf[__configinfo_struct[:default_value]] = nil
                end
              end
              if ( conf[__configinfo_struct[:current_value]] )
                begin
                  conf[__configinfo_struct[:current_value]] =
                    bool(conf[__configinfo_struct[:current_value]])
                rescue
                  conf[__configinfo_struct[:current_value]] = nil
                end
              end

            when /^(#{__listval_optkeys.join('|')})$/
              if ( __configinfo_struct[:default_value] \
                  && conf[__configinfo_struct[:default_value]] )
                conf[__configinfo_struct[:default_value]] =
                  simplelist(conf[__configinfo_struct[:default_value]])
              end
              if ( conf[__configinfo_struct[:current_value]] )
                conf[__configinfo_struct[:current_value]] =
                  simplelist(conf[__configinfo_struct[:current_value]])
              end

            when /^(#{__numlistval_optkeys.join('|')})$/
              if ( __configinfo_struct[:default_value] \
                  && conf[__configinfo_struct[:default_value]] \
                  && conf[__configinfo_struct[:default_value]] =~ /^[0-9]/ )
                conf[__configinfo_struct[:default_value]] =
                  list(conf[__configinfo_struct[:default_value]])
              end
              if ( conf[__configinfo_struct[:current_value]] \
                  && conf[__configinfo_struct[:current_value]] =~ /^[0-9]/ )
                conf[__configinfo_struct[:current_value]] =
                  list(conf[__configinfo_struct[:current_value]])
              end

            when /^(#{__tkvariable_optkeys.join('|')})$/
              if ( __configinfo_struct[:default_value] \
                  && conf[__configinfo_struct[:default_value]] )
                v = conf[__configinfo_struct[:default_value]]
                if v.empty?
                  conf[__configinfo_struct[:default_value]] = nil
                else
                  conf[__configinfo_struct[:default_value]] = TkVarAccess.new
                end
              end
              if ( conf[__configinfo_struct[:current_value]] )
                v = conf[__configinfo_struct[:current_value]]
                if v.empty?
                  conf[__configinfo_struct[:current_value]] = nil
                else
                  conf[__configinfo_struct[:current_value]] = TkVarAccess.new
                end
              end

            else
              if ( __configinfo_struct[:default_value] \
                  && conf[__configinfo_struct[:default_value]] )
                if conf[__configinfo_struct[:default_value]].index('{')
                  conf[__configinfo_struct[:default_value]] =
                    tk_split_list(conf[__configinfo_struct[:default_value]])
                else
                  conf[__configinfo_struct[:default_value]] =
                    tk_tcl2ruby(conf[__configinfo_struct[:default_value]])
                end
              end
              if conf[__configinfo_struct[:current_value]]
                if conf[__configinfo_struct[:current_value]].index('{')
                  conf[__configinfo_struct[:current_value]] =
                    tk_split_list(conf[__configinfo_struct[:current_value]])
                else
                  conf[__configinfo_struct[:current_value]] =
                    tk_tcl2ruby(conf[__configinfo_struct[:current_value]])
                end
              end
            end

            if ( __configinfo_struct[:alias] \
                && conf.size == __configinfo_struct[:alias] + 1 )
              if conf[__configinfo_struct[:alias]][0] == ?-
                conf[__configinfo_struct[:alias]] =
                  conf[__configinfo_struct[:alias]][1..-1]
              end
              ret[conf[0]] = conf[1]
            else
              ret[conf.shift] = conf
            end
          }

          __font_optkeys.each{|optkey|
            optkey = optkey.to_s
            fontconf = ret[optkey]
            if fontconf.kind_of?(Array)
              ret.delete(optkey)
              ret.delete('latin' << optkey)
              ret.delete('ascii' << optkey)
              ret.delete('kanji' << optkey)
              fnt = fontconf[__configinfo_struct[:default_value]]
              if TkFont.is_system_font?(fnt)
                fontconf[__configinfo_struct[:default_value]] \
                  = TkNamedFont.new(fnt)
              end
              fontconf[__configinfo_struct[:current_value]] = fontobj(optkey)
              ret[optkey] = fontconf
            end
          }

          __methodcall_optkeys.each{|optkey, m|
            ret[optkey.to_s] = ['', '', '', self.__send__(m)]
          }

          ret
        end
      end
    end
  end
  private :__configinfo_core

  def configinfo(slot = nil)
    if slot && TkConfigMethod.__IGNORE_UNKNOWN_CONFIGURE_OPTION__
      begin
        __configinfo_core(slot)
      rescue
        Array.new(__configinfo_struct.values.max).unshift(slot.to_s)
      end
    else
      __configinfo_core(slot)
    end
  end

  def current_configinfo(slot = nil)
    if TkComm::GET_CONFIGINFO_AS_ARRAY
      if slot
        org_slot = slot
        begin
          conf = configinfo(slot)
          if ( ! __configinfo_struct[:alias] \
              || conf.size > __configinfo_struct[:alias] + 1 )
            return {conf[0] => conf[-1]}
          end
          slot = conf[__configinfo_struct[:alias]]
        end while(org_slot != slot)
        fail RuntimeError,
          "there is a configure alias loop about '#{org_slot}'"
      else
        ret = {}
        configinfo().each{|cnf|
          if ( ! __configinfo_struct[:alias] \
              || cnf.size > __configinfo_struct[:alias] + 1 )
            ret[cnf[0]] = cnf[-1]
          end
        }
        ret
      end
    else # ! TkComm::GET_CONFIGINFO_AS_ARRAY
      ret = {}
      configinfo(slot).each{|key, cnf|
        ret[key] = cnf[-1] if cnf.kind_of?(Array)
      }
      ret
    end
  end
end

class TkObject<TkKernel
  extend  TkCore
  include Tk
  include TkConfigMethod
  include TkBindCore

### --> definition is moved to TkUtil module
#  def path
#    @path
#  end

  def epath
    @path
  end

  def to_eval
    @path
  end

  def tk_send(cmd, *rest)
    tk_call(path, cmd, *rest)
  end
  def tk_send_without_enc(cmd, *rest)
    tk_call_without_enc(path, cmd, *rest)
  end
  def tk_send_with_enc(cmd, *rest)
    tk_call_with_enc(path, cmd, *rest)
  end
  # private :tk_send, :tk_send_without_enc, :tk_send_with_enc

  def tk_send_to_list(cmd, *rest)
    tk_call_to_list(path, cmd, *rest)
  end
  def tk_send_to_list_without_enc(cmd, *rest)
    tk_call_to_list_without_enc(path, cmd, *rest)
  end
  def tk_send_to_list_with_enc(cmd, *rest)
    tk_call_to_list_with_enc(path, cmd, *rest)
  end
  def tk_send_to_simplelist(cmd, *rest)
    tk_call_to_simplelist(path, cmd, *rest)
  end
  def tk_send_to_simplelist_without_enc(cmd, *rest)
    tk_call_to_simplelist_without_enc(path, cmd, *rest)
  end
  def tk_send_to_simplelist_with_enc(cmd, *rest)
    tk_call_to_simplelist_with_enc(path, cmd, *rest)
  end

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
        if self.kind_of?(TkWindow) && name != "to_ary" && name != "to_str"
          fail NameError,
               "unknown option '#{id}' for #{self.inspect} (deleted widget?)"
        else
          super(id, *args)
        end
#        fail NameError,
#             "undefined local variable or method `#{name}' for #{self.to_s}",
#             error_at
      end
    else
      super(id, *args)
#      fail NameError, "undefined method `#{name}' for #{self.to_s}", error_at
    end
  end

=begin
  def [](id)
    cget(id)
  end

  def []=(id, val)
    configure(id, val)
    val
  end
=end

  def event_generate(context, keys=nil)
    if context.kind_of?(TkEvent::Event)
      context.generate(self, ((keys)? keys: {}))
    elsif keys
      #tk_call('event', 'generate', path,
      #       "<#{tk_event_sequence(context)}>", *hash_kv(keys))
      tk_call_without_enc('event', 'generate', path,
                          "<#{tk_event_sequence(context)}>",
                          *hash_kv(keys, true))
    else
      #tk_call('event', 'generate', path, "<#{tk_event_sequence(context)}>")
      tk_call_without_enc('event', 'generate', path,
                          "<#{tk_event_sequence(context)}>")
    end
  end

  def tk_trace_variable(v)
    #unless v.kind_of?(TkVariable)
    #  fail(ArgumentError, "type error (#{v.class}); must be TkVariable object")
    #end
    v
  end
  private :tk_trace_variable

  def destroy
    #tk_call 'trace', 'vdelete', @tk_vn, 'w', @var_id if @var_id
  end
end


class TkWindow<TkObject
  include TkWinfo
  extend TkBindCore
  include Tk::Wm_for_General
  include Tk::Busy

  @@WIDGET_INSPECT_FULL = false
  def TkWindow._widget_inspect_full_?
    @@WIDGET_INSPECT_FULL
  end
  def TkWindow._widget_inspect_full_=(mode)
    @@WIDGET_INSPECT_FULL = (mode && true) || false
  end

  TkCommandNames = [].freeze
  ## ==> If TkCommandNames[0] is a string (not a null string),
  ##     assume the string is a Tcl/Tk's create command of the widget class.
  WidgetClassName = ''.freeze
  # WidgetClassNames[WidgetClassName] = self
  ## ==> If self is a widget class, entry to the WidgetClassNames table.
  def self.to_eval
    self::WidgetClassName
  end

  def initialize(parent=nil, keys=nil)
    if parent.kind_of? Hash
      keys = _symbolkey2str(parent)
      parent = keys.delete('parent')
      widgetname = keys.delete('widgetname')
      install_win(if parent then parent.path end, widgetname)
      without_creating = keys.delete('without_creating')
      # if without_creating && !widgetname
      #   fail ArgumentError,
      #        "if set 'without_creating' to true, need to define 'widgetname'"
      # end
    elsif keys
      keys = _symbolkey2str(keys)
      widgetname = keys.delete('widgetname')
      install_win(if parent then parent.path end, widgetname)
      without_creating = keys.delete('without_creating')
      # if without_creating && !widgetname
      #   fail ArgumentError,
      #        "if set 'without_creating' to true, need to define 'widgetname'"
      # end
    else
      install_win(if parent then parent.path end)
    end
    if self.method(:create_self).arity == 0
      p 'create_self has no arg' if $DEBUG
      create_self unless without_creating
      if keys
        # tk_call @path, 'configure', *hash_kv(keys)
        configure(keys)
      end
    else
      p 'create_self has args' if $DEBUG
      fontkeys = {}
      methodkeys = {}
      if keys
        #['font', 'kanjifont', 'latinfont', 'asciifont'].each{|key|
        #  fontkeys[key] = keys.delete(key) if keys.key?(key)
        #}
        __font_optkeys.each{|key|
          fkey = key.to_s
          fontkeys[fkey] = keys.delete(fkey) if keys.key?(fkey)

          fkey = "kanji#{key}"
          fontkeys[fkey] = keys.delete(fkey) if keys.key?(fkey)

          fkey = "latin#{key}"
          fontkeys[fkey] = keys.delete(fkey) if keys.key?(fkey)

          fkey = "ascii#{key}"
          fontkeys[fkey] = keys.delete(fkey) if keys.key?(fkey)
        }

        __optkey_aliases.each{|alias_name, real_name|
          alias_name = alias_name.to_s
          if keys.has_key?(alias_name)
            keys[real_name.to_s] = keys.delete(alias_name)
          end
        }

        __methodcall_optkeys.each{|key|
          key = key.to_s
          methodkeys[key] = keys.delete(key) if keys.key?(key)
        }

        __ruby2val_optkeys.each{|key, method|
          key = key.to_s
          keys[key] = method.call(keys[key]) if keys.has_key?(key)
        }
      end
      if without_creating && keys
        #configure(keys)
        configure(__conv_keyonly_opts(keys))
      else
        #create_self(keys)
        create_self(__conv_keyonly_opts(keys))
      end
      font_configure(fontkeys) unless fontkeys.empty?
      configure(methodkeys) unless methodkeys.empty?
    end
  end

  def create_self(keys)
    # may need to override
    begin
      cmd = self.class::TkCommandNames[0]
      fail unless (cmd.kind_of?(String) && cmd.length > 0)
    rescue
      fail RuntimeError, "class #{self.class} may be an abstract class"
    end

    if keys and keys != None
      unless TkConfigMethod.__IGNORE_UNKNOWN_CONFIGURE_OPTION__
        tk_call_without_enc(cmd, @path, *hash_kv(keys, true))
      else
        begin
          tk_call_without_enc(cmd, @path, *hash_kv(keys, true))
        rescue => e
          tk_call_without_enc(cmd, @path)
          keys = __check_available_configure_options(keys)
          unless keys.empty?
            begin
              # try to configure
              configure(keys)
            rescue
              # fail => includes options adaptable when creattion only?
              begin
                tk_call_without_enc('destroy', @path)
              rescue
                # cannot rescue options error
                fail e
              else
                # re-create widget
                tk_call_without_enc(cmd, @path, *hash_kv(keys, true))
              end
            end
          end
        end
      end
    else
      tk_call_without_enc(cmd, @path)
    end
  end
  private :create_self

  def inspect
    if @@WIDGET_INSPECT_FULL
      super
    else
      str = super
      str[0..(str.index(' '))] << '@path=' << @path.inspect << '>'
    end
  end

  def exist?
    TkWinfo.exist?(self)
  end

  def bind_class
    @db_class || self.class()
  end

  def database_classname
    TkWinfo.classname(self)
  end
  def database_class
    name = database_classname()
    if WidgetClassNames[name]
      WidgetClassNames[name]
    else
      TkDatabaseClass.new(name)
    end
  end
  def self.database_classname
    self::WidgetClassName
  end
  def self.database_class
    WidgetClassNames[self::WidgetClassName]
  end

  def pack(keys = nil)
    #tk_call_without_enc('pack', epath, *hash_kv(keys, true))
    if keys
      TkPack.configure(self, keys)
    else
      TkPack.configure(self)
    end
    self
  end

  def pack_in(target, keys = nil)
    if keys
      keys = keys.dup
      keys['in'] = target
    else
      keys = {'in'=>target}
    end
    #tk_call 'pack', epath, *hash_kv(keys)
    TkPack.configure(self, keys)
    self
  end

  def pack_forget
    #tk_call_without_enc('pack', 'forget', epath)
    TkPack.forget(self)
    self
  end
  alias unpack pack_forget

  def pack_config(slot, value=None)
    #if slot.kind_of? Hash
    #  tk_call 'pack', 'configure', epath, *hash_kv(slot)
    #else
    #  tk_call 'pack', 'configure', epath, "-#{slot}", value
    #end
    if slot.kind_of? Hash
      TkPack.configure(self, slot)
    else
      TkPack.configure(self, slot=>value)
    end
  end
  alias pack_configure pack_config

  def pack_info()
    #ilist = list(tk_call('pack', 'info', epath))
    #info = {}
    #while key = ilist.shift
    #  info[key[1..-1]] = ilist.shift
    #end
    #return info
    TkPack.info(self)
  end

  def pack_propagate(mode=None)
    #if mode == None
    #  bool(tk_call('pack', 'propagate', epath))
    #else
    #  tk_call('pack', 'propagate', epath, mode)
    #  self
    #end
    if mode == None
      TkPack.propagate(self)
    else
      TkPack.propagate(self, mode)
      self
    end
  end

  def pack_slaves()
    #list(tk_call('pack', 'slaves', epath))
    TkPack.slaves(self)
  end

  def grid(keys = nil)
    #tk_call 'grid', epath, *hash_kv(keys)
    if keys
      TkGrid.configure(self, keys)
    else
      TkGrid.configure(self)
    end
    self
  end

  def grid_in(target, keys = nil)
    if keys
      keys = keys.dup
      keys['in'] = target
    else
      keys = {'in'=>target}
    end
    #tk_call 'grid', epath, *hash_kv(keys)
    TkGrid.configure(self, keys)
    self
  end

  def grid_anchor(anchor=None)
    if anchor == None
      TkGrid.anchor(self)
    else
      TkGrid.anchor(self, anchor)
      self
    end
  end

  def grid_forget
    #tk_call('grid', 'forget', epath)
    TkGrid.forget(self)
    self
  end
  alias ungrid grid_forget

  def grid_bbox(*args)
    #list(tk_call('grid', 'bbox', epath, *args))
    TkGrid.bbox(self, *args)
  end

  def grid_config(slot, value=None)
    #if slot.kind_of? Hash
    #  tk_call 'grid', 'configure', epath, *hash_kv(slot)
    #else
    #  tk_call 'grid', 'configure', epath, "-#{slot}", value
    #end
    if slot.kind_of? Hash
      TkGrid.configure(self, slot)
    else
      TkGrid.configure(self, slot=>value)
    end
  end
  alias grid_configure grid_config

  def grid_columnconfig(index, keys)
    #tk_call('grid', 'columnconfigure', epath, index, *hash_kv(keys))
    TkGrid.columnconfigure(self, index, keys)
  end
  alias grid_columnconfigure grid_columnconfig

  def grid_rowconfig(index, keys)
    #tk_call('grid', 'rowconfigure', epath, index, *hash_kv(keys))
    TkGrid.rowconfigure(self, index, keys)
  end
  alias grid_rowconfigure grid_rowconfig

  def grid_columnconfiginfo(index, slot=nil)
    #if slot
    #  tk_call('grid', 'columnconfigure', epath, index, "-#{slot}").to_i
    #else
    #  ilist = list(tk_call('grid', 'columnconfigure', epath, index))
    #  info = {}
    #  while key = ilist.shift
    #   info[key[1..-1]] = ilist.shift
    #  end
    #  info
    #end
    TkGrid.columnconfiginfo(self, index, slot)
  end

  def grid_rowconfiginfo(index, slot=nil)
    #if slot
    #  tk_call('grid', 'rowconfigure', epath, index, "-#{slot}").to_i
    #else
    #  ilist = list(tk_call('grid', 'rowconfigure', epath, index))
    #  info = {}
    #  while key = ilist.shift
    #   info[key[1..-1]] = ilist.shift
    #  end
    #  info
    #end
    TkGrid.rowconfiginfo(self, index, slot)
  end

  def grid_column(index, keys=nil)
    if keys.kind_of?(Hash)
      grid_columnconfigure(index, keys)
    else
      grid_columnconfiginfo(index, keys)
    end
  end

  def grid_row(index, keys=nil)
    if keys.kind_of?(Hash)
      grid_rowconfigure(index, keys)
    else
      grid_rowconfiginfo(index, keys)
    end
  end

  def grid_info()
    #list(tk_call('grid', 'info', epath))
    TkGrid.info(self)
  end

  def grid_location(x, y)
    #list(tk_call('grid', 'location', epath, x, y))
    TkGrid.location(self, x, y)
  end

  def grid_propagate(mode=None)
    #if mode == None
    #  bool(tk_call('grid', 'propagate', epath))
    #else
    #  tk_call('grid', 'propagate', epath, mode)
    #  self
    #end
    if mode == None
      TkGrid.propagate(self)
    else
      TkGrid.propagate(self, mode)
      self
    end
  end

  def grid_remove()
    #tk_call 'grid', 'remove', epath
    TkGrid.remove(self)
    self
  end

  def grid_size()
    #list(tk_call('grid', 'size', epath))
    TkGrid.size(self)
  end

  def grid_slaves(keys = nil)
    #list(tk_call('grid', 'slaves', epath, *hash_kv(args)))
    TkGrid.slaves(self, keys)
  end

  def place(keys)
    #tk_call 'place', epath, *hash_kv(keys)
    TkPlace.configure(self, keys)
    self
  end

  def place_in(target, keys = nil)
    if keys
      keys = keys.dup
      keys['in'] = target
    else
      keys = {'in'=>target}
    end
    #tk_call 'place', epath, *hash_kv(keys)
    TkPlace.configure(self, keys)
    self
  end

  def  place_forget
    #tk_call 'place', 'forget', epath
    TkPlace.forget(self)
    self
  end
  alias unplace place_forget

  def place_config(slot, value=None)
    #if slot.kind_of? Hash
    #  tk_call 'place', 'configure', epath, *hash_kv(slot)
    #else
    #  tk_call 'place', 'configure', epath, "-#{slot}", value
    #end
    TkPlace.configure(self, slot, value)
  end
  alias place_configure place_config

  def place_configinfo(slot = nil)
    # for >= Tk8.4a2 ?
    #if slot
    #  conf = tk_split_list(tk_call('place', 'configure', epath, "-#{slot}") )
    #  conf[0] = conf[0][1..-1]
    #  conf
    #else
    #  tk_split_simplelist(tk_call('place',
    #                             'configure', epath)).collect{|conflist|
    #   conf = tk_split_simplelist(conflist)
    #   conf[0] = conf[0][1..-1]
    #   conf
    #  }
    #end
    TkPlace.configinfo(self, slot)
  end

  def place_info()
    #ilist = list(tk_call('place', 'info', epath))
    #info = {}
    #while key = ilist.shift
    #  info[key[1..-1]] = ilist.shift
    #end
    #return info
    TkPlace.info(self)
  end

  def place_slaves()
    #list(tk_call('place', 'slaves', epath))
    TkPlace.slaves(self)
  end

  def set_focus(force=false)
    if force
      tk_call_without_enc('focus', '-force', path)
    else
      tk_call_without_enc('focus', path)
    end
    self
  end
  alias focus set_focus

  def grab(opt = nil)
    unless opt
      tk_call_without_enc('grab', 'set', path)
      return self
    end

    case opt
    when 'set', :set
      tk_call_without_enc('grab', 'set', path)
      return self
    when 'global', :global
      #return(tk_call('grab', 'set', '-global', path))
      tk_call_without_enc('grab', 'set', '-global', path)
      return self
    when 'release', :release
      #return tk_call('grab', 'release', path)
      tk_call_without_enc('grab', 'release', path)
      return self
    when 'current', :current
      return window(tk_call_without_enc('grab', 'current', path))
    when 'status', :status
      return tk_call_without_enc('grab', 'status', path)
    else
      return tk_call_without_enc('grab', opt, path)
    end
  end

  def grab_current
    grab('current')
  end
  alias current_grab grab_current
  def grab_release
    grab('release')
  end
  alias release_grab grab_release
  def grab_set
    grab('set')
  end
  alias set_grab grab_set
  def grab_set_global
    grab('global')
  end
  alias set_global_grab grab_set_global
  def grab_status
    grab('status')
  end

  def lower(below=None)
    # below = below.epath if below.kind_of?(TkObject)
    below = _epath(below)
    tk_call 'lower', epath, below
    self
  end
  alias lower_window lower
  def raise(above=None)
    #above = above.epath if above.kind_of?(TkObject)
    above = _epath(above)
    tk_call 'raise', epath, above
    self
  end
  alias raise_window raise

  def command(cmd=nil, &b)
    if cmd
      configure_cmd('command', cmd)
    elsif b
      configure_cmd('command', Proc.new(&b))
    else
      cget('command')
    end
  end

  def colormodel(model=None)
    tk_call('tk', 'colormodel', path, model)
    self
  end

  def caret(keys=nil)
    TkXIM.caret(path, keys)
  end

  def destroy
    super
    children = []
    rexp = /^#{self.path}\.[^.]+$/
    TkCore::INTERP.tk_windows.each{|path, obj|
      children << [path, obj] if path =~ rexp
    }
    if defined?(@cmdtbl)
      for id in @cmdtbl
        uninstall_cmd id
      end
    end

    children.each{|path, obj|
      obj.instance_eval{
        if defined?(@cmdtbl)
          for id in @cmdtbl
            uninstall_cmd id
          end
        end
      }
      TkCore::INTERP.tk_windows.delete(path)
    }

    begin
      tk_call_without_enc('destroy', epath)
    rescue
    end
    uninstall_win
  end

  def wait_visibility(on_thread = true)
    if $SAFE >= 4
      fail SecurityError, "can't wait visibility at $SAFE >= 4"
    end
    on_thread &= (Thread.list.size != 1)
    if on_thread
      INTERP._thread_tkwait('visibility', path)
    else
      INTERP._invoke('tkwait', 'visibility', path)
    end
  end
  def eventloop_wait_visibility
    wait_visibility(false)
  end
  def thread_wait_visibility
    wait_visibility(true)
  end
  alias wait wait_visibility
  alias tkwait wait_visibility
  alias eventloop_wait eventloop_wait_visibility
  alias eventloop_tkwait eventloop_wait_visibility
  alias eventloop_tkwait_visibility eventloop_wait_visibility
  alias thread_wait thread_wait_visibility
  alias thread_tkwait thread_wait_visibility
  alias thread_tkwait_visibility thread_wait_visibility

  def wait_destroy(on_thread = true)
    if $SAFE >= 4
      fail SecurityError, "can't wait destroy at $SAFE >= 4"
    end
    on_thread &= (Thread.list.size != 1)
    if on_thread
      INTERP._thread_tkwait('window', epath)
    else
      INTERP._invoke('tkwait', 'window', epath)
    end
  end
  alias wait_window wait_destroy
  def eventloop_wait_destroy
    wait_destroy(false)
  end
  alias eventloop_wait_window eventloop_wait_destroy
  def thread_wait_destroy
    wait_destroy(true)
  end
  alias thread_wait_window thread_wait_destroy

  alias tkwait_destroy wait_destroy
  alias tkwait_window wait_destroy

  alias eventloop_tkwait_destroy eventloop_wait_destroy
  alias eventloop_tkwait_window eventloop_wait_destroy

  alias thread_tkwait_destroy thread_wait_destroy
  alias thread_tkwait_window thread_wait_destroy

  def bindtags(taglist=nil)
    if taglist
      fail ArgumentError, "taglist must be Array" unless taglist.kind_of? Array
      tk_call('bindtags', path, taglist)
      taglist
    else
      list(tk_call('bindtags', path)).collect{|tag|
        if tag.kind_of?(String)
          if cls = WidgetClassNames[tag]
            cls
          elsif btag = TkBindTag.id2obj(tag)
            btag
          else
            tag
          end
        else
          tag
        end
      }
    end
  end

  def bindtags=(taglist)
    bindtags(taglist)
    taglist
  end

  def bindtags_shift
    taglist = bindtags
    tag = taglist.shift
    bindtags(taglist)
    tag
  end

  def bindtags_unshift(tag)
    bindtags(bindtags().unshift(tag))
  end
end
TkWidget = TkWindow

# freeze core modules
#TclTkLib.freeze
#TclTkIp.freeze
#TkUtil.freeze
#TkKernel.freeze
#TkComm.freeze
#TkComm::Event.freeze
#TkCore.freeze
#Tk.freeze

module Tk
  RELEASE_DATE = '2010-06-03'.freeze

  autoload :AUTO_PATH,        'tk/variable'
  autoload :TCL_PACKAGE_PATH, 'tk/variable'
  autoload :PACKAGE_PATH,     'tk/variable'
  autoload :TCL_LIBRARY_PATH, 'tk/variable'
  autoload :LIBRARY_PATH,     'tk/variable'
  autoload :TCL_PRECISION,    'tk/variable'
end

# call setup script for Tk extension libraries (base configuration)
begin
  require 'tkextlib/version.rb'
  require 'tkextlib/setup.rb'
rescue LoadError
  # ignore
end
