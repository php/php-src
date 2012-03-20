#
#  tkextlib/tkHTML/htmlwidget.rb
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'

# call setup script for general 'tkextlib' libraries
require 'tkextlib/setup.rb'

# call setup script
require 'tkextlib/tkHTML/setup.rb'

# TkPackage.require('Tkhtml', '2.0')
TkPackage.require('Tkhtml')

module Tk
  class HTML_Widget < TkWindow
    PACKAGE_NAME = 'Tkhtml'.freeze
    def self.package_name
      PACKAGE_NAME
    end

    def self.package_version
      begin
        TkPackage.require('Tkhtml')
      rescue
        ''
      end
    end

    class ClippingWindow < TkWindow
    end
  end
end

class Tk::HTML_Widget::ClippingWindow
  WidgetClassName = 'HtmlClip'.freeze
  WidgetClassNames[WidgetClassName] ||= self

  HtmlClip_TBL = TkCore::INTERP.create_table

  TkCore::INTERP.init_ip_env{
    HtmlClip_TBL.mutex.synchronize{ HtmlClip_TBL.clear }
  }

  def self.new(parent, keys={})
    if parent.kind_of?(Hash)
      keys = TkComm._symbolkey2str(parent)
      parent = keys.delete('parent')
    end

    if parent.kind_of?(String)
      ppath = parent.path
    elsif parent
      ppath = parent
    else
      ppath = ''
    end
    HtmlClip_TBL.mutex.synchronize{
      return HtmlClip_TBL[ppath] if HtmlClip_TBL[ppath]
    }

    widgetname = keys.delete('widgetname')
    if widgetname =~ /^(.*)\.[^.]+$/
      ppath2 = $1
      if ppath2[0] != ?.
        ppath2 = ppath + '.' + ppath2
      end
      HtmlClip_TBL.mutex.synchronize{
        return HtmlClip_TBL[ppath2] if HtmlClip_TBL[ppath2]
      }

      ppath = ppath2
    end

    parent = TkComm._genobj_for_tkwidget(ppath)
    unless parent.kind_of?(Tk::HTML_Widget)
      fail ArgumentError, "parent must be a Tk::HTML_Widget instance"
    end

    super(parent)
  end

  def initialize(parent)
    @parent = parent
    @ppath = parent.path
    @path = @id = @ppath + '.x'
    HtmlClip_TBL.mutex.synchronize{
      HtmlClip_TBL[@ppath] = self
    }
  end

  def method_missing(m, *args, &b)
    @parent.__send__(m, *args, &b)
  end
end

class Tk::HTML_Widget
  include Scrollable

  TkCommandNames = ['html'.freeze].freeze
  WidgetClassName = 'Html'.freeze
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
    super() << 'base' << 'selectioncolor' << 'unvisitedcolor' << 'visitedcolor'
  end
  private :__strval_optkeys

  ###################################
  #  class methods
  ###################################
  def self.reformat(src, dst, txt)
    tk_call('html', 'reformat', src, dst, txt)
  end

  def self.url_join(*args) # args := sheme authority path query fragment
    tk_call('html', 'urljoin', *args)
  end

  def self.url_split(uri)
    tk_call('html', 'urlsplit', uri)
  end

  def self.lockcopy(src, dst)
    tk_call('html', 'lockcopy', src, dst)
  end

  def self.gzip_file(file, dat)
    tk_call('html', 'gzip', 'file', file, dat)
  end

  def self.gunzip_file(file, dat)
    tk_call('html', 'gunzip', 'file', filet)
  end

  def self.gzip_data(dat)
    tk_call('html', 'gzip', 'data', file, dat)
  end

  def self.gunzip_data(dat)
    tk_call('html', 'gunzip', 'data', filet)
  end

  def self.base64_encode(dat)
    tk_call('html', 'base64', 'encode', dat)
  end

  def self.base64_decode(dat)
    tk_call('html', 'base64', 'encode', dat)
  end

  def self.text_format(dat, len)
    tk_call('html', 'text', 'format', dat, len)
  end

  def self.xor(cmd, *args)
    tk_call('html', 'xor', cmd, *args)
  end

  def self.stdchan(cmd, channel)
    tk_call('html', 'stdchan', cmd, channel)
  end

  def self.crc32(data)
    tk_call('html', 'crc32', data)
  end

  ###################################
  #  instance methods
  ###################################
  def clipping_window
    ClippingWindow.new(self)
  end
  alias clipwin  clipping_window
  alias htmlclip clipping_window

  def bgimage(image, tid=None)
    tk_send('bgimage', image, tid)
    self
  end

  def clear()
    tk_send('clear')
    self
  end

  def coords(index=None, percent=None)
    tk_send('coords', index, percent)
  end

  def forminfo(*args)
    tk_send('forminfo', *args)
  end
  alias form_info forminfo

  def href(x, y)
    simplelist(tk_send('href', x, y))
  end

  def image_add(id, img)
    tk_send('imageadd', id, img)
    self
  end

  def image_at(x, y)
    tk_send('imageat', x, y)
  end

  def images()
    list(tk_send('images'))
  end

  def image_set(id, num)
    tk_send('imageset', id, num)
    self
  end

  def image_update(id, imgs)
    tk_send('imageupdate', id, imgs)
    self
  end

  def index(idx, count=None, unit=None)
    tk_send('index', idx, count, unit)
  end

  def insert_cursor(idx)
    tk_send('insert', idx)
  end

  def names()
    simple_list(tk_send('names'))
  end

  def on_screen(id, x, y)
    bool(tk_send('onscreen', id, x, y))
  end

  def over(x, y)
    list(tk_send('over', x, y))
  end

  def over_markup(x, y)
    list(tk_send('over', x, y, '-muponly'))
  end

  def over_attr(x, y, attrs)
    list(tk_send('overattr', x, y, attrs))
  end

  def parse(txt)
    tk_send('parse', txt)
    self
  end

  def resolver(*uri_args)
    tk_send('resolver', *uri_args)
  end

  def selection_clear()
    tk_send('selection', 'clear')
    self
  end

  def selection_set(first, last)
    tk_send('selection', 'set', first, last)
    self
  end

  def refresh(*opts)
    tk_send('refresh', *opts)
  end

  def layout()
    tk_send('layout')
  end

  def sizewindow(*args)
    tk_send('sizewindow', *args)
  end

  def postscript(*args)
    tk_send('postscript', *args)
  end

  def source()
    tk_send('source')
  end

  def plain_text(first, last)
    tk_send('text', 'ascii', first, last)
  end
  alias ascii_text plain_text
  alias text_ascii plain_text

  def text_delete(first, last)
    tk_send('text', 'delete', first, last)
    self
  end

  def html_text(first, last)
    tk_send('text', 'html', first, last)
  end
  alias text_html html_text

  def text_insert(idx, txt)
    tk_send('text', 'insert', idx, txt)
    self
  end

  def break_text(idx)
    tk_send('text', 'break', idx)
  end
  alias text_break break_text

  def text_find(txt, *args)
    tk_send('text', 'find', txt, *args)
  end

  def text_table(idx, imgs=None, attrs=None)
    tk_send('text', 'table', idx, imgs, attrs)
  end

  def token_append(tag, *args)
    tk_send('token', 'append', tag, *args)
    self
  end

  def token_delete(first, last=None)
    tk_send('token', 'delete', first, last)
    self
  end

  def token_define(*args)
    tk_send('token', 'defile', *args)
    self
  end

  def token_find(tag, *args)
    list(tk_send('token', 'find', tag, *args))
  end

  def token_get(first, last=None)
    list(tk_send('token', 'get', first, last))
  end

  def token_list(first, last=None)
    list(tk_send('token', 'list', first, last))
  end

  def token_markup(first, last=None)
    list(tk_send('token', 'markup', first, last))
  end

  def token_DOM(first, last=None)
    list(tk_send('token', 'domtokens', first, last))
  end
  alias token_dom token_DOM
  alias token_domtokens token_DOM
  alias token_dom_tokens token_DOM

  def token_get_end(idx)
    tk_send('token', 'getend', idx)
  end
  alias token_getend token_get_end

  def token_offset(start, num1, num2)
    list(tk_send('token', 'offset', start, num1, num2))
  end

  def token_get_attr(idx, name=None)
    list(tk_send('token', 'attr', idx, name))
  end

  def token_set_attr(idx, name=None, val=None)
    tk_send('token', 'attr', idx, name, val)
    self
  end

  def token_handler(tag, cmd=nil, &b)
    cmd = Proc.new(&b) if !cmd && b
    if cmd
      tk_send('token', 'handler', tag, cmd)
      return self
    else
      return tk_send('token', 'handler', tag)
    end
  end

  def token_insert(idx, tag, *args)
    tk_send('token', 'insert', idx, tag, *args)
    self
  end

  def token_attrs(*args)
    list(tk_send('token', 'attrs', *args))
  end

  def token_unique(*args)
    list(tk_send('token', 'unique', *args))
  end

  def token_on_events(*args)
    list(tk_send('token', 'onEvents', *args))
  end

  def dom_nameidx(tag, name)
    number(tk_send('dom', 'nameidx', tag, name))
  end
  alias dom_name_index dom_nameidx

  def dom_radioidx(tag, name)
    number(tk_send('dom', 'radioidx', tag, name))
  end
  alias dom_radio_index dom_radioidx

  def dom_id(*spec)
    tk_send('dom', 'id', *spec)
  end

  def dom_ids(*spec)
    list(tk_send('dom', 'ids', *spec))
  end

  def dom_value(*spec)
    list(tk_send('dom', 'value', *spec))
  end

  def dom_attr(idx)
    tk_send('dom', 'attr', idx)
  end

  def dom_formel(name)
    tk_send('dom', 'formel', name)
  end
  alias dom_form_element dom_formel

  def dom_tree(idx, val)
    list(tk_send('dom', 'tree', idx, val))
  end
end
