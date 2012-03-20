#
#  tkextlib/itk/incr_tcl.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'

# call setup script
require 'tkextlib/itcl.rb'

# TkPackage.require('Itcl', '3.2')
TkPackage.require('Itcl')

module Tk
  module Itcl
    include Tk
    extend Tk

    LIBRARY = TkVarAccess.new('::itcl::library')
    PURIST = TkVarAccess.new('::itcl::purist')

    VERSION    = TkCore::INTERP._invoke("set", "::itcl::version").freeze
    PATCHLEVEL = TkCore::INTERP._invoke("set", "::itcl::patchLevel").freeze

    PACKAGE_NAME = 'Itcl'.freeze
    def self.package_name
      PACKAGE_NAME
    end

    def self.package_version
      begin
        TkPackage.require('Itcl')
      rescue
        ''
      end
    end

    ##############################################

    class ItclObject < TkObject
      ITCL_CLASSNAME = ''.freeze

      (ITCL_OBJ_ID = ['itclobj'.freeze, TkUtil.untrust('00000')]).instance_eval{
        @mutex = Mutex.new
        def mutex; @mutex; end
        freeze
      }
      ITCL_OBJ_TBL = TkUtil.untrust({})

      def initialize(*args)
        if (@klass = self.class::ITCL_CLASSNAME).empty?
          fail RuntimeError, 'unknown itcl class (abstract class?)'
        end
        Tk::Itcl::ItclObject::ITCL_OBJ_ID.mutex.synchronize{
          @id = Tk::Itcl::ItclObject::TCL_OBJ_ID.join(TkCore::INTERP._ip_id_)
          Tk::Itcl::ItclObject::ITCL_OBJ_ID[1].succ!
        }
        @path = @id
      end

      def self.call_proc(name, *args)
        tk_call("#{ITCL_CLASSNAME}::#{cmd}", *args)
      end

      def call_method(name, *args)
        tk_call(@path, name, *args)
      end

      def isa(klass)
        bool(tk_call(@path, 'isa', klass))
      end
      alias itcl_kind_of? isa

      def info_class
        tk_call(@path, 'info', 'class')
      end

      def info_inherit
        simplelist(tk_call(@path, 'info', 'inherit'))
      end

      def info_heritage
        list(tk_call(@path, 'info', 'heritage'))
      end

      def info_function(*args)
        if args[-1].kind_of?(Array)
          params = args.pop
          params.each{|param|
            param = param.to_s
            args << ( (param[0] == ?-)? param: "-#{param}" )
          }
        end
        list(tk_call(@path, 'info', 'function', *args))
      end

      def info_variable(*args)
        if args[-1].kind_of?(Array)
          params = args.pop
          params.each{|param|
            param = param.to_s
            args << ( (param[0] == ?-)? param: "-#{param}" )
          }
        end
        list(tk_call(@path, 'info', 'variable', *args))
      end
    end

    ##############################################

    def self.body(klass, func, args, body)
      tk_call('::itcl::body', "#{klass}::#{func}", args, body)
    end

    def self.code(cmd, *args)
      tk_call('::itcl::code', cmd, *args)
    end

    def self.code_in_namespace(namespace, cmd, *args)
      tk_call('::itcl::code', '-namespace', namespace, cmd, *args)
    end

    def self.configbody(klass, var, body)
      tk_call('::itcl::configbody', "#{klass}::#{var}", body)
    end

    def self.create_itcl_class(name, body)
      TkCore::INTERP._invoke('::itcl::class', name, body)
      klass = Class.new(Tk::Itcl::ItclObject)
      klass.const_set('ITCL_CLASSNAME', name.dup.freeze)
      klass
    end

    def self.delete_itcl_class(*names)
      tk_call('::itcl::delete', 'class',  *names)
    end

    def self.delete_itcl_object(*names)
      tk_call('::itcl::delete', 'object',  *names)
    end

    def self.delete_namespace(*names)
      tk_call('::itcl::delete', 'namespace',  *names)
    end

    def self.ensemble(name, *args)
      tk_call('::itcl::ensemble', name, *args)
    end

    def self.find_classes(pat=None)
      simplelist(tk_call('::itcl::find', 'classes', pat))
    end

    def self.find_objects(*args)
      simplelist(tk_call('::itcl::find', 'objects', *args))
    end

    def self.is_itcl_class(target)
      bool(tk_call('::itcl::is', 'class', target))
    end

    def self.is_itcl_object(target)
      bool(tk_call('::itcl::is', 'object', target))
    end

    def self.create_local_obj(klass, name, *args)
      tk_call('::itcl::local', klass, name, *args)
    end

    def self.is_itcl_instance(klass, target)
      bool(tk_call('::itcl::is', 'object', '-class', klass, target))
    end

    def self.scope(var)
      tk_call('::itcl::scope', var)
    end
  end
end
