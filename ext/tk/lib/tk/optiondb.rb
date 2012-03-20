#
# tk/optiondb.rb : treat option database
#
require 'tk'

module TkOptionDB
  include Tk
  extend Tk

  TkCommandNames = ['option'.freeze].freeze
  (CmdClassID = ['CMD_CLASS'.freeze, TkUtil.untrust('00000')]).instance_eval{
    @mutex = Mutex.new
    def mutex; @mutex; end
    freeze
  }

  module Priority
    WidgetDefault = 20
    StartupFile   = 40
    UserDefault   = 60
    Interactive   = 80
  end

  def add(pat, value, pri=None)
    # if $SAFE >= 4
    #   fail SecurityError, "can't call 'TkOptionDB.add' at $SAFE >= 4"
    # end
    tk_call('option', 'add', pat, value, pri)
  end
  def clear
    # if $SAFE >= 4
    #   fail SecurityError, "can't call 'TkOptionDB.crear' at $SAFE >= 4"
    # end
    tk_call_without_enc('option', 'clear')
  end
  def get(win, name, klass)
    tk_call('option', 'get', win ,name, klass)
  end
  def readfile(file, pri=None)
    tk_call('option', 'readfile', file, pri)
  end
  alias read_file readfile
  module_function :add, :clear, :get, :readfile, :read_file

  def read_entries(file, f_enc=nil)
    if TkCore::INTERP.safe?
      fail SecurityError,
        "can't call 'TkOptionDB.read_entries' on a safe interpreter"
    end

    i_enc = ((Tk.encoding)? Tk.encoding : Tk.encoding_system)

    unless f_enc
      f_enc = i_enc
    end

    ent = []
    cline = ''
    open(file, 'r') {|f|
      while line = f.gets
        #cline += line.chomp!
        cline.concat(line.chomp!)
        case cline
        when /\\$/    # continue
          cline.chop!
          next
        when /^\s*(!|#)/     # coment
          cline = ''
          next
        when /^([^:]+):(.*)$/
          pat = $1.strip
          val = $2.lstrip
          p "ResourceDB: #{[pat, val].inspect}" if $DEBUG
          pat = TkCore::INTERP._toUTF8(pat, f_enc)
          pat = TkCore::INTERP._fromUTF8(pat, i_enc)
          val = TkCore::INTERP._toUTF8(val, f_enc)
          val = TkCore::INTERP._fromUTF8(val, i_enc)
          ent << [pat, val]
          cline = ''
        else          # unknown --> ignore
          cline = ''
          next
        end
      end
    }
    ent
  end
  module_function :read_entries

  def read_with_encoding(file, f_enc=nil, pri=None)
    # try to read the file as an OptionDB file
    read_entries(file, f_enc).each{|pat, val|
      add(pat, val, pri)
    }

=begin
    i_enc = Tk.encoding()

    unless f_enc
      f_enc = i_enc
    end

    cline = ''
    open(file, 'r') {|f|
      while line = f.gets
        cline += line.chomp!
        case cline
        when /\\$/    # continue
          cline.chop!
          next
        when /^\s*!/     # coment
          cline = ''
          next
        when /^([^:]+):\s(.*)$/
          pat = $1
          val = $2
          p "ResourceDB: #{[pat, val].inspect}" if $DEBUG
          pat = TkCore::INTERP._toUTF8(pat, f_enc)
          pat = TkCore::INTERP._fromUTF8(pat, i_enc)
          val = TkCore::INTERP._toUTF8(val, f_enc)
          val = TkCore::INTERP._fromUTF8(val, i_enc)
          add(pat, val, pri)
          cline = ''
        else          # unknown --> ignore
          cline = ''
          next
        end
      end
    }
=end
  end
  module_function :read_with_encoding

  # support procs on the resource database
  @@resource_proc_class = Class.new

  @@resource_proc_class.const_set(:CARRIER, '.'.freeze)

  @@resource_proc_class.instance_variable_set('@method_tbl',
                                              TkCore::INTERP.create_table)
  @@resource_proc_class.instance_variable_set('@add_method', false)
  @@resource_proc_class.instance_variable_set('@safe_mode', 4)

  class << @@resource_proc_class
    private :new

=begin
    CARRIER    = '.'.freeze
    METHOD_TBL = TkCore::INTERP.create_table
    ADD_METHOD = false
    SAFE_MODE  = 4
=end

=begin
    def __closed_block_check__(str)
      depth = 0
      str.scan(/[{}]/){|x|
        if x == "{"
          depth += 1
        elsif x == "}"
          depth -= 1
        end
        if depth <= 0 && !($' =~ /\A\s*\Z/)
          fail RuntimeError, "bad string for procedure : #{str.inspect}"
        end
      }
      str
    end
    private :__closed_block_check__
=end

    def __check_proc_string__(str)
      # If you want to check the proc_string, do it in this method.
      # Please define this in the block given to 'new_proc_class' method.
      str
    end

    def method_missing(id, *args)
      #res_proc, proc_str = self::METHOD_TBL[id]
      res_proc, proc_str = @method_tbl[id]

      proc_source = TkOptionDB.get(self::CARRIER, id.id2name, '').strip
      res_proc = nil if proc_str != proc_source # resource is changed

      # unless res_proc.kind_of?(Proc)
      unless TkComm._callback_entry?(res_proc)
        #if id == :new || !(self::METHOD_TBL.has_key?(id) || self::ADD_METHOD)
        if id == :new || !(@method_tbl.has_key?(id) || @add_method)
          raise NoMethodError,
                "not support resource-proc '#{id.id2name}' for #{self.name}"
        end
        proc_str = proc_source
        proc_str = '{' + proc_str + '}' unless /\A\{.*\}\Z/ =~ proc_str
        #proc_str = __closed_block_check__(proc_str)
        proc_str = __check_proc_string__(proc_str)
        res_proc = proc{
          begin
            #eval("$SAFE = #{self::SAFE_MODE};\nProc.new" + proc_str)
            eval("$SAFE = #{@safe_mode};\nProc.new" + proc_str)
          rescue SyntaxError=>err
            raise SyntaxError,
              TkCore::INTERP._toUTF8(err.message.gsub(/\(eval\):\d:/,
                                                      "(#{id.id2name}):"))
          end
        }.call
        #self::METHOD_TBL[id] = [res_proc, proc_source]
        @method_tbl[id] = [res_proc, proc_source]
      end
      res_proc.call(*args)
    end

    private :__check_proc_string__, :method_missing
  end
  @@resource_proc_class.freeze

=begin
  def __create_new_class(klass, func, safe = 4, add = false, parent = nil)
    klass = klass.to_s if klass.kind_of? Symbol
    unless (?A..?Z) === klass[0]
      fail ArgumentError, "bad string '#{klass}' for class name"
    end
    unless func.kind_of? Array
      fail ArgumentError, "method-list must be Array"
    end
    func_str = func.join(' ')
    if parent == nil
      install_win(parent)
    elsif parent <= @@resource_proc_class
      install_win(parent::CARRIER)
    else
      fail ArgumentError, "parent must be Resource-Proc class"
    end
    carrier = Tk.tk_call_without_enc('frame', @path, '-class', klass)

    body = <<-"EOD"
      class #{klass} < TkOptionDB.module_eval('@@resource_proc_class')
        CARRIER    = '#{carrier}'.freeze
        METHOD_TBL = TkCore::INTERP.create_table
        ADD_METHOD = #{add}
        SAFE_MODE  = #{safe}
        %w(#{func_str}).each{|f| METHOD_TBL[f.intern] = nil }
      end
    EOD

    if parent.kind_of?(Class) && parent <= @@resource_proc_class
      parent.class_eval(body)
      eval(parent.name + '::' + klass)
    else
      eval(body)
      eval('TkOptionDB::' + klass)
    end
  end
=end
  def __create_new_class(klass, func, safe = 4, add = false, parent = nil)
    if klass.kind_of?(TkWindow)
      carrier = klass.path
      CmdClassID.mutex.synchronize{
        klass = CmdClassID.join(TkCore::INTERP._ip_id_)
        CmdClassID[1].succ!
      }
      parent = nil # ignore parent
    else
      klass = klass.to_s if klass.kind_of?(Symbol)
      unless (?A..?Z) === klass[0]
        fail ArgumentError, "bad string '#{klass}' for class name"
      end
      if parent == nil
        install_win(nil)
      elsif parent.kind_of?(TkWindow)
        install_win(parent.path)
      elsif parent <= @@resource_proc_class
        install_win(parent::CARRIER)
      else
        fail ArgumentError, "parent must be Resource-Proc class"
      end
      carrier = Tk.tk_call_without_enc('frame', @path, '-class', klass)
    end

    unless func.kind_of?(Array)
      fail ArgumentError, "method-list must be Array"
    end
    func_str = func.join(' ')

    if parent.kind_of?(Class) && parent <= @@resource_proc_class
      cmd_klass = Class.new(parent)
    else
      cmd_klass = Class.new(TkOptionDB.module_eval('@@resource_proc_class'))
    end
    cmd_klass.const_set(:CARRIER, carrier.dup.freeze)

    cmd_klass.instance_variable_set('@method_tbl', TkCore::INTERP.create_table)
    cmd_klass.instance_variable_set('@add_method', add)
    cmd_klass.instance_variable_set('@safe_mode', safe)
    func.each{|f|
      cmd_klass.instance_variable_get('@method_tbl')[f.to_s.intern] = nil
    }
=begin
    cmd_klass.const_set(:METHOD_TBL, TkCore::INTERP.create_table)
    cmd_klass.const_set(:ADD_METHOD, add)
    cmd_klass.const_set(:SAFE_MODE, safe)
    func.each{|f| cmd_klass::METHOD_TBL[f.to_s.intern] = nil }
=end

    cmd_klass
  end
  module_function :__create_new_class
  private_class_method :__create_new_class

  def __remove_methods_of_proc_class(klass)
    # for security, make these methods invalid
    class << klass
      def __null_method(*args); nil; end
      [ :class_eval, :name, :superclass, :clone, :dup, :autoload, :autoload?,
        :ancestors, :const_defined?, :const_get, :const_set, :const_missing,
        :class_variables, :constants, :included_modules, :instance_methods,
        :method_defined?, :module_eval, :private_instance_methods,
        :protected_instance_methods, :public_instance_methods,
        :singleton_methods, :remove_const, :remove_method, :undef_method,
        :to_s, :inspect, :display, :method, :methods, :respond_to?,
        :instance_variable_get, :instance_variable_set, :instance_method,
        :instance_eval, :instance_exec, :instance_variables, :kind_of?, :is_a?,
        :private_methods, :protected_methods, :public_methods ].each{|m|
        alias_method(m, :__null_method)
      }
    end
  end
  module_function :__remove_methods_of_proc_class
  private_class_method :__remove_methods_of_proc_class

  RAND_BASE_CNT = [0]
  RAND_BASE_HEAD = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'
  RAND_BASE_CHAR = RAND_BASE_HEAD + 'abcdefghijklmnopqrstuvwxyz0123456789_'
  def __get_random_basename
    name = '%s%03d' % [RAND_BASE_HEAD[rand(RAND_BASE_HEAD.size),1],
                       RAND_BASE_CNT[0]]
    len = RAND_BASE_CHAR.size
    (6+rand(10)).times{
      name << RAND_BASE_CHAR[rand(len),1]
    }
    RAND_BASE_CNT[0] = RAND_BASE_CNT[0] + 1
    name
  end
  module_function :__get_random_basename
  private_class_method :__get_random_basename

  # define new proc class :
  # If you want to modify the new class or create a new subclass,
  # you must do such operation in the block parameter.
  # Because the created class is flozen after evaluating the block.
  def new_proc_class(klass, func, safe = 4, add = false, parent = nil, &b)
    new_klass = __create_new_class(klass, func, safe, add, parent)
    new_klass.class_eval(&b) if block_given?
    __remove_methods_of_proc_class(new_klass)
    new_klass.freeze
    new_klass
  end
  module_function :new_proc_class

  def eval_under_random_base(parent = nil, &b)
    new_klass = __create_new_class(__get_random_basename(),
                                   [], 4, false, parent)
    ret = new_klass.class_eval(&b) if block_given?
    __remove_methods_of_proc_class(new_klass)
    new_klass.freeze
    ret
  end
  module_function :eval_under_random_base

  def new_proc_class_random(klass, func, safe = 4, add = false, &b)
    eval_under_random_base(){
      TkOptionDB.new_proc_class(klass, func, safe, add, self, &b)
    }
  end
  module_function :new_proc_class_random
end
TkOption = TkOptionDB
TkResourceDB = TkOptionDB
