#
#  tkextlib/tkDND/shape.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'

# call setup script for general 'tkextlib' libraries
require 'tkextlib/setup.rb'

# call setup script
require 'tkextlib/tkDND/setup.rb'

# TkPackage.require('Shape', '0.3')
TkPackage.require('Shape')

module Tk
  module TkDND
    module Shape
      extend TkCore

      PACKAGE_NAME = 'Shape'.freeze
      def self.package_name
        PACKAGE_NAME
      end

=begin
      def self.package_version
        begin
          TkPackage.require('Shape')
        rescue
          ''
        end
      end
=end
      class << self
        def package_version
          Tk.tk_call('set', 'shape_version')
        end
        alias shape_version package_version

        def package_patchlevel
          Tk.tk_call('set', 'shape_patchLevel')
        end
        alias shape_patchlevel package_patchlevel

        def version
          tk_call('shape', 'version')
        end
        alias xshape_version version
      end

      ############################

      def shape_bounds(kind=nil)
        if kind
          ret = tk_call('shape', 'bounds', @path, "-#{kind}")
        else
          ret = tk_call('shape', 'bounds', @path)
        end
        if ret == ""
          nil
        else
          list(ret)
        end
      end

      def shape_get(kind=nil)
        if kind
          list(tk_call('shape', 'get', @path, "-#{kind}"))
        else
          list(tk_call('shape', 'get', @path))
        end
      end

      def shape_offset(x, y, kind=nil)
        if kind
          tk_call('shape', 'get', @path, "-#{kind}", x, y)
        else
          tk_call('shape', 'get', @path, x, y)
        end
        self
      end

      def _parse_shapespec_param(args)
        cmd = []

        kind_keys    = ['bounding', 'clip', 'both']
        offset_keys  = ['offset']
        srckind_keys = ['bitmap', 'rectangles', 'reset', 'test', 'window']

        cmd << "-#{args.shift}" if kind_keys.member?(args[0].to_s)

        if offset_keys.member?(args[0].to_s)
          cmd << "-#{args.shift}"
          cmd << args.shift # xOffset
          cmd << args.shift # yOffset
        end

        if srckind_keys.member?(args[0].to_s)
          cmd << "-#{args.shift}"
        end

        cmd.concat(args)

        cmd
      end
      private :_parse_shapespec_param

      def shape_set(*args) # ?kind? ?offset <x> <y>? srckind ?arg ...?
        tk_call('shape', 'set', @path, *(_parse_shapespec_param(args)))
        self
      end

      def shape_update(op, *args) # ?kind? ?offset <x> <y>? srckind ?arg ...?
        tk_call('shape', 'update', @path, op, *(_parse_shapespec_param(args)))
        self
      end
    end
  end
end

class TkWindow
  include Tk::TkDND::Shape
end
