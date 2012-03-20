# entry3.rb --
#
# This demonstration script creates several entry widgets whose
# permitted input is constrained in some way.  It also shows off a
# password entry.
#
# based on Tcl/Tk8.4.4 widget demos

if defined?($entry3_demo) && $entry3_demo
  $entry3_demo.destroy
  $entry3_demo = nil
end

$entry3_demo = TkToplevel.new {|w|
  title("Constrained Entry Demonstration")
  iconname("entry3")
  positionWindow(w)
}

base_frame = TkFrame.new($entry3_demo).pack(:fill=>:both, :expand=>true)

TkLabel.new(base_frame,
            :font=>$font, :wraplength=>'5i', :justify=>:left,
            :text=><<EOL).pack(:side=>:top)
Four different entries are displayed below.  You can add characters \
by pointing, clicking and typing, though each is constrained in what \
it will accept.  The first only accepts integers or the empty string \
(checking when focus leaves it) and will flash to indicate any \
problem.  The second only accepts strings with fewer than ten \
characters and sounds the bell when an attempt to go over the limit \
is made.  The third accepts US phone numbers, mapping letters to \
their digit equivalent and sounding the bell on encountering an \
invalid character or if trying to type over a character that is not \
a digit.  The fourth is a password field that accepts up to eight \
characters (silently ignoring further ones), and displaying them as \
asterisk characters.
EOL

TkFrame.new(base_frame){|f|
  pack(:side=>:bottom, :fill=>:x, :pady=>'2m')

  TkButton.new(f, :text=>'Dismiss', :width=>15, :command=>proc{
                 $entry3_demo.destroy
                 $entry3_demo = nil
               }).pack(:side=>:left, :expand=>true)

  TkButton.new(f, :text=>'See Code', :width=>15, :command=>proc{
                 showCode 'entry3'
               }).pack(:side=>:left, :expand=>true)
}

# focusAndFlash --
# Error handler for entry widgets that forces the focus onto the
# widget and makes the widget flash by exchanging the foreground and
# background colours at intervals of 200ms (i.e. at approximately
# 2.5Hz).
#
# Arguments:
# widget -      entry widget to flash
# fg -          Initial foreground colour
# bg -          Initial background colour
# count -       Counter to control the number of times flashed
def focusAndFlash(widget, fg, bg, count=5)
  return if count <= 0
  if fg && !fg.empty? && bg && !bg.empty?
    TkTimer.new(200, count,
                proc{widget.configure(:foreground=>bg, :background=>fg)},
                proc{widget.configure(:foreground=>fg, :background=>bg)}
                ).start
  else
    # TkTimer.new(150, 3){Tk.bell}.start
    Tk.bell
    TkTimer.new(200, count,
                proc{widget.configure(:foreground=>'white',
                                      :background=>'black')},
                proc{widget.configure(:foreground=>'black',
                                      :background=>'white')}
                ).at_end{begin
                           widget.configure(:foreground=>fg,
                                            :background=>bg)
                         rescue
                           # ignore
                         end}.start
  end
  widget.focus(true)
end

l1 = TkLabelFrame.new(base_frame, :text=>"Integer Entry")
TkEntry.new(l1, :validate=>:focus,
            :vcmd=>[
              proc{|s| s == '' || /^[+-]?\d+$/ =~ s }, '%P'
            ]) {|e|
  fg = e.foreground
  bg = e.background
  invalidcommand [proc{|w| focusAndFlash(w, fg, bg)}, '%W']
  pack(:fill=>:x, :expand=>true, :padx=>'1m', :pady=>'1m')
}

l2 = TkLabelFrame.new(base_frame, :text=>"Length-Constrained Entry")
TkEntry.new(l2, :validate=>:key, :invcmd=>proc{Tk.bell},
            :vcmd=>[proc{|s| s.length < 10}, '%P']
            ).pack(:fill=>:x, :expand=>true, :padx=>'1m', :pady=>'1m')

### PHONE NUMBER ENTRY ###
# Note that the source to this is quite a bit longer as the behaviour
# demonstrated is a lot more ambitious than with the others.

# Initial content for the third entry widget
entry3content = TkVariable.new("1-(000)-000-0000")

# Mapping from alphabetic characters to numbers.
$phoneNumberMap = {}
Hash[*(%w(abc 2 def 3 ghi 4 jkl 5 mno 6 pqrs 7 tuv 8 wxyz 9))].each{|chars, n|
  chars.split('').each{|c|
    $phoneNumberMap[c] = n
    $phoneNumberMap[c.upcase] = n
  }
}

# phoneSkipLeft --
# Skip over fixed characters in a phone-number string when moving left.
#
# Arguments:
# widget -      The entry widget containing the phone-number.
def phoneSkipLeft(widget)
  idx = widget.index('insert')
  if idx == 8
    # Skip back two extra characters
    widget.cursor = idx - 2
  elsif idx == 7 || idx == 12
    # Skip back one extra character
    widget.cursor = idx - 1
  elsif idx <= 3
    # Can't move any further
    Tk.bell
    Tk.callback_break
  end
end

# phoneSkipRight --
# Skip over fixed characters in a phone-number string when moving right.
#
# Arguments:
# widget -      The entry widget containing the phone-number.
# add - Offset to add to index before calculation (used by validation.)
def phoneSkipRight(widget, add = 0)
  idx = widget.index('insert')
  if (idx + add == 5)
    # Skip forward two extra characters
    widget.cursor = idx + 2
  elsif (idx + add == 6 || idx + add == 10)
    # Skip forward one extra character
    widget.cursor = idx + 1
  elsif (idx + add == 15 && add == 0)
    # Can't move any further
    Tk.bell
    Tk.callback_break
  end
end

# validatePhoneChange --
# Checks that the replacement (mapped to a digit) of the given
# character in an entry widget at the given position will leave a
# valid phone number in the widget.
#
# widget - entry widget to validate
# vmode -  The widget's validation mode
# idx -    The index where replacement is to occur
# char -   The character (or string, though that will always be
#          refused) to be overwritten at that point.

def validatePhoneChange(widget, vmode, idx, char)
  return true if idx == nil
  Tk.after_idle(proc{widget.configure(:validate=>vmode,
                                      :invcmd=>proc{Tk.bell})})
  if !(idx<3 || idx==6 || idx==7 || idx==11 || idx>15) && char =~ /[0-9A-Za-z]/
    widget.delete(idx)
    widget.insert(idx, $phoneNumberMap[char] || char)
    Tk.after_idle(proc{phoneSkipRight(widget, -1)})
    return true
    # Tk.update(true) # <- Don't work 'update' inter validation callback.
                      #    It depends on Tcl/Tk side (tested on Tcl/Tk8.5a1).
  end
  return false
end


l3 = TkLabelFrame.new(base_frame, :text=>"US Phone-Number Entry")
TkEntry.new(l3, :validate=>:key, :invcmd=>proc{Tk.bell},
            :textvariable=>entry3content,
            :vcmd=>[
              proc{|w,v,i,s| validatePhoneChange(w,v,i,s)},
              "%W %v %i %S"
            ]){|e|
  # Click to focus goes to the first editable character...
  bind('FocusIn', proc{|d,w|
         if d != "NotifyAncestor"
           w.cursor = 3
           Tk.after_idle(proc{w.selection_clear})
         end
       }, '%d %W')
  bind('Left',  proc{|w| phoneSkipLeft(w)},  '%W')
  bind('Right', proc{|w| phoneSkipRight(w)}, '%W')
  pack(:fill=>:x, :expand=>true, :padx=>'1m', :pady=>'1m')
}

l4 = TkLabelFrame.new(base_frame, :text=>"Password Entry")
TkEntry.new(l4, :validate=>:key, :show=>'*',
            :vcmd=>[
              proc{|s| s.length <= 8},
              '%P'
            ]).pack(:fill=>:x, :expand=>true, :padx=>'1m', :pady=>'1m')

TkFrame.new(base_frame){|f|
  lower
  TkGrid.configure(l1, l2, :in=>f, :padx=>'3m', :pady=>'1m', :sticky=>:ew)
  TkGrid.configure(l3, l4, :in=>f, :padx=>'3m', :pady=>'1m', :sticky=>:ew)
  TkGrid.columnconfigure(f, [0,1], :uniform=>1)
  pack(:fill=>:both, :expand=>true)
}
