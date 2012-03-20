#
# Demo: Layout
#
def demoLayout(t)
  t.configure(:showroot=>false, :showrootbutton=>true, :showbuttons=>true,
              :showlines=>true, :itemheight=>0, :selectmode=>:browse)

  if $HasColumnCreate
    t.column_create(:text=>'Layout')
  else
    t.column_configure(0, :text=>'Layout')
  end

  t.element_create('e1', :rect, :width=>30, :height=>30, :fill=>'gray20')
  t.element_create('e2', :rect, :width=>30, :height=>30, :fill=>'gray40',
                   :outline=>'blue', :outlinewidth=>3)
  t.element_create('e3', :rect, :fill=>'gray60')
  t.element_create('e4', :rect, :showfocus=>true,
                   :fill=>[
                     @SystemHighlight, ['selected', 'focus'], 'gray80', []
                   ])
  t.element_create('e5', :rect, :fill=>'{sky blue}', :width=>20, :height=>20)
  t.element_create('e6', :rect, :fill=>'{sea green}', :width=>30, :height=>16)
  t.element_create('e7', :rect, :fill=>'{sky blue}', :width=>30, :height=>16)
  t.element_create('e8', :rect, :fill=>'gray70', :height=>1)

  s = t.style_create('s1')
  t.style_elements(s, ['e4', 'e3', 'e1', 'e2', 'e5', 'e6', 'e7'])
  t.style_layout(s, 'e1', :padx=>[28, 4], :pady=>4)
  t.style_layout(s, 'e2', :expand=>:es, :padx=>[0, 38])
  t.style_layout(s, 'e3', :union=>['e1', 'e2'], :ipadx=>4, :ipady=>4, :pady=>2)
  t.style_layout(s, 'e4', :detach=>true, :iexpand=>:es)
  t.style_layout(s, 'e5', :detach=>true, :padx=>[2,0], :pady=>2, :iexpand=>:s)
  t.style_layout(s, 'e6', :detach=>true, :expand=>:ws,
                 :padx=>[0,2], :pady=>[2,0])
  t.style_layout(s, 'e7', :detach=>true, :expand=>:wn,
                 :padx=>[0,2], :pady=>[0,2])

  if $Version_1_1_OrLater
    i = t.item_create(:button=>true)
  else
    i = t.item_create
    t.item_hasbutton(i, true)
  end
  t.item_style_set(i, 0, s)
  t.item_lastchild(:root, i)
  parent = i

  i = t.item_create()
  unless $Version_1_1_OrLater
    t.item_hasbutton(i, false)
  end
  t.item_style_set(i, 0, s)
  t.item_lastchild(parent, i)

  ###

  s = t.style_create('s2')
  t.style_elements(s, ['e4', 'e3', 'e1'])
  t.style_layout(s, 'e1', :padx=>8, :pady=>8, :iexpand=>:e)
  t.style_layout(s, 'e3', :union=>['e1'], :ipadx=>[20,4], :ipady=>[4,12])
  t.style_layout(s, 'e4', :detach=>true, :iexpand=>:es)

  if $Version_1_1_OrLater
    i = t.item_create(:button=>true)
  else
    i = t.item_create
    t.item_hasbutton(i, true)
  end
  t.item_style_set(i, 0, s)
  t.item_lastchild(:root, i)

  i2 = t.item_create()
  unless $Version_1_1_OrLater
    t.item_hasbutton(i2, false)
  end
  t.item_style_set(i2, 0, s)
  t.item_lastchild(i, i2)

  ###

  s = t.style_create('s3')
  t.style_elements(s, ['e4', 'e3', 'e1', 'e5', 'e6'])
  t.style_layout(s, 'e4', :union=>['e1', 'e6'], :ipadx=>8, :ipady=>[8,0])
  t.style_layout(s, 'e3', :union=>['e1', 'e5'], :ipadx=>4, :ipady=>4)
  t.style_layout(s, 'e5', :ipady=>[0,20])

  if $Version_1_1_OrLater
    i = t.item_create(:button=>true)
  else
    i = t.item_create
    t.item_hasbutton(i, true)
  end
  t.item_style_set(i, 0, s)
  t.item_lastchild(:root, i)

  i2 = t.item_create()
  unless $Version_1_1_OrLater
    t.item_hasbutton(i2, false)
  end
  t.item_style_set(i2, 0, s)
  t.item_lastchild(i, i2)

  ###

  t.element_create('eb', :border, :background=>@SystemButtonFace,
                   :relief=>[:sunken, ['selected'], :raised, []],
                   :thickness=>2, :filled=>true)
  t.element_create('et', :text)

  text = "Here is a text element surrounded by a border element.\nResize the column to watch me wrap."

  s = t.style_create('e4')
  t.style_elements(s, ['eb', 'et'])
  t.style_layout(s, 'eb', :union=>['et'], :ipadx=>2, :ipady=>2)
  t.style_layout(s, 'et', :squeeze=>:x)

  if $Version_1_1_OrLater
    i = t.item_create(:button=>true)
  else
    i = t.item_create
    t.item_hasbutton(i, true)
  end
  t.item_style_set(i, 0, s)
  t.item_text(i, 0, text)
  t.item_lastchild(:root, i)
  parent = i

  i = t.item_create()
  unless $Version_1_1_OrLater
    t.item_hasbutton(i, false)
  end
  t.item_style_set(i, 0, s)
  t.item_text(i, 0, text)
  t.item_lastchild(parent, i)

  ###

  styleNum = 5
  [
    [:horizontal, [:s, :ns, :n]],
    [:vertical,   [:e, :we, :w]]
  ].each{|orient, expandList|
    expandList.each{|expand|
      s = t.style_create("s#{styleNum}", :orient=>orient)
      t.style_elements(s, ['e4', 'e8', 'e2', 'e5', 'e6'])
      t.style_layout(s, 'e4', :detach=>true, :iexpand=>:es)
      t.style_layout(s, 'e8', :detach=>true, :expand=>:n, :iexpand=>:e)
      t.style_layout(s, 'e2', :expand=>expand)
      t.style_layout(s, 'e5', :expand=>expand)
      t.style_layout(s, 'e6', :expand=>expand)
      styleNum += 1

      i = t.item_create()
      t.item_style_set(i, 0, s)
      t.item_lastchild(:root, i)
    }
  }
end
