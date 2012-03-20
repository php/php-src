#
#  tkextlib/tcllib/autoscroll.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * Provides for a scrollbar to automatically mapped and unmapped as needed
#
# (The following is the original description of the library.)
#
# This package allows scrollbars to be mapped and unmapped as needed
# depending on the size and content of the scrollbars scrolled widget.
# The scrollbar must be managed by either pack or grid, other geometry
# managers are not supported.
#
# When managed by pack, any geometry changes made in the scrollbars parent
# between the time a scrollbar is unmapped, and when it is mapped will be
# lost. It is an error to destroy any of the scrollbars siblings while the
# scrollbar is unmapped. When managed by grid, if anything becomes gridded
# in the same row and column the scrollbar occupied it will be replaced by
# the scrollbar when remapped.
#
# This package may be used on any scrollbar-like widget as long as it
# supports the set subcommand in the same style as scrollbar. If the set
# subcommand is not used then this package will have no effect.
#

require 'tk'
require 'tk/scrollbar'
require 'tkextlib/tcllib.rb'

module Tk
  module Tcllib
    module Autoscroll
      PACKAGE_NAME = 'autoscroll'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('autoscroll')
        rescue
          ''
        end
      end

      def self.not_available
        fail RuntimeError, "'tkextlib/tcllib/autoscroll' extension is not available on your current environment."
      end

      def self.autoscroll(win)
        Tk::Tcllib::Autoscroll.not_available
      end

      def self.unautoscroll(win)
        Tk::Tcllib::Autoscroll.not_available
      end
    end
  end
end

module Tk
  module Scrollable
    def autoscroll(mode = nil)
      case mode
      when :x, 'x'
        if @xscrollbar
          Tk::Tcllib::Autoscroll.autoscroll(@xscrollbar)
        end
      when :y, 'y'
        if @yscrollbar
          Tk::Tcllib::Autoscroll.autoscroll(@yscrollbar)
        end
      when nil, :both, 'both'
        if @xscrollbar
          Tk::Tcllib::Autoscroll.autoscroll(@xscrollbar)
        end
        if @yscrollbar
          Tk::Tcllib::Autoscroll.autoscroll(@yscrollbar)
        end
      else
        fail ArgumentError, "'x', 'y' or 'both' (String or Symbol) is expected"
      end
      self
    end
    def unautoscroll(mode = nil)
      case mode
      when :x, 'x'
        if @xscrollbar
          Tk::Tcllib::Autoscroll.unautoscroll(@xscrollbar)
        end
      when :y, 'y'
        if @yscrollbar
          Tk::Tcllib::Autoscroll.unautoscroll(@yscrollbar)
        end
      when nil, :both, 'both'
        if @xscrollbar
          Tk::Tcllib::Autoscroll.unautoscroll(@xscrollbar)
        end
        if @yscrollbar
          Tk::Tcllib::Autoscroll.unautoscroll(@yscrollbar)
        end
      else
        fail ArgumentError, "'x', 'y' or 'both' (String or Symbol) is expected"
      end
      self
    end
  end
end

class Tk::Scrollbar
  def autoscroll
    # Arranges for the already existing scrollbar to be mapped
    # and unmapped as needed.
    #tk_call_without_enc('::autoscroll::autoscroll', @path)
    Tk::Tcllib::Autoscroll.autoscroll(self)
    self
  end
  def unautoscroll
    #     Returns the scrollbar to its original static state.
    #tk_call_without_enc('::autoscroll::unautoscroll', @path)
    Tk::Tcllib::Autoscroll.unautoscroll(self)
    self
  end
end

# TkPackage.require('autoscroll', '1.0')
# TkPackage.require('autoscroll', '1.1')
TkPackage.require('autoscroll')

module Tk
  module Tcllib
    class << Autoscroll
      undef not_available
    end

    module Autoscroll
      extend TkCore
      def self.autoscroll(win)
        tk_call_without_enc('::autoscroll::autoscroll', win.path)
      end

      def self.unautoscroll(win)
        tk_call_without_enc('::autoscroll::unautoscroll', win.path)
      end

      def self.wrap
        # v1.1
        tk_call_without_enc('::autoscroll::wrap')
      end

      def self.unwrap
        # v1.1
        tk_call_without_enc('::autoscroll::unwrap')
      end
    end
  end
end
