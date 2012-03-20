#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

helpvar = TkVariable.new
viewmode = TkVariable.new

menu_spec = [
  [:menubutton, 'file', {
      :text=>'File', :menu=>[
        [:options, {:tearoff=>false}],

        [:command, 'new', {
            :label=>'New', :helpstr=>'Open new document',
            :command=>proc{puts 'NEW'}
          }
        ],

        [:command, 'close', {
            :label=>'Close', :helpstr=>'Close current document',
            :command=>proc{puts 'CLOSE'}
          }
        ],

        [:separator, 'sep1'],

        [:command, 'exit', {
            :label=>'Exit', :helpstr=>'Exit application',
            :command=>proc{exit}
          }
        ]
      ]
    }
  ],

  [:menubutton, 'edit', {
      :text=>'Edit', :menu=>[
        [:options, {:tearoff=>false}],

        [:command, 'undo', {
            :label=>'Undo', :underline=>0,
            :helpstr=>'Undo last command',
            :command=>proc{puts 'UNDO'}
          }
        ],

        [:separator, 'sep2'],

        [:command, 'cut', {
            :label=>'Cut', :underline=>1,
            :helpstr=>'Cut selection to clipboard',
            :command=>proc{puts 'CUT'}
          }
        ],

        [:command, 'copy', {
            :label=>'Copy', :underline=>1,
            :helpstr=>'Copy selection to clipboard',
            :command=>proc{puts 'COPY'}
          }
        ],

        [:command, 'paste', {
            :label=>'Paste', :underline=>0,
            :helpstr=>'Paste clipboard contents',
            :command=>proc{puts 'PASTE'}
          }
        ]
      ]
    }
  ],

  [:menubutton, 'options', {
      :text=>'Options', :menu=>[
        [:options, {:tearoff=>false, :selectcolor=>'blue'}],

        [:radiobutton, 'byName', {
            :variable=>viewmode, :value=>'NAME',
            :label=>'by Name', :helpstr=>'View files by name order',
            :command=>proc{puts 'NAME'}
          }
        ],

        [:radiobutton, 'byDate', {
            :variable=>viewmode, :value=>'DATE',
            :label=>'by Date', :helpstr=>'View files by date order',
            :command=>proc{puts 'DATE'}
          }
        ],

        [:cascade, 'prefs', {
            :label=>'Preferences', :menu=>[
              [:command, 'colors', {
                  :label=>'Colors...', :helpstr=>'Change text colors',
                  :command=>proc{puts 'COLORS'}
                }
              ],

              [:command, 'fonts', {
                  :label=>'Fonts...', :helpstr=>'Change text font',
                  :command=>proc{puts 'COLORS'}
                }
              ]
            ]
          }
        ]
      ]
    }
  ]
]

#mb = Tk::Iwidgets::Menubar.new(:helpvariable=>helpvar,
#                              :menubuttons=>menu_spec)
mb = Tk::Iwidgets::Menubar.new(:helpvariable=>helpvar)
mb.configure(:menubuttons=>menu_spec)

fr = TkFrame.new(:width=>300, :height=>300)
ef = TkEntry.new(:textvariable=>helpvar)

mb.pack(:anchor=>:nw, :fill=>:x, :expand=>true)
fr.pack(:fill=>:both, :expand=>true)
ef.pack(:anchor=>:sw, :fill=>:x, :expand=>true)

Tk.mainloop
