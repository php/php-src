#!/usr/bin/env ruby

require 'tk'
require 'tkextlib/vu/charts'

#######################################

xbm = File.join(File.dirname(File.expand_path(__FILE__)), 'm128_000.xbm')

sval = [ 11, 22, 33, 44, 55, 66, 77, 88, 99 ]

l0 = TkLabel.new(:width=>128, :height=>128,
                 :bitmap=>"@#{xbm}", :relief=>:groove).pack(:side=>:left)

c0 = TkCanvas.new(:width=>80, :height=>80,
                  :insertwidth=>0, :highlightthickness=>0,
                  :selectborderwidth=>0, :borderwidth=>2,
                  :relief=>:ridge).place(:in=>l0, :relx=>0.5, :rely=>0.5,
                                         :anchor=>:c)

st = Tk::Vu::TkcStripchart.new(c0, 3, 3, 80, 80,
                               :background=>"#b7c0d7", :fill=>'slategray3',
                               :jumpscroll=>1, :outline=>'black',
                               :scaleline=>'blue', :stripline=>'red',
                               :selected=>1, :values=>sval)

TkcText.create(c0, 40, 40,
               :text=>Tk::TCL_PATCHLEVEL, :fill=>'cyan', :tags=>'text')


l1 = TkLabel.new(:width=>128, :height=>128,
                 :bitmap=>"@#{xbm}", :relief=>:groove).pack(:side=>:left)

c1 = TkCanvas.new(:width=>80, :height=>80,
                  :insertwidth=>0, :highlightthickness=>0,
                  :selectborderwidth=>0, :borderwidth=>2,
                  :relief=>:ridge).place(:in=>l1, :relx=>0.5, :rely=>0.5,
                                         :anchor=>:c)

bar1 = Tk::Vu::TkcBarchart.new(c1, 3, 3, 80, 80,
                               :background=>"#b7c0d7", :scalevalue=>10.0,
                               :autocolor=>true, :selected=>1,
                               :outline=>'black', :barline=>'yellow',
                               :scalelinestyle=>0)

bar2 = Tk::Vu::TkcBarchart.new(c1, 53, 3, 80, 80,
                               :background=>"#b7c0d7", :scalevalue=>10.0,
                               :autocolor=>true, :selected=>1,
                               :outline=>'black', :fill=>"#b7c0d7",
                               :barline=>'red', :scalelinestyle=>22)


l2 = TkLabel.new(:width=>128, :height=>128,
                 :bitmap=>"@#{xbm}", :relief=>:groove).pack(:side=>:left)

c2 = TkCanvas.new(:width=>80, :height=>80,
                  :insertwidth=>0, :highlightthickness=>0,
                  :selectborderwidth=>0, :borderwidth=>2,
                  :relief=>:ridge).place(:in=>l2, :relx=>0.5, :rely=>0.5,
                                         :anchor=>:c)

begin
  stick = Tk::Vu::TkcSticker.new(c2, 3, 3, 80, 80,
                                 :text=>"Tcl/Tk", :space=>0, :color=>'red',
                                 :outline=>'red', :font=>'Helvetica 14 bold',
                                 :fill=>'', :stipple=>'', :bar=>'blue',
                                 :orient=>:vertical, :anchor=>:s,
                                 :relheight=>1.0, :relwidth=>0.15,
                                 :relx=>0.1, :rely=>0.0)
rescue
  stick = nil
  TkcText.new(c2, 40, 10, :text=>"No Sticker Item")
end

c_do = proc{
  st[:values]   = rand() * 100

  bar1[:values] = rand() * 10
  bar2[:values] = rand() * 10

  stick[:rely]  = rand() * 100 * 0.01 if stick
}

c0.bind('1', c_do)

TkTimer.new(100, -1, c_do).start

Tk.root.bind('q', proc{exit})

Tk.mainloop
