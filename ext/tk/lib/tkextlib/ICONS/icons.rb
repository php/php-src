#
#  tkextlib/ICONS/icons.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'

# call setup script for general 'tkextlib' libraries
require 'tkextlib/setup.rb'

# call setup script
require 'tkextlib/ICONS/setup.rb'

# TkPackage.require('icons', '1.0')
TkPackage.require('icons')

module Tk
  class ICONS < TkImage
    extend Tk

    PACKAGE_NAME = 'icons'.freeze
    def self.package_name
      PACKAGE_NAME
    end

    def self.package_version
      begin
        TkPackage.require('icons')
      rescue
        ''
      end
    end

    def self.create(*args)  # icon, icon, ..., ?option=>value, ...?
      if args[-1].kind_of?(Hash)
        keys = args.pop
        icons = simplelist(tk_call('::icons::icons', 'create',
                                   *(hash_kv(keys) << (args.flatten))))
      else
        icons = simplelist(tk_call('::icons::icons', 'create',
                                   args.flatten))
      end

      icons.collect{|icon| self.new(icon, :without_creating=>true)}
    end

    def self.delete(*icons)  # icon, icon, ...
      icons = icons.flatten
      return if icons.empty?
      icons.map!{|icon|
        if icon.kind_of?(Tk::ICONS)
          Tk_IMGTBL.delete(icon.path)
          icon.name
        elsif icon.to_s =~ /^::icon::(.*)/
          name = $1
          Tk_IMGTBL.delete(icon)
          name
        else
          Tk_IMGTBL.delete("::icon::#{icon}")
          icon
        end
      }
      tk_call('::icons::icons', 'delete', icons)
    end

    def self.query(*args)  # icon, icon, ..., ?option=>value, ...?
      if args[-1].kind_of?(Hash)
        keys = args.pop
        simplelist(tk_call('::icons::icons', 'query',
                           *(hash_kv(keys) << (args.flatten))))
      else
        simplelist(tk_call('::icons::icons', 'query', args.flatten))
      end . map{|inf| list(inf) }
    end

    ##########################################

    class << self
      alias _new new

      def new(name, keys=nil)
        if obj = Tk_IMGTBL["::icon::#{name}"]
          if keys
            keys = _symbolkey2str(keys)
            unless keys.delete('without_creating')
              tk_call('::icons::icons', 'create', *(hash_kv(keys) << obj.name))
            end
          end
        else
          obj = _new(name, keys)
        end
        obj
      end
    end

    ##########################################

    def initialize(name, keys=nil)
      if name.kind_of?(String) && name =~ /^::icon::(.+)$/
          @name = $1
          @path = name
      else
        @name = name.to_s
        @path = "::icon::#{@name}"
      end
      keys = _symbolkey2str(keys)
      unless keys.delete('without_creating')
        tk_call('::icons::icons', 'create', *(hash_kv(keys) << @name))
      end
      Tk_IMGTBL[@path] = self
    end

    def name
      @name
    end

    def delete
      Tk_IMGTBL.delete(@path)
      tk_call('::icons::icons', 'delete', @name)
      self
    end

    def query(keys={})
      list(simplelist(tk_call('::icons::icons', 'query',
                               *(hash_kv(keys) << @name))
                      )[0])
    end
  end
end
