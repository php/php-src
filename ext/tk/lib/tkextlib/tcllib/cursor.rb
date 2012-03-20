#
#  tkextlib/tcllib/cursor.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * Procedures to handle CURSOR data
#

require 'tk'
require 'tkextlib/tcllib.rb'

module Tk
  module Tcllib
    module Cursor
      PACKAGE_NAME = 'cursor'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('cursor')
        rescue
          ''
        end
      end

      def self.not_available
        fail RuntimeError, "'tkextlib/tcllib/cursor' extension is not available on your current environment."
      end

      def self.cursor_display(win=None)
        Tk::Tcllib::Cursor.not_available
      end

      def self.cursor_propagate(win, cursor)
        Tk::Tcllib::Cursor.not_available
      end

      def self.cursor_restore(win, cursor = None)
        Tk::Tcllib::Cursor.not_available
      end
    end
  end

  def self.cursor_display(parent=None)
    # Pops up a dialog with a listbox containing all the cursor names.
    # Selecting a cursor name will display it in that dialog.
    # This is simply for viewing any available cursors on the platform .
    #tk_call_without_enc('::cursor::display', parent)
    Tk::Tcllib::Cursor.cursor_display(parent)
  end
end

class TkWindow
  def cursor_propagate(cursor)
    # Sets the cursor for self and all its descendants to cursor.
    #tk_call_without_enc('::cursor::propagate', @path, cursor)
    Tk::Tcllib::Cursor.cursor_propagate(self, cursor)
  end
  def cursor_restore(cursor = None)
    # Restore the original or previously set cursor for self and all its
    # descendants. If cursor is specified, that will be used if on any
    # widget that did not have a preset cursor (set by a previous call
    # to TkWindow#cursor_propagate).
    #tk_call_without_enc('::cursor::restore', @path, cursor)
    Tk::Tcllib::Cursor.cursor_restore(self, cursor)
  end
end

# TkPackage.require('cursor', '0.1')
TkPackage.require('cursor')

module Tk
  module Tcllib
    class << Cursor
      undef not_available
    end

    module Cursor
      extend TkCore
      def self.cursor_display(win=None)
        tk_call_without_enc('::cursor::display', _epath(win))
      end

      def self.cursor_propagate(win, cursor)
        #tk_call_without_enc('::cursor::propagate', win.path, cursor)
        tk_call_without_enc('::cursor::propagate', _epath(win), cursor)
      end

      def self.cursor_restore(win, cursor = None)
        #tk_call_without_enc('::cursor::restore', win.path, cursor)
        tk_call_without_enc('::cursor::restore', _epath(win), cursor)
      end
    end
  end
end
