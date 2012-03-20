#
#  BLT support
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tk/variable'

# call setup script for general 'tkextlib' libraries
require 'tkextlib/setup.rb'

# call setup script
require 'tkextlib/blt/setup.rb'

# load all image format handlers
#TkPackage.require('BLT', '2.4')
TkPackage.require('BLT')

module Tk
  module BLT
    TkComm::TkExtlibAutoloadModule.unshift(self)
    # Require autoload-symbols which is a same name as widget classname.
    # Those are used at  TkComm._genobj_for_tkwidget method.

    extend TkCore

    VERSION = tk_call('set', 'blt_version')
    PATCH_LEVEL = tk_call('set', 'blt_patchLevel')

    begin
      lib = TkCore::INTERP._invoke('set', 'blt_library')
    rescue
      lib = ''
    end
    LIBRARY  = TkVarAccess.new('blt_library', lib)

    begin
      lib = TkCore::INTERP._invoke('set', 'blt_libPath')
    rescue
      lib = ''
    end
    LIB_PATH = TkVarAccess.new('blt_libPath', lib)

    PACKAGE_NAME = 'BLT'.freeze
    def self.package_name
      PACKAGE_NAME
    end

    def self.package_version
      begin
        TkPackage.require('BLT')
      rescue
        ''
      end
    end

    ####################################################

    def self.beep(percent = 50)
      tk_call('::blt::beep', percent)
    end

    def self.bgexec(*args)
      if args[0].kind_of?(TkVariable)
        var = args.shift
      else
        var = TkVariable.new
      end
      params = [var]

      params.concat(hash_kv(args.shift, true)) if args[0].kind_of?(Hash)

      params << '--' if args[0] =~ /^\s*-[^-]/
      params.concat(args)

      tk_call('::blt::bgexec', *params)
      var
    end

    def self.detach_bgexec(*args)
      if args[0].kind_of?(TkVariable)
        var = args.shift
      else
        var = TkVariable.new
      end
      params = [var]

      params.concat(hash_kv(args.shift, true)) if args[0].kind_of?(Hash)

      params << '--' if args[0] =~ /^\s*-[^-]/
      params.concat(args)
      params << '&'

      [var, tk_split_list(tk_call('::blt::bgexec', *params))]
    end

    def self.bltdebug(lvl = nil)
      if lvl
        tk_call('::blt::bltdebug', lvl)
      else
        number(tk_call('::blt::bltdebug'))
      end
    end

    def self.crc32_file(name)
      tk_call_without_enc('::blt::crc32', name)
    end
    def self.crc32_data(dat)
      tk_call_without_enc('::blt::crc32', '-data', dat)
    end

    ####################################################

    def self.active_legend(graph)
      tk_call_without_enc('Blt_ActiveLegend', graph)
    end
    def self.crosshairs(graph)
      tk_call_without_enc('Blt_Crosshairs', graph)
    end
    def self.zoom_stack(graph)
      tk_call_without_enc('Blt_ZoomStack', graph)
    end
    def self.print_key(graph)
      tk_call_without_enc('Blt_PrintKey', graph)
    end
    def self.closest_point(graph)
      tk_call_without_enc('Blt_ClosestPoint', graph)
    end

    module GraphCommand
      def active_legend
        tk_call_without_enc('Blt_ActiveLegend', @path)
        self
      end
      def crosshairs
        tk_call_without_enc('Blt_Crosshairs', @path)
        self
      end
      def zoom_stack
        tk_call_without_enc('Blt_ZoomStack', @path)
        self
      end
      def print_key
        tk_call_without_enc('Blt_PrintKey', @path)
        self
      end
      def closest_point
        tk_call_without_enc('Blt_ClosestPoint', @path)
        self
      end
    end

    ####################################################

    autoload :PlotComponent,'tkextlib/blt/component.rb'

    autoload :Barchart,     'tkextlib/blt/barchart.rb'
    autoload :Bitmap,       'tkextlib/blt/bitmap.rb'
    autoload :Busy,         'tkextlib/blt/busy.rb'
    autoload :Container,    'tkextlib/blt/container.rb'
    autoload :CutBuffer,    'tkextlib/blt/cutbuffer.rb'
    autoload :DragDrop,     'tkextlib/blt/dragdrop.rb'
    autoload :EPS,          'tkextlib/blt/eps.rb'
    autoload :Htext,        'tkextlib/blt/htext.rb'
    autoload :Graph,        'tkextlib/blt/graph.rb'
    autoload :Spline,       'tkextlib/blt/spline.rb'
    autoload :Stripchart,   'tkextlib/blt/stripchart.rb'
    autoload :Table,        'tkextlib/blt/table.rb'
    autoload :Tabnotebook,  'tkextlib/blt/tabnotebook.rb'
    autoload :Tabset,       'tkextlib/blt/tabset.rb'
    autoload :Ted,          'tkextlib/blt/ted.rb'
    autoload :Tile,         'tkextlib/blt/tile.rb'
    autoload :Tree,         'tkextlib/blt/tree.rb'
    autoload :TreeView,     'tkextlib/blt/treeview.rb'
    autoload :Hiertable,    'tkextlib/blt/treeview.rb'
    # Hierbox is obsolete
    autoload :Vector,       'tkextlib/blt/vector.rb'
    autoload :VectorAccess, 'tkextlib/blt/vector.rb'
    autoload :Watch,        'tkextlib/blt/watch.rb'
    autoload :Winop,        'tkextlib/blt/winop.rb'
    autoload :WinOp,        'tkextlib/blt/winop.rb'

    # Unix only
    autoload :DnD,          'tkextlib/blt/unix_dnd.rb'

    # Windows only
    autoload :Printer,      'tkextlib/blt/win_printer.rb'
  end
end
