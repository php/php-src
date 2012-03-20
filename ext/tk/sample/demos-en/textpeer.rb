#
# text widget peering demo (called by 'widget')
#
# based on Tcl/Tk8.5.0 widget demos

if defined?($textpeer_demo) && $textpeer_demo
  $textpeer_demo.destroy
  $textpeer_demo = nil
end

# demo toplevel widget
$textpeer_demo = TkToplevel.new {|w|
  title("Text Wdget Peering Demonstration")
  iconname("textpeer")
  positionWindow(w)
}

base_frame = TkFrame.new($textpeer_demo).pack(:fill=>:both, :expand=>true)

count = [0]

## Define a widget that we peer from; it won't ever actually be shown though
first = TkText.new(base_frame, :widgetname=>"text#{count[0] += 1}")
first.insert :end,"This is a coupled pair of text widgets; they are peers to "
first.insert :end,"each other. They have the same underlying data model, but "
first.insert :end,"can show different locations, have different current edit "
first.insert :end,"locations, and have different selections. You can also "
first.insert :end,"create additional peers of any of these text widgets using "
first.insert :end,"the Make Peer button beside the text widget to clone, and "
first.insert :end,"delete a particular peer widget using the Delete Peer "
first.insert :end,"button."

Tk.update_idletasks  ## for 'first' widget

## Procedures to make and kill clones; most of this is just so that the demo
## looks nice...
def makeClone(count, win, txt)
  cnt = (count[0] += 1)
  peer = TkText::Peer.new(txt, win, :widgetname=>"text#{cnt}")
  sbar = TkScrollbar.new(win, :widgetname=>"sb#{cnt}")
  peer.yscrollbar sbar
  b1 = TkButton.new(win, :widgetname=>"clone#{cnt}", :text=>'Make Peer',
                    :command=>proc{makeClone(count, win, peer)})
  b2 = TkButton.new(win, :widgetname=>"kill#{cnt}", :text=>'Delete Peer',
                    :command=>proc{killClone(win, cnt)})
  row = cnt * 2
  TkGrid.configure(peer, sbar, b1, :sticky=>'nsew', :row=>row)
  TkGrid.configure('^',  '^',  b2, :sticky=>'nsew', :row=>(row+=1))
  TkGrid.configure(b1,  b2, :sticky=>'new')
  TkGrid.rowconfigure(win,  b2, :weight=>1)
end

def killClone(win, cnt)
  Tk.destroy("#{win.path}.text#{cnt}",  "#{win.path}.sb#{cnt}",
             "#{win.path}.clone#{cnt}", "#{win.path}.kill#{cnt}")
end

## Now set up the GUI
makeClone(count, base_frame, first)
makeClone(count, base_frame, first)
first.destroy

## See Code / Dismiss buttons
TkFrame.new(base_frame){|f|
  TkButton.new(f, :text=>'Dismiss', :width=>15, :command=>proc{
                 $textpeer_demo.destroy
                 $textpeer_demo = nil
               }).pack(:side=>:left, :expand=>true)

  TkButton.new(f, :text=>'See Code', :width=>15, :command=>proc{
                 showCode 'textpeer'
               }).pack(:side=>:left, :expand=>true)

  TkGrid.configure(f, '-', '-', :sticky=>'ew', :row=>5000)
}
TkGrid.columnconfigure(base_frame, 0, :weight=>1)
