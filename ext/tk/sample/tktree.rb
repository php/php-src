##########################################################################
# TkTree widget class
#
#    see <http://wiki.tcl.tk/10615>
#
#  Note:  optional argument '-font' of the Tcl library is changed to
#         'itemfont' on this Ruby library, because of avoiding font
#         operation trouble in 'initialize' method  ( see the following
#         test script ).
#
##########################################################################
require 'tk'

class TkTree < TkCanvas
  TCL_SCRIPT_PATH = File.join(File.dirname(__FILE__), 'tktree.tcl')

  def create_self(keys)
    args = [@path]
    if keys.kind_of?(Hash)
      font = keys.delete('itemfont')
#      font = hash_kv(font) if font.kind_of?(Hash)
      keys['font'] = font if font
#      args.concat(hash_kv(keys))
      args << keys
    end
    begin
      tk_call('::tktree::treecreate', *args)
    rescue NameError, RuntimeError
      Tk.load_tclscript(TkTree::TCL_SCRIPT_PATH)
      tk_call('::tktree::treecreate', *args)
    end
  end

  def newitem(itempath, keys = nil)
    if keys.kind_of?(Hash)
      keys = _symbolkey2str(keys)
      font = keys.delete('itemfont')
#      font = hash_kv(font) if font.kind_of?(Hash)
      keys['font'] = font if font
#      tk_call('::tktree::newitem', @path, itempath, *hash_kv(keys))
      tk_call('::tktree::newitem', @path, itempath, keys)
    else
      tk_call('::tktree::newitem', @path, itempath)
    end
  end

  def delitem(itempath)
    tk_call('::tktree::delitem', @path, itempath)
  end

  def labelat(xpos, ypos)
    tk_call('::tktree::delitem', @path, xpos, ypos)
  end

  def getselection
    tk_call('::tktree::getselection', @path)
  end

  def setselection(itempath)
    tk_call('::tktree::getselection', @path, itempath)
  end
end


##########################################################################
# test script
##########################################################################
if __FILE__ == $0
  TkLabel.new(:text=><<EOL, :relief=>:ridge, :justify=>:left).pack

 This is a sample to use a Tcl library script on Ruby/Tk.
 This sample loads tktree.tcl (see <http://wiki.tcl.tk/10615>)
 and calls functions of the Tcl script.
EOL

  items = %w(/group1/item1 /group1/item2 /group1/subgroup/item1 /group2/item1 /item1)

  tr1 = TkTree.new.pack(:expand=>true, :fill=>:both)
  tr1.focus

  items.each{|item|
    tr1.newitem(item,
                :command=>proc{Tk.messageBox(:message=>"#{item} executed")})
  }

  f = TkFrame.new.pack(:expand=>true, :fill=>:both)
  tr2 = TkTree.new(f, :bg=>'black', #:itemfont=>{:family=>'Times', :size=>14},
                   :textcolor=>'red', :bd=>4, :relief=>:ridge,
                   :selectbackground=>'darkBlue', :selectforeground=>'yellow',
                   :selectborderwidth=>3, :linecolor=>'yellow') {
    yscrollbar(TkScrollbar.new(f, :width=>10).pack(:side=>:right, :fill=>:y))
    xscrollbar(TkScrollbar.new(f, :width=>10).pack(:side=>:bottom, :fill=>:x))
    pack(:expand=>true, :fill=>:both)
  }

  items.each{|item|
    tr2.newitem(item, :textcolor=>'green', :image=>'',
                :itemfont=>{:family=>'Times', :size=>10},
                :command=>proc{Tk.messageBox(:message=>"#{item} executed")})
  }

  Tk.mainloop
end
