# ttknote.rb --
#
# This demonstration script creates a toplevel window containing a Ttk
# notebook widget.
#
# based on "Id: ttknote.tcl,v 1.5 2007/12/13 15:27:07 dgp Exp"

if defined?($ttknote_demo) && $ttknote_demo
  $ttknote_demo.destroy
  $ttknote_demo = nil
end

$ttknote_demo = TkToplevel.new {|w|
  title("Ttk Notebook Widget")
  iconname("ttknote")
  positionWindow(w)
}

## See Code / Dismiss
Ttk::Frame.new($ttknote_demo) {|frame|
  sep = Ttk::Separator.new(frame)
  Tk.grid(sep, :columnspan=>4, :row=>0, :sticky=>'ew', :pady=>2)
  TkGrid('x',
         Ttk::Button.new(frame, :text=>'See Code',
                         :image=>$image['view'], :compound=>:left,
                         :command=>proc{showCode 'ttknote'}),
         Ttk::Button.new(frame, :text=>'Dismiss',
                         :image=>$image['delete'], :compound=>:left,
                         :command=>proc{
                           $ttknote_demo.destroy
                           $ttknote_demo = nil
                         }),
         :padx=>4, :pady=>4)
  grid_columnconfigure(0, :weight=>1)
  pack(:side=>:bottom, :fill=>:x)
}

base_frame = Ttk::Frame.new($ttknote_demo).pack(:fill=>:both, :expand=>true)

## Make the notebook and set up Ctrl+Tab traversal
notebook = Ttk::Notebook.new(base_frame).pack(:fill=>:both, :expand=>true,
                                              :padx=>2, :pady=>3)
notebook.enable_traversal

## Popuplate the first pane
f_msg = Ttk::Frame.new(notebook)
msg_m = Ttk::Label.new(f_msg, :font=>$font, :wraplength=>'4i',
                       :justify=>:left, :anchor=>'n', :text=><<EOL)
Ttk is the new Tk themed widget set. \
One of the widgets it includes is the notebook widget, \
which provides a set of tabs that allow the selection of a group of panels, \
each with distinct content. \
They are a feature of many modern user interfaces. \
Not only can the tabs be selected with the mouse, \
but they can also be switched between using Ctrl+Tab \
when the notebook page heading itself is selected. \
Note that the second tab is disabled, and cannot be selected.
EOL
neat = TkVariable.new
after_id = nil
msg_b = Ttk::Button.new(f_msg, :text=>'Neat!', :underline=>0,
                        :command=>proc{
                          neat.value = 'Yeah, I know...'
                          Tk.after_cancel(after_id) if after_id
                          after_id = Tk.after(500){neat.value = ''}
                        })
msg_b.winfo_toplevel.bind('Alt-n'){ msg_b.focus; msg_b.invoke }
msg_l = Ttk::Label.new(f_msg, :textvariable=>neat)
notebook.add(f_msg, :text=>'Description', :underline=>0, :padding=>2)
Tk.grid(msg_m, '-', :sticky=>'new', :pady=>2)
Tk.grid(msg_b, msg_l, :pady=>[2, 4])
f_msg.grid_rowconfigure(1, :weight=>1)
f_msg.grid_columnconfigure([0, 1], :weight=>1, :uniform=>1)

## Populate the second pane. Note that the content doesn't really matter
f_disabled = Ttk::Frame.new(notebook)
notebook.add(f_disabled, :text=>'Disabled', :state=>:disabled)

## Popuplate the third pane
f_editor = Ttk::Frame.new(notebook)
notebook.add(f_editor, :text=>'Text Editor', :underline=>0)
editor_t = Tk::Text.new(f_editor, :width=>40, :height=>10, :wrap=>:char)
if Tk.windowingsystem != 'aqua'
  editor_s = editor_t.yscrollbar(Ttk::Scrollbar.new(f_editor))
else
  editor_s = editor_t.yscrollbar(Tk::Scrollbar.new(f_editor))
end
editor_s.pack(:side=>:right, :fill=>:y, :padx=>[0,2], :pady=>2)
editor_t.pack(:fill=>:both, :expand=>true, :padx=>[2,0], :pady=>2)
