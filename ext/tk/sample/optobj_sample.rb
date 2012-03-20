#!/usr/bin/env ruby
#
#  sample script of Tk::OptionObj
#
require "tk"

optobj = Tk::OptionObj.new('foreground'=>'red', 'background'=>'black')

f = TkFrame.new.pack(:side=>:left, :anchor=>:n, :padx=>5, :pady=>30)

b1 = TkButton.new(f, :text=>'AAA').pack(:fill=>:x)
b2 = TkButton.new(f, :text=>'BBB').pack(:fill=>:x)
b3 = TkButton.new(f, :text=>'CCC').pack(:fill=>:x)

optobj.assign( b1,
              [ b2, 'configure',
                { 'foreground'=>'background',
                  'background'=>'foreground' } ],
              [ b3, nil,
                { 'foreground'=>'background',
                  'activeforeground'=>nil,
                  'background'=>['foreground', 'activeforeground'] } ] )

optobj.update('activeforeground'=>'yellow')

TkButton.new(f){
  configure( optobj.assign(self) + {:text=>'DDD'} )
  pack(:fill=>:x)
}

TkButton.new(f){
  configure( optobj.assign([self, nil,
                             {'foreground'=>'activeforeground',
                              'background'=>'foreground',
                              'activeforeground'=>'background'}]) \
             + {:text=>'EEE', :relief=>:groove, :borderwidth=>5} )
  pack(:fill=>:x)
}

optobj.notify  # To apply the convert_key ( 3rd element of widget info
               # (that is, {'foreground'=>'activeforeground', ,,, } )
               # of the 'EEE' button

TkButton.new(f, :text=>'toggle',
             :command=>proc{
               fg = optobj['foreground']
               bg = optobj['background']
               optobj.configure('foreground'=>bg, 'background'=>fg)
             }).pack(:fill=>:x, :pady=>10)

TkButton.new(f, :text=>'exit',
                :command=>proc{exit}).pack(:fill=>:x, :pady=>10)

TkFrame.new{|f|
  pack(:side=>:right, :expand=>true, :fill=>:both)
  TkLabel.new(f, :text=>'source::').pack(:anchor=>:w)
  TkFrame.new(f){|ff|
    TkText.new(ff){
      yscrollbar(TkScrollbar.new(ff){pack(:fill=>:y, :side=>:right)})
      insert('end', File.read(__FILE__))
      pack(:side=>:left, :expand=>true, :fill=>:both)
    }
    pack(:expand=>true, :fill=>:both)
  }
}

Tk.mainloop
