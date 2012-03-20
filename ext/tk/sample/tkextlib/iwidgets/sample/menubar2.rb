#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

helpvar = TkVariable.new
viewmode = TkVariable.new

mb = Tk::Iwidgets::Menubar.new
mb.menubuttons = [
  [:menubutton, 'file', {
      :text=>'File', :menu=>[
        [:command,   'new',   {:label=>'New'}],
        [:command,   'close', {:label=>'Close'}],
        [:separator, 'sep1'],
        [:command,   'quit',  {:label=>'Quit'}]
      ]
    }
  ],
  [:menubutton, 'edit', {:text=>'Edit'}]
]

mb.add(:command, '.edit.undo', :label=>'Undo', :underline=>0)
mb.add(:separator, '.edit.sep2')
mb.add(:command, '.edit.cut',   :label=>'Cut',   :underline=>1)
mb.add(:command, '.edit.copy',  :label=>'Copy',  :underline=>1)
mb.add(:command, '.edit.paste', :label=>'Paste', :underline=>0)

mb.add(:menubutton, '.options', :text=>'Options', :menu=>[
         [:radiobutton, 'byName', {
             :variable=>viewmode, :value=>'NAME', :label=>'by Name'}
         ],
         [:radiobutton, 'byDate', {
             :variable=>viewmode, :value=>'DATE', :label=>'by Date'}
         ]
       ])

mb.add(:cascade, '.options.prefs', :label=>'Preferences', :menu=>[
         [:command, 'colors', {:label=>'Colors...'}],
         [:command, 'fonts',  {:label=>'Fonts...'}]
       ])

mb.pack(:side=>:left, :anchor=>:nw, :fill=>:x, :expand=>true)

Tk.mainloop
