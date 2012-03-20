# unicodeout.rb --
#
# This demonstration script shows how you can produce output (in label
# widgets) using many different alphabets.
#
# based on Tcl/Tk8.4.4 widget demos

if defined?($unicodeout_demo) && $unicodeout_demo
  $unicodeout_demo.destroy
  $unicodeout_demo = nil
end

$unicodeout_demo = TkToplevel.new {|w|
  title("Unicode Label Demonstration")
  iconname("unicodeout")
  positionWindow(w)
}

base_frame = TkFrame.new($unicodeout_demo).pack(:fill=>:both, :expand=>true)

TkLabel.new(base_frame,
            :font=>$font, :wraplength=>'5.4i', :justify=>:left,
            :text=><<EOL).pack(:side=>:top)
This is a sample of Tk's support for languages that use non-Western \
character sets.  However, what you will actually see below depends \
largely on what character sets you have installed, and what you see \
for characters that are not present varies greatly between platforms as well. \
Please try to click the 'See Code' button, \
and click the 'Rerun Demo' button after editing \
(the source file is not changed) \
the definition of @@font on the Unicodeout_SampleFrame class.
The strings are written in Tcl using UNICODE characters \
using the \\uXXXX escape so as to do so in a portable fashion.

ATTENTION:
The strings are converted to the encoded string objects \
(completed to rewrite Tcl's escapes) by Tk::UTF8_String method. \
And the Tk::UTF8_String objects are passed to the label widgets.
EOL

TkFrame.new(base_frame){|f|
  pack(:side=>:bottom, :fill=>:x, :pady=>'2m')

  TkButton.new(f, :text=>'Dismiss', :width=>15, :command=>proc{
                 $unicodeout_demo.destroy
                 $unicodeout_demo = nil
               }).pack(:side=>:left, :expand=>true)

  TkButton.new(f, :text=>'See Code', :width=>15, :command=>proc{
                 showCode 'unicodeout'
               }).pack(:side=>:left, :expand=>true)
}

wait_msg = TkLabel.new(base_frame,
                       :text=>"Please wait while loading fonts...",
                       :font=>"Helvetica 12 italic").pack

class Unicodeout_SampleFrame < TkFrame
  @@font = $font
  # @@font = 'Helvetica 14'
  # @@font = 'Courier 12'
  # @@font = 'clearlyu 16'
  # @@font = 'fixed 12'
  # @@font = 'Times 12'
  # @@font = 'Newspaper 12'
  # @@font = '{New century schoolbook} 12'

  def initialize(base)
    super(base)
    grid_columnconfig(1, :weight=>1)
  end

  def add_sample(lang, *args)
    sample_txt = Tk::UTF8_String(args.join(''))
    l = TkLabel.new(self, :font=>@@font, :text=>lang+':',
                    :anchor=>:nw, :pady=>0)
    #s = TkLabel.new(self, :font=>@@font, :text=>sample_txt,
    s = TkLabel.new(self, :font=>TkFont.new(@@font), :text=>sample_txt,
                    :anchor=>:nw, :width=>30, :pady=>0)
    Tk.grid(l, s, :sticky=>:ew, :pady=>0)
    l.grid_config(:padx, '1m')
  end
end
f = Unicodeout_SampleFrame.new(base_frame)
f.pack(:expand=>true, :fill=>:both, :padx=>'2m', :pady=>'1m')

# Processing when some characters are missing might take a while, so make
# sure we're displaying something in the meantime...

oldCursor = $unicodeout_demo.cursor
$unicodeout_demo.cursor('watch')
Tk.update

f.add_sample('Arabic',
             '\uFE94\uFEF4\uFE91\uFEAE\uFECC\uFEDF\uFE8D\uFE94',
             '\uFEE4\uFEE0\uFEDC\uFEDF\uFE8D')
f.add_sample('Trad. Chinese', '\u4E2D\u570B\u7684\u6F22\u5B57')
f.add_sample('Simpl. Chinese', '\u6C49\u8BED')
f.add_sample('Greek',
             '\u0395\u03BB\u03BB\u03B7\u03BD\u03B9\u03BA\u03AE ',
             '\u03B3\u03BB\u03CE\u03C3\u03C3\u03B1')
f.add_sample('Hebrew',
             '\u05DD\u05D9\u05DC\u05E9\u05D5\u05E8\u05D9 ',
             '\u05DC\u05D9\u05D0\u05E8\u05E9\u05D9')
f.add_sample('Japanese',
             '\u65E5\u672C\u8A9E\u306E\u3072\u3089\u304C\u306A, ',
             '\u6F22\u5B57\u3068\u30AB\u30BF\u30AB\u30CA')
f.add_sample('Korean', '\uB300\uD55C\uBBFC\uAD6D\uC758 \uD55C\uAE00')
f.add_sample('Russian',
             '\u0420\u0443\u0441\u0441\u043A\u0438\u0439 ',
             '\u044F\u0437\u044B\u043A')

wait_msg.destroy
$unicodeout_demo.cursor(oldCursor)
