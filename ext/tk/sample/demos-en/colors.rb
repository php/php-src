# colors.rb
#
# This demonstration script creates a listbox widget that displays
# many of the colors from the X color database.  You can click on
# a color to change the application's palette.
#
# listbox widget demo 'colors' (called by 'widget')
#

# toplevel widget
if defined?($colors_demo) && $colors_demo
  $colors_demo.destroy
  $colors_demo = nil
end

# demo toplevel widget
$colors_demo = TkToplevel.new {|w|
  title("Listbox Demonstration (colors)")
  iconname("colors")
  positionWindow(w)
}

base_frame = TkFrame.new($colors_demo).pack(:fill=>:both, :expand=>true)

# label
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '4i'
  justify 'left'
  text "A listbox containing several color names is displayed below, along with a scrollbar.  You can scan the list either using the scrollbar or by dragging in the listbox window with button 2 pressed.  If you double-click button 1 on a color, then the application's color palette will be set to match that color"
}
msg.pack('side'=>'top')

# frame
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    text 'Dismiss'
    command proc{
      tmppath = $colors_demo
      $colors_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'Show Code'
    command proc{showCode 'colors'}
  }.pack('side'=>'left', 'expand'=>'yes')

}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# frame
colors_lbox = nil
TkFrame.new(base_frame, 'borderwidth'=>10) {|w|
  s = TkScrollbar.new(w)
  colors_lbox = TkListbox.new(w) {
    setgrid 1
    width  10
    height 12
    yscrollcommand proc{|first,last| s.set first,last}
  }
  s.command(proc{|*args| colors_lbox.yview(*args)})
  s.pack('side'=>'right', 'fill'=>'y')
  colors_lbox.pack('side'=>'left', 'expand'=>1, 'fill'=>'both')
}.pack('side'=>'top', 'expand'=>'yes', 'fill'=>'y')

#colors_lbox.bind('Double-1', proc{TkPalette.setPalette TkSelection.get})
colors_lbox.bind('Double-1', proc{
                   begin
                     TkPalette.setPalette TkSelection.get
                   rescue => e
                     p e
                     Tk.tk_call_without_enc('destroy', '.___tk_set_palette')
                   end
                 })

ins_data = [
  'gray60','gray70','gray80','gray85','gray90','gray95',
  'snow1','snow2','snow3','snow4','seashell1','seashell2',
  'seashell3','seashell4','AntiqueWhite1','AntiqueWhite2',
  'AntiqueWhite3','AntiqueWhite4','bisque1','bisque2',
  'bisque3','bisque4','PeachPuff1','PeachPuff2',
  'PeachPuff3','PeachPuff4','NavajoWhite1','NavajoWhite2',
  'NavajoWhite3','NavajoWhite4','LemonChiffon1',
  'LemonChiffon2','LemonChiffon3','LemonChiffon4',
  'cornsilk1','cornsilk2','cornsilk3','cornsilk4',
  'ivory1','ivory2','ivory3','ivory4','honeydew1',
  'honeydew2','honeydew3','honeydew4','LavenderBlush1',
  'LavenderBlush2','LavenderBlush3','LavenderBlush4',
  'MistyRose1','MistyRose2','MistyRose3','MistyRose4',
  'azure1','azure2','azure3','azure4','SlateBlue1',
  'SlateBlue2','SlateBlue3','SlateBlue4','RoyalBlue1',
  'RoyalBlue2','RoyalBlue3','RoyalBlue4','blue1','blue2',
  'blue3','blue4','DodgerBlue1','DodgerBlue2',
  'DodgerBlue3','DodgerBlue4','SteelBlue1','SteelBlue2',
  'SteelBlue3','SteelBlue4','DeepSkyBlue1','DeepSkyBlue2',
  'DeepSkyBlue3','DeepSkyBlue4','SkyBlue1','SkyBlue2',
  'SkyBlue3','SkyBlue4','LightSkyBlue1','LightSkyBlue2',
  'LightSkyBlue3','LightSkyBlue4','SlateGray1',
  'SlateGray2','SlateGray3','SlateGray4',
  'LightSteelBlue1','LightSteelBlue2','LightSteelBlue3',
  'LightSteelBlue4','LightBlue1','LightBlue2',
  'LightBlue3','LightBlue4','LightCyan1','LightCyan2',
  'LightCyan3','LightCyan4','PaleTurquoise1',
  'PaleTurquoise2','PaleTurquoise3','PaleTurquoise4',
  'CadetBlue1','CadetBlue2','CadetBlue3','CadetBlue4',
  'turquoise1','turquoise2','turquoise3','turquoise4',
  'cyan1','cyan2','cyan3','cyan4','DarkSlateGray1',
  'DarkSlateGray2','DarkSlateGray3','DarkSlateGray4',
  'aquamarine1','aquamarine2','aquamarine3','aquamarine4',
  'DarkSeaGreen1','DarkSeaGreen2','DarkSeaGreen3',
  'DarkSeaGreen4','SeaGreen1','SeaGreen2','SeaGreen3',
  'SeaGreen4','PaleGreen1','PaleGreen2','PaleGreen3',
  'PaleGreen4','SpringGreen1','SpringGreen2',
  'SpringGreen3','SpringGreen4','green1','green2',
  'green3','green4','chartreuse1','chartreuse2',
  'chartreuse3','chartreuse4','OliveDrab1','OliveDrab2',
  'OliveDrab3','OliveDrab4','DarkOliveGreen1',
  'DarkOliveGreen2','DarkOliveGreen3','DarkOliveGreen4',
  'khaki1','khaki2','khaki3','khaki4','LightGoldenrod1',
  'LightGoldenrod2','LightGoldenrod3','LightGoldenrod4',
  'LightYellow1','LightYellow2','LightYellow3',
  'LightYellow4','yellow1','yellow2','yellow3','yellow4',
  'gold1','gold2','gold3','gold4','goldenrod1',
  'goldenrod2','goldenrod3','goldenrod4','DarkGoldenrod1',
  'DarkGoldenrod2','DarkGoldenrod3','DarkGoldenrod4',
  'RosyBrown1','RosyBrown2','RosyBrown3','RosyBrown4',
  'IndianRed1','IndianRed2','IndianRed3','IndianRed4',
  'sienna1','sienna2','sienna3','sienna4','burlywood1',
  'burlywood2','burlywood3','burlywood4','wheat1',
  'wheat2','wheat3','wheat4','tan1','tan2','tan3','tan4',
  'chocolate1','chocolate2','chocolate3','chocolate4',
  'firebrick1','firebrick2','firebrick3','firebrick4',
  'brown1','brown2','brown3','brown4','salmon1','salmon2',
  'salmon3','salmon4','LightSalmon1','LightSalmon2',
  'LightSalmon3','LightSalmon4','orange1','orange2',
  'orange3','orange4','DarkOrange1','DarkOrange2',
  'DarkOrange3','DarkOrange4','coral1','coral2','coral3',
  'coral4','tomato1','tomato2','tomato3','tomato4',
  'OrangeRed1','OrangeRed2','OrangeRed3','OrangeRed4',
  'red1','red2','red3','red4','DeepPink1','DeepPink2',
  'DeepPink3','DeepPink4','HotPink1','HotPink2',
  'HotPink3','HotPink4','pink1','pink2','pink3','pink4',
  'LightPink1','LightPink2','LightPink3','LightPink4',
  'PaleVioletRed1','PaleVioletRed2','PaleVioletRed3',
  'PaleVioletRed4','maroon1','maroon2','maroon3',
  'maroon4','VioletRed1','VioletRed2','VioletRed3',
  'VioletRed4','magenta1','magenta2','magenta3',
  'magenta4','orchid1','orchid2','orchid3','orchid4',
  'plum1','plum2','plum3','plum4','MediumOrchid1',
  'MediumOrchid2','MediumOrchid3','MediumOrchid4',
  'DarkOrchid1','DarkOrchid2','DarkOrchid3',
  'DarkOrchid4','purple1','purple2','purple3','purple4',
  'MediumPurple1','MediumPurple2','MediumPurple3',
  'MediumPurple4','thistle1','thistle2','thistle3', 'thistle4'
]

colors_lbox.insert(0, *ins_data)
