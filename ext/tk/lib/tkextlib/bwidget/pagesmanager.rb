#
#  tkextlib/bwidget/pagesmanager.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tk/frame'
require 'tkextlib/bwidget.rb'

module Tk
  module BWidget
    class PagesManager < TkWindow
    end
  end
end

class Tk::BWidget::PagesManager
  TkCommandNames = ['PagesManager'.freeze].freeze
  WidgetClassName = 'PagesManager'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  def tagid(id)
    # id.to_s
    _get_eval_string(id)
  end

  def add(page, &b)
    win = window(tk_send('add', tagid(page)))
    if b
      if TkCore::WITH_RUBY_VM  ### Ruby 1.9 !!!!
        win.instance_exec(self, &b)
      else
        win.instance_eval(&b)
      end
    end
    win
  end

  def compute_size
    tk_send('compute_size')
    self
  end

  def delete(page)
    tk_send('delete', tagid(page))
    self
  end

  def get_frame(page, &b)
    win = window(tk_send('getframe', tagid(page)))
    if b
      if TkCore::WITH_RUBY_VM  ### Ruby 1.9 !!!!
        win.instance_exec(self, &b)
      else
        win.instance_eval(&b)
      end
    end
    win
  end

  def get_page(page)
    tk_send('pages', page)
  end

  def pages(first=None, last=None)
    list(tk_send('pages', first, last))
  end

  def raise(page=None)
    tk_send('raise', page)
    self
  end
end
