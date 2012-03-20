#
# tk/menubar.rb
#
# Original version:
#   Copyright (C) 1998 maeda shugo. All rights reserved.
#   This file can be distributed under the terms of the Ruby.

# Usage:
#
# menu_spec = [
#   [['File', 0],
#     ['Open', proc{puts('Open clicked')}, 0],
#     '---',
#     ['Quit', proc{exit}, 0]],
#   [['Edit', 0],
#     ['Cut', proc{puts('Cut clicked')}, 2],
#     ['Copy', proc{puts('Copy clicked')}, 0],
#     ['Paste', proc{puts('Paste clicked')}, 0]]
# ]
# menubar = TkMenubar.new(nil, menu_spec,
#                       'tearoff'=>false,
#                       'foreground'=>'grey40',
#                       'activeforeground'=>'red',
#                       'font'=>'-adobe-helvetica-bold-r-*--12-*-iso8859-1')
# menubar.pack('side'=>'top', 'fill'=>'x')
#
#
# OR
#
#
# menubar = TkMenubar.new
# menubar.add_menu([['File', 0],
#                  ['Open', proc{puts('Open clicked')}, 0],
#                  '---',
#                  ['Quit', proc{exit}, 0]])
# menubar.add_menu([['Edit', 0],
#                  ['Cut', proc{puts('Cut clicked')}, 2],
#                  ['Copy', proc{puts('Copy clicked')}, 0],
#                  ['Paste', proc{puts('Paste clicked')}, 0]])
# menubar.configure('tearoff', false)
# menubar.configure('foreground', 'grey40')
# menubar.configure('activeforeground', 'red')
# menubar.configure('font', '-adobe-helvetica-bold-r-*--12-*-iso8859-1')
# menubar.pack('side'=>'top', 'fill'=>'x')
#
#
# OR
#
# radio_var = TkVariable.new('y')
# menu_spec = [
#   [['File', 0],
#     {:label=>'Open', :command=>proc{puts('Open clicked')}, :underline=>0},
#     '---',
#     ['Check_A', TkVariable.new(true), 6],
#     {:type=>'checkbutton', :label=>'Check_B',
#                 :variable=>TkVariable.new, :underline=>6},
#     '---',
#     ['Radio_X', [radio_var, 'x'], 6],
#     ['Radio_Y', [radio_var, 'y'], 6],
#     ['Radio_Z', [radio_var, 'z'], 6],
#     '---',
#     ['cascade', [
#                    ['sss', proc{p 'sss'}, 0],
#                    ['ttt', proc{p 'ttt'}, 0],
#                    ['uuu', proc{p 'uuu'}, 0],
#                    ['vvv', proc{p 'vvv'}, 0],
#                 ], 0],
#     '---',
#     ['Quit', proc{exit}, 0]],
#   [['Edit', 0],
#     ['Cut', proc{puts('Cut clicked')}, 2],
#     ['Copy', proc{puts('Copy clicked')}, 0],
#     ['Paste', proc{puts('Paste clicked')}, 0]]
# ]
# menubar = TkMenubar.new(nil, menu_spec,
#                        'tearoff'=>false,
#                        'foreground'=>'grey40',
#                        'activeforeground'=>'red',
#                        'font'=>'Helvetia 12 bold')
# menubar.pack('side'=>'top', 'fill'=>'x')

# See tk/menuspce.rb about the format of the menu_spec

# To use add_menu, configuration must be done by calling configure after
# adding all menus by add_menu, not by the constructor arguments.

require 'tk'
require 'tk/frame'
require 'tk/composite'
require 'tk/menuspec'

class TkMenubar<Tk::Frame
  include TkComposite
  include TkMenuSpec

  def initialize(parent = nil, spec = nil, options = {})
    if parent.kind_of? Hash
      options = parent
      parent = nil
      spec = (options.has_key?('spec'))? options.delete('spec'): nil
    end

    _symbolkey2str(options)
    menuspec_opt = {}
    TkMenuSpec::MENUSPEC_OPTKEYS.each{|key|
      menuspec_opt[key] = options.delete(key) if options.has_key?(key)
    }

    super(parent, options)

    @menus = []

    spec.each{|info| add_menu(info, menuspec_opt)} if spec

    options.each{|key, value| configure(key, value)} if options
  end

  def add_menu(menu_info, menuspec_opt={})
    mbtn, menu = _create_menubutton(@frame, menu_info, menuspec_opt)

    submenus = _get_cascade_menus(menu).flatten

    @menus.push([mbtn, menu])
    delegate('tearoff', menu, *submenus)
    delegate('foreground', mbtn, menu, *submenus)
    delegate('background', mbtn, menu, *submenus)
    delegate('disabledforeground', mbtn, menu, *submenus)
    delegate('activeforeground', mbtn, menu, *submenus)
    delegate('activebackground', mbtn, menu, *submenus)
    delegate('font', mbtn, menu, *submenus)
    delegate('kanjifont', mbtn, menu, *submenus)
  end

  def [](index)
    return @menus[index]
  end
end
