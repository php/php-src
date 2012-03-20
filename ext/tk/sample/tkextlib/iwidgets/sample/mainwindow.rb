#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

mw = Tk::Iwidgets::Mainwindow.new

mw.menubar.add(:menubutton, 'file', :text=>'File', :underline=>0,
               :padx=>8, :pady=>2, :menu=>[
                 [:options, {:tearoff=>false}],

                 [:command, 'new', {
                     :label=>'New', :underline=>0,
                     :helpstr=>'Create a new file'
                   }
                 ],

                 [:command, 'open', {
                     :label=>'Open ...', :underline=>0,
                     :helpstr=>'Open an existing file'
                   }
                 ],

                 [:command, 'save', {
                     :label=>'Save', :underline=>0,
                     :helpstr=>'Save the current file'
                   }
                 ],

                 [:command, 'saveas', {
                     :label=>'Save As', :underline=>5,
                     :helpstr=>'Save the file as a different name'
                   }
                 ],

                 [:command, 'print', {
                     :label=>'Print', :underline=>0,
                     :helpstr=>'Print the file'
                   }
                 ],

                 [:separator, 'sep1'],

                 [:command, 'close', {
                     :label=>'Close', :underline=>0,
                     :helpstr=>'Close the file'
                   }
                 ],

                 [:separator, 'sep2'],

                 [:command, 'exit', {
                     :label=>'Exit', :underline=>1,
                     :helpstr=>'Exit this application'
                   }
                 ],

                 nil
               ])

Tk::Iwidgets::Scrolledtext.new(mw.child_site).pack(:fill=>:both, :expand=>true)

mw.activate

Tk.mainloop
