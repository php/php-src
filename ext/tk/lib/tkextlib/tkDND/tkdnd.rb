#
#  tkextlib/tkDND/tkdnd.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'

# call setup script for general 'tkextlib' libraries
require 'tkextlib/setup.rb'

# call setup script
require 'tkextlib/tkDND/setup.rb'

TkPackage.require('tkdnd')

module Tk
  module TkDND
    PACKAGE_NAME = 'tkdnd'.freeze
    def self.package_name
      PACKAGE_NAME
    end

    def self.package_version
      begin
        TkPackage.require('tkdnd')
      rescue
        ''
      end
    end

    class DND_Subst < TkUtil::CallbackSubst
      KEY_TBL = [
        [ ?a, ?l, :actions ],
        [ ?A, ?s, :action ],
        [ ?b, ?L, :codes ],
        [ ?c, ?s, :code ],
        [ ?d, ?l, :descriptions ],
        [ ?D, ?l, :data ],
        [ ?L, ?l, :source_types ],
        [ ?m, ?l, :modifiers ],
        [ ?t, ?l, :types ],
        [ ?T, ?s, :type ],
        [ ?W, ?w, :widget ],
        [ ?x, ?n, :x ],
        [ ?X, ?n, :x_root ],
        [ ?y, ?n, :y ],
        [ ?Y, ?n, :y_root ],
        nil
      ]

      PROC_TBL = [
        [ ?n, TkComm.method(:num_or_str) ],
        [ ?s, TkComm.method(:string) ],
        [ ?l, TkComm.method(:list) ],
        [ ?L, TkComm.method(:simplelist) ],
        [ ?w, TkComm.method(:window) ],
        nil
      ]

=begin
      # for Ruby m17n :: ?x --> String --> char-code ( getbyte(0) )
      KEY_TBL.map!{|inf|
        if inf.kind_of?(Array)
          inf[0] = inf[0].getbyte(0) if inf[0].kind_of?(String)
          inf[1] = inf[1].getbyte(0) if inf[1].kind_of?(String)
        end
        inf
      }

      PROC_TBL.map!{|inf|
        if inf.kind_of?(Array)
          inf[0] = inf[0].getbyte(0) if inf[0].kind_of?(String)
        end
        inf
      }
=end

      # setup tables
      _setup_subst_table(KEY_TBL, PROC_TBL);
    end

    module DND
      def self.version
        begin
          TkPackage.require('tkdnd')
        rescue
          ''
        end
      end

      def dnd_bindtarget_info(type=nil, event=nil)
        if event
          procedure(tk_call('dnd', 'bindtarget', @path, type, event))
        elsif type
          procedure(tk_call('dnd', 'bindtarget', @path, type))
        else
          simplelist(tk_call('dnd', 'bindtarget', @path))
        end
      end

      #def dnd_bindtarget(type, event, cmd=Proc.new, prior=50, *args)
      #  event = tk_event_sequence(event)
      #  if prior.kind_of?(Numeric)
      #    tk_call('dnd', 'bindtarget', @path, type, event,
      #            install_bind_for_event_class(DND_Subst, cmd, *args),
      #            prior)
      #  else
      #    tk_call('dnd', 'bindtarget', @path, type, event,
      #            install_bind_for_event_class(DND_Subst, cmd, prior, *args))
      #  end
      #  self
      #end
      def dnd_bindtarget(type, event, *args)
        # if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
        if TkComm._callback_entry?(args[0]) || !block_given?
          cmd = args.shift
        else
          cmd = Proc.new
        end

        prior = 50
        prior = args.shift unless args.empty?

        event = tk_event_sequence(event)
        if prior.kind_of?(Numeric)
          tk_call('dnd', 'bindtarget', @path, type, event,
                  install_bind_for_event_class(DND_Subst, cmd, *args),
                  prior)
        else
          tk_call('dnd', 'bindtarget', @path, type, event,
                  install_bind_for_event_class(DND_Subst, cmd, prior, *args))
        end
        self
      end

      def dnd_cleartarget
        tk_call('dnd', 'cleartarget', @path)
        self
      end

      def dnd_bindsource_info(type=nil)
        if type
          procedure(tk_call('dnd', 'bindsource', @path, type))
        else
          simplelist(tk_call('dnd', 'bindsource', @path))
        end
      end

      #def dnd_bindsource(type, cmd=Proc.new, prior=None)
      #  tk_call('dnd', 'bindsource', @path, type, cmd, prior)
      #  self
      #end
      def dnd_bindsource(type, *args)
        # if args[0].kind_of?(Proc) || args[0].kind_of?(Method)
        if TkComm._callback_entry?(args[0]) || !block_given?
          cmd = args.shift
        else
          cmd = Proc.new
        end

        args = [None] if args.empty

        tk_call('dnd', 'bindsource', @path, type, cmd, *args)
        self
      end

      def dnd_clearsource()
        tk_call('dnd', 'clearsource', @path)
        self
      end

      def dnd_drag(keys=nil)
        tk_call('dnd', 'drag', @path, *hash_kv(keys))
        self
      end
    end
  end
end

class TkWindow
  include Tk::TkDND::DND
end
