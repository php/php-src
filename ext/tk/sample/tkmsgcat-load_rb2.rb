#!/usr/bin/env ruby

require 'tk'

demo_dir = File.dirname($0)
msgcat_dir = [demo_dir, 'msgs_rb2'].join(File::Separator)
top_win = nil
#msgcat = TkMsgCatalog.new('::tk')
msgcat = TkMsgCatalog.new('::tkmsgcat_demo')
default_locale = msgcat.locale
#msgcat.load_rb(msgcat_dir)
msgcat.load(msgcat_dir)

col_proc = TkComm.install_bind(proc{|w, color, frame, label|
                                 TkComm.window(frame).background(color)
                                 Tk.update
                                 TkComm.window(label).text(
                                          msgcat["%1$s:: %2$s", 'Color',
                                                 color.capitalize])
                                 w.flash; w.flash
                                 Tk.callback_break;
                              }, "%W")

del_proc = TkComm.install_cmd(proc{top_win.destroy; top_win = nil})

err_proc = TkComm.install_cmd(proc{fail(RuntimeError,
                                        msgcat['Application Error'])})

show_sample = proc{|loc|
  top_win = TkToplevel.new(:title=>loc)

  msgcat.locale = loc
  #msgcat.load_rb(msgcat_dir)
  msgcat.load(msgcat_dir)

  TkLabel.new(top_win){
    text "preferences:: #{msgcat.preferences.join(' ')}"
    pack(:pady=>10, :padx=>10)
  }

  lbl = TkLabel.new(top_win, :text=>msgcat["%1$s:: %2$s",
                                           'Color', '']).pack(:anchor=>'w')

  bg = TkFrame.new(top_win).pack(:ipadx=>20, :ipady=>10,
                                 :expand=>true, :fill=>:both)

  TkFrame.new(bg){|f|
    ['blue', 'green', 'red'].each{|col|
      TkButton.new(f, :text=>msgcat[col]){
        bind('ButtonRelease-1', col_proc, "#{col} #{bg.path} #{lbl.path}")
      }.pack(:fill=>:x)
    }
  }.pack(:anchor=>'center', :pady=>15)

  TkFrame.new(top_win){|f|
    TkButton.new(f, :text=>msgcat['Delete'],
                 :command=>del_proc).pack(:side=>:right, :padx=>5)
    TkButton.new(f, :text=>msgcat['Error'],
                 :command=>err_proc).pack(:side=>:left, :padx=>5)
  }.pack(:side=>:bottom, :fill=>:x)

  top_win
}


#  listbox for locale list
TkLabel.new(:text=>"Please click a locale.").pack(:padx=>5, :pady=>3)

TkFrame.new{|f|
  TkButton.new(f, :text=>msgcat['Exit'],
               :command=>proc{exit}).pack(:side=>:right, :padx=>5)
}.pack(:side=>:bottom, :fill=>:x)

f = TkFrame.new.pack(:side=>:top, :fill=>:both, :expand=>true)
lbox = TkListbox.new(f).pack(:side=>:left, :fill=>:both, :expand=>true)
lbox.yscrollbar(TkScrollbar.new(f, :width=>12).pack(:side=>:right, :fill=>:y))

lbox.bind('ButtonRelease-1'){|ev|
  idx = lbox.index("@#{ev.x},#{ev.y}")
  if idx == 0
    loc = default_locale
  else
    loc = lbox.get(idx)
  end
  if top_win != nil && top_win.exist?
    top_win.destroy
  end
  top_win = show_sample.call(loc)
}

lbox.insert('end', 'default')

Dir.entries(msgcat_dir).sort.each{|f|
  if f =~ /^(.*).msg$/
    lbox.insert('end', $1)
  end
}

top_win = show_sample.call(default_locale)

#  start eventloop
Tk.mainloop
