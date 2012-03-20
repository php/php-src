#
#  tkextlib/tcllib/tkpiechart.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * Create 2D or 3D pies with labels in Tcl canvases
#

require 'tk'
require 'tk/canvas'
require 'tkextlib/tcllib.rb'

# TkPackage.require('tkpiechart', '6.6')
TkPackage.require('tkpiechart')

module Tk
  module Tcllib
    module Tkpiechart
    end
  end
end

module Tk::Tcllib::Tkpiechart
  PACKAGE_NAME = 'tkpiechart'.freeze
  def self.package_name
    PACKAGE_NAME
  end

  def self.package_version
    begin
      TkPackage.require('tkpiechart')
    rescue
      ''
    end
  end

  module ConfigMethod
    include TkConfigMethod

    def __pathname
      self.path + ';' + self.tag
    end
    private :__pathname

    def __cget_cmd
      ['::switched::cget', self.tag]
    end

    def __config_cmd
      ['::switched::configure', self.tag]
    end
    private :__config_cmd

    def __configinfo_struct
      {:key=>0, :alias=>nil, :db_name=>nil, :db_class=>nil,
        :default_value=>1, :current_value=>2}
    end
    private :__configinfo_struct

    def __boolval_optkeys
      super() << 'select' << 'autoupdate' << 'selectable'
    end
    private :__boolval_optkeys

    def __strval_optkeys
      super() << 'bordercolor' << 'textbackground' <<
        'widestvaluetext' << 'title'
    end
    private :__strval_optkeys

    def __listval_optkeys
      super() << 'colors'
    end
    private :__listval_optkeys
  end

  ####################################
  class PieChartObj < TkcItem
    include ConfigMethod

    def __font_optkeys
      ['titlefont']
    end
    private :__font_optkeys
  end

  ####################################
  class Pie < TkcItem
    include ConfigMethod

    def create_self(x, y, width, height, keys=None)
      if keys and keys != None
        @tag_key = tk_call_without_enc('::stooop::new', 'pie',
                                       @c, x, y, *hash_kv(keys, true))
      else
        @tag_key = tk_call_without_enc('::stooop::new', 'pie', @c, x, y)
      end

      @slice_tbl = {}

      id = "pie(#{@tag_key})"

      @tag = @tag_pie = TkcNamedTag(@c, id)
      @tag_slices = TkcNamedTag(@c, "pieSlices(#{@tag_key})")

      id
    end
    private :create_self

    def tag_key
      @tag_key
    end
    def tag
      @tag
    end
    def canvas
      @c
    end
    def _entry_slice(slice)
      @slice_tbl[slice.to_eval] = slice
    end
    def _delete_slice(slice)
      @slice_tbl.delete(slice.to_eval)
    end

    def delete
      tk_call_without_enc('::stooop::delete', @tag_key)
      CItemID_TBL.mutex.synchronize{
        CItemID_TBL[@path].delete(@id) if CItemID_TBL[@path]
      }
      self
    end

    def new_slice(text=None)
      Slice.new(self, text)
    end

    def delete_slice(slice)
      unless slice.kind_of?(Slice)
        unless (slice = @slice_tbl[slice])
          return tk_call_without_enc('pie::deleteSlice', @tag_key, slice)
        end
      end
      unless slice.kind_of?(Slice) && slice.pie == self
        fail ArgumentError, "argument is not a slice of self"
      end
      slice.delete
    end

    def selected_slices
      tk_split_simplelist(tk_call_without_enc('pie::selectedSlices',
                                              @tag_key)).collect{|slice|
        @slice_tbl[slice] || Slice.new(:no_create, self, slice)
      }
    end
  end

  ####################################
  class Slice < TkcItem
    include ConfigMethod

    def __config_cmd
      ['::switched::configure', self.tag]
    end
    private :__config_cmd

    #------------------------

    def initialize(pie, *args)
      unless pie.kind_of?(Pie) && pie != :no_create
        fail ArgumentError, "expects TkPiechart::Pie for 1st argument"
      end

      if pie == :no_create
        @pie, @tag_key = args
      else
        text = args[0] || None
        @pie = pie
        @tag_key = tk_call_without_enc('pie::newSlice', @pie.tag_key, text)
      end
      @parent = @c = @pie.canvas
      @path = @parent.path

      @pie._entry_slice(self)

      @id = "slices(#{@tag_key})"
      @tag = TkcNamedTag.new(@pie.canvas, @id)

      CItemID_TBL.mutex.synchronize{
        CItemID_TBL[@path] = {} unless CItemID_TBL[@path]
        CItemID_TBL[@path][@id] = self
      }
    end

    def tag_key
      @tag_key
    end
    def tag
      @tag
    end
    def pie
      @pie
    end

    def delete
      tk_call_without_enc('pie::deleteSlice', @pie.tag_key, @tag_key)
      CItemID_TBL.mutex.synchronize{
        CItemID_TBL[@path].delete(@id) if CItemID_TBL[@path]
      }
      @pie._delete_slice(self)
      self
    end

    def size(share, disp=None)
      tk_call_without_enc('pie::sizeSlice',
                          @pie.tag_key, @tag_key, share, disp)
      self
    end

    def label(text)
      tk_call_without_enc('pie::labelSlice', @pie.tag_key, @tag_key, text)
      self
    end
  end

  ####################################
  class BoxLabeler < TkcItem
    include ConfigMethod

    def __config_cmd
      ['::switched::configure', self.tag]
    end
    private :__config_cmd

    #------------------------

    def create_self(keys=None)
      if keys and keys != None
        @tag_key = tk_call_without_enc('::stooop::new', 'pieBoxLabeler',
                                       *hash_kv(keys, true))
      else
        @tag_key = tk_call_without_enc('::stooop::new', 'pieBoxLabeler')
      end

      id = "pieBoxLabeler(#{@tag_key})"
      @tag = TkcNamedTag(@c, id)

      id
    end
    private :create_self
  end

  ####################################
  class PeripheralLabeler < TkcItem
    include ConfigMethod

    def __font_optkeys
      ['font', 'smallfont']
    end
    private :__font_optkeys

    def __config_cmd
      ['::switched::configure', self.tag]
    end
    private :__config_cmd

    #------------------------

    def create_self(keys=None)
      if keys and keys != None
        @tag_key = tk_call_without_enc('::stooop::new',
                                       'piePeripheralLabeler',
                                       *hash_kv(keys, true))
      else
        @tag_key = tk_call_without_enc('::stooop::new', 'piePeripheralLabeler')
      end

      id = "piePeripheralLabeler(#{@tag_key})"
      @tag = TkcNamedTag(@c, id)

      id
    end
    private :create_self
  end

  ####################################
  class Label < TkcItem
    include ConfigMethod

    def __config_cmd
      ['::switched::configure', self.tag]
    end
    private :__config_cmd

    #------------------------

    def create_self(x, y, keys=None)
      if keys and keys != None
        @tag_key = tk_call_without_enc('::stooop::new', 'canvasLabel',
                                       @c, x, y, width, height,
                                       *hash_kv(keys, true))
      else
        @tag_key = tk_call_without_enc('::stooop::new', 'canvasLabel',
                                       @c, x, y, width, height)
      end

      id = "canvasLabel(#{@tag_key})"
      @tag = TkcNamedTag(@c, id)

      id
    end
    private :create_self
  end
end
