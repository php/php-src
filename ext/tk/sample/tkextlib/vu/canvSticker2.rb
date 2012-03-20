#!/usr/bin/env ruby

require 'tk'
require 'tkextlib/vu/charts'

#######################################

c = TkCanvas.new.pack

begin
  st = Tk::Vu::TkcSticker.new(c, 0, 0, 10, 10)
rescue
  Tk.messageBox(:type=>'ok', :title=>"No sticker Item",
                :message=>"This build of vu does not include the sticker item")
  exit
end

c.destroy

#---
#--- set STRING {{x0 y0 x1 y1} {...text...} {resize point: center}

#sti_conf = [ [10, 10, 180, 180], "Sticker äöüß@²³¼½¾",  :center ]
#txt_conf = [ [210, 210],        "Text    äöüß@²³¼½¾",  :center ]
sti_conf = [ [10, 10, 350, 350],
             Tk::UTF8_String('Sticker \u00E4\u00F6\u00FC\u00DF\u0040\u00B2\u00B3\u00BC\u00BD\u00BE'),
             :center ]
txt_conf = [ [250, 250],
             Tk::UTF8_String('Text    \u00E4\u00F6\u00FC\u00DF\u0040\u00B2\u00B3\u00BC\u00BD\u00BE'),
             :center ]

#p sti_conf

fnt = TkFont.new('Helvetica 24 bold')

#---GUI
c = TkCanvas.new(:width=>500, :height=>500, :bg=>'aquamarine3').pack

#---CRRW Use the technique of eval the coord ...
sti = Tk::Vu::TkcSticker.new(c, sti_conf[0]){
  anchor    sti_conf[2]
  bar       'black'
  color     'red'
  fill      ''
  font      fnt
  lefttrunc 0
  outline   ''
  relheight 0.0
  relwidth  0.0
  relx      0.0
  rely      0.0
  space     0
  stipple   ''
  tags      'sti'
  text      sti_conf[1]
  width     0
  orient    :vertical
  minwidth  0
  minheight 0
  maxwidth  32767
  maxheight 32767
}

txt = TkcText.new(c, txt_conf[0]){
  activefill      ''
  activestipple   ''
  anchor          txt_conf[2]
  disabledfill    ''
  disabledstipple ''
  fill            'blue'
  font            fnt
  justify         :left
  offset          '0,0'
  state           ''
  stipple         ''
  tags            ['tex']
  text            txt_conf[1]
  width           0
}

#---BINDINGS
c.bind('2', proc{
         sti[:orient] = :horizontal
         txt[:width] = 0  # horizontal
       })

c.bind('3', proc{
         sti[:orient] = :vertical
         txt[:width] = 1  # top down
       })

Tk.root.bind('p', proc{
               c.postscript(:file=>'DEMO.ps')
               puts "DEMO.ps printed"
             })

Tk.root.bind('q', proc{exit})

#####################

Tk.mainloop
