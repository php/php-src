#!/usr/bin/env ruby

require 'tk'
require 'tkextlib/blt'

require 'date'

dir = File.join(File.dirname(File.expand_path(__FILE__)), 'images')
file = File.join(dir, 'chalk.gif')
active = File.join(dir, 'rain.gif')

texture1 = TkPhotoImage.new(:file=>file)
texture2 = TkPhotoImage.new(:file=>active)

TkOption.add('*Tile', texture1)

TkOption.add('*HighlightThickness', 0)
TkOption.add('*calendar.weekframe*Tile', texture2)
TkOption.add('*Calendar.Label.borderWidth', 0)
TkOption.add('*Calendar.Label.relief', :sunken)
TkOption.add('*Calendar.Frame.borderWidth', 2)
TkOption.add('*Calendar.Frame.relief', :raised)
TkOption.add('*Calendar.Label.font', 'Helvetica 11')
TkOption.add('*Calendar.Label.foreground', 'navyblue')
TkOption.add('*button.foreground', 'navyblue')
TkOption.add('*background', 'grey85')
TkOption.add('*Label.ipadX', 200)

TkOption.add('*tile', texture2)

class BLT_Calendar_sample
  @@monthInfo = [
    nil,  # dummy
    ['January', 31],
    ['February', 28],
    ['March', 31],
    ['April', 30],
    ['May', 31],
    ['June', 30],
    ['July', 31],
    ['August', 31],
    ['Septembar', 30],
    ['October', 31],
    ['November', 30],
    ['December', 31]
  ]

  @@abbrDays = [ 'Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat' ]

  def initialize()
    today = Date.today

    if TkComm.bool(Tk.info(:commands, '.calendar'))
      Tk.destroy('.calendar')
    end
    cal = Tk::BLT::Tile::Frame.new(:widgetname=>'.calendar',
                                   :classname=>'Calendar',
                                   :width=>'3i', :height=>'3i')

    mon = Tk::BLT::Tile::Label.new(cal, :font=>'Courier 14 bold',
                                   :text=>"#{@@monthInfo[today.month][0]} " +
                                          "#{today.year}")
    Tk::BLT::Table.add(cal, mon, [1, 0], :cspan=>7, :pady=>10)

    week_f = Tk::BLT::Tile::Frame.new(cal, :widgetname=>'weekframe',
                                      :relief=>:sunken, :borderwidth=>1)
    Tk::BLT::Table.add(cal, week_f, [2, 0], :columnspan=>7, :fill=>:both)

    @@abbrDays.each_with_index{|dayName, idx|
      Tk::BLT::Table.add(cal,
                         Tk::BLT::Tile::Label.new(cal, :text=>dayName,
                                                  :font=>'Helvetica 12'),
                         [2, idx], :pady=>2, :padx=>2)
    }

    Tk::BLT::Table.itemconfigure(cal, 'c*', 'r2', :pad=>4)

    numDays = @@monthInfo[today.month][1]
    week = 0
    cnt = 1

    wkday = today.wday - ((today.day - 1) % 7)
    wkday += 7 if wkday < 0

    while cnt <= numDays
      Tk::BLT::Table.add(cal,
                         Tk::BLT::Tile::Label.new(cal, :text=>cnt){
                           self.configure(:borderwidth=>1,
                                          :relief=>:sunken) if cnt == today.day
                         },
                         [week+3, wkday], :fill=>:both, :ipadx=>10, :ipady=>4)
      cnt += 1
      wkday += 1
      if wkday == 7
        week += 1
        wkday = 0
      end
    end

    Tk::BLT::Tile::Frame.new(cal, :borderwidth=>1, :relief=>:sunken){|f|
      Tk::BLT::Table.add(f,
                         Tk::BLT::Tile::Button.new(f, :widgetname=>'button',
                                                   :command=>proc{exit},
                                                   :borderwidth=>2,
                                                   :text=>'Quit'),
                         :padx=>4, :pady=>4)
      Tk::BLT::Table.add(cal, f, [week+4, 5], :cspan=>2, :pady=>4)
    }

    Tk::BLT::Table.add(Tk.root, cal, :fill=>:both)
    Tk::BLT::Table.itemconfigure(cal, 'r0', :resize=>:none)
  end
end

BLT_Calendar_sample.new

Tk.mainloop
