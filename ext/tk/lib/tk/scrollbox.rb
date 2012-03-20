#
#               tk/scrollbox.rb - Tk Listbox with Scrollbar
#                                 as an example of Composite Widget
#                       by Yukihiro Matsumoto <matz@netlab.co.jp>
#
require 'tk'
require 'tk/listbox'

class TkScrollbox<Tk::Listbox
  include TkComposite
  def initialize_composite(keys=nil)
    #list = Tk::Listbox.new(@frame)
    # -> use current TkListbox class
    list = TkListbox.new(@frame)
    #scroll = Tk::Scrollbar.new(@frame)
    # -> use current TkScrollbar class
    scroll = TkScrollbar.new(@frame)
    @path = list.path

=begin
    list.configure 'yscroll', scroll.path+" set"
    list.pack 'side'=>'left','fill'=>'both','expand'=>'yes'
    scroll.configure 'command', list.path+" yview"
    scroll.pack 'side'=>'right','fill'=>'y'
=end
    list.yscrollbar(scroll)
    list.pack('side'=>'left','fill'=>'both','expand'=>'yes')
    scroll.pack('side'=>'right','fill'=>'y')

    delegate('DEFAULT', list)
    delegate('foreground', list)
    delegate('background', list, scroll)
    delegate('borderwidth', @frame)
    delegate('relief', @frame)

    configure keys if keys
  end
  private :initialize_composite
end
