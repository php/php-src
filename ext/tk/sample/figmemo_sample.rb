#!/usr/bin/env ruby
require 'tk'

begin
  # try to use Img extension
  require 'tkextlib/tkimg'
rescue Exception
  # cannot use Img extention --> ignore
end


############################
# scrolled_canvas
class TkScrolledCanvas < TkCanvas
  include TkComposite

  def initialize_composite(keys={})
    @h_scr = TkScrollbar.new(@frame)
    @v_scr = TkScrollbar.new(@frame)

    @canvas = TkCanvas.new(@frame)
    @path = @canvas.path

    @canvas.xscrollbar(@h_scr)
    @canvas.yscrollbar(@v_scr)

    TkGrid.rowconfigure(@frame, 0, :weight=>1, :minsize=>0)
    TkGrid.columnconfigure(@frame, 0, :weight=>1, :minsize=>0)

    @canvas.grid(:row=>0, :column=>0, :sticky=>'news')
    @h_scr.grid(:row=>1, :column=>0, :sticky=>'ew')
    @v_scr.grid(:row=>0, :column=>1, :sticky=>'ns')

    delegate('DEFAULT', @canvas)
    delegate('background', @canvas, @h_scr, @v_scr)
    delegate('activebackground', @h_scr, @v_scr)
    delegate('troughcolor', @h_scr, @v_scr)
    delegate('repeatdelay', @h_scr, @v_scr)
    delegate('repeatinterval', @h_scr, @v_scr)
    delegate('borderwidth', @frame)
    delegate('relief', @frame)

    delegate_alias('canvasborderwidth', 'borderwidth', @canvas)
    delegate_alias('canvasrelief', 'relief', @canvas)

    delegate_alias('scrollbarborderwidth', 'borderwidth', @h_scr, @v_scr)
    delegate_alias('scrollbarrelief', 'relief', @h_scr, @v_scr)

    configure(keys) unless keys.empty?
  end
end

############################
class PhotoCanvas < TkScrolledCanvas

USAGE = <<EOT
--- WHAT IS ---
You can write comments on the loaded image, and save it as a Postscipt
file (original image file is not modified). Each comment is drawn as a
set of an indicator circle, an arrow, and a memo text. See the following
how to write comments.
This can save the list of memo texts to another file. It may useful to
search the saved Postscript file by the comments on them.
This may not support multibyte characters (multibyte texts are broken on
a Postscript file). It depends on features of canvas widgets of Tcl/Tk
libraries linked your Ruby/Tk. If you use Tcl/Tk8.0-jp (Japanized Tcl/Tk),
you can (possibly) get a Japanese Postscript file.

--- BINDINGS ---
* Button-1 : draw comments by following steps
    1st - Set center of a indicator circle.
    2nd - Set head position of an arrow.
    3rd - Set tail position of an arrow, and show an entry box.
          Input a memo text and hit 'Enter' key to entry the comment.

* Button-2-drag : scroll the canvas

* Button-3 : when drawing, cancel current drawing

* Double-Button-3 : delete the clicked comment (text, arrow, and circle)
EOT

  def initialize(*args)
    super(*args)

    self.highlightthickness = 0
    self.selectborderwidth = 0

    @photo = TkPhotoImage.new
    @img = TkcImage.new(self, 0, 0, :image=>@photo)

    width  = self.width
    height = self.height
    @scr_region = [-width, -height, width, height]
    self.scrollregion(@scr_region)
    self.xview_moveto(0.25)
    self.yview_moveto(0.25)

    @col = 'red'
    @font = 'Helvetica -12'

    @memo_id_num = -1
    @memo_id_head = 'memo_'
    @memo_id_tag = nil
    @overlap_d = 2

    @state = TkVariable.new
    @border = 2
    @selectborder = 1
    @delta = @border + @selectborder
    @entry = TkEntry.new(self, :relief=>:ridge, :borderwidth=>@border,
                         :selectborderwidth=>@selectborder,
                         :highlightthickness=>0)
    @entry.bind('Return'){@state.value = 0}

    @mode = old_mode = 0

    _state0()

    bind('2', :x, :y){|x,y| scan_mark(x,y)}
    bind('B2-Motion', :x, :y){|x,y| scan_dragto(x,y)}

    bind('3'){
      next if (old_mode = @mode) == 0
      @items.each{|item| item.delete }
      _state0()
    }

    bind('Double-3', :widget, :x, :y){|w, x, y|
      next if old_mode != 0
      x = w.canvasx(x)
      y = w.canvasy(y)
      tag = nil
      w.find_overlapping(x - @overlap_d, y - @overlap_d,
                         x + @overlap_d, y + @overlap_d).find{|item|
        ! (item.tags.find{|name|
             if name =~ /^(#{@memo_id_head}\d+)$/
               tag = $1
             end
           }.empty?)
      }
      w.delete(tag) if tag
    }
  end

  #-----------------------------------
  private
  def _state0() # init
    @mode = 0

    @memo_id_num += 1
    @memo_id_tag = @memo_id_head + @memo_id_num.to_s

    @target = nil
    @items = []
    @mark = [0, 0]
    bind_remove('Motion')
    bind('ButtonRelease-1', proc{|x,y| _state1(x,y)}, '%x', '%y')
  end

  def _state1(x,y) # set center
    @mode = 1

    @target = TkcOval.new(self,
                          [canvasx(x), canvasy(y)], [canvasx(x), canvasy(y)],
                          :outline=>@col, :width=>3, :tags=>[@memo_id_tag])
    @items << @target
    @mark = [x,y]

    bind('Motion', proc{|x,y| _state2(x,y)}, '%x', '%y')
    bind('ButtonRelease-1', proc{|x,y| _state3(x,y)}, '%x', '%y')
  end

  def _state2(x,y) # create circle
    @mode = 2

    r = Integer(Math.sqrt((x-@mark[0])**2 + (y-@mark[1])**2))
    @target.coords([canvasx(@mark[0] - r), canvasy(@mark[1] - r)],
                   [canvasx(@mark[0] + r), canvasy(@mark[1] + r)])
  end

  def _state3(x,y) # set line start
    @mode = 3

    @target = TkcLine.new(self,
                          [canvasx(x), canvasy(y)], [canvasx(x), canvasy(y)],
                          :arrow=>:first, :arrowshape=>[10, 14, 5],
                          :fill=>@col, :tags=>[@memo_id_tag])
    @items << @target
    @mark = [x, y]

    bind('Motion', proc{|x,y| _state4(x,y)}, '%x', '%y')
    bind('ButtonRelease-1', proc{|x,y| _state5(x,y)}, '%x', '%y')
  end

  def _state4(x,y) # create line
    @mode = 4

    @target.coords([canvasx(@mark[0]), canvasy(@mark[1])],
                   [canvasx(x), canvasy(y)])
  end

  def _state5(x,y) # set text
    @mode = 5

    if x - @mark[0] >= 0
      justify = 'left'
      dx = - @delta

      if y - @mark[1] >= 0
        anchor = 'nw'
        dy = - @delta
      else
        anchor = 'sw'
        dy = @delta
      end
    else
      justify = 'right'
      dx = @delta

      if y - @mark[1] >= 0
        anchor = 'ne'
        dy = - @delta
      else
        anchor = 'se'
        dy = @delta
      end
    end

    bind_remove('Motion')

    @entry.value = ''
    @entry.configure(:justify=>justify, :font=>@font, :foreground=>@col)

    ewin = TkcWindow.new(self, [canvasx(x)+dx, canvasy(y)+dy],
                         :window=>@entry, :state=>:normal, :anchor=>anchor,
                         :tags=>[@memo_id_tag])

    @entry.focus
    @entry.grab
    @state.wait
    @entry.grab_release

    ewin.delete

    @target = TkcText.new(self, [canvasx(x), canvasy(y)],
                          :anchor=>anchor, :justify=>justify,
                          :fill=>@col, :font=>@font, :text=>@entry.value,
                          :tags=>[@memo_id_tag])

    _state0()
  end

  #-----------------------------------
  public
  def load_photo(filename)
    @photo.configure(:file=>filename)
  end

  def modified?
    ! ((find_withtag('all') - [@img]).empty?)
  end

  def fig_erase
    (find_withtag('all') - [@img]).each{|item| item.delete}
  end

  def reset_region
    width = @photo.width
    height = @photo.height

    if width > @scr_region[2]
      @scr_region[0] = -width
      @scr_region[2] = width
    end

    if height > @scr_region[3]
      @scr_region[1] = -height
      @scr_region[3] = height
    end

    self.scrollregion(@scr_region)
    self.xview_moveto(0.25)
    self.yview_moveto(0.25)
  end

  def get_texts
    ret = []
    find_withtag('all').each{|item|
      if item.kind_of?(TkcText)
        ret << item[:text]
      end
    }
    ret
  end
end
############################

# define methods for menu
def open_file(canvas, fname)
  if canvas.modified?
    ret = Tk.messageBox(:icon=>'warning',:type=>'okcancel',:default=>'cancel',
                        :message=>'Canvas may be modified. Realy erase? ')
    return if ret == 'cancel'
  end

  filetypes = [
    ['GIF Files', '.gif'],
    ['GIF Files', [], 'GIFF'],
    ['PPM Files', '.ppm'],
    ['PGM Files', '.pgm']
  ]

  begin
    if Tk::Img::package_version != ''
      filetypes << ['JPEG Files', ['.jpg', '.jpeg']]
      filetypes << ['PNG Files', '.png']
      filetypes << ['PostScript Files', '.ps']
      filetypes << ['PDF Files', '.pdf']
      filetypes << ['Windows Bitmap Files', '.bmp']
      filetypes << ['Windows Icon Files', '.ico']
      filetypes << ['PCX Files', '.pcx']
      filetypes << ['Pixmap Files', '.pixmap']
      filetypes << ['SGI Files', '.sgi']
      filetypes << ['Sun Raster Files', '.sun']
      filetypes << ['TGA Files', '.tga']
      filetypes << ['TIFF Files', '.tiff']
      filetypes << ['XBM Files', '.xbm']
      filetypes << ['XPM Files', '.xpm']
    end
  rescue
  end

  filetypes << ['ALL Files', '*']

  fpath = Tk.getOpenFile(:filetypes=>filetypes)
  return if fpath.empty?

  begin
    canvas.load_photo(fpath)
  rescue => e
    Tk.messageBox(:icon=>'error', :type=>'ok',
                  :message=>"Fail to read '#{fpath}'.\n#{e.message}")
  end

  canvas.fig_erase
  canvas.reset_region

  fname.value = fpath
end

# --------------------------------
def save_memo(canvas, fname)
  initname = fname.value
  if initname != '-'
    initname = File.basename(initname, File.extname(initname))
    fpath = Tk.getSaveFile(:filetypes=>[ ['Text Files', '.txt'],
                                         ['ALL Files', '*'] ],
                           :initialfile=>initname)
  else
    fpath = Tk.getSaveFile(:filetypes=>[ ['Text Files', '.txt'],
                                         ['ALL Files', '*'] ])
  end
  return if fpath.empty?

  begin
    fid = open(fpath, 'w')
  rescue => e
    Tk.messageBox(:icon=>'error', :type=>'ok',
                  :message=>"Fail to open '#{fname.value}'.\n#{e.message}")
  end

  begin
    canvas.get_texts.each{|txt|
      fid.print(txt, "\n")
    }
  ensure
    fid.close
  end
end

# --------------------------------
def ps_print(canvas, fname)
  initname = fname.value
  if initname != '-'
    initname = File.basename(initname, File.extname(initname))
    fpath = Tk.getSaveFile(:filetypes=>[ ['Postscript Files', '.ps'],
                                         ['ALL Files', '*'] ],
                           :initialfile=>initname)
  else
    fpath = Tk.getSaveFile(:filetypes=>[ ['Postscript Files', '.ps'],
                                         ['ALL Files', '*'] ])
  end
  return if fpath.empty?

  bbox = canvas.bbox('all')
  canvas.postscript(:file=>fpath, :x=>bbox[0], :y=>bbox[1],
                    :width=>bbox[2] - bbox[0], :height=>bbox[3] - bbox[1])
end

# --------------------------------
def quit(canvas)
  ret = Tk.messageBox(:icon=>'warning', :type=>'okcancel',
                      :default=>'cancel',
                      :message=>'Realy quit? ')
  exit if ret == 'ok'
end

# --------------------------------
# setup root
root = TkRoot.new(:title=>'Fig Memo')

# create canvas frame
canvas = PhotoCanvas.new(root).pack(:fill=>:both, :expand=>true)
usage_frame = TkFrame.new(root, :relief=>:ridge, :borderwidth=>2)
hide_btn = TkButton.new(usage_frame, :text=>'hide usage',
                        :font=>{:size=>8}, :pady=>1,
                        :command=>proc{usage_frame.unpack})
hide_btn.pack(:anchor=>'e', :padx=>5)
usage = TkLabel.new(usage_frame, :text=>PhotoCanvas::USAGE,
                    :font=>'Helvetica 8', :justify=>:left).pack

show_usage = proc{
  usage_frame.pack(:before=>canvas, :fill=>:x, :expand=>true)
}

fname = TkVariable.new('-')
f = TkFrame.new(root, :relief=>:sunken, :borderwidth=>1).pack(:fill=>:x)
label = TkLabel.new(f, :textvariable=>fname,
                    :font=>{:size=>-12, :weight=>:bold},
                    :anchor=>'w').pack(:side=>:left, :fill=>:x, :padx=>10)

# create menu
mspec = [
  [ ['File', 0],
    ['Show Usage',      proc{show_usage.call}, 5],
    '---',
    ['Open Image File', proc{open_file(canvas, fname)}, 0],
    ['Save Memo Texts', proc{save_memo(canvas, fname)}, 0],
    '---',
    ['Save Postscript', proc{ps_print(canvas, fname)}, 5],
    '---',
    ['Quit', proc{quit(canvas)}, 0]
  ]
]
root.add_menubar(mspec)

# manage wm_protocol
root.protocol(:WM_DELETE_WINDOW){quit(canvas)}

# show usage
show_usage.call

# --------------------------------
# start eventloop
Tk.mainloop
