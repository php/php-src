#!/usr/bin/env ruby
#
#  menubutton sample : based on sample menubuttons on the Tcl/Tk demo script
#
require 'tk'

TkLabel.new(:text=>'Sample of TkMenubutton').pack(:side=>:top)

TkFrame.new{|f|
  pack(:side=>:top)


  TkMenubutton.new(:parent=>f, :text=>'Right', :underline=>0,
                   :direction=>:right, :relief=>:raised){|mb|
    menu TkMenu.new(:parent=>mb, :tearoff=>0){
      add(:command, :label=>'Right menu: first item',
                    :command=>proc{print 'You have selected the first item' +
                                         " from the Right menu.\n"})
      add(:command, :label=>'Right menu: second item',
                    :command=>proc{print 'You have selected the second item' +
                                         " from the Right menu.\n"})
    }
    pack(:side=>:left, :padx=>25, :pady=>25)
  }

  TkMenubutton.new(:parent=>f, :text=>'Below', :underline=>0,
                   :direction=>:below, :relief=>:raised){|mb|
    menu(TkMenu.new(:parent=>mb, :tearoff=>0){
      add(:command, :label=>'Below menu: first item',
                    :command=>proc{print 'You have selected the first item' +
                                         " from the Below menu.\n"})
      add(:command, :label=>'Below menu: second item',
                    :command=>proc{print 'You have selected the second item' +
                                         " from the Below menu.\n"})
    })
    pack(:side=>:left, :padx=>25, :pady=>25)
  }

  TkMenubutton.new(:parent=>f, :text=>'Above', :underline=>0,
                   :direction=>:above, :relief=>:raised){|mb|
    menu TkMenu.new(:parent=>mb, :tearoff=>0){
      add(:command, :label=>'Above menu: first item',
                    :command=>proc{print 'You have selected the first item' +
                                         " from the Above menu.\n"})
      add(:command, :label=>'Above menu: second item',
                    :command=>proc{print 'You have selected the second item' +
                                         " from the Above menu.\n"})
    }
    pack(:side=>:left, :padx=>25, :pady=>25)
  }

  TkMenubutton.new(:parent=>f, :text=>'Left', :underline=>0,
                   :direction=>:left, :relief=>:raised){|mb|
    menu(TkMenu.new(:parent=>mb, :tearoff=>0){
      add(:command, :label=>'Left menu: first item',
                    :command=>proc{print 'You have selected the first item' +
                                         " from the Left menu.\n"})
      add(:command, :label=>'Left menu: second item',
                    :command=>proc{print 'You have selected the second item' +
                                         " from the Left menu.\n"})
    })
    pack(:side=>:left, :padx=>25, :pady=>25)
  }
}

############################
TkFrame.new(:borderwidth=>2, :relief=>:sunken,
            :height=>5).pack(:side=>:top, :fill=>:x, :padx=>20)
############################

TkLabel.new(:text=>'Sample of TkOptionMenu').pack(:side=>:top)

colors = %w(Black red4 DarkGreen NavyBlue gray75 Red Green Blue gray50
            Yellow Cyan Magenta White Brown DarkSeaGreen DarkViolet)

TkFrame.new{|f|
  pack(:side=>:top)

  b1 = TkOptionMenubutton .
    new(:parent=>f, :values=>%w(one two three)) .
    pack(:side=>:left, :padx=>25, :pady=>25)

  b2 = TkOptionMenubutton.new(:parent=>f, :values=>colors) {|optMB|
    colors.each{|color|
      no_sel = TkPhotoImage.new(:height=>16, :width=>16){
        put 'gray50', *[ 0,  0, 16,  1]
        put 'gray50', *[ 0,  1,  1, 16]
        put 'gray75', *[ 0, 15, 16, 16]
        put 'gray75', *[15,  1, 16, 16]
        put color,    *[ 1,  1, 15, 15]
      }
      sel = TkPhotoImage.new(:height=>16, :width=>16){
        put 'Black',  *[ 0,  0, 16,  2]
        put 'Black',  *[ 0,  2,  2, 16]
        put 'Black',  *[ 2, 14, 16, 16]
        put 'Black',  *[14,  2, 16, 14]
        put color,    *[ 2,  2, 14, 14]
      }
      optMB.entryconfigure(color, :hidemargin=>1,
                           :image=>no_sel, :selectimage=>sel)
    }
    optMB.menuconfigure(:tearoff, 1)
    %w(Black gray75 gray50 White).each{|color|
      optMB.entryconfigure(color, :columnbreak=>true)
    }
    pack(:side=>:left, :padx=>25, :pady=>25)
  }

  TkButton.new(:parent=>f){
    text 'show values'
    command proc{p [b1.value, b2.value]}
    pack(:side=>:left, :padx=>25, :pady=>5, :anchor=>:s)
  }
}

############################
TkFrame.new(:borderwidth=>2, :relief=>:sunken,
            :height=>5).pack(:side=>:top, :fill=>:x, :padx=>20)
############################

root = TkRoot.new(:title=>'menubutton samples')

TkButton.new(root, :text=>'exit', :command=>proc{exit}){
  pack(:side=>:top, :padx=>25, :pady=>5, :anchor=>:e)
}

# VirtualEvent <<MenuSelect>> on Tcl/Tk ==> '<MenuSelect>' on Ruby/Tk
# ( remove the most external <, > for Ruby/Tk notation )
TkMenu.bind('<MenuSelect>', proc{|widget|
              p widget.entrycget('active', :label)
            }, '%W')

############################

Tk.mainloop
