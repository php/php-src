#
#  tkextlib/tcllib/ctext.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
#   * Part of tcllib extension
#   * Overloads the text widget and provides new commands
#

require 'tk'
require 'tk/text'
require 'tkextlib/tcllib.rb'

# TkPackage.require('ctext', '3.1')
TkPackage.require('ctext')

module Tk
  module Tcllib
    class CText < Tk::Text
      PACKAGE_NAME = 'ctext'.freeze
      def self.package_name
        PACKAGE_NAME
      end

      def self.package_version
        begin
          TkPackage.require('ctext')
        rescue
          ''
        end
      end
    end
  end
end

class Tk::Tcllib::CText
  TkCommandNames = ['ctext'.freeze].freeze
  WidgetClassName = 'Ctext'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def create_self(keys)
    if keys and keys != None
      tk_call_without_enc(self.class::TkCommandNames[0], @path,
                          *hash_kv(keys, true))
    else
      tk_call_without_enc(self.class::TkCommandNames[0], @path)
    end
  end
  private :create_self

  def __strval_optkeys
    super() << 'linemapfg' << 'linemapbg' <<
      'linemap_select_fg' << 'linemap_select_bg'
  end
  private :__strval_optkeys

  def __boolval_optkeys
    super() << 'highlight' << 'linemap_markable'
  end
  private :__boolval_optkeys

  def append(*args)
    tk_send('append', *args)
  end

  def copy
    tk_send('copy')
  end

  def cut
    tk_send('cut')
  end

  def fast_delete(*args)
    tk_send('fastdelete', *args)
  end

  def fast_insert(*args)
    tk_send('fastinsert', *args)
  end

  def highlight(*args)
    tk_send('highlight', *args)
  end

  def paste
    tk_send('paste')
  end

  def edit(*args)
    tk_send('edit', *args)
  end

  def add_highlight_class(klass, col, *keywords)
    tk_call('ctext::addHighlightClass', @path, klass, col, keywords.flatten)
    self
  end

  def add_highlight_class_for_special_chars(klass, col, *chrs)
    tk_call('ctext::addHighlightClassForSpecialChars',
            @path, klass, col, chrs.join(''))
    self
  end

  def add_highlight_class_for_regexp(klass, col, tcl_regexp)
    tk_call('ctext::addHighlightClassForRegexp',
            @path, klass, col, tcl_regexp)
    self
  end

  def add_highlight_class_with_only_char_start(klass, col, chr)
    tk_call('ctext::addHighlightClassWithOnlyCharStart',
            @path, klass, col, chr)
    self
  end

  def clear_highlight_classes
    tk_call('ctext::clearHighlightClasses', @path)
    self
  end

  def get_highlight_classes
    tk_split_simplelist(tk_call('ctext::getHighlightClasses', @path))
  end

  def delete_highlight_class(klass)
    tk_call('ctext::deleteHighlightClass', @path, klass)
    self
  end

  def enable_C_comments
    tk_call('ctext::enableComments', @path)
    self
  end

  def disable_C_comments
    tk_call('ctext::disableComments', @path)
    self
  end

  def find_next_char(idx, chr)
    tk_call('ctext::findNextChar', @path, idx, chr)
  end

  def find_next_space(idx)
    tk_call('ctext::findNextSpace', @path, idx)
  end

  def find_previous_space(idx)
    tk_call('ctext::findPreviousSpace', @path, idx)
  end

  def set_update_proc(cmd=Proc.new)
    tk_call('proc', 'ctext::update', '', cmd)
    self
  end

  def modified?(mode)
    bool(tk_call('ctext::modified', @path, mode))
  end
end
