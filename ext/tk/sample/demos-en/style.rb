# style.rb
#
# This demonstration script creates a text widget that illustrates the
# various display styles that may be set for tags.
#
# text (display styles) widget demo (called by 'widget')
#

# toplevel widget
if defined?($style_demo) && $style_demo
  $style_demo.destroy
  $style_demo = nil
end

# demo toplevel widget
$style_demo = TkToplevel.new {|w|
  title("Text Demonstration - Display Styles")
  iconname("style")
  positionWindow(w)
}

base_frame = TkFrame.new($style_demo).pack(:fill=>:both, :expand=>true)

# frame
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    text 'Dismiss'
    command proc{
      tmppath = $style_demo
      $style_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'Show Code'
    command proc{showCode 'style'}
  }.pack('side'=>'left', 'expand'=>'yes')
}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# text
txt = TkText.new(base_frame){|t|
  #
  setgrid 'true'
  #width  70
  #height 32
  wrap 'word'
  font $font
  TkScrollbar.new(base_frame) {|s|
    pack('side'=>'right', 'fill'=>'y')
    command proc{|*args| t.yview(*args)}
    t.yscrollcommand proc{|first,last| s.set first,last}
  }
  pack('expand'=>'yes', 'fill'=>'both')

  #
  family = 'Courier'

  if $tk_version =~ /^4.*/
    style_tag_bold = TkTextTag.new(t, 'font'=>'-*-Courier-Bold-O-Normal--*-120-*-*-*-*-*-*')
    style_tag_big = TkTextTag.new(t, 'font'=>'-*-Courier-Bold-R-Normal--*-140-*-*-*-*-*-*', 'kanjifont'=>$msg_kanji_font)
    style_tag_verybig = TkTextTag.new(t, 'font'=>'-*-Helvetica-Bold-R-Normal--*-240-*-*-*-*-*-*')
    #    style_tag_small = TkTextTag.new(t, 'font'=>'-Adobe-Helvetica-Bold-R-Normal-*-100-*', 'kanjifont'=>$kanji_font)
    style_tag_small = TkTextTag.new(t, 'font'=>'-Adobe-Helvetica-Bold-R-Normal-*-100-*')
  else
    style_tag_bold = TkTextTag.new(t, 'font'=>[family, 12, :bold, :italic])
    style_tag_big = TkTextTag.new(t, 'font'=>[family, 14, :bold])
    style_tag_verybig = TkTextTag.new(t, 'font'=>['Helvetica', 24, :bold])
    style_tag_small = TkTextTag.new(t, 'font'=>'Times 8 bold')
  end
###
#  case($tk_version)
#  when /^4.*/
#    style_tag_big = TkTextTag.new(t, 'font'=>'-*-Courier-Bold-R-Normal--*-140-*-*-*-*-*-*', 'kanjifont'=>$msg_kanji_font)
#    style_tag_small = TkTextTag.new(t, 'font'=>'-Adobe-Helvetica-Bold-R-Normal-*-100-*', 'kanjifont'=>$kanji_font)
#  when /^8.*/
#    unless $style_demo_do_first
#      $style_demo_do_first = true
#      Tk.tk_call('font', 'create', '@bigascii',
#                '-copy', '-*-Courier-Bold-R-Normal--*-140-*-*-*-*-*-*')
#      Tk.tk_call('font', 'create', '@smallascii',
#                '-copy', '-Adobe-Helvetica-Bold-R-Normal-*-100-*')
#      Tk.tk_call('font', 'create', '@cBigFont',
#                '-compound', '@bigascii @msg_knj')
#      Tk.tk_call('font', 'create', '@cSmallFont',
#                '-compound', '@smallascii @kanji')
#    end
#    style_tag_big = TkTextTag.new(t, 'font'=>'@cBigFont')
#    style_tag_small = TkTextTag.new(t, 'font'=>'@cSmallFont')
#  end

  #
  if TkWinfo.depth($root).to_i > 1
    style_tag_color1 = TkTextTag.new(t, 'background'=>'#a0b7ce')
    style_tag_color2 = TkTextTag.new(t, 'foreground'=>'red')
    style_tag_raised = TkTextTag.new(t, 'relief'=>'raised', 'borderwidth'=>1)
    style_tag_sunken = TkTextTag.new(t, 'relief'=>'sunken', 'borderwidth'=>1)
  else
    style_tag_color1 = TkTextTag.new(t, 'background'=>'black',
                                     'foreground'=>'white')
    style_tag_color2 = TkTextTag.new(t, 'background'=>'black',
                                     'foreground'=>'white')
    style_tag_raised = TkTextTag.new(t, 'background'=>'white',
                                     'relief'=>'raised', 'borderwidth'=>1)
    style_tag_sunken = TkTextTag.new(t, 'background'=>'white',
                                     'relief'=>'sunken', 'borderwidth'=>1)
  end

  #
  if $tk_version =~ /^4\.[01]/
    style_tag_bgstipple = TkTextTag.new(t, 'background'=>'black',
                                        'borderwidth'=>0,
                                        'bgstipple'=>'gray25')
  else
    style_tag_bgstipple = TkTextTag.new(t, 'background'=>'black',
                                        'borderwidth'=>0,
                                        'bgstipple'=>'gray12')
  end
  style_tag_fgstipple = TkTextTag.new(t, 'fgstipple'=>'gray50')
  style_tag_underline = TkTextTag.new(t, 'underline'=>'on')
  style_tag_overstrike = TkTextTag.new(t, 'overstrike'=>'on')
  style_tag_right  = TkTextTag.new(t, 'justify'=>'right')
  style_tag_center = TkTextTag.new(t, 'justify'=>'center')
  if $tk_version =~ /^4.*/
    style_tag_super = TkTextTag.new(t, 'offset'=>'4p', 'font'=>'-Adobe-Courier-Medium-R-Normal--*-100-*-*-*-*-*-*')
    style_tag_sub = TkTextTag.new(t, 'offset'=>'-2p', 'font'=>'-Adobe-Courier-Medium-R-Normal--*-100-*-*-*-*-*-*')
  else
    style_tag_super = TkTextTag.new(t, 'offset'=>'4p', 'font'=>[family, 10])
    style_tag_sub = TkTextTag.new(t, 'offset'=>'-2p', 'font'=>[family, 10])
  end
  style_tag_margins = TkTextTag.new(t, 'lmargin1'=>'12m', 'lmargin2'=>'6m',
                                    'rmargin'=>'10m')
  style_tag_spacing = TkTextTag.new(t, 'spacing1'=>'10p', 'spacing2'=>'2p',
                                    'lmargin1'=>'12m', 'lmargin2'=>'6m',
                                    'rmargin'=>'10m')

  #
  insert('end', 'Text widgets like this one allow you to display information in a
variety of styles.  Display styles are controlled using a mechanism
called ')
  insert('end', 'tags', style_tag_big)
  insert('end', '. Tags are just textual names that you can apply to one
or more ranges of characters within a text widget.  You can configure
tags with various display styles.  If you do this, then the tagged
characters will be displayed with the styles you chose.  The
available display styles are:
')
  insert('end', "\n1. Font.", style_tag_big)
  insert('end', "  You can choose any X font, ")
  insert('end', "large", style_tag_verybig)
  insert('end', " or ")
  insert('end', "small", style_tag_small)
  insert('end', ".\n")
  insert('end', "\n2. Color.", style_tag_big)
  insert('end', "  You can change either the ")
  insert('end', "background", style_tag_color1)
  insert('end', " or ")
  insert('end', "foreground", style_tag_color2)
  insert('end', "\ncolor, or ")
  insert('end', "both", style_tag_color1, style_tag_color2)
  insert('end', ".\n")
  insert('end', "\n3. Stippling.", style_tag_big)
  insert('end', "  You can cause either the ")
  insert('end', "background", style_tag_bgstipple)
  insert('end', " or ")
  insert('end', "foreground", style_tag_fgstipple)
  insert('end', "\ninformation to be drawn with a stipple fill instead of a solid fill.\n")
  insert('end', "\n4. Underlining.", style_tag_big)
  insert('end', "  You can ")
  insert('end', "underline", style_tag_underline)
  insert('end', " ranges of text.\n")
  insert('end', "\n5. Overstrikes.", style_tag_big)
  insert('end', "  You can ")
  insert('end', "draw lines through", style_tag_overstrike)
  insert('end', " ranges of text.\n")
  insert('end', "\n6. 3-D effects.", style_tag_big)
  insert('end', "  You can arrange for the background to be drawn\nwith a border that makes characters appear either\n")
  insert('end', "raised", style_tag_raised)
  insert('end', " or ")
  insert('end', "sunken", style_tag_sunken)
  insert('end', ".\n")
  insert('end', "\n7. Justification.", style_tag_big)
  insert('end', " You can arrange for lines to be displayed\n")
  insert('end', "left-justified,\n")
  insert('end', "right-justified, or\n", style_tag_right)
  insert('end', "centered.\n", style_tag_center)
  insert('end', "\n8. Superscripts and subscripts.", style_tag_big)
  insert('end', " You can control the vertical\n")
  insert('end', "position of text to generate superscript effects like 10")
  insert('end', "n", style_tag_super)
  insert('end', " or\nsubscript effects like X")
  insert('end', "i", style_tag_sub)
  insert('end', ".\n")
  insert('end', "\n9. Margins.", style_tag_big)
  insert('end', " You can control the amount of extra space left")
  insert('end', " on\neach side of the text:\n")
  insert('end', "This paragraph is an example of the use of ", style_tag_margins)
  insert('end', "margins.  It consists of a single line of text ", style_tag_margins)
  insert('end', "that wraps around on the screen.  There are two ", style_tag_margins)
  insert('end', "separate left margin values, one for the first ", style_tag_margins)
  insert('end', "display line associated with the text line, ", style_tag_margins)
  insert('end', "and one for the subsequent display lines, which ", style_tag_margins)
  insert('end', "occur because of wrapping.  There is also a ", style_tag_margins)
  insert('end', "separate specification for the right margin, ", style_tag_margins)
  insert('end', "which is used to choose wrap points for lines.\n", style_tag_margins)
  insert('end', "\n10. Spacing.", style_tag_big)
  insert('end', " You can control the spacing of lines with three\n")
  insert('end', "separate parameters.  \"Spacing1\" tells how much ")
  insert('end', "extra space to leave\nabove a line, \"spacing3\" ")
  insert('end', "tells how much space to leave below a line,\nand ")
  insert('end', "if a text line wraps, \"spacing2\" tells how much ")
  insert('end', "space to leave\nbetween the display lines that ")
  insert('end', "make up the text line.\n")
  insert('end', "These indented paragraphs illustrate how spacing ", style_tag_spacing)
  insert('end', "can be used.  Each paragraph is actually a ", style_tag_spacing)
  insert('end', "single line in the text widget, which is ", style_tag_spacing)
  insert('end', "word-wrapped by the widget.\n", style_tag_spacing)
  insert('end', "Spacing1 is set to 10 points for this text, ", style_tag_spacing)
  insert('end', "which results in relatively large gaps between ", style_tag_spacing)
  insert('end', "the paragraphs.  Spacing2 is set to 2 points, ", style_tag_spacing)
  insert('end', "which results in just a bit of extra space ", style_tag_spacing)
  insert('end', "within a pararaph.  Spacing3 isn't used ", style_tag_spacing)
  insert('end', "in this example.\n", style_tag_spacing)
  insert('end', "To see where the space is, select ranges of ", style_tag_spacing)
  insert('end', "text within these paragraphs.  The selection ", style_tag_spacing)
  insert('end', "highlight will cover the extra space.", style_tag_spacing)

}

txt.width 70
txt.height 32
