require 'tk'

TkMessage.new(:width=>400, :text=><<EOM).pack
This sample shows how to use Tk::EncodedString class. \
This reads 'iso2022-kr' text (from discription of \
Korean language environment of GNU Emacs 20.7.2) \
and inserts the text into the text widget.
EOM

t1 = TkText.new(:height=>5).pack
t2 = TkText.new(:height=>5).pack
t3 = TkText.new(:height=>5).pack

src_str = IO.readlines(File.join(File.dirname(__FILE__),'iso2022-kr.txt')).join

t1.insert('end',
          "use neither Tk::EncodedString class nor Tk.encoding= method\n\n")
t1.insert('end', src_str)

enc_str = Tk::EncodedString(src_str, 'iso2022-kr')
t2.insert('end',
          "use Tk::EncodedString class (Tk.encoding => '#{Tk.encoding}')\n\n")
t2.insert('end', enc_str)

Tk.encoding = 'iso2022-kr'
t3.insert('end', "use Tk.encoding = 'iso2022-kr' (Tk.force_default_encoding? == #{Tk.force_default_encoding?})\n\n")

t3.insert('end', src_str)

Tk.mainloop
