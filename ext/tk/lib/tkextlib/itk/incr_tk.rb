#
#  tkextlib/itk/incr_tk.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tk/menuspec'
require 'tkextlib/itcl.rb'

# call setup script
require 'tkextlib/itk.rb'

#TkPackage.require('Itk', '3.2')
TkPackage.require('Itk')

module Tk
  module Itk
    include Tk
    extend Tk

    LIBRARY = TkVarAccess.new('::itk::library')

    PACKAGE_NAME = 'Itk'.freeze
    def self.package_name
      PACKAGE_NAME
    end

    def self.package_version
      begin
        TkPackage.require('Itk')
      rescue
        ''
      end
    end

    def self.usual(arg, *args)
      tk_call('::itk::usual', arg, *args)
    end

    def self.usual_names
      list(tk_call('::itk::usual'))
    end

    ############################

    class Archetype < TkWindow
      TkCommandNames = [].freeze
      # WidgetClassName = 'Archetype'.freeze
      # WidgetClassNames[WidgetClassName] = self

      def self.to_eval
        '::itk::' << self::WidgetClassName
      end

      def __destroy_hook__
        Tk::Itk::Component::ComponentID_TBL.delete(self.path)
      end

      #### [incr Tk] public methods
      def component
        simplelist(tk_send('component'))
      end

      def component_path(name)
        window(tk_send('component', name))
      end
      alias component_widget component_path

      def component_invoke(name, cmd, *args)
        window(tk_send('component', name, cmd, *args))
      end

      def component_obj(*names)
        names = component if names.empty?
        names.collect{|name| Tk::Itk::Component.new(self.path, name) }
      end

      #### [incr Tk] protected methods
=begin
      def itk_component_add(visibility, name, create_cmds, option_cmds=None)
        args = []
        visibility.each{|v| v = v.to_s; args << ( (v[0] == ?-)? v: "-#{v}" )}
        args << '--' << name << create_cmd << option_cmds
        tk_call('itk_component', 'add', *args)
      end

      def itk_component_delete(*names)
        tk_call('itk_component', 'delete', *names)
      end

      def itk_initialize(keys={})
        tk_call('itk_initialize', keys)
      end

      def itk_option_add(*args)
        tk_call('itk_option', 'add', *args)
      end

      def itk_option_define(name, resource, klass, init, config=None)
        tk_call('itk_option', 'define', name, resource, klass, init, config)
      end

      def itk_option_remove(*args)
        tk_call('itk_option', 'remove', *args)
      end
=end
    end

    ############################

    class Toplevel < Archetype
      TkCommandNames = ['::itk::Toplevel'].freeze
      WidgetClassName = 'Toplevel'.freeze
      WidgetClassNames[WidgetClassName] ||= self

      include Wm
      include TkMenuSpec

      def __strval_optkeys
        super() << 'title'
      end
      private :__strval_optkeys
    end

    ############################

    class Widget < Archetype
      TkCommandNames = ['::itk::Widget'].freeze
      WidgetClassName = 'Widget'.freeze
      WidgetClassNames[WidgetClassName] ||= self
    end


    ############################

    class Component < TkObject
      def __cget_cmd
        [self.master, 'component', self.name, 'cget']
      end
      private :__cget_cmd

      def __config_cmd
        [self.master, 'component', self.name, 'configure']
      end
      private :__config_cmd

      ComponentID_TBL = TkCore::INTERP.create_table

      (Itk_Component_ID = ['itk:component'.freeze, TkUtil.untrust('00000')]).instance_eval{
        @mutex = Mutex.new
        def mutex; @mutex; end
        freeze
      }

      TkCore::INTERP.init_ip_env{
        ComponentID_TBL.mutex.synchronize{ ComponentID_TBL.clear }
      }

      def self.id2obj(master, id)
        if master.kind_of?(TkObject)
          master = master.path
        else
          master = master.to_s
        end
        ComponentID_TBL.mutex.synchronize{
          if ComponentID_TBL.key?(master)
            (ComponentID_TBL[master].key?(id))? ComponentID_TBL[master][id]: id
          else
            id
          end
        }
      end

      def self.new(master, component=nil)
        if master.kind_of?(TkObject)
          master = master.path
        else
          master = master.to_s
        end

        if component.kind_of?(Tk::Itk::Component)
          component = component.name
        elsif component
          component = component.to_s
        else
          Itk_Component_ID.mutex.synchronize{
            component = Itk_Component_ID.join(TkCore::INTERP._ip_id_)
            Itk_Component_ID[1].succ!
          }
        end

        ComponentID_TBL.mutex.synchronize{
          if ComponentID_TBL.key?(master)
            if ComponentID_TBL[master].key?(component)
              return ComponentID_TBL[master][component]
            end
          else
            ComponentID_TBL[master] = {}
          end
        }

        super(master, component)
      end

      def initialize(master, component)
        @master = master
        @component = component

        ComponentID_TBL.mutex.synchronize{
          ComponentID_TBL[@master][@component] = self
        }

        begin
          @widget = window(tk_call(@master, 'component', @component))
          @path = @widget.path
        rescue
          @widget = nil
          @path = nil
        end
      end

      def path
        unless @path
          begin
            @widget = window(tk_call(@master, 'component', @component))
            @path = @widget.path
          rescue
            fail RuntimeError, 'component is not assigned to a widget'
          end
        end
        @path
      end

      def epath
        path()
      end

      def to_eval
        path()
      end

      def master
        @master
      end

      def name
        @component
      end

      def widget
        unless @widget
          begin
            @widget = window(tk_call(@master, 'component', @component))
            @path = @widget.path
          rescue
            fail RuntimeError, 'component is not assigned to a widget'
          end
        end
        @widget
      end

      def widget_class
        unless @widget
          begin
            @widget = window(tk_call(@master, 'component', @component))
            @path = @widget.path
            @widget.classname
          rescue
            nil
          end
        end
      end

      def method_missing(id, *args)
        name = id.id2name

        # try 1 : component command
        begin
          return tk_call(@master, 'component', @component, name, *args)
        rescue
        end

        # try 2 : component configure
        len = args.length
        begin
          case len
          when 1
            if name[-1] == ?=
              return configure(name[0..-2], args[0])
            else
              return configure(name, args[0])
            end
          when 0
            return cget(name)
          end
        rescue
        end

        # try 3 : widget method or widget configure
        begin
          unless @widget
            @widget = window(tk_call(@master, 'component', @component))
            @path = @widget.path
          end
          @widget.__send__(id, *args)
        rescue
        end

        # unknown method
        super(id, *args)
        # fail RuntimeError, "unknown method '#{name}' for #{self.inspect}"
      end

      def tk_send(cmd, *rest)
        begin
          tk_call(@master, 'component', @component, cmd, *rest)
        rescue
          unless @path
            begin
              @widget = window(tk_call(@master, 'component', @component))
              @path = @widget.path
            rescue
              fail RuntimeError, 'component is not assigned to a widget'
            end
          end
          tk_call(@path, cmd, *rest)
        end
      end

      def tk_send_without_enc(cmd, *rest)
        begin
          tk_call_without_enc(@master, 'component', @component, cmd, *rest)
        rescue
          unless @path
            begin
              @widget = window(tk_call(@master, 'component', @component))
              @path = @widget.path
            rescue
              fail RuntimeError, 'component is not assigned to a widget'
            end
          end
          tk_call_without_enc(@path, cmd, *rest)
        end
      end

      def tk_send_with_enc(cmd, *rest)
        begin
          tk_call_with_enc(@master, 'component', @component, cmd, *rest)
        rescue
          unless @path
            begin
              @widget = window(tk_call(@master, 'component', @component))
              @path = @widget.path
            rescue
              fail RuntimeError, 'component is not assigned to a widget'
            end
          end
          tk_call_with_enc(@path, cmd, *rest)
        end
      end

      #def bind(*args)
      #  unless @widget
      #    begin
      #      @widget = window(tk_call(@master, 'component', @component))
      #      @path = @widget.path
      #    rescue
      #      fail RuntimeError, 'component is not assigned to a widget'
      #    end
      #  end
      #  @widget.bind(*args)
      #end
      def bind(context, *args)
        unless @widget
          begin
            @widget = window(tk_call(@master, 'component', @component))
            @path = @widget.path
          rescue
            fail RuntimeError, 'component is not assigned to a widget'
          end
        end
        # if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
        if TkComm._callback_entry?(args[0]) || !block_given?
          cmd = args.shift
        else
          cmd = Proc.new
        end
        @widget.bind(context, cmd, *args)
      end

      #def bind_append(*args)
      #  unless @widget
      #    begin
      #      @widget = window(tk_call(@master, 'component', @component))
      #      @path = @widget.path
      #    rescue
      #      fail RuntimeError, 'component is not assigned to a widget'
      #    end
      #  end
      #  @widget.bind_append(*args)
      #end
      def bind_append(context, *args)
        unless @widget
          begin
            @widget = window(tk_call(@master, 'component', @component))
            @path = @widget.path
          rescue
            fail RuntimeError, 'component is not assigned to a widget'
          end
        end
        # if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
        if TkComm._callback_entry?(args[0]) || !block_given?
          cmd = args.shift
        else
          cmd = Proc.new
        end
        @widget.bind_append(context, cmd, *args)
      end

      def bind_remove(*args)
        unless @widget
          begin
            @widget = window(tk_call(@master, 'component', @component))
            @path = @widget.path
          rescue
            fail RuntimeError, 'component is not assigned to a widget'
          end
        end
        @widget.bind_remove(*args)
      end

      def bindinfo(*args)
        unless @widget
          begin
            @widget = window(tk_call(@master, 'component', @component))
            @path = @widget.path
          rescue
            fail RuntimeError, 'component is not assigned to a widget'
          end
        end
        @widget.bindinfo(*args)
      end

    end
  end
end
