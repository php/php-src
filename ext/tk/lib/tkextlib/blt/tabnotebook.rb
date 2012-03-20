#
#  tkextlib/blt/tabnotebook.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tkextlib/blt.rb'
require 'tkextlib/blt/tabset.rb'

module Tk::BLT
  class Tabnotebook < Tabset
    TkCommandNames = ['::blt::tabnotebook'.freeze].freeze
    WidgetClassName = 'Tabnotebook'.freeze
    WidgetClassNames[WidgetClassName] ||= self

    class Tab < Tk::BLT::Tabset::Tab
      def self.new(parent, pos=nil, name=nil, keys={})
        if pos.kind_of?(Hash)
          keys = pos
          name = nil
          pos  = nil
        end
        if name.kind_of?(Hash)
          keys = name
          name = nil
        end
        obj = nil
        TabID_TBL.mutex.synchronize{
          if name && TabID_TBL[parent.path] && TabID_TBL[parent.path][name]
            obj = TabID_TBL[parent.path][name]
            if pos
              if pos.to_s == 'end'
                obj.move_after('end')
              else
                obj.move_before(pos)
              end
            end
            obj.configure if keys && ! keys.empty?
          else
            (obj = self.allocate).instance_eval{
              initialize(parent, pos, name, keys)
              TabID_TBL[@tpath] = {} unless TabID_TBL[@tpath]
              TabID_TBL[@tpath][@id] = self
            }
          end
        }
        obj
      end

      def initialize(parent, pos, name, keys)
        @t = parent
        @tpath = parent.path
        if name
          @path = @id = name
          unless (list(tk_call(@tpath, 'tab', 'names', @id)).empty?)
            if pos
              idx = tk_call(@tpath, 'index', @id)
              if pos.to_s == 'end'
                tk_call(@tpath, 'move', idx, 'after', 'end')
              else
                tk_call(@tpath, 'move', idx, 'before', pos)
              end
            end
            tk_call(@tpath, 'tab', 'configure', @id, keys)
          else
            fail ArgumentError, "can't find tab \"#{@id}\" in #{@t}"
          end
        else
          pos = 'end' unless pos
          @path = @id = tk_call(@tpath, 'insert', pos, keys)
        end
      end
    end

    #######################################

    def get_tab(index)
      if (idx = tk_send_without_enc('id', tagindex(index))).empty?
        nil
      else
        Tk::BLT::Tabset::Tab.id2obj(self, idx)
      end
    end
    alias get_id get_tab

    def get_tabobj(index)
      if (idx = tk_send_without_enc('id', tagindex(index))).empty?
        nil
      else
        Tk::BLT::Tabnotebook::Tab.new(self, nil, idx)
      end
    end

    alias index_name index

    def insert(pos=nil, keys={})
      if pos.kind_of?(Hash)
        keys = pos
        pos = nil
      end
      pos = 'end' if pos.nil?
      Tk::BLT::Tabnotebook::Tab.new(self, nil,
                                    tk_send('insert', tagindex(pos), keys))

    end
    undef :insert_tabs

    undef :tab_pageheight, :tab_pagewidth
  end
end
