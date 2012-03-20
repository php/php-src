#
# tk/composite.rb :
#
require 'tk'

module TkComposite
  include Tk
  extend Tk

=begin
  def initialize(parent=nil, *args)
    @delegates = {}
    @option_methods = {}
    @option_setting = {}

    if parent.kind_of? Hash
      keys = _symbolkey2str(parent)
      parent = keys.delete('parent')
      @frame = TkFrame.new(parent)
      @path = @epath = @frame.path
      initialize_composite(keys)
    else
      @frame = TkFrame.new(parent)
      @path = @epath = @frame.path
      initialize_composite(*args)
    end
  end
=end

  def _choice_classname_of_baseframe
    base_class_name = nil

    klass = WidgetClassNames[self.class::WidgetClassName]

    if klass
      # WidgetClassName is a known class
      #if klass <= TkFrame || klass < TkComposite
      if klass <= TkFrame || klass < Tk::Frame || klass < TkComposite
        # klass is valid for the base frame
        if self.class <= klass
          # use my classname
          base_class_name = self.class.name
          if base_class_name == ''
            # anonymous class -> use ancestor's name
            base_class_name = klass.name
          end
        else
          # not subclass -> use WidgetClassName
          base_class_name = klass.name
        end

      else
        # klass is invalid for the base frame
        #if self.class < TkFrame || self.class.superclass < TkComposite
        if self.class < TkFrame || self.class.superclass < Tk::Frame || self.class.superclass < TkComposite
          # my class name is valid for the base frame -> use my classname
          base_class_name = self.class.name
          if base_class_name == ''
            # anonymous class -> use TkFrame
            base_class_name = nil
          end
        else
          # no idea for the base frame -> use TkFrame
          base_class_name = nil
        end
      end

    elsif self.class::WidgetClassName && ! self.class::WidgetClassName.empty?
      # unknown WidgetClassName is defined -> use it for the base frame
      base_class_name = self.class::WidgetClassName

    else
      # no valid WidgetClassName
      #if self.class < TkFrame || self.class.superclass < TkComposite
      if self.class < TkFrame || self.class.superclass < Tk::Frame || self.class.superclass < TkComposite
        # my class name is valid for the base frame -> use my classname
        base_class_name = self.class.name
        if base_class_name == ''
          # anonymous class -> use TkFrame
          base_class_name = nil
        end
      else
        # no idea for the base frame -> use TkFrame
        base_class_name = nil
      end
    end

    base_class_name
  end
  private :_choice_classname_of_baseframe

  # def initialize(parent=nil, *args)
  def initialize(*args)
    @delegates = {}
    @option_methods = {}
    @option_setting = {}

    if args[-1].kind_of?(Hash)
      keys = _symbolkey2str(args.pop)
    else
      keys = {}
    end
    parent = args.shift
    parent = keys.delete('parent') if keys.has_key?('parent')

    if keys.key?('classname')
      keys['class'] = keys.delete('classname')
    end
    if (base_class_name = (keys.delete('class')).to_s).empty?
      base_class_name = _choice_classname_of_baseframe
    end

    if base_class_name
      # @frame = Tk::Frame.new(parent, :class=>base_class_name)
      # --> use current TkFrame class
      @frame = TkFrame.new(parent, :class=>base_class_name)
    else
      # @frame = Tk::Frame.new(parent)
      # --> use current TkFrame class
      @frame = TkFrame.new(parent)
    end
    @path = @epath = @frame.path

    args.push(keys) unless keys.empty?
    initialize_composite(*args)
  end

  def database_classname
    @frame.database_classname
  end

  def database_class
    @frame.database_class
  end

  def epath
    @epath
  end

  def initialize_composite(*args) end
  private :initialize_composite

  def inspect
    str = super
    str.chop << ' @epath=' << @epath.inspect << '>'
  end

  def _get_opt_method_list(arg)
    m_set, m_cget, m_info = arg
    m_set  = m_set.to_s
    m_cget = m_set if !m_cget && self.method(m_set).arity == -1
    m_cget = m_cget.to_s if m_cget
    m_info = m_info.to_s if m_info
    [m_set, m_cget, m_info]
  end
  private :_get_opt_method_list

  def option_methods(*opts)
    if opts.size == 1 && opts[0].kind_of?(Hash)
      # {name => [m_set, m_cget, m_info], name => method} style
      opts[0].each{|name, arg|
        m_set, m_cget, m_info = _get_opt_method_list(arg)
        @option_methods[name.to_s] = {
          :set => m_set, :cget => m_cget, :info => m_info
        }
      }
    else
      # [m_set, m_cget, m_info] or method style
      opts.each{|arg|
        m_set, m_cget, m_info = _get_opt_method_list(arg)
        @option_methods[m_set] = {
          :set => m_set, :cget => m_cget, :info => m_info
        }
      }
    end
  end

  def delegate_alias(alias_opt, option, *wins)
    if wins.length == 0
      fail ArgumentError, "target widgets are not given"
    end
    if alias_opt != option && (alias_opt == 'DEFAULT' || option == 'DEFAULT')
      fail ArgumentError, "cannot alias 'DEFAULT' option"
    end
    alias_opt = alias_opt.to_s
    option = option.to_s
    if @delegates[alias_opt].kind_of?(Array)
      if (elem = @delegates[alias_opt].assoc(option))
        wins.each{|w| elem[1].push(w)}
      else
        @delegates[alias_opt] << [option, wins]
      end
    else
      @delegates[alias_opt] = [ [option, wins] ]
    end
  end

  def delegate(option, *wins)
    delegate_alias(option, option, *wins)
  end

  def __cget_delegates(slot)
    slot = slot.to_s

    if @option_methods.include?(slot)
      if @option_methods[slot][:cget]
        return self.__send__(@option_methods[slot][:cget])
      else
        if @option_setting[slot]
          return @option_setting[slot]
        else
          return ''
        end
      end
    end

    tbl = @delegates[slot]
    tbl = @delegates['DEFAULT'] unless tbl

    begin
      if tbl
        opt, wins = tbl[-1]
        opt = slot if opt == 'DEFAULT'
        if wins && wins[-1]
          # return wins[-1].cget(opt)
          return wins[-1].cget_strict(opt)
        end
      end
    rescue
    end

    return None
  end
  private :__cget_delegates

  def cget_tkstring(slot)
    if (ret = __cget_delegates(slot)) == None
      super(slot)
    else
      _get_eval_string(ret)
    end
  end

  def cget(slot)
    if (ret = __cget_delegates(slot)) == None
      super(slot)
    else
      ret
    end
  end

  def cget_strict(slot)
    if (ret = __cget_delegates(slot)) == None
      super(slot)
    else
      ret
    end
  end

=begin
  def cget(slot)
    slot = slot.to_s

    if @option_methods.include?(slot)
      if @option_methods[slot][:cget]
        return self.__send__(@option_methods[slot][:cget])
      else
        if @option_setting[slot]
          return @option_setting[slot]
        else
          return ''
        end
      end
    end

    tbl = @delegates[slot]
    tbl = @delegates['DEFAULT'] unless tbl

    begin
      if tbl
        opt, wins = tbl[-1]
        opt = slot if opt == 'DEFAULT'
        if wins && wins[-1]
          return wins[-1].cget(opt)
        end
      end
    rescue
    end

    super(slot)
  end
=end

  def configure(slot, value=None)
    if slot.kind_of? Hash
      slot.each{|slot,value| configure slot, value}
      return self
    end

    slot = slot.to_s

    if @option_methods.include?(slot)
      unless @option_methods[slot][:cget]
        if value.kind_of?(Symbol)
          @option_setting[slot] = value.to_s
        else
          @option_setting[slot] = value
        end
      end
      return self.__send__(@option_methods[slot][:set], value)
    end

    tbl = @delegates[slot]
    tbl = @delegates['DEFAULT'] unless tbl

    begin
      if tbl
        last = nil
        tbl.each{|opt, wins|
          opt = slot if opt == 'DEFAULT'
          wins.each{|w| last = w.configure(opt, value)}
        }
        return last
      end
    rescue
    end

    super(slot, value)
  end

  def configinfo(slot = nil)
    if TkComm::GET_CONFIGINFO_AS_ARRAY
      if slot
        slot = slot.to_s
        if @option_methods.include?(slot)
          if @option_methods[slot][:info]
            return self.__send__(@option_methods[slot][:info])
          else
            return [slot, '', '', '', self.cget(slot)]
          end
        end

        tbl = @delegates[slot]
        tbl = @delegates['DEFAULT'] unless tbl

        begin
          if tbl
            if tbl.length == 1
              opt, wins = tbl[0]
              if slot == opt || opt == 'DEFAULT'
                return wins[-1].configinfo(slot)
              else
                info = wins[-1].configinfo(opt)
                info[0] = slot
                return info
              end
            else
              opt, wins = tbl[-1]
              return [slot, '', '', '', wins[-1].cget(opt)]
            end
          end
        rescue
        end

        super(slot)

      else # slot == nil
        info_list = super(slot)

        tbl = @delegates['DEFAULT']
        if tbl
          wins = tbl[0][1]
          if wins && wins[-1]
            wins[-1].configinfo.each{|info|
              slot = info[0]
              info_list.delete_if{|i| i[0] == slot} << info
            }
          end
        end

        @delegates.each{|slot, tbl|
          next if slot == 'DEFAULT'
          if tbl.length == 1
            opt, wins = tbl[0]
            next unless wins && wins[-1]
            if slot == opt
              info_list.delete_if{|i| i[0] == slot} <<
                wins[-1].configinfo(slot)
            else
              info = wins[-1].configinfo(opt)
              info[0] = slot
              info_list.delete_if{|i| i[0] == slot} << info
            end
          else
            opt, wins = tbl[-1]
            info_list.delete_if{|i| i[0] == slot} <<
              [slot, '', '', '', wins[-1].cget(opt)]
          end
        }

        @option_methods.each{|slot, m|
          if m[:info]
            info = self.__send__(m[:info])
          else
            info = [slot, '', '', '', self.cget(slot)]
          end
          info_list.delete_if{|i| i[0] == slot} << info
        }

        info_list
      end

    else # ! TkComm::GET_CONFIGINFO_AS_ARRAY
      if slot
        slot = slot.to_s
        if @option_methods.include?(slot)
          if @option_methods[slot][:info]
            return self.__send__(@option_methods[slot][:info])
          else
            return {slot => ['', '', '', self.cget(slot)]}
          end
        end

        tbl = @delegates[slot]
        tbl = @delegates['DEFAULT'] unless tbl

        begin
          if tbl
            if tbl.length == 1
              opt, wins = tbl[0]
              if slot == opt || opt == 'DEFAULT'
                return wins[-1].configinfo(slot)
              else
                return {slot => wins[-1].configinfo(opt)[opt]}
              end
            else
              opt, wins = tbl[-1]
              return {slot => ['', '', '', wins[-1].cget(opt)]}
            end
          end
        rescue
        end

        super(slot)

      else # slot == nil
        info_list = super(slot)

        tbl = @delegates['DEFAULT']
        if tbl
          wins = tbl[0][1]
          info_list.update(wins[-1].configinfo) if wins && wins[-1]
        end

        @delegates.each{|slot, tbl|
          next if slot == 'DEFAULT'
          if tbl.length == 1
            opt, wins = tbl[0]
            next unless wins && wins[-1]
            if slot == opt
              info_list.update(wins[-1].configinfo(slot))
            else
              info_list.update({slot => wins[-1].configinfo(opt)[opt]})
            end
          else
            opt, wins = tbl[-1]
            info_list.update({slot => ['', '', '', wins[-1].cget(opt)]})
          end
        }

        @option_methods.each{|slot, m|
          if m[:info]
            info = self.__send__(m[:info])
          else
            info = {slot => ['', '', '', self.cget(slot)]}
          end
          info_list.update(info)
        }

        info_list
      end
    end
  end
end
