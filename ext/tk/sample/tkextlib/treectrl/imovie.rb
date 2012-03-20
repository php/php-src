#
# Demo: iMovie
#
def demoIMovie(t)
  t.configure(:showroot=>false, :showbuttons=>false, :showlines=>false,
              :selectmode=>:browse, :orient=>:horizontal, :wrap=>:window,
              :showheader=>false, :background=>'#dcdcdc')

  if $HasColumnCreate
    t.column_create
  end

  init_pics('imovie-*')

  case @thisPlatform
  when 'macintosh', 'macosx'
    font1 = TkFont.new(['Geneva', 9])
    font2 = TkFont.new(['Geneva', 10])
  when 'unix'
    font1 = TkFont.new(['Helvetica', -12])
    font2 = TkFont.new(['Helvetica', -14])
  else
    font1 = TkFont.new(['Helvetica', 8])
    font2 = TkFont.new(['Helvetica', 10])
  end

  t.element_create('elemTime', :text, :font=>font1)
  t.element_create('elemName', :text, :font=>font2, :lines=>1, :width=>80)
  t.element_create('elemRect', :rect, :outline=>'#827878', :outlinewidth=>1,
                   :fill=>['#ffdc5a', ['selected'], 'white', []])
  t.element_create('elemImg', :image)
  t.element_create('elemShadow', :rect, :outline=>'gray', :outlinewidth=>1,
                   :open=>:wn)

  s = t.style_create('STYLE', :orient=>:vertical)
  t.style_elements(s, [
                     'elemShadow', 'elemRect', 'elemTime',
                     'elemImg', 'elemName'
                   ])
  t.style_layout(s, 'elemShadow', :detach=>true,
                 :padx=>[1,2], :pady=>[1,2], :iexpand=>:es)
  t.style_layout(s, 'elemTime', :padx=>[2,0])
  t.style_layout(s, 'elemImg', :pady=>[0,1])
  t.style_layout(s, 'elemName', :expand=>:we, :ipady=>[0,2], :padx=>[0,3],
                 :squeeze=>:x)
  t.style_layout(s, 'elemRect', :union=>['elemTime', 'elemImg', 'elemName'],
                 :ipadx=>6, :padx=>[0,3], :pady=>[0,3])

  # Set default item style
  if $Version_1_1_OrLater
    t.defaultstyle([s])
  end

  (0..4).each{|i|
    [
      ['15:20', 'Clip 1', @images['imovie-01']],
      ['19:18', 'Clip 2', @images['imovie-02']],
      ['07:20', 'Clip 3', @images['imovie-03']],
      ['07:20', 'Clip 4', @images['imovie-04']],
      ['07:20', 'Clip 5', @images['imovie-05']],
      ['07:20', 'Clip 6', @images['imovie-06']],
      ['07:20', 'Clip 7', @images['imovie-07']]
    ].each{|time, name, image|
      item = t.item_create
      unless $Version_1_1_OrLater
        t.item_style_set(item, 0, s)
      end
      t.item_element_configure(item, 0, 'elemTime', :text=>time)
      t.item_element_configure(item, 0, 'elemName', :text=>name)
      t.item_element_configure(item, 0, 'elemImg', :image=>image)
      t.item_lastchild(:root, item)
    }
  }

  t.notify_bind(t, 'Edit-accept', proc{|w, i, c, e, tt|
                  w.item_element_configure(i, c, e, :text=>tt)
                }, '%T %I %C %E %t')

  iMovie = TkBindTag.new
  iMovie.bind('ButtonPress-1', proc{|w, x, y|
                iMovieButton1(w, x, y)
              }, '%W %x %y')

  t.bindtags = [t, iMovie, Tk::TreeCtrl, t.winfo_toplevel, TkBindTag::ALL]
end

def iMovieButton1(w, x, y)
  w.set_focus
  id = w.identify(x,y)

  if id.empty?
    # Click outside any item

  elsif id[0] == 'header'
    # Click in header
    Tk::TreeCtrl::BindCallback.buttonPress1(w, x, y)

  elsif id[0] == 'item'
    # Click in item
    Tk::TreeCtrl::BindCallback.buttonPress1(w, x, y)
    Tk.update
    where, item, arg1, arg2, arg3, arg4 = id
    case arg1
    when 'column'
      i = id[1]
      if id.length == 6
        e = id[-1]
        if e == 'elemName'
          exists = TkWinfo.exist?(w.path + '.entry')
          Tk::TreeCtrl::BindCallback.entryOpen(w, i, 0, e)
          ent = TkComm.window(w.path + '.entry')
          unless exists
            ent.configure(:borderwidth=>0, :justify=>:center,
                          :background=>'#ffdc5a')
            x1, y1, x2, y2 = w.item_bbox(i, 0, e)
            ent.place(:y=>y1 - 1)
          end
          ent.selection_clear
          x1, y1, x2, y2 = w.item_bbox(i)
          ent.place(:x=>x1 + 1, :width=>x2 - x1 - 5)
          puts "@#{x - (x1 + 1)}"
          # ent.icursor = ent.index("@#{x - (x1 + 1)}")
          ent.icursor = ent.index(TkComm._at(x - (x1 + 1)))
        end
      end
    end
  end

  Tk.callback_break
end
