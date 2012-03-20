
def demoExplorerAux(t, dir_proc, file_proc)
  base_dir = File.dirname(File.dirname(@ScriptDir))

  clicks = Tk::Clock.clicks
  globDirs = Dir.glob(File.join(base_dir, '*')).find_all{|file|
    FileTest.directory?(file)
  }
  clickGlobDirs = Tk::Clock.clicks - clicks

  clicks = Tk::Clock.clicks
  list = globDirs.sort
  clickSortDirs = Tk::Clock.clicks - clicks

  clicks = Tk::Clock.clicks
  list.each{|file| dir_proc.call(file)}
  clickAddDirs = Tk::Clock.clicks - clicks

  clicks = Tk::Clock.clicks
  globFiles = Dir.glob(File.join(base_dir, '*')).find_all{|file|
    FileTest.file?(file)
  }
  clickGlobFiles = Tk::Clock.clicks - clicks

  clicks = Tk::Clock.clicks
  list = globFiles.sort
  clickSortFiles = Tk::Clock.clicks - clicks

  clicks = Tk::Clock.clicks
  list.each{|file| file_proc.call(file)}
  clickAddFiles = Tk::Clock.clicks - clicks

  gd = '%.2g' % (clickGlobDirs / 1000000.0)
  sd = '%.2g' % (clickSortDirs / 1000000.0)
  ad = '%.2g' % (clickAddDirs  / 1000000.0)
  gf = '%.2g' % (clickGlobFiles / 1000000.0)
  sf = '%.2g' % (clickSortFiles / 1000000.0)
  af = '%.2g' % (clickAddFiles  / 1000000.0)

  puts "dirs(#{globDirs.length}) glob/sort/add #{gd}/#{sd}/#{ad}    files(#{globFiles.length}) glob/sort/add #{gf}/#{sf}/#{af}"

  @Priv[:DirCnt, t] = globDirs.length
end

#
# Demo: explorer files
#
def demoExplorerDetails(t)
  height = t.font.metrics(:linespace)
  height = 18 if height < 18

  t.configure(:showroot=>false, :showbuttons=>false, :showlines=>false,
              :itemheight=>height, :selectmode=>:extended,
              :xscrollincrement=>20, :scrollmargin=>16,
              :xscrolldelay=>[500, 50], :yscrolldelay=>[500, 50])

  init_pics('small-*')

  if $HasColumnCreate
    t.column_create(:text=>'Name', :tag=>'name',
                    :width=>200, :arrow=>:up, :arrowpad=>6)
    t.column_create(:text=>'Size', :tag=>'size', :justify=>:right,
                    :width=>60, :arrowside=>:left, :arrowgravity=>:right)
    t.column_create(:text=>'Type', :tag=>'type', :width=>120)
    t.column_create(:text=>'Modified', :tag=>'modified', :width=>130)
  else
    t.column_configure(0, :text=>'Name', :tag=>'name',
                       :width=>200, :arrow=>:up, :arrowpad=>6)
    t.column_configure(1, :text=>'Size', :tag=>'size', :justify=>:right,
                       :width=>60, :arrowside=>:left, :arrowgravity=>:right)
    t.column_configure(2, :text=>'Type', :tag=>'type', :width=>120)
    t.column_configure(3, :text=>'Modified', :tag=>'modified', :width=>130)
  end

  t.element_create('e1', :image,
                   :image=>[
                     @sel_images['small-folder'], ['selected'],
                     @images['small-folder'], []
                   ])
  t.element_create('e2', :text, :lines=>1,
                   :fill=>[@SystemHighlightText, ['selected', 'focus']])
  t.element_create('txtType', :text, :lines=>1)
  t.element_create('txtSize', :text, :lines=>1,
                   :datatype=>:integer, :format=>'%dKB')
  t.element_create('txtDate', :text, :lines=>1,
                   :datatype=>:time, :format=>'%d/%m/%y %I:%M %p')
  t.element_create('e4', :rect, :showfocus=>true,
                   :fill=>[
                     @SystemHighlight, ['selected', 'focus'],
                     'gray', ['selected', '!focus']
                   ])

  # image + text
  s = t.style_create('styName', :orient=>:horizontal)
  t.style_elements(s, ['e4', 'e1', 'e2'])
  t.style_layout(s, 'e1', :expand=>:ns)
  t.style_layout(s, 'e2', :padx=>[2,0], :squeeze=>:x, :expand=>:ns)
  t.style_layout(s, 'e4', :union=>['e2'], :iexpand=>:ns, :ipadx=>2)

  # column 1: text
  s = t.style_create('stySize')
  t.style_elements(s, ['txtSize'])
  t.style_layout(s, 'txtSize', :padx=>6, :squeeze=>:x, :expand=>:ns)

  # column 2: text
  s = t.style_create('styType')
  t.style_elements(s, ['txtType'])
  t.style_layout(s, 'txtType', :padx=>6, :squeeze=>:x, :expand=>:ns)

  # column 3: text
  s = t.style_create('styDate')
  t.style_elements(s, ['txtDate'])
  t.style_layout(s, 'txtDate', :padx=>6, :squeeze=>:x, :expand=>:ns)

  @Priv[:edit, t] = ['e2']
  @Priv[:sensitive, t] = [ ['name', 'styName', 'e1', 'e2'] ]
  @Priv[:dragimage, t] = [ ['name', 'styName', 'e1', 'e2'] ]

  t.notify_bind(t, 'Edit-accept',
                proc{|w, i, tt| w.item_text(i, 0, tt)}, '%T %I %t')

  dir_proc = proc{|file|
    item = t.item_create
    t.item_style_set(item, 0, 'styName', 2, 'styType', 3, 'styDate')
    t.item_complex(item,
                   [['e2', {:text=>File.basename(file)}]],
                   [],
                   [['txtType', {:text=>'Folder'}]],
                   [['txtDate', {:data=>File.mtime(file).tv_sec}]])
    t.item_lastchild(:root, item)
  }

  file_proc = proc{|file|
    item = t.item_create
    t.item_style_set(item, 0, 'styName', 1, 'stySize',
                           2, 'styType', 3, 'styDate')

    ext = File.extname(file)
    case ext
    when '.dll'
      img = 'small-dll'
    when '.exe'
      img = 'small-exe'
    when '.txt'
      img = 'small-txt'
    else
      img = 'small-file'
    end

    type = ext.upcase
    type = type[1..-1] << ' ' unless type.empty?
    type << 'File'

    t.item_complex(item,
                   [
                     ['e1', {:image=>[@sel_images[img], ['selected'],
                                      @images[img], []]}],
                     ['e2', {:text=>File.basename(file)}]
                   ],
                   [ ['txtSize', {:data=>File.size(file)/1024 + 1}] ],
                   [ ['txtType', {:text=>type}] ],
                   [ ['txtDate', {:data=>File.mtime(file).tv_sec}] ]
                   )
    t.item_lastchild(:root, item)
  }

  demoExplorerAux(t, dir_proc, file_proc)

  @SortColumn = 0
  t.notify_bind(t, 'Header-invoke',
                proc{|w, c| explorerHeaderInvoke(t, w, c)}, '%T %C')

  t.bindtags = [ t, 'TreeCtrlFileList', Tk::TreeCtrl, t.winfo_toplevel, :all ]
end

def explorerHeaderInvoke(t, w, c)
  if (c == @SortColumn)
    if t.column_cget(@SortColumn, :arrow) == 'down'
      order = :increasing
      arrow = :up
    else
      order = :decreasing
      arrow = :down
    end
  else
    if t.column_cget(@SortColumn, :arrow) == 'down'
      order = :decreasing
      arrow = :down
    else
      order = :increasing
      arrow = :up
    end
    t.column_configure(@SortColumn, :arrow=>:none)
    @SortColumn = c
  end

  t.column_configure(c, :arrow=>arrow)
  dirCount = TkComm.number(@Priv[:DirCnt, t])
  lastDir = dirCount - 1
  case t.column_cget(c, :tag)
  when 'name'
    if dirCount > 0
      t.item_sort(:root, order, {:last=>"root child #{lastDir}"},
                  {:column=>c, :dictionary=>true})
    end
    if dirCount < t.numitems - 1
      t.item_sort(:root, order, {:first=>"root child #{dirCount}"},
                  {:column=>c, :dictionary=>true})
    end

  when 'size'
    if dirCount < t.numitems - 1
      t.item_sort(:root, order, {:first=>"root child #{dirCount}"},
                  {:column=>c, :integer=>true},
                  {:column=>'name', :dictionary=>true})
    end

  when 'type'
    if dirCount < t.numitems - 1
      t.item_sort(:root, order, {:first=>"root child #{dirCount}"},
                  {:column=>c, :dictionary=>true},
                  {:column=>'name', :dictionary=>true})
    end

  when 'modified'
    if dirCount > 0
      t.item_sort(:root, order, {:last=>"root child #{lastDir}"},
                  {:column=>c, :integer=>true},
                  {:column=>'name', :dictionary=>true})
    end
    if dirCount < t.numitems - 1
      t.item_sort(:root, order, {:first=>"root child #{dirCount}"},
                  {:column=>c, :integer=>true},
                  {:column=>'name', :dictionary=>true})
    end

  end
end

def demoExplorerLargeIcons(t)
  # Item height is 32 for icon, 4 padding, 3 lines of text
  itemHeight = 32 + 4 + t.font.metrics(:linespace) * 3

  t.configure(:showroot=>false, :showbuttons=>false, :showlines=>false,
              :selectmode=>:extended, :wrap=>:window, :orient=>:horizontal,
              :itemheight=>itemHeight, :showheader=>false, :scrollmargin=>16,
              :xscrolldelay=>[500, 50], :yscrolldelay=>[500, 50])

  init_pics('big-*')

  if $HasColumnCreate
    t.column_create(:width=>75)
  else
    t.column_configure(0, :width=>75)
  end

  t.element_create('elemImg', :image,
                   :image=>[
                     @sel_images['big-folder'], ['selected'],
                     @images['big-folder'], []
                   ])
  t.element_create('elemTxt', :text, :justify=>:center,
                   :lines=>1, :width=>71, :wrap=>:word,
                   :fill=>[@SystemHighlightText, ['selected', 'focus']])
  t.element_create('elemSel', :rect, :showfocus=>true,
                   :fill=>[
                     @SystemHighlight, ['selected', 'focus'],
                     'gray', ['selected']
                   ])

  # image + text
  s = t.style_create('STYLE', :orient=>:vertical)
  t.style_elements(s, ['elemSel', 'elemImg', 'elemTxt'])
  t.style_layout(s, 'elemImg', :expand=>:we)
  t.style_layout(s, 'elemTxt',
                 :pady=>[4,0], :padx=>2, :squeeze=>:x, :expand=>:we)
  t.style_layout(s, 'elemSel', :union=>['elemTxt'])

  @Priv[:edit, t] = ['elemTxt']
  @Priv[:sensitive, t] = [ [0, 'STYLE', 'elemImg', 'elemTxt'] ]
  @Priv[:dragimage, t] = [ [0, 'STYLE', 'elemImg', 'elemTxt'] ]

  t.notify_bind(t, 'Edit-accept',
                proc{|w, i, tt| w.item_text(i, 0, tt)}, '%T %I %t')

  dir_proc = proc{|file|
    item = t.item_create
    t.item_style_set(item, 0, 'STYLE')
    t.item_text(item, 0, File.basename(file))
    t.item_lastchild(:root, item)
  }

  file_proc = proc{|file|
    item = t.item_create
    t.item_style_set(item, 0, 'STYLE')

    ext = File.extname(file)
    case ext
    when '.dll'
      img = 'big-dll'
    when '.exe'
      img = 'big-exe'
    when '.txt'
      img = 'big-txt'
    else
      img = 'big-file'
    end

    type = ext.upcase
    type = type[1..-1] << ' ' unless type.empty?
    type << 'File'

    t.item_complex(item,
                   [
                     ['elemImg', {:image=>[@sel_images[img], ['selected'],
                                           @images[img], []]}],
                     ['elemTxt', {:text=>File.basename(file)}]
                   ])
    t.item_lastchild(:root, item)
  }

  demoExplorerAux(t, dir_proc, file_proc)

  t.activate(t.index('root firstchild'))

  t.notify_bind(t, 'ActiveItem',
                proc{|w, a, c|
                  w.item_element_configure(a, 0, 'elemTxt', :lines=>'')
                  w.item_element_configure(c, 0, 'elemTxt', :lines=>3)
                }, '%T %p %c')

  t.bindtags = [ t, 'TreeCtrlFileList', Tk::TreeCtrl, t.winfo_toplevel, :all ]
end

# Tree is horizontal, wrapping occurs at right edge of window, each item
# is as wide as the smallest needed multiple of 110 pixels
def demoExplorerSmallIcons(t)
  demoExplorerList(t)
  t.configure(:orient=>:horizontal, :xscrollincrement=>0)
  t.column_configure(0, :width=>'', :stepwidth=>110, :widthhack=>false)
end

# Tree is vertical, wrapping occurs at bottom of window, each range has the
# same width (as wide as the longest item), xscrollincrement is by range
def demoExplorerList(t)
  height = t.font.metrics(:linespace) + 2
  height = 18 if height < 18

  t.configure(:showroot=>false, :showbuttons=>false, :showlines=>false,
              :itemheight=>height, :selectmode=>:extended, :wrap=>:window,
              :showheader=>false, :scrollmargin=>16,
              :xscrolldelay=>[500, 50], :yscrolldelay=>[500, 50])

  init_pics('small-*')

  if $HasColumnCreate
    t.column_create(:widthhack=>true)
  else
    t.column_configure(0, :widthhack=>true)
  end

  t.element_create('elemImg', :image,
                   :image=>[
                     @sel_images['small-folder'], ['selected'],
                     @images['small-folder'], []
                   ])
  t.element_create('elemTxt', :text, :lines=>1,
                   :fill=>[@SystemHighlightText, ['selected', 'focus']])
  t.element_create('elemSel', :rect, :showfocus=>true,
                   :fill=>[
                     @SystemHighlight, ['selected', 'focus'],
                     'gray', ['selected', '!focus']
                   ])

  # image + text
  s = t.style_create('STYLE')
  t.style_elements(s, ['elemSel', 'elemImg', 'elemTxt'])
  t.style_layout(s, 'elemImg', :expand=>:ns)
  t.style_layout(s, 'elemTxt', :squeeze=>:x, :expand=>:ns, :padx=>[2,0])
  t.style_layout(s, 'elemSel', :union=>['elemTxt'], :iexpand=>:ns, :ipadx=>2)

  @Priv[:edit, t] = ['elemTxt']
  @Priv[:sensitive, t] = [ [0, 'STYLE', 'elemImg', 'elemTxt'] ]
  @Priv[:dragimage, t] = [ [0, 'STYLE', 'elemImg', 'elemTxt'] ]

  t.notify_bind(t, 'Edit-accept',
                proc{|w, i, tt| w.item_text(i, 0, tt)}, '%T %I %t')

  dir_proc = proc{|file|
    item = t.item_create
    t.item_style_set(item, 0, 'STYLE')
    t.item_text(item, 0, File.basename(file))
    t.item_lastchild(:root, item)
  }

  file_proc = proc{|file|
    item = t.item_create
    t.item_style_set(item, 0, 'STYLE')

    ext = File.extname(file)
    case ext
    when '.dll'
      img = 'small-dll'
    when '.exe'
      img = 'small-exe'
    when '.txt'
      img = 'small-txt'
    else
      img = 'small-file'
    end

    type = ext.upcase
    type = type[1..-1] << ' ' unless type.empty?
    type << 'File'

    t.item_complex(item,
                   [
                     ['elemImg', {:image=>[@sel_images[img], ['selected'],
                                           @images[img], []]}],
                     ['elemTxt', {:text=>File.basename(file)}]
                   ])
    t.item_lastchild(:root, item)
  }

  demoExplorerAux(t, dir_proc, file_proc)

  t.activate(t.item_firstchild(:root))

  t.bindtags = [ t, 'TreeCtrlFileList', Tk::TreeCtrl, t.winfo_toplevel, :all ]
end
