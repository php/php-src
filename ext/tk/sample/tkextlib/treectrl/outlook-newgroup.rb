#
# Demo: Outlook Express newsgroup messages
#
def demoOutlookNewsgroup(t)
  init_pics('outlook-*')

  height = t.font.metrics(:linespace)
  height = 18 if height < 18
  t.configure(:itemheight=>height, :selectmode=>:browse, :showlines=>false,
              :showroot=>false, :showrootbutton=>false, :showbuttons=>true)

  if $Version_1_1_OrLater
    t.column_create(:image=>@images['outlook-clip'], :tag=>'clip')
    t.column_create(:image=>@images['outlook-arrow'], :tag=>'arrow')
    t.column_create(:image=>@images['outlook-watch'], :tag=>'watch')
    t.column_create(:text=>'Subject', :width=>250, :tag=>'subject')
    t.column_create(:text=>'From', :width=>150, :tag=>'from')
    t.column_create(:text=>'Sent', :width=>150, :tag=>'sent')
    t.column_create(:text=>'Size', :width=>60, :justify=>:right, :tag=>'size')
  else # TreeCtrl 1.0
    t.column_configure(0, :image=>@images['outlook-clip'], :tag=>'clip')
    t.column_configure(1, :image=>@images['outlook-arrow'], :tag=>'arrow')
    t.column_configure(2, :image=>@images['outlook-watch'], :tag=>'watch')
    t.column_configure(3, :text=>'Subject', :width=>250, :tag=>'subject')
    t.column_configure(4, :text=>'From', :width=>150, :tag=>'from')
    t.column_configure(5, :text=>'Sent', :width=>150, :tag=>'sent')
    t.column_configure(6, :text=>'Size', :width=>60, :justify=>:right,
                       :tag=>'size')
  end

  # Would be nice if I could specify a column -tag too
  t.treecolumn = 3

  # State for a read message
  t.state_define('read')

  # State for a message with unread descendants
  t.state_define('unread')

  t.element_create('elemImg', :image,
                   :image=>[
                     @sel_images['outlook-read-2'],
                          ['selected', 'read', 'unread', '!open'],
                     @images['outlook-read-2'], ['read', 'unread', '!open'],
                     @sel_images['outlook-read'], ['selected', 'read'],
                     @images['outlook-read'], ['read'],
                     @sel_images['outlook-unread'], ['selected'],
                     @images['outlook-unread'], []
                   ])
  t.element_create('elemTxt', :text, :lines=>1,
                   :fill=>[@SystemHighlightText, ['selected', 'focus']],
                   :font=>[
                     t.font.dup.weight(:bold), ['read', 'unread', '!open'],
                     t.font.dup.weight(:bold), ['!read']
                   ])
  t.element_create('sel.e', :rect, :open=>:e, :showfocus=>true,
                   :fill=>[
                     @SystemHighlight, ['selected', 'focus'],
                     'gray',  ['selected', '!focus']
                   ])
  t.element_create('sel.w', :rect, :open=>:w, :showfocus=>true,
                   :fill=>[
                     @SystemHighlight, ['selected', 'focus'],
                     'gray',  ['selected', '!focus']
                   ])
  t.element_create('sel.we', :rect, :open=>:we, :showfocus=>true,
                   :fill=>[
                     @SystemHighlight, ['selected', 'focus'],
                     'gray',  ['selected', '!focus']
                   ])

  # Image + text
  s = t.style_create('s1')
  t.style_elements(s, ['sel.e', 'elemImg', 'elemTxt'])
  t.style_layout(s, 'elemImg', :expand=>:ns)
  t.style_layout(s, 'elemTxt', :padx=>[2,6], :squeeze=>:x, :expand=>:ns)
  t.style_layout(s, 'sel.e', :union=>['elemTxt'],
                 :iexpand=>:nes, :ipadx=>[2,0])

  # Text
  s = t.style_create('s2.we')
  t.style_elements(s, ['sel.we', 'elemTxt'])
  t.style_layout(s, 'elemTxt', :padx=>6, :squeeze=>:x, :expand=>:ns)
  t.style_layout(s, 'sel.we', :detach=>true, :iexpand=>:es)

  # Text
  s = t.style_create('s2.w')
  t.style_elements(s, ['sel.w', 'elemTxt'])
  t.style_layout(s, 'elemTxt', :padx=>6, :squeeze=>:x, :expand=>:ns)
  t.style_layout(s, 'sel.w', :detach=>true, :iexpand=>:es)

  # Set default item style
  if $Version_1_1_OrLater
    t.defaultstyle = ['', '', '', 's1', 's2.we', 's2.we', 's2.w']
  end

  msgCnt = 100
  thread = 0

  @Message = Hash.new{|k, v| k[v] = Hash.new}
  @Message[:count][0] = 0

  items = [t.index(:root)]

  (1...(msgCnt)).each{|i|
    item_i = t.item_create
    item_j = nil
    j = nil
    loop {
      j = rand(i)
      item_j = items[j]
      break if j == 0
      next if t.depth(item_j) == 5
      next if @Message[:count][@Message[:thread][item_j]] == 15
      break
    }
    t.item_lastchild(item_j, item_i)

    @Message[:read][item_i] = (rand(2) == 0)
    if j == 0
      thread += 1
      @Message[:thread][item_i] = thread
      @Message[:seconds][item_i] = (Tk::Clock.seconds - rand(500000))
      @Message[:seconds2][item_i] = @Message[:seconds][item_i]
      @Message[:count][thread] = 1
    else
      @Message[:thread][item_i] = @Message[:thread][item_j]
      @Message[:seconds][item_i] = (@Message[:seconds2][item_j] + rand(10000))
      @Message[:seconds2][item_i] = @Message[:seconds][item_i]
      @Message[:seconds2][item_j] = @Message[:seconds][item_i]
      @Message[:count][@Message[:thread][item_j]] += 1
    end
    items << item_i
  }

  (1...(msgCnt)).each{|i|
    item_i = items[i]
    subject = "This is thread number #{@Message[:thread][item_i]}"
    from = 'somebody@somewhere.net'
    sent = Tk::Clock.format(@Message[:seconds][item_i], "%d/%m/%y %I:%M %p")
    size = "#{1 + rand(10)}KB"

    # This message has been read
    t.item_state_set(item_i, 'read') if @Message[:read][item_i]

    # This message has unread descendants
    t.item_state_set(item_i, 'unread')  if anyUnreadDescendants(t, item_i)

    if t.item_numchildren(item_i) > 0
      if $Version_1_1_OrLater
        t.item_configure(item_i, :button=>true)
      else # TreeCtrl 1.0
        t.item_hasbutton(item_i, true)
      end

      # Collapse some messages
      if $Version_1_1_OrLater
        t.item_collapse(item_i) if rand(2) == 0
      else # TreeCtrl 1.0
        t.collapse(item_i) if rand(2) == 0
      end
    end

    unless $Version_1_1_OrLater
      t.item_style_set(item_i, 3, 's1', 4, 's2.we', 5, 's2.we', 6, 's2.w')
    end
    t.item_text(item_i, 3, subject, 4, from, 5, sent, 6, size)
  }

  # Do something when the selection changes
  t.notify_bind(t, 'Selection',
                proc{|w|
                  if w.selection_count == 1
                    # One item is selected
                    if @Message[:afterId][:id]
                      Tk.after_cancel(@Message[:afterId][:id])
                    end
                    @Message[:afterId][:item] = w.selection_get[0]
                    @Message[:afterId][:id] = Tk.after(500, proc{
                                                         messageReadDelayed(w)
                                                       })
                  end
                }, '%T')
end

def messageReadDelayed(t)
  @Message[:afterId].delete(:id)
  i = @Message[:afterId][:item]
  return unless t.selection_includes(i)

  # This message is not read
  unless @Message[:read][i]
    # Read the message
    t.item_state_set(i, 'read')
    @Message[:read][i] = true

    # Check ancestors (except root)
    t.item_ancestors(i)[0..-2].each{|i2|
      # This ancestor has no more unread descendants
      t.item_state_set(i2, '!unread') unless anyUnreadDescendants(t, i2)
    }
  end
end

# Alternate implementation which does not rely on run-time states
def demoOutlookNewsgroup2(t)
  init_pics('outlook-*')

  height = t.font.metrics(:linespace)
  height = 18 if height < 18
  t.configure(:itemheight=>height, :selectmode=>:browse, :showlines=>false,
              :showroot=>false, :showrootbutton=>false, :showbuttons=>true)

  if $Version_1_1_OrLater
    t.column_create(:image=>@images['outlook-clip'], :tag=>'clip')
    t.column_create(:image=>@images['outlook-arrow'], :tag=>'arrow')
    t.column_create(:image=>@images['outlook-watch'], :tag=>'watch')
    t.column_create(:text=>'Subject', :width=>250, :tag=>'subject')
    t.column_create(:text=>'From', :width=>150, :tag=>'from')
    t.column_create(:text=>'Sent', :width=>150, :tag=>'sent')
    t.column_create(:text=>'Size', :width=>60, :justify=>:right, :tag=>'size')
  else # TreeCtrl 1.0
    t.column_configure(0, :image=>@images['outlook-clip'], :tag=>'clip')
    t.column_configure(1, :image=>@images['outlook-arrow'], :tag=>'arrow')
    t.column_configure(2, :image=>@images['outlook-watch'], :tag=>'watch')
    t.column_configure(3, :text=>'Subject', :width=>250, :tag=>'subject')
    t.column_configure(4, :text=>'From', :width=>150, :tag=>'from')
    t.column_configure(5, :text=>'Sent', :width=>150, :tag=>'sent')
    t.column_configure(6, :text=>'Size', :width=>60, :justify=>:right,
                       :tag=>'size')
  end

  t.treecolumn = 3

  t.element_create('image.unread', :image, :image=>@images['outlook-unread'])
  t.element_create('image.read', :image, :image=>@images['outlook-read'])
  t.element_create('image.read2', :image, :image=>@images['outlook-read-2'])
  t.element_create('text.read', :text, :lines=>1,
                   :fill=>[@SystemHighlightText, ['selected', 'focus']])
  t.element_create('text.unread', :text, :lines=>1,
                   :fill=>[@SystemHighlightText, ['selected', 'focus']],
                   :font=>t.font.dup.weight(:bold))
  t.element_create('sel.e', :rect, :open=>:e, :showfocus=>true,
                   :fill=>[
                     @SystemHighlight, ['selected', 'focus'],
                     'gray',  ['selected', '!focus']
                   ])
  t.element_create('sel.w', :rect, :open=>:w, :showfocus=>true,
                   :fill=>[
                     @SystemHighlight, ['selected', 'focus'],
                     'gray',  ['selected', '!focus']
                   ])
  t.element_create('sel.we', :rect, :open=>:we, :showfocus=>true,
                   :fill=>[
                     @SystemHighlight, ['selected', 'focus'],
                     'gray',  ['selected', '!focus']
                   ])

  # Image + text
  s = t.style_create('unread')
  t.style_elements(s, ['sel.e', 'image.unread', 'text.unread'])
  t.style_layout(s, 'image.unread', :expand=>:ns)
  t.style_layout(s, 'text.unread', :padx=>[2,6], :squeeze=>:x, :expand=>:ns)
  t.style_layout(s, 'sel.e', :union=>['text.unread'],
                 :iexpand=>:nes, :ipadx=>[2,0])

  # Image + text
  s = t.style_create('read')
  t.style_elements(s, ['sel.e', 'image.read', 'text.read'])
  t.style_layout(s, 'image.read', :expand=>:ns)
  t.style_layout(s, 'text.read', :padx=>[2,6], :squeeze=>:x, :expand=>:ns)
  t.style_layout(s, 'sel.e', :union=>['text.read'],
                 :iexpand=>:nes, :ipadx=>[2,0])

  # Image + text
  s = t.style_create('read2')
  t.style_elements(s, ['sel.e', 'image.read2', 'text.unread'])
  t.style_layout(s, 'image.read2', :expand=>:ns)
  t.style_layout(s, 'text.unread', :padx=>[2,6], :squeeze=>:x, :expand=>:ns)
  t.style_layout(s, 'sel.e', :union=>['text.unread'],
                 :iexpand=>:nes, :ipadx=>[2,0])

  # Text
  s = t.style_create('unread.we')
  t.style_elements(s, ['sel.we', 'text.unread'])
  t.style_layout(s, 'text.unread', :padx=>6, :squeeze=>:x, :expand=>:ns)
  t.style_layout(s, 'sel.we', :detach=>true, :iexpand=>:es)

  # Text
  s = t.style_create('read.we')
  t.style_elements(s, ['sel.we', 'text.read'])
  t.style_layout(s, 'text.read', :padx=>6, :squeeze=>:x, :expand=>:ns)
  t.style_layout(s, 'sel.we', :detach=>true, :iexpand=>:es)

  # Text
  s = t.style_create('unread.w')
  t.style_elements(s, ['sel.w', 'text.unread'])
  t.style_layout(s, 'text.unread', :padx=>6, :squeeze=>:x, :expand=>:ns)
  t.style_layout(s, 'sel.w', :detach=>true, :iexpand=>:es)

  # Text
  s = t.style_create('read.w')
  t.style_elements(s, ['sel.w', 'text.read'])
  t.style_layout(s, 'text.read', :padx=>6, :squeeze=>:x, :expand=>:ns)
  t.style_layout(s, 'sel.w', :detach=>true, :iexpand=>:es)

  msgCnt = 100
  thread = 0

  @Message = Hash.new{|k, v| k[v] = Hash.new}
  @Message[:count][0] = 0

  (1...(msgCnt)).each{|i|
    t.item_create
    j = nil
    loop {
      j = rand(i)
      break if j == 0
      next if t.depth(j) == 5
      next if @Message[:count][@Message[:thread][j]] == 15
      break
    }
    t.item_lastchild(j, i)

    @Message[:read][i] = (rand(2) == 0)
    if j == 0
      thread += 1
      @Message[:thread][i] = thread
      @Message[:seconds][i] = (Tk::Clock.seconds - rand(500000))
      @Message[:seconds2][i] = @Message[:seconds][i]
      @Message[:count][thread] = 1
    else
      @Message[:thread][i] = @Message[:thread][j]
      @Message[:seconds][i] = (@Message[:seconds2][j] + rand(10000))
      @Message[:seconds2][i] = @Message[:seconds][i]
      @Message[:seconds2][j] = @Message[:seconds][i]
      @Message[:count][@Message[:thread][j]] += 1
    end
  }

  (1...(msgCnt)).each{|i|
    subject = "This is thread number #{@Message[:thread][i]}"
    from = 'somebody@somewhere.net'
    sent = Tk::Clock.format(@Message[:seconds][i], "%d/%m/%y %I:%M %p")
    size = "#{1 + rand(10)}KB"
    if @Message[:read][i]
      style = 'read'
      style2 = 'read2'
    else
      style = 'unread'
      style2 = 'unread2'
    end
    t.item_style_set(i, 3, style, 4, "#{style2}.we", 5, "#{style2}.we",
                     6, "#{style2}.w")
    t.item_text(i, 3, subject, 4, from, 5, sent, 6, size)
    if t.item_numchildren(i) > 0
      t.item_configure(item_i, :button=>true)
    end
  }

  # Do something when the selection changes
  t.notify_bind(t, 'Selection',
                proc{|w|
                  if w.selection_count == 1
                    i = t.selection_get[0]
                    unless @Message[:read][i]
                      if t.item_isopen(i) || !anyUnreadDescendants(t, i)
                        # unread -> read
                        t.item_style_map(i, 'subject', 'read',
                                         ['text.unread', 'text.read'])
                        t.item_style_map(i, 'from', 'read.we',
                                         ['text.unread', 'text.read'])
                        t.item_style_map(i, 'sent', 'read.we',
                                         ['text.unread', 'text.read'])
                        t.item_style_map(i, 'size', 'read.w',
                                         ['text.unread', 'text.read'])
                      else
                        # unread -> read2
                        t.item_style_map(i, 'subject', 'read2',
                                         ['text.unread', 'text.unread'])
                      end

                      @Message[:read][i] = true
                      @display_styles_in_item.call(i)
                    end
                  end
                }, '%T')

  t.notify_bind(t, 'Expand-after',
                proc{|w, i|
                  if @Messge[:read][i] && anyUnreadDescendants(t, i)
                    # read2 -> read
                    t.item_style_map(i, 'subject', 'read',
                                     ['text.unread', 'text.read'])
                    # unread -> read
                    t.item_style_map(i, 'from', 'read.we',
                                     ['text.unread', 'text.read'])
                    t.item_style_map(i, 'sent', 'read.we',
                                     ['text.unread', 'text.read'])
                    t.item_style_map(i, 'size', 'read.w',
                                     ['text.unread', 'text.read'])
                  end
                }, '%T %I')

  t.notify_bind(t, 'Collapse-after',
                proc{|w, i|
                  if @Messge[:read][i] && anyUnreadDescendants(t, i)
                    # read -> read2
                    t.item_style_map(i, 'subject', 'read2',
                                     ['text.read', 'text.unread'])
                    # read -> unread
                    t.item_style_map(i, 'from', 'unread.we',
                                     ['text.read', 'text.unread'])
                    t.item_style_map(i, 'sent', 'unread.we',
                                     ['text.read', 'text.unread'])
                    t.item_style_map(i, 'size', 'unread.w',
                                     ['text.read', 'text.unread'])
                  end
                }, '%T %I')

  (1...(msgCnt)).each{|i|
    if rand(2) == 0
      if t.item_numchildren(i) > 0
        if $Version_1_1_OrLater
          t.item_collapse(i)
        else # TreeCtrl 1.0
          t.collapse(i)
        end
      end
    end
  }
end

def anyUnreadDescendants(t, i)
  itemList = []
  item = t.item_firstchild(i)
  itemList.push(item) if item != ''

  while item = itemList.pop
    return true unless @Message[:read][item]

    item2 = t.item_nextsibling(item)
    itemList.push(item2) if item2 != ''
    item2 = t.item_firstchild(item)
    itemList.push(item2) if item2 != ''
  end
  false
end
