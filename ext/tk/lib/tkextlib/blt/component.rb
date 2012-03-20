#
#  tkextlib/blt/component.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/blt.rb'

module Tk::BLT
  module PlotComponent
    include TkItemConfigMethod

    module OptKeys
      def __item_font_optkeys(id)
        ['font', 'tickfont', 'titlefont']
      end
      private :__item_font_optkeys

      def __item_numstrval_optkeys(id)
        ['xoffset', 'yoffset']
      end
      private :__item_numstrval_optkeys

      def __item_boolval_optkeys(id)
        ['hide', 'under', 'descending', 'logscale', 'loose', 'showticks',
          'titlealternate', 'scalesymbols', 'minor', 'raised',
          'center', 'decoration', 'landscape', 'maxpect']
      end
      private :__item_boolval_optkeys

      def __item_strval_optkeys(id)
        ['text', 'label', 'limits', 'title',
          'show', 'file', 'maskdata', 'maskfile',
          'color', 'titlecolor', 'fill', 'outline', 'offdash']
      end
      private :__item_strval_optkeys

      def __item_listval_optkeys(id)
        ['bindtags']
      end
      private :__item_listval_optkeys

      def __item_numlistval_optkeys(id)
        ['dashes', 'majorticks', 'minorticks']
      end
      private :__item_numlistval_optkeys

      def __item_tkvariable_optkeys(id)
        ['variable', 'textvariable', 'colormap', 'fontmap']
      end
      private :__item_tkvariable_optkeys
    end

    include OptKeys

    def __item_cget_cmd(id)
      if id.kind_of?(Array)
        # id := [ type, name ]
        [self.path, id[0], 'cget', id[1]]
      else
        [self.path, id, 'cget']
      end
    end
    private :__item_cget_cmd

    def __item_config_cmd(id)
      if id.kind_of?(Array)
        # id := [ type, name, ... ]
        type, *names = id
        [self.path, type, 'configure'].concat(names)
      else
        [self.path, id, 'configure']
      end
    end
    private :__item_config_cmd

    def __item_pathname(id)
      if id.kind_of?(Array)
        id = tagid(id[1])
      end
      [self.path, id].join(';')
    end
    private :__item_pathname

    def axis_cget_tkstring(id, option)
      ret = itemcget_tkstring(['axis', tagid(id)], option)
    end
    def axis_cget(id, option)
      ret = itemcget(['axis', tagid(id)], option)
    end
    def axis_cget_strict(id, option)
      ret = itemcget_strict(['axis', tagid(id)], option)
    end
    def axis_configure(*args)
      slot = args.pop
      if slot.kind_of?(Hash)
        value = None
        slot = _symbolkey2str(slot)
        if cmd = slot.delete('command')
          slot['command'] = proc{|w, tick|
            cmd.call(TkComm.window(w), TkComm.num_or_str(tick))
          }
        end
      else
        value = slot
        slot = args.pop
        if slot == :command || slot == 'command'
          cmd = value
          value = proc{|w, tick|
            cmd.call(TkComm.window(w), TkComm.num_or_str(tick))
          }
        end
      end
      id_list = args.flatten.collect!{|id| tagid(id)}.unshift('axis')
      itemconfigure(id_list, slot, value)
    end
    def axis_configinfo(id, slot=nil)
      itemconfiginfo(['axis', tagid(id)], slot)
    end
    def current_axis_configinfo(id, slot=nil)
      current_itemconfiginfo(['axis', tagid(id)], slot)
    end

    def crosshairs_cget_tkstring(option)
      itemcget_tkstring('crosshairs', option)
    end
    def crosshairs_cget(option)
      itemcget('crosshairs', option)
    end
    def crosshairs_cget_strict(option)
      itemcget_strict('crosshairs', option)
    end
    def crosshairs_configure(slot, value=None)
      itemconfigure('crosshairs', slot, value)
    end
    def crosshairs_configinfo(slot=nil)
      itemconfiginfo('crosshairs', slot)
    end
    def current_crosshairs_configinfo(slot=nil)
      current_itemconfiginfo('crosshairs', slot)
    end

    def element_cget_tkstring(id, option)
      itemcget_tkstring(['element', tagid(id)], option)
    end
    def element_cget(id, option)
      itemcget(['element', tagid(id)], option)
    end
    def element_cget_strict(id, option)
      itemcget_strict(['element', tagid(id)], option)
    end
    def element_configure(*args)
      slot = args.pop
      if slot.kind_of?(Hash)
        value = None
      else
        value = slot
        slot = args.pop
      end
      id_list = args.flatten.collect!{|id| tagid(id)}.unshift('element')
      itemconfigure(id_list, slot, value)
    end
    def element_configinfo(id, slot=nil)
      itemconfiginfo(['element', tagid(id)], slot)
    end
    def current_element_configinfo(id, slot=nil)
      current_itemconfiginfo(['element', tagid(id)], slot)
    end

    def bar_cget_tkstring(id, option)
      itemcget_tkstring(['bar', tagid(id)], option)
    end
    def bar_cget(id, option)
      itemcget(['bar', tagid(id)], option)
    end
    def bar_cget_strict(id, option)
      itemcget_strict(['bar', tagid(id)], option)
    end
    def bar_configure(*args)
      slot = args.pop
      if slot.kind_of?(Hash)
        value = None
      else
        value = slot
        slot = args.pop
      end
      id_list = args.flatten.collect!{|id| tagid(id)}.unshift('bar')
      itemconfigure(id_list, slot, value)
    end
    def bar_configinfo(id, slot=nil)
      itemconfiginfo(['bar', tagid(id)], slot)
    end
    def current_bar_configinfo(id, slot=nil)
      current_itemconfiginfo(['bar', tagid(id)], slot)
    end

    def line_cget_tkstring(id, option)
      itemcget_tkstring(['line', tagid(id)], option)
    end
    def line_cget(id, option)
      itemcget(['line', tagid(id)], option)
    end
    def line_cget_strict(id, option)
      itemcget_strict(['line', tagid(id)], option)
    end
    def line_configure(*args)
      slot = args.pop
      if slot.kind_of?(Hash)
        value = None
      else
        value = slot
        slot = args.pop
      end
      id_list = args.flatten.collect!{|id| tagid(id)}.unshift('line')
      itemconfigure(id_list, slot, value)
    end
    def line_configinfo(id, slot=nil)
      itemconfiginfo(['line', tagid(id)], slot)
    end
    def current_line_configinfo(id, slot=nil)
      current_itemconfiginfo(['line', tagid(id)], slot)
    end

    def gridline_cget_tkstring(option)
      itemcget_tkstring('grid', option)
    end
    def gridline_cget(option)
      itemcget('grid', option)
    end
    def gridline_cget_strict(option)
      itemcget_strict('grid', option)
    end
    def gridline_configure(slot, value=None)
      itemconfigure('grid', slot, value)
    end
    def gridline_configinfo(slot=nil)
      itemconfiginfo('grid', slot)
    end
    def current_gridline_configinfo(slot=nil)
      current_itemconfiginfo('grid', slot)
    end

    def legend_cget_tkstring(option)
      itemcget_tkstring('legend', option)
    end
    def legend_cget(option)
      itemcget('legend', option)
    end
    def legend_cget_strict(option)
      itemcget_strict('legend', option)
    end
    def legend_configure(slot, value=None)
      itemconfigure('legend', slot, value)
    end
    def legend_configinfo(slot=nil)
      itemconfiginfo('legend', slot)
    end
    def current_legend_configinfo(slot=nil)
      current_itemconfiginfo('legend', slot)
    end

    def pen_cget_tkstring(id, option)
      itemcget_tkstring(['pen', tagid(id)], option)
    end
    def pen_cget(id, option)
      itemcget(['pen', tagid(id)], option)
    end
    def pen_cget_strict(id, option)
      itemcget_strict(['pen', tagid(id)], option)
    end
    def pen_configure(*args)
      slot = args.pop
      if slot.kind_of?(Hash)
        value = None
      else
        value = slot
        slot = args.pop
      end
      id_list = args.flatten.collect!{|id| tagid(id)}.unshift('pen')
      itemconfigure(id_list, slot, value)
    end
    def pen_configinfo(id, slot=nil)
      itemconfiginfo(['pen', tagid(id)], slot)
    end
    def current_pen_configinfo(id, slot=nil)
      current_itemconfiginfo(['pen', tagid(id)], slot)
    end

    def postscript_cget_tkstring(option)
      itemcget_tkstring('postscript', option)
    end
    def postscript_cget(option)
      itemcget('postscript', option)
    end
    def postscript_cget_strict(option)
      itemcget_strict('postscript', option)
    end
    def postscript_configure(slot, value=None)
      itemconfigure('postscript', slot, value)
    end
    def postscript_configinfo(slot=nil)
      itemconfiginfo('postscript', slot)
    end
    def current_postscript_configinfo(slot=nil)
      current_itemconfiginfo('postscript', slot)
    end

    def marker_cget_tkstring(id, option)
      itemcget_tkstring(['marker', tagid(id)], option)
    end
    def marker_cget(id, option)
      itemcget(['marker', tagid(id)], option)
    end
    def marker_cget_strict(id, option)
      itemcget_strict(['marker', tagid(id)], option)
    end
    def marker_configure(*args)
      slot = args.pop
      if slot.kind_of?(Hash)
        value = None
      else
        value = slot
        slot = args.pop
      end
      id_list = args.flatten.collect!{|id| tagid(id)}.unshift('marker')
      itemconfigure(id_list, slot, value)
    end
    def marker_configinfo(id, slot=nil)
      itemconfiginfo(['marker', tagid(id)], slot)
    end
    def current_marker_configinfo(id, slot=nil)
      current_itemconfiginfo(['marker', tagid(id)], slot)
    end

    alias __itemcget_tkstring itemcget_tkstring
    alias __itemcget itemcget
    alias __itemcget_strict itemcget_strict
    alias __itemconfiginfo itemconfiginfo
    alias __current_itemconfiginfo current_itemconfiginfo
    private :__itemcget_tkstring, :__itemcget, :__itemconfiginfo, :__current_itemconfiginfo

    def itemcget_tkstring(tagOrId, option)
      __itemcget_tkstring(tagid(tagOrId), option)
    end
    def itemcget_strict(tagOrId, option)
      ret = __itemcget(tagid(tagOrId), option)
      if option == 'bindtags' || option == :bindtags
        ret.collect{|tag| TkBindTag.id2obj(tag)}
      else
        ret
      end
    end
    def itemcget(tagOrId, option)
      unless TkItemConfigMethod.__IGNORE_UNKNOWN_CONFIGURE_OPTION__
        itemcget_strict(tagOrId, option)
      else
        begin
          itemcget_strict(tagOrId, option)
        rescue => e
          begin
            if current_itemconfiginfo(tagOrId).has_key?(option.to_s)
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
    def itemconfiginfo(tagOrId, slot = nil)
      ret = __itemconfiginfo(tagid(tagOrId), slot)

      if TkComm::GET_CONFIGINFO_AS_ARRAY
        if slot
          if slot == 'bindtags' || slot == :bindtags
            ret[-2] = ret[-2].collect{|tag| TkBindTag.id2obj(tag)}
            ret[-1] = ret[-1].collect{|tag| TkBindTag.id2obj(tag)}
          end
        else
          if (inf = ret.assoc('bindtags'))
            inf[-2] = inf[-2].collect{|tag| TkBindTag.id2obj(tag)}
            inf[-1] = inf[-1].collect{|tag| TkBindTag.id2obj(tag)}
          end
        end

      else # ! TkComm::GET_CONFIGINFO_AS_ARRAY
        if (inf = ret['bindtags'])
          inf[-2] = inf[-2].collect{|tag| TkBindTag.id2obj(tag)}
          inf[-1] = inf[-1].collect{|tag| TkBindTag.id2obj(tag)}
          ret['bindtags'] = inf
        end
      end

      ret
    end
    def current_itemconfiginfo(tagOrId, slot = nil)
      ret = __current_itemconfiginfo(tagid(tagOrId), slot)

      if (val = ret['bindtags'])
        ret['bindtags'] = val.collect{|tag| TkBindTag.id2obj(tag)}
      end

      ret
    end

    private :itemcget_tkstring, :itemcget, :itemcget_strict
    private :itemconfigure, :itemconfiginfo, :current_itemconfiginfo

    #################

    class Axis < TkObject
      (OBJ_ID = ['blt_chart_axis'.freeze, TkUtil.untrust('00000')]).instance_eval{
        @mutex = Mutex.new
        def mutex; @mutex; end
        freeze
      }

      AxisID_TBL = TkCore::INTERP.create_table

      TkCore::INTERP.init_ip_env{
        AxisID_TBL.mutex.synchronize{ AxisID_TBL.clear }
      }

      def self.id2obj(chart, id)
        cpath = chart.path
        AxisID_TBL.mutex.synchronize{
          return id unless AxisID_TBL[cpath]
          AxisID_TBL[cpath][id]? AxisID_TBL[cpath][id]: id
        }
      end

      def self.new(chart, axis=nil, keys={})
        if axis.kind_of?(Hash)
          keys = axis
          axis = nil
        end
        if keys
          keys = _symbolkey2str(keys)
          not_create = keys.delete('without_creating')
        else
          not_create = false
        end

        obj = nil
        AxisID_TBL.mutex.synchronize{
          chart_path = chart.path
          AxisID_TBL[chart_path] ||= {}
          if axis && AxisID_TBL[chart_path][axis]
            obj = AxisID_TBL[chart_path][axis]
          else
            (obj = self.allocate).instance_eval{
              if axis
                @axis = @id = axis.to_s
              else
                OBJ_ID.mutex.synchronize{
                  @axis = @id = OBJ_ID.join(TkCore::INTERP._ip_id_).freeze
                  OBJ_ID[1].succ!
                }
              end
              @path = @id
              @parent = @chart = chart
              @cpath = @chart.path
              Axis::AxisID_TBL[@cpath][@axis] = self
              unless not_create
                tk_call(@chart, 'axis', 'create', @axis, keys)
                return obj
              end
            }
          end
        }

        obj.configure(keys) if obj && ! keys.empty?
        obj
      end

      def initialize(chart, axis=nil, keys={})
        # dummy:: not called by 'new' method

        if axis.kind_of?(Hash)
          keys = axis
          axis = nil
        end
        if axis
          @axis = @id = axis.to_s
        else
          OBJ_ID.mutex.synchronize{
            @axis = @id = OBJ_ID.join(TkCore::INTERP._ip_id_).freeze
            OBJ_ID[1].succ!
          }
        end
        @path = @id
        @parent = @chart = chart
        @cpath = @chart.path
        # Axis::AxisID_TBL[@cpath][@axis] = self
        keys = _symbolkey2str(keys)
        unless keys.delete('without_creating')
          # @chart.axis_create(@axis, keys)
          tk_call(@chart, 'axis', 'create', @axis, keys)
        end
      end

      def id
        @id
      end

      def to_eval
        @id
      end

      def cget_tkstring(option)
        @chart.axis_cget_tkstring(@id, option)
      end
      def cget(option)
        @chart.axis_cget(@id, option)
      end
      def cget_strict(option)
        @chart.axis_cget_strict(@id, option)
      end
      def configure(key, value=None)
        @chart.axis_configure(@id, key, value)
        self
      end
      def configinfo(key=nil)
        @chart.axis_configinfo(@id, key)
      end
      def current_configinfo(key=nil)
        @chart.current_axis_configinfo(@id, key)
      end

      def command(cmd=nil, &b)
        if cmd
          configure('command', cmd)
        elsif b
          configure('command', Proc.new(&b))
        else
          cget('command')
        end
      end

      def delete
        @chart.axis_delete(@id)
        self
      end

      def invtransform(val)
        @chart.axis_invtransform(@id, val)
      end

      def limits
        @chart.axis_limits(@id)
      end

      def name
        @axis
      end

      def transform(val)
        @chart.axis_transform(@id, val)
      end

      def view
        @chart.axis_view(@id)
        self
      end

      def use(name=None) # if @id == xaxis | x2axis | yaxis | y2axis
        @chart.axis_use(@id, name)
      end

      def use_as(axis) # axis := xaxis | x2axis | yaxis | y2axis
        @chart.axis_use(axis, @id)
      end
    end

    #################

    class Crosshairs < TkObject
      CrosshairsID_TBL = TkCore::INTERP.create_table

      TkCore::INTERP.init_ip_env{
        CrosshairsID_TBL.mutex.synchronize{ CrosshairsID_TBL.clear }
      }

      def self.new(chart, keys={})
        obj = nil
        CrosshairsID_TBL.mutex.synchronize{
          unless (obj = CrosshairsID_TBL[chart.path])
            (obj = self.allocate).instance_eval{
              @parent = @chart = chart
              @cpath = @chart.path
              @path = @id = 'crosshairs'
              Crosshairs::CrosshairsID_TBL[@cpath] = self
            }
          end
        }
        chart.crosshair_configure(keys) if obj && ! keys.empty?
        obj
      end

      def initialize(chart, keys={})
        # dummy:: not called by 'new' method

        @parent = @chart = chart
        @cpath = @chart.path
        # Crosshairs::CrosshairsID_TBL[@cpath] = self
        @chart.crosshair_configure(keys) unless keys.empty?
        @path = @id = 'crosshairs'
      end

      def id
        @id
      end

      def to_eval
        @id
      end

      def cget_tkstring(option)
        @chart.crosshair_cget_tkstring(option)
      end
      def cget(option)
        @chart.crosshair_cget(option)
      end
      def cget_strict(option)
        @chart.crosshair_cget_strict(option)
      end
      def configure(key, value=None)
        @chart.crosshair_configure(key, value)
        self
      end
      def configinfo(key=nil)
        @chart.crosshair_configinfo(key)
      end
      def current_configinfo(key=nil)
        @chart.current_crosshair_configinfo(key)
      end

      def off
        @chart.crosshair_off
        self
      end
      def on
        @chart.crosshair_on
        self
      end
      def toggle
        @chart.crosshair_toggle
        self
      end
    end

    #################

    class Element < TkObject
      extend Tk
      extend TkItemFontOptkeys
      extend TkItemConfigOptkeys

      extend Tk::BLT::PlotComponent::OptKeys

      ElementTypeName = 'element'
      ElementTypeToClass = { ElementTypeName=>self }

      ElementID_TBL = TkCore::INTERP.create_table

      TkCore::INTERP.init_ip_env{
        ElementID_TBL.mutex.synchronize{ ElementID_TBL.clear }
      }

      (OBJ_ID = ['blt_chart_element'.freeze, TkUtil.untrust('00000')]).instance_eval{
        @mutex = Mutex.new
        def mutex; @mutex; end
        freeze
      }

      def Element.type2class(type)
        ElementTypeToClass[type]
      end

      def Element.id2obj(chart, id)
        cpath = chart.path
        ElementID_TBL.mutex.synchronize{
          return id unless ElementID_TBL[cpath]
          ElementID_TBL[cpath][id]? ElementID_TBL[cpath][id]: id
        }
      end

      def self.new(chart, element=nil, keys={})
        if element.kind_of?(Hash)
          keys = element
          element = nil
        end
        if keys
          keys = _symbolkey2str(keys)
          not_create = keys.delete('without_creating')
        else
          not_create = false
        end

        obj = nil
        ElementID_TBL.mutex.synchronize{
          chart_path = chart.path
          ElementID_TBL[chart_path] ||= {}
          if element && ElementID_TBL[chart_path][element]
            obj = ElementID_TBL[chart_path][element]
          else
            (obj = self.allocate).instance_eval{
              if element
                @element = @id = element.to_s
              else
                OBJ_ID.mutex.synchronize{
                  @element = @id = OBJ_ID.join(TkCore::INTERP._ip_id_).freeze
                  OBJ_ID[1].succ!
                }
              end
              @path = @id
              @parent = @chart = chart
              @cpath = @chart.path
              @typename = self.class::ElementTypeName
              Element::ElementID_TBL[@cpath][@element] = self
              unless not_create
                tk_call(@chart, @typename, 'create', @element, keys)
                return obj
              end
            }
          end
        }

        obj.configure(keys) if obj && ! keys.empty?
        obj
      end

      def initialize(chart, element=nil, keys={})
        # dummy:: not called by 'new' method

        if element.kind_of?(Hash)
          keys = element
          element = nil
        end
        if element
          @element = @id = element.to_s
        else
          OBJ_ID.mutex.synchronize{
            @element = @id = OBJ_ID.join(TkCore::INTERP._ip_id_).freeze
            OBJ_ID[1].succ!
          }
        end
        @path = @id
        @parent = @chart = chart
        @cpath = @chart.path
        @typename = self.class::ElementTypeName
        # Element::ElementID_TBL[@cpath][@element] = self
        keys = _symbolkey2str(keys)
        unless keys.delete('without_creating')
          # @chart.element_create(@element, keys)
          tk_call(@chart, @typename, 'create', @element, keys)
        end
      end

      def id
        @id
      end

      def to_eval
        @id
      end

      def cget_tkstring(option)
        # @chart.element_cget(@id, option)
        @chart.__send__(@typename + '_cget_tkstring', @id, option)
      end
      def cget(option)
        # @chart.element_cget(@id, option)
        @chart.__send__(@typename + '_cget', @id, option)
      end
      def cget_strict(option)
        @chart.__send__(@typename + '_cget_strict', @id, option)
      end
      def configure(key, value=None)
        # @chart.element_configure(@id, key, value)
        @chart.__send__(@typename + '_configure', @id, key, value)
        self
      end
      def configinfo(key=nil)
        # @chart.element_configinfo(@id, key)
        @chart.__send__(@typename + '_configinfo', @id, key)
      end
      def current_configinfo(key=nil)
        # @chart.current_element_configinfo(@id, key)
        @chart.__send__('current_' << @typename << '_configinfo', @id, key)
      end

      def activate(*args)
        @chart.element_activate(@id, *args)
      end

      def closest(x, y, var, keys={})
        # @chart.element_closest(x, y, var, @id, keys)
        @chart.__send__(@typename + '_closest', x, y, var, @id, keys)
      end

      def deactivate
        @chart.element_deactivate(@id)
        self
      end

      def delete
        @chart.element_delete(@id)
        self
      end

      def exist?
        @chart.element_exist?(@id)
      end

      def name
        @element
      end

      def type
        @chart.element_type(@id)
      end
    end

    class Bar < Element
      ElementTypeName = 'bar'.freeze
      ElementTypeToClass[ElementTypeName] = self
    end
    class Line < Element
      ElementTypeName = 'line'.freeze
      ElementTypeToClass[ElementTypeName] = self
    end

    #################

    class GridLine < TkObject
      GridLineID_TBL = TkCore::INTERP.create_table
      TkCore::INTERP.init_ip_env{
        GridLineID_TBL.mutex.synchronize{ GridLineID_TBL.clear }
      }

      def self.new(chart, keys={})
        obj = nil
        GridLineID_TBL.mutex.synchronize{
          unless (obj = GridLineID_TBL[chart.path])
            (obj = self.allocate).instance_eval{
              @parent = @chart = chart
              @cpath = @chart.path
              @path = @id = 'grid'
              GridLine::GridLineID_TBL[@cpath] = self
            }
          end
        }
        chart.gridline_configure(keys) if obj && ! keys.empty?
        obj
      end

      def initialize(chart, keys={})
        # dummy:: not called by 'new' method

        @parent = @chart = chart
        @cpath = @chart.path
        # GridLine::GridLineID_TBL[@cpath] = self
        @chart.gridline_configure(keys) unless keys.empty?
        @path = @id = 'grid'
      end

      def id
        @id
      end

      def to_eval
        @id
      end

      def cget_tkstring(option)
        @chart.gridline_cget_tkstring(option)
      end
      def cget(option)
        @chart.gridline_cget(option)
      end
      def cget_strict(option)
        @chart.gridline_cget_strict(option)
      end
      def configure(key, value=None)
        @chart.gridline_configure(key, value)
        self
      end
      def configinfo(key=nil)
        @chart.gridline_configinfo(key)
      end
      def current_configinfo(key=nil)
        @chart.current_gridline_configinfo(key)
      end

      def off
        @chart.gridline_off
        self
      end
      def on
        @chart.gridline_on
        self
      end
      def toggle
        @chart.gridline_toggle
        self
      end
    end

    #################

    class Legend < TkObject
      LegendID_TBL = TkCore::INTERP.create_table

      TkCore::INTERP.init_ip_env{
        LegendID_TBL.mutex.synchronize{ LegendID_TBL.clear }
      }

      def self.new(chart, keys={})
        obj = nil
        LegenedID_TBL.mutex.synchronize{
          unless (obj = LegenedID_TBL[chart.path])
            (obj = self.allocate).instance_eval{
              @parent = @chart = chart
              @cpath = @chart.path
              @path = @id = 'crosshairs'
              Legend::LegenedID_TBL[@cpath] = self
            }
          end
        }
        chart.legend_configure(keys) if obj && ! keys.empty?
        obj
      end

      def initialize(chart, keys={})
        # dummy:: not called by 'new' method

        @parent = @chart = chart
        @cpath = @chart.path
        # Legend::LegendID_TBL[@cpath] = self
        @chart.legend_configure(keys) unless keys.empty?
        @path = @id = 'legend'
      end

      def id
        @id
      end

      def to_eval
        @id
      end

      def cget_tkstring(option)
        @chart.legend_cget_tkstring(option)
      end
      def cget(option)
        @chart.legend_cget(option)
      end
      def cget_strict(option)
        @chart.legend_cget_strict(option)
      end
      def configure(key, value=None)
        @chart.legend_configure(key, value)
        self
      end
      def configinfo(key=nil)
        @chart.legend_configinfo(key)
      end
      def current_configinfo(key=nil)
        @chart.current_legend_configinfo(key)
      end

      def activate(*args)
        @chart.legend_activate(*args)
      end

      def deactivate(*args)
        @chart.legend_deactivate(*args)
      end

      def get(pos, y=nil)
        @chart.legend_get(pos, y)
      end
    end

    #################

    class Pen < TkObject
      (OBJ_ID = ['blt_chart_pen'.freeze, TkUtil.untrust('00000')]).instance_eval{
        @mutex = Mutex.new
        def mutex; @mutex; end
        freeze
      }

      PenID_TBL = TkCore::INTERP.create_table

      TkCore::INTERP.init_ip_env{
        PenID_TBL.mutex.synchronize{ PenID_TBL.clear }
      }

      def self.id2obj(chart, id)
        cpath = chart.path
        PenID_TBL.mutex.synchronize{
          return id unless PenID_TBL[cpath]
          PenID_TBL[cpath][id]? PenID_TBL[cpath][id]: id
        }
      end

      def self.new(chart, pen=nil, keys={})
        if pen.kind_of?(Hash)
          keys = pen
          pen = nil
        end
        if keys
          keys = _symbolkey2str(keys)
          not_create = keys.delete('without_creating')
        else
          not_create = false
        end

        obj = nil
        PenID_TBL.mutex.synchronize{
          chart_path = chart.path
          PenID_TBL[chart_path] ||= {}
          if pen && PenID_TBL[chart_path][pen]
            obj = PenID_TBL[chart_path][pen]
          else
            (obj = self.allocate).instance_eval{
              if pen
                @pen = @id = pen.to_s
              else
                OBJ_ID.mutex.synchronize{
                  @pen = @id = OBJ_ID.join(TkCore::INTERP._ip_id_).freeze
                  OBJ_ID[1].succ!
                }
              end
              @path = @id
              @parent = @chart = chart
              @cpath = @chart.path
              Pen::PenID_TBL[@cpath][@pen] = self
              unless not_create
                tk_call(@chart, 'pen', 'create', @pen, keys)
                return obj
              end
            }
          end
        }

        obj.configure(keys) if obj && ! keys.empty?
        obj
      end

      def initialize(chart, pen=nil, keys={})
        if pen.kind_of?(Hash)
          keys = pen
          pen = nil
        end
        if pen
          @pen = @id = pen.to_s
        else
          OBJ_ID.mutex.synchronize{
            @pen = @id = OBJ_ID.join(TkCore::INTERP._ip_id_).freeze
            OBJ_ID[1].succ!
          }
        end
        @path = @id
        @parent = @chart = chart
        @cpath = @chart.path
        Pen::PenID_TBL[@cpath][@pen] = self
        keys = _symbolkey2str(keys)
        unless keys.delete('without_creating')
          # @chart.pen_create(@pen, keys)
          tk_call(@chart, 'pen', 'create', @pen, keys)
        end
      end

      def id
        @id
      end

      def to_eval
        @id
      end

      def cget_tkstring(option)
        @chart.pen_cget_tkstring(@id, option)
      end
      def cget(option)
        @chart.pen_cget(@id, option)
      end
      def cget_strict(option)
        @chart.pen_cget_strict(@id, option)
      end
      def configure(key, value=None)
        @chart.pen_configure(@id, key, value)
        self
      end
      def configinfo(key=nil)
        @chart.pen_configinfo(@id, key)
      end
      def current_configinfo(key=nil)
        @chart.current_pen_configinfo(@id, key)
      end

      def delete
        @chart.pen_delete(@id)
        self
      end

      def name
        @pen
      end
    end

    #################

    class Postscript < TkObject
      PostscriptID_TBL = TkCore::INTERP.create_table

      TkCore::INTERP.init_ip_env{
        PostscriptID_TBL.mutex.synchronize{ PostscriptID_TBL.clear }
      }

      def self.new(chart, keys={})
        obj = nil
        PostscriptID_TBL.mutex.synchronize{
          unless (obj = PostscriptID_TBL[chart.path])
            (obj = self.allocate).instance_eval{
              @parent = @chart = chart
              @cpath = @chart.path
              @path = @id = 'postscript'
              Postscript::PostscriptID_TBL[@cpath] = self
            }
          end
        }
        chart.postscript_configure(keys) if obj && ! keys.empty?
        obj
      end

      def initialize(chart, keys={})
        # dummy:: not called by 'new' method

        @parent = @chart = chart
        @cpath = @chart.path
        # Postscript::PostscriptID_TBL[@cpath] = self
        @chart.postscript_configure(keys) unless keys.empty?
        @path = @id = 'postscript'
      end

      def id
        @id
      end

      def to_eval
        @id
      end

      def cget_tkstring(option)
        @chart.postscript_cget_tkstring(option)
      end
      def cget(option)
        @chart.postscript_cget(option)
      end
      def cget_strict(option)
        @chart.postscript_cget_strict(option)
      end
      def configure(key, value=None)
        @chart.postscript_configure(key, value)
        self
      end
      def configinfo(key=nil)
        @chart.postscript_configinfo(key)
      end
      def current_configinfo(key=nil)
        @chart.current_postscript_configinfo(key)
      end

      def output(file=nil, keys={})
        if file.kind_of?(Hash)
          keys = file
          file = nil
        end

        ret = @chart.postscript_output(file, keys)

        if file
          self
        else
          ret
        end
      end
    end

    #################
    class Marker < TkObject
      extend Tk
      extend TkItemFontOptkeys
      extend TkItemConfigOptkeys

      extend Tk::BLT::PlotComponent::OptKeys

      MarkerTypeName = nil
      MarkerTypeToClass = {}
      MarkerID_TBL = TkCore::INTERP.create_table

      TkCore::INTERP.init_ip_env{
        MarkerID_TBL.mutex.synchronize{ MarkerID_TBL.clear }
      }

      def Marker.type2class(type)
        MarkerTypeToClass[type]
      end

      def Marker.id2obj(chart, id)
        cpath = chart.path
        MarkerID_TBL.mutex.synchronize{
          if MarkerID_TBL[cpath]
            MarkerID_TBL[cpath][id]? MarkerID_TBL[cpath][id]: id
          else
            id
          end
        }
      end

      def self._parse_create_args(keys)
        fontkeys = {}
        methodkeys = {}
        if keys.kind_of? Hash
          keys = _symbolkey2str(keys)

          __item_font_optkeys(nil).each{|key|
            fkey = key.to_s
            fontkeys[fkey] = keys.delete(fkey) if keys.key?(fkey)

            fkey = "kanji#{key}"
            fontkeys[fkey] = keys.delete(fkey) if keys.key?(fkey)

            fkey = "latin#{key}"
            fontkeys[fkey] = keys.delete(fkey) if keys.key?(fkey)

            fkey = "ascii#{key}"
            fontkeys[fkey] = keys.delete(fkey) if keys.key?(fkey)
          }

          __item_optkey_aliases(nil).each{|alias_name, real_name|
            alias_name = alias_name.to_s
            if keys.has_key?(alias_name)
              keys[real_name.to_s] = keys.delete(alias_name)
            end
          }

          __item_methodcall_optkeys(nil).each{|key|
            key = key.to_s
            methodkeys[key] = keys.delete(key) if keys.key?(key)
          }

          __item_ruby2val_optkeys(nil).each{|key, method|
            key = key.to_s
            keys[key] = method.call(keys[key]) if keys.has_key?(key)
          }

          args = itemconfig_hash_kv(nil, keys)
        else
          args = []
        end

        [args, fontkeys, methodkeys]
      end
      private_class_method :_parse_create_args

      def self.create(chart, keys={})
        unless self::MarkerTypeName
          fail RuntimeError, "#{self} is an abstract class"
        end
        args, fontkeys, methodkeys = _parse_create_args(keys)
        idnum = tk_call_without_enc(chart.path, 'marker', 'create',
                                    self::MarkerTypeName, *args)
        chart.marker_configure(idnum, fontkeys) unless fontkeys.empty?
        chart.marker_configure(idnum, methodkeys) unless methodkeys.empty?
        idnum.to_i  # 'item id' is an integer number
      end

      def self.create_type(chart, type, keys={})
        args, fontkeys, methodkeys = _parse_create_args(keys)
        idnum = tk_call_without_enc(chart.path, 'marker', 'create',
                                    type, *args)
        chart.marker_configure(idnum, fontkeys) unless fontkeys.empty?
        chart.marker_configure(idnum, methodkeys) unless methodkeys.empty?
        id = idnum.to_i  # 'item id' is an integer number
        obj = self.allocate
        obj.instance_eval{
          @parent = @chart = chart
          @cpath = chart.path
          @id = id
          Tk::BLT::PlotComponent::Marker::MarkerID_TBL.mutex.synchronize{
            Tk::BLT::PlotComponent::Marker::MarkerID_TBL[@cpath] ||= {}
            Tk::BLT::PlotComponent::Marker::MarkerID_TBL[@cpath][@id] = self
          }
        }
        obj
      end

      def initialize(parent, *args)
        @parent = @chart = parent
        @cpath = parent.path

        @path = @id = create_self(*args) # an integer number as 'item id'
        Tk::BLT::PlotComponent::Marker::MarkerID_TBL.mutex.synchronize{
          Tk::BLT::PlotComponent::Marker::MarkerID_TBL[@cpath] ||= {}
          Tk::BLT::PlotComponent::Marker::MarkerID_TBL[@cpath][@id] = self
        }
      end
      def create_self(*args)
        self.class.create(@chart, *args) # return an integer as 'item id'
      end
      private :create_self

      def id
        @id
      end

      def to_eval
        @id
      end

      def cget_tkstring(option)
        @chart.marker_cget_tkstring(@id, option)
      end
      def cget(option)
        @chart.marker_cget(@id, option)
      end
      def cget_strict(option)
        @chart.marker_cget_strict(@id, option)
      end
      def configure(key, value=None)
        @chart.marker_configure(@id, key, value)
        self
      end
      def configinfo(key=nil)
        @chart.marker_configinfo(@id, key)
      end
      def current_configinfo(key=nil)
        @chart.current_marker_configinfo(@id, key)
      end

      def after(target=None)
        @chart.marker_after(@id, target)
      end

      def before(target=None)
        @chart.marker_before(@id, target)
      end

      def delete
        @chart.marker_delete(@id)
      end

      def exist?
        @chart.marker_exist(@id)
      end

      def type
        @chart.marker_type(@id)
      end
    end

    class TextMarker < Marker
      MarkerTypeName = 'text'.freeze
      MarkerTypeToClass[MarkerTypeName] = self
    end
    class LineMarker < Marker
      MarkerTypeName = 'line'.freeze
      MarkerTypeToClass[MarkerTypeName] = self
    end
    class BitmapMarker < Marker
      MarkerTypeName = 'bitmap'.freeze
      MarkerTypeToClass[MarkerTypeName] = self
    end
    class ImageMarker < Marker
      MarkerTypeName = 'image'.freeze
      MarkerTypeToClass[MarkerTypeName] = self
    end
    class PolygonMarker < Marker
      MarkerTypeName = 'polygon'.freeze
      MarkerTypeToClass[MarkerTypeName] = self
    end
    class WindowMarker < Marker
      MarkerTypeName = 'window'.freeze
      MarkerTypeToClass[MarkerTypeName] = self
    end

    #################

    def __destroy_hook__
      Axis::AxisID_TBL.delete(@path)
      Crosshairs::CrosshairsID_TBL.delete(@path)
      Element::ElementID_TBL.delete(@path)
      GridLine::GridLineID_TBL.delete(@path)
      Legend::LegendID_TBL.delete(@path)
      Pen::PenID_TBL.delete(@path)
      Postscript::PostscriptID_TBL.delete(@path)
      Marker::MarkerID_TBL.delete(@path)
      super()
    end

    #################

    def tagid(tag)
      if tag.kind_of?(Axis) ||
          tag.kind_of?(Crosshairs) ||
          tag.kind_of?(Element) ||
          tag.kind_of?(GridLine) ||
          tag.kind_of?(Legend) ||
          tag.kind_of?(Pen) ||
          tag.kind_of?(Postscript) ||
          tag.kind_of?(Marker)
        tag.id
      else
        tag  # maybe an Array of configure paramters
      end
    end

    def _component_bind(target, tag, context, *args)
      if TkComm._callback_entry?(args[0]) || !block_given?
        cmd = args.shift
      else
        cmd = Proc.new
      end
      _bind([path, target, 'bind', tagid(tag)], context, cmd, *args)
      self
    end
    def _component_bind_append(target, tag, context, *args)
      if TkComm._callback_entry?(args[0]) || !block_given?
        cmd = args.shift
      else
        cmd = Proc.new
      end
      _bind_append([path, target, 'bind', tagid(tag)], context, cmd, *args)
      self
    end
    def _component_bind_remove(target, tag, context)
      _bind_remove([path, target, 'bind', tagid(tag)], context)
      self
    end
    def _component_bindinfo(target, tag, context=nil)
      _bindinfo([path, target, 'bind', tagid(tag)], context)
    end
    private :_component_bind, :_component_bind_append
    private :_component_bind_remove, :_component_bindinfo

    def axis_bind(tag, context, *args)
      _component_bind('axis', tag, context, *args)
    end
    def axis_bind_append(tag, context, *args)
      _component_bind_append('axis', tag, context, *args)
    end
    def axis_bind_remove(tag, context)
      _component_bind_remove('axis', tag, context)
    end
    def axis_bindinfo(tag, context=nil)
      _component_bindinfo('axis', tag, context)
    end

    def element_bind(tag, context, *args)
      _component_bind('element', tag, context, *args)
    end
    def element_bind_append(tag, context, *args)
      _component_bind_append('element', tag, context, *args)
    end
    def element_bind_remove(tag, context)
      _component_bind_remove('element', tag, context)
    end
    def element_bindinfo(tag, context=nil)
      _component_bindinfo('element', tag, context)
    end

    def bar_bind(tag, context, *args)
      _component_bind('bar', tag, context, *args)
    end
    def bar_bind_append(tag, context, *args)
      _component_bind_append('bar', tag, context, *args)
    end
    def bar_bind_remove(tag, context)
      _component_bind_remove('bar', tag, context)
    end
    def bar_bindinfo(tag, context=nil)
      _component_bindinfo('bar', tag, context)
    end

    def line_bind(tag, context, *args)
      _component_bind('line', tag, context, *args)
    end
    def line_bind_append(tag, context, *args)
      _component_bind_append('line', tag, context, *args)
    end
    def line_bind_remove(tag, context)
      _component_bind_remove('line', tag, context)
    end
    def line_bindinfo(tag, context=nil)
      _component_bindinfo('line', tag, context)
    end

    def legend_bind(tag, context, *args)
      _component_bind('legend', tag, context, *args)
    end
    def legend_bind_append(tag, context, *args)
      _component_bind_append('legend', tag, context, *args)
    end
    def legend_bind_remove(tag, context)
      _component_bind_remove('legend', tag, context)
    end
    def legend_bindinfo(tag, context=nil)
      _component_bindinfo('legend', tag, context)
    end

    def marker_bind(tag, context, *args)
      _component_bind('marker', tag, context, *args)
    end
    def marker_bind_append(tag, context, *args)
      _component_bind_append('marker', tag, context, *args)
    end
    def marker_bind_remove(tag, context)
      _component_bind_remove('marker', tag, context)
    end
    def marker_bindinfo(tag, context=nil)
      _component_bindinfo('marker', tag, context)
    end

    ###################

    def axis_create(id=nil, keys={})
      # tk_send('axis', 'create', tagid(id), keys)
      Tk::BLT::PlotComponent::Axis.new(self, tagid(id), keys)
    end
    def axis_delete(*ids)
      tk_send('axis', 'delete', *(ids.collect{|id| tagid(id)}))
      self
    end
    def axis_invtransform(id, val)
      list(tk_send('axis', 'invtransform', tagid(id), val))
    end
    def axis_limits(id)
      list(tk_send('axis', 'limits', tagid(id)))
    end
    def axis_names(*pats)
      simplelist(tk_send('axis', 'names',
                         *(pats.collect{|pat| tagid(pat)}))).collect{|axis|
        Tk::BLT::PlotComponent::Axis.id2obj(self, axis)
      }
    end
    def axis_transform(id, val)
      list(tk_send('axis', 'transform', tagid(id), val))
    end
    def axis_view(id)
      tk_send('axis', 'view', tagid(id))
      self
    end
    def axis_use(id, target=nil)
      if target
        Tk::BLT::PlotComponent::Axis.id2obj(self,
                                            tk_send('axis', 'use',
                                                    tagid(id), tagid(target)))
      else
        Tk::BLT::PlotComponent::Axis.id2obj(self,
                                            tk_send('axis', 'use', tagid(id)))
      end
    end

    ###################

    def crosshairs_off
      tk_send_without_enc('crosshairs', 'off')
      self
    end
    def crosshairs_on
      tk_send_without_enc('crosshairs', 'on')
      self
    end
    def crosshairs_toggle
      tk_send_without_enc('crosshairs', 'toggle')
      self
    end

    ###################

    def element_create(id=nil, keys={})
      # tk_send('element', 'create', tagid(id), keys)
      Tk::BLT::PlotComponent::Element.new(self, tagid(id), keys)
    end
    def element_activate(*args)
      if args.empty?
        list(tk_send('element', 'activate')).collect{|elem|
          Tk::BLT::PlotComponent::Element.id2obj(self, elem)
        }
      else
        # id, *indices
        id = args.shift
        tk_send('element', 'activate', tagid(id), *args)
      end
    end
    def element_closest(x, y, var, *args)
      if args[-1].kind_of?(Hash)
        keys = args.pop
        bool(tk_send('element', 'closest', x, y, var,
                     *(hash_kv(keys).concat(args.collect{|id| tagid(id)}))))
      else
        bool(tk_send('element', 'closest', x, y, var,
                     *(args.collect{|id| tagid(id)})))
      end
    end
    def element_deactivate(*ids)
      tk_send('element', 'deactivate', *(ids.collect{|id| tagid(id)}))
      self
    end
    def element_delete(*ids)
      tk_send('element', 'delete', *(ids.collect{|id| tagid(id)}))
      self
    end
    def element_exist?(id)
      bool(tk_send('element', 'exists', tagid(id)))
    end
    def element_names(*pats)
      simplelist(tk_send('element', 'names',
                         *(pats.collect{|pat| tagid(pat)}))).collect{|elem|
        Tk::BLT::PlotComponent::Element.id2obj(self, elem)
      }
    end
    def element_show(*names)
      if names.empty?
        simplelist(tk_send('element', 'show'))
      else
        tk_send('element', 'show', *(names.collect{|n| tagid(n)}))
        self
      end
    end
    def element_type(id)
      tk_send('element', 'type', tagid(id))
    end

    ###################

    def bar_create(id=nil, keys={})
      # tk_send('bar', 'create', tagid(id), keys)
      Tk::BLT::PlotComponent::Bar.new(self, tagid(id), keys)
    end
    alias bar bar_create
    def bar_activate(*args)
      if args.empty?
        list(tk_send('bar', 'activate')).collect{|elem|
          Tk::BLT::PlotComponent::Element.id2obj(self, elem)
        }
      else
        # id, *indices
        id = args.shift
        tk_send('bar', 'activate', tagid(id), *args)
      end
    end
    def bar_closest(x, y, var, *args)
      if args[-1].kind_of?(Hash)
        keys = args.pop
        bool(tk_send('bar', 'closest', x, y, var,
                     *(hash_kv(keys).concat(args.collect{|id| tagid(id)}))))
      else
        bool(tk_send('bar', 'closest', x, y, var,
                     *(args.collect{|id| tagid(id)})))
      end
    end
    def bar_deactivate(*ids)
      tk_send('bar', 'deactivate', *(ids.collect{|id| tagid(id)}))
      self
    end
    def bar_delete(*ids)
      tk_send('bar', 'delete', *(ids.collect{|id| tagid(id)}))
      self
    end
    def bar_exist?(id)
      bool(tk_send('bar', 'exists', tagid(id)))
    end
    def bar_names(*pats)
      simplelist(tk_send('bar', 'names',
                         *(pats.collect{|pat| tagid(pat)}))).collect{|elem|
        Tk::BLT::PlotComponent::Element.id2obj(self, elem)
      }
    end
    def bar_show(*names)
      if names.empty?
        simplelist(tk_send('bar', 'show'))
      else
        tk_send('bar', 'show', *(names.collect{|n| tagid(n)}))
        self
      end
    end
    def bar_type(id)
      tk_send('bar', 'type', tagid(id))
    end

    ###################

    def line_create(id=nil, keys={})
      # tk_send('line', 'create', tagid(id), keys)
      Tk::BLT::PlotComponent::Line.new(self, tagid(id), keys)
    end
    alias bar line_create
    def line_activate(*args)
      if args.empty?
        list(tk_send('line', 'activate')).collect{|elem|
          Tk::BLT::PlotComponent::Element.id2obj(self, elem)
        }
      else
        # id, *indices
        id = args.shift
        tk_send('line', 'activate', tagid(id), *args)
      end
    end
    def line_closest(x, y, var, *args)
      if args[-1].kind_of?(Hash)
        keys = args.pop
        bool(tk_send('line', 'closest', x, y, var,
                     *(hash_kv(keys).concat(args.collect{|id| tagid(id)}))))
      else
        bool(tk_send('line', 'closest', x, y, var,
                     *(args.collect{|id| tagid(id)})))
      end
    end
    def line_deactivate(*ids)
      tk_send('line', 'deactivate', *(ids.collect{|id| tagid(id)}))
      self
    end
    def line_delete(*ids)
      tk_send('line', 'delete', *(ids.collect{|id| tagid(id)}))
      self
    end
    def line_exist?(id)
      bool(tk_send('line', 'exists', tagid(id)))
    end
    def line_names(*pats)
      simplelist(tk_send('line', 'names',
                         *(pats.collect{|pat| tagid(pat)}))).collect{|elem|
        Tk::BLT::PlotComponent::Element.id2obj(self, elem)
      }
    end
    def line_show(*names)
      if names.empty?
        simplelist(tk_send('line', 'show'))
      else
        tk_send('line', 'show', *(names.collect{|n| tagid(n)}))
        self
      end
    end
    def line_type(id)
      tk_send('line', 'type', tagid(id))
    end

    ###################

    def gridline_off
      tk_send_without_enc('grid', 'off')
      self
    end
    def gridline_on
      tk_send_without_enc('grid', 'on')
      self
    end
    def gridline_toggle
      tk_send_without_enc('grid', 'toggle')
      self
    end

    ###################

    def legend_window_create(parent=nil, keys=nil)
      if parent.kind_of?(Hash)
        keys = _symbolkey2str(parent)
        parent = keys.delete('parent')
        widgetname = keys.delete('widgetname')
        keys.delete('without_creating')
      elsif keys
        keys = _symbolkey2str(keys)
        widgetname = keys.delete('widgetname')
        keys.delete('without_creating')
      end

      legend = self.class.new(parent, :without_creating=>true,
                              :widgetname=>widgetname)
      class << legend
        def __destroy_hook__
          TkCore::INTERP.tk_windows.delete(@path)
        end
      end

      if keys
        self.legend_configure(keys.update('position'=>legend))
      else
        self.legend_configure('position'=>legend)
      end
      legend
    end

    def legend_activate(*pats)
      list(tk_send('legend', 'activate',
                   *(pats.collect{|pat| tagid(pat)}))).collect{|elem|
        Tk::BLT::PlotComponent::Element.id2obj(self, elem)
      }
    end
    def legend_deactivate(*pats)
      list(tk_send('legend', 'deactivate',
                   *(pats.collect{|pat| tagid(pat)}))).collect{|elem|
        Tk::BLT::PlotComponent::Element.id2obj(self, elem)
      }
    end
    def legend_get(pos, y=nil)
      if y
        Tk::BLT::PlotComponent::Element.id2obj(self,
                                               tk_send('legend', 'get',
                                                       _at(pos, y)))
      else
        Tk::BLT::PlotComponent::Element.id2obj(self,
                                               tk_send('legend', 'get', pos))
      end
    end

    ###################

    def pen_create(id=nil, keys={})
      # tk_send('pen', 'create', tagid(id), keys)
      Tk::BLT::PlotComponent::Pen.new(self, tagid(id), keys)
    end
    def pen_delete(*ids)
      tk_send('pen', 'delete', *(ids.collect{|id| tagid(id)}))
      self
    end
    def pen_names(*pats)
      simplelist(tk_send('pen', 'names',
                         *(pats.collect{|pat| tagid(pat)}))).collect{|pen|
        Tk::BLT::PlotComponent::Pen.id2obj(self, pen)
      }
    end

    ###################

    def postscript_output(file=nil, keys={})
      if file.kind_of?(Hash)
        keys = file
        file = nil
      end

      if file
        tk_send('postscript', 'output', file, keys)
        self
      else
        tk_send('postscript', 'output', keys)
      end
    end

    ###################

    def marker_create(type, keys={})
      case type
      when :text, 'text'
        Tk::BLT::PlotComponent::TextMarker.new(self, keys)
      when :line, 'line'
        Tk::BLT::PlotComponent::LineMarker.new(self, keys)
      when :bitmap, 'bitmap'
        Tk::BLT::PlotComponent::BitmapMarker.new(self, keys)
      when :image, 'image'
        Tk::BLT::PlotComponent::ImageMarker.new(self, keys)
      when :polygon, 'polygon'
        Tk::BLT::PlotComponent::PolygonMarker.new(self, keys)
      when :window, 'window'
        Tk::BLT::PlotComponent::WindowMarker.new(self, keys)
      else
        if type.kind_of?(Tk::BLT::PlotComponent::Marker)
          type.new(self, keys)
        else
          Tk::BLT::PlotComponent::Marker.create_type(self, type, keys)
        end
      end
    end
    def marker_after(id, target=nil)
      if target
        tk_send_without_enc('marker', 'after', tagid(id), tagid(target))
      else
        tk_send_without_enc('marker', 'after', tagid(id))
      end
      self
    end
    def marker_before(id, target=None)
      if target
        tk_send_without_enc('marker', 'before', tagid(id), tagid(target))
      else
        tk_send_without_enc('marker', 'before', tagid(id))
      end
      self
    end
    def marker_delete(*ids)
      tk_send('marker', 'delete', *(ids.collect{|id| tagid(id)}))
      self
    end
    def marker_exist?(id)
      bool(tk_send('marker', 'exists', tagid(id)))
    end
    def marker_names(*pats)
      simplelist(tk_send('marker', 'names',
                         *(pats.collect{|pat| tagid(pat)}))).collect{|id|
        Tk::BLT::PlotComponent::Marker.id2obj(self, id)
      }
    end
    def marker_type(id)
      tk_send('marker', 'type', tagid(id))
    end

    ###################

    def xaxis_cget_tkstring(option)
      itemcget_tkstring('xaxis', option)
    end
    def xaxis_cget(option)
      itemcget('xaxis', option)
    end
    def xaxis_cget_strict(option)
      itemcget_strict('xaxis', option)
    end
    def xaxis_configure(slot, value=None)
      if slot.kind_of?(Hash)
        slot = _symbolkey2str(slot)
        if cmd = slot.delete('command')
          slot['command'] = proc{|w, tick|
            cmd.call(TkComm.window(w), TkComm.num_or_str(tick))
          }
        end
      elsif slot == :command || slot == 'command'
        cmd = value
        value = proc{|w, tick|
          cmd.call(TkComm.window(w), TkComm.num_or_str(tick))
        }
      end
      itemconfigure('xaxis', slot, value)
    end
    def xaxis_configinfo(slot=nil)
      itemconfiginfo('xaxis', slot)
    end
    def current_xaxis_configinfo(slot=nil)
      current_itemconfiginfo('xaxis', slot)
    end
    def xaxis_bind(context, *args)
      if TkComm._callback_entry?(args[0]) || !block_given?
        cmd = args.shift
      else
        cmd = Proc.new
      end
      _bind([path, 'xaxis', 'bind'], context, cmd, *args)
      self
    end
    def xaxis_bind_append(context, *args)
      if TkComm._callback_entry?(args[0]) || !block_given?
        cmd = args.shift
      else
        cmd = Proc.new
      end
      _bind_append([path, 'xaxis', 'bind'], context, cmd, *args)
      self
    end
    def xaxis_bind_remove(context)
      _bind_remove([path, 'xaxis', 'bind'], context)
      self
    end
    def xaxis_bindinfo(context=nil)
      _bindinfo([path, 'xaxis', 'bind'], context)
    end
    def xaxis_invtransform(val)
      list(tk_send('xaxis', 'invtransform', val))
    end
    def xaxis_limits
      list(tk_send('xaxis', 'limits'))
    end
    def xaxis_transform(val)
      list(tk_send('xaxis', 'transform', val))
    end
    def xaxis_use(target=nil)
      if target
        Tk::BLT::PlotComponent::Axis.id2obj(self,
                                            tk_send('xaxis', 'use',
                                                    tagid(target)))
      else
        Tk::BLT::PlotComponent::Axis.id2obj(self, tk_send('xaxis', 'use'))
      end
    end

    def x2axis_cget_tkstring(option)
      itemcget_tkstring('x2axis', option)
    end
    def x2axis_cget(option)
      itemcget('x2axis', option)
    end
    def x2axis_cget_strict(option)
      itemcget_strict('x2axis', option)
    end
    def x2axis_configure(slot, value=None)
      if slot.kind_of?(Hash)
        slot = _symbolkey2str(slot)
        if cmd = slot.delete('command')
          slot['command'] = proc{|w, tick|
            cmd.call(TkComm.window(w), TkComm.num_or_str(tick))
          }
        end
      elsif slot == :command || slot == 'command'
        cmd = value
        value = proc{|w, tick|
          cmd.call(TkComm.window(w), TkComm.num_or_str(tick))
        }
      end
      itemconfigure('x2axis', slot, value)
    end
    def x2axis_configinfo(slot=nil)
      itemconfiginfo('x2axis', slot)
    end
    def current_x2axis_configinfo(slot=nil)
      current_itemconfiginfo('x2axis', slot)
    end
    def x2axis_bind(context, *args)
      if TkComm._callback_entry?(args[0]) || !block_given?
        cmd = args.shift
      else
        cmd = Proc.new
      end
      _bind([path, 'x2axis', 'bind'], context, cmd, *args)
      self
    end
    def x2axis_bind_append(context, *args)
      if TkComm._callback_entry?(args[0]) || !block_given?
        cmd = args.shift
      else
        cmd = Proc.new
      end
      _bind_append([path, 'x2axis', 'bind'], context, cmd, *args)
      self
    end
    def x2axis_bind_remove(context)
      _bind_remove([path, 'x2axis', 'bind'], context)
      self
    end
    def x2axis_bindinfo(context=nil)
      _bindinfo([path, 'x2axis', 'bind'], context)
    end
    def x2axis_invtransform(val)
      list(tk_send('x2axis', 'invtransform', val))
    end
    def x2axis_limits
      list(tk_send('x2axis', 'limits'))
    end
    def x2axis_transform(val)
      list(tk_send('x2axis', 'transform', val))
    end
    def x2axis_use(target=nil)
      if target
        Tk::BLT::PlotComponent::Axis.id2obj(self,
                                            tk_send('x2axis', 'use',
                                                    tagid(target)))
      else
        Tk::BLT::PlotComponent::Axis.id2obj(self, tk_send('x2axis', 'use'))
      end
    end

    def yaxis_cget_tkstring(option)
      itemcget_tkstring('yaxis', option)
    end
    def yaxis_cget(option)
      itemcget('yaxis', option)
    end
    def yaxis_cget_strict(option)
      itemcget_strict('yaxis', option)
    end
    def yaxis_configure(slot, value=None)
      if slot.kind_of?(Hash)
        slot = _symbolkey2str(slot)
        if cmd = slot.delete('command')
          slot['command'] = proc{|w, tick|
            cmd.call(TkComm.window(w), TkComm.num_or_str(tick))
          }
        end
      elsif slot == :command || slot == 'command'
        cmd = value
        value = proc{|w, tick|
          cmd.call(TkComm.window(w), TkComm.num_or_str(tick))
        }
      end
      itemconfigure('yaxis', slot, value)
    end
    def yaxis_configinfo(slot=nil)
      itemconfiginfo('yaxis', slot)
    end
    def current_yaxis_configinfo(slot=nil)
      current_itemconfiginfo('yaxis', slot)
    end
    def yaxis_bind(context, *args)
      if TkComm._callback_entry?(args[0]) || !block_given?
        cmd = args.shift
      else
        cmd = Proc.new
      end
      _bind([path, 'yaxis', 'bind'], context, cmd, *args)
      self
    end
    def yaxis_bind_append(context, *args)
      if TkComm._callback_entry?(args[0]) || !block_given?
        cmd = args.shift
      else
        cmd = Proc.new
      end
      _bind_append([path, 'yaxis', 'bind'], context, cmd, *args)
      self
    end
    def yaxis_bind_remove(context)
      _bind_remove([path, 'yaxis', 'bind'], context)
      self
    end
    def yaxis_bindinfo(context=nil)
      _bindinfo([path, 'yaxis', 'bind'], context)
    end
    def yaxis_invtransform(val)
      list(tk_send('yaxis', 'invtransform', val))
    end
    def yaxis_limits
      list(tk_send('yaxis', 'limits'))
    end
    def yaxis_transform(val)
      list(tk_send('yaxis', 'transform', val))
    end
    def yaxis_use(target=nil)
      if target
        Tk::BLT::PlotComponent::Axis.id2obj(self,
                                            tk_send('yaxis', 'use',
                                                    tagid(target)))
      else
        Tk::BLT::PlotComponent::Axis.id2obj(self, tk_send('yaxis', 'use'))
      end
    end

    def y2axis_cget_tkstring(option)
      itemcget_tkstring('y2axis', option)
    end
    def y2axis_cget(option)
      itemcget('y2axis', option)
    end
    def y2axis_cget_strict(option)
      itemcget_strict('y2axis', option)
    end
    def y2axis_configure(slot, value=None)
      if slot.kind_of?(Hash)
        slot = _symbolkey2str(slot)
        if cmd = slot.delete('command')
          slot['command'] = proc{|w, tick|
            cmd.call(TkComm.window(w), TkComm.num_or_str(tick))
          }
        end
      elsif slot == :command || slot == 'command'
        cmd = value
        value = proc{|w, tick|
          cmd.call(TkComm.window(w), TkComm.num_or_str(tick))
        }
      end
      itemconfigure('y2axis', slot, value)
    end
    def y2axis_configinfo(slot=nil)
      axis_configinfo('y2axis', slot)
    end
    def current_y2axis_configinfo(slot=nil)
      current_itemconfiginfo('y2axis', slot)
    end
    def y2axis_bind(context, *args)
      if TkComm._callback_entry?(args[0]) || !block_given?
        cmd = args.shift
      else
        cmd = Proc.new
      end
      _bind([path, 'y2axis', 'bind'], context, cmd, *args)
      self
    end
    def y2axis_bind_append(context, *args)
      if TkComm._callback_entry?(args[0]) || !block_given?
        cmd = args.shift
      else
        cmd = Proc.new
      end
      _bind_append([path, 'y2axis', 'bind'], context, cmd, *args)
      self
    end
    def y2axis_bind_remove(context)
      _bind_remove([path, 'y2axis', 'bind'], context)
      self
    end
    def y2axis_bindinfo(context=nil)
      _bindinfo([path, 'y2axis', 'bind'], context)
    end
    def y2axis_invtransform(val)
      list(tk_send('y2axis', 'invtransform', val))
    end
    def y2axis_limits
      list(tk_send('y2axis', 'limits'))
    end
    def y2axis_transform(val)
      list(tk_send('y2axis', 'transform', val))
    end
    def y2axis_use(target=nil)
      if target
        Tk::BLT::PlotComponent::Axis.id2obj(self,
                                            tk_send('y2axis', 'use',
                                                    tagid(target)))
      else
        Tk::BLT::PlotComponent::Axis.id2obj(self, tk_send('y2axis', 'use'))
      end
    end
  end
end
