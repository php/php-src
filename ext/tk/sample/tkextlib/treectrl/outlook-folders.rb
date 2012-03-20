#
# Demo: Outlook Express folder list
#
def demoOutlookFolders(t)
  init_pics('outlook-*')

  height = t.font.metrics(:linespace) + 2
  height = 18 if height < 18

  t.configure(:itemheight=>height, :selectmode=>:browse, :showlines=>true,
              :showroot=>true, :showrootbutton=>false, :showbuttons=>true)

  if $HasColumnCreate
    t.column_create(:text=>'Folders')
  else
    t.column_configure(0, :text=>'Folders')
  end

  t.element_create('e1', :image)
  t.element_create('e2', :text, :lines=>1,
                   :fill=>[@SystemHighlightText, ['selected', 'focus']])
  t.element_create('e3', :text, :lines=>1, :font=>t.font.dup.weight(:bold),
                   :fill=>[@SystemHighlightText, ['selected', 'focus']])
  t.element_create('e4', :text, :fill=>'blue')
  t.element_create('e5', :image, :image=>@images['outlook-folder'])
  t.element_create('e6', :rect, :showfocus=>true,
                   :fill=>[
                     @SystemHighlight, ['selected', 'focus'],
                     'gray', ['selected', '!focus']
                   ])

  # image + text
  s = t.style_create('s1')
  t.style_elements(s, ['e6', 'e1', 'e2'])
  t.style_layout(s, 'e1', :expand=>:ns)
  t.style_layout(s, 'e2', :padx=>[4,0], :expand=>:ns, :squeeze=>:x)
  t.style_layout(s, 'e6', :union=>['e2'], :iexpand=>:ns, :ipadx=>2)

  # image + text + text
  s = t.style_create('s2')
  t.style_elements(s, ['e6', 'e1', 'e3', 'e4'])
  t.style_layout(s, 'e1', :expand=>:ns)
  t.style_layout(s, 'e3', :padx=>4, :expand=>:ns, :squeeze=>:x)
  t.style_layout(s, 'e4', :expand=>:ns)
  t.style_layout(s, 'e6', :union=>['e3'], :iexpand=>:ns, :ipadx=>2)

  # folder + text
  s = t.style_create('s3')
  t.style_elements(s, ['e6', 'e5', 'e2'])
  t.style_layout(s, 'e5', :expand=>:ns)
  t.style_layout(s, 'e2', :padx=>[4,0], :expand=>:ns, :squeeze=>:x)
  t.style_layout(s, 'e6', :union=>['e2'], :iexpand=>:ns, :ipadx=>2)

  # folder + text + text
  s = t.style_create('s4')
  t.style_elements(s, ['e6', 'e5', 'e3', 'e4'])
  t.style_layout(s, 'e5', :expand=>:ns)
  t.style_layout(s, 'e3', :padx=>4, :expand=>:ns, :squeeze=>:x)
  t.style_layout(s, 'e4', :expand=>:ns)
  t.style_layout(s, 'e6', :union=>['e3'], :iexpand=>:ns, :ipadx=>2)

  t.item_style_set(:root, 0, 's1')
  t.item_complex(:root,
                 [
                   ['e1', {:image=>@images['outlook-main']}],
                   ['e2', {:text=>'Outlook Express'}]
                 ])

  parentList = [:root, '', '', '', '', '', '']
  parent = :root
  [
     [0, :local, "Local Folders", true, 0],
        [1, :inbox, 'Inbox', false, 5],
        [1, :outbox, 'Outbox', false, 0],
        [1, :sent, "Sent Items", false, 0],
        [1, :deleted, "Deleted Items", false, 50],
        [1, :draft, 'Drafts', false, 0],
        [1, :folder, "Messages to Dad", false, 0],
        [1, :folder, "Messages to Sis", false, 0],
        [1, :folder, "Messages to Me", false, 0],
           [2, :folder, "2001", false, 0],
           [2, :folder, "2000", false, 0],
           [2, :folder, "1999", false, 0],
     [0, :server, "news.gmane.org", true, 0],
        [1, :group, "gmane.comp.lang.lua.general", false, 498]
  ].each{|depth, img, text, button, unread|
    if $Version_1_1_OrLater
      item = t.item_create(:button=>button)
    else
      item = t.item_create
      t.item_hasbutton(item, button)
    end
    if img == :folder
      if unread != 0
        t.item_style_set(item, 0, 's4')
        t.item_complex(item,
                       [['e3', {:text=>text}], ['e4', {:text=>"(#{unread})"}]])
      else
        t.item_style_set(item, 0, 's3')
        t.item_complex(item, [['e2', {:text=>text}]])
      end
    else
      if unread != 0
        t.item_style_set(item, 0, 's2')
        t.item_complex(item,
                       [
                         ['e1', {:image=>@images["outlook-#{img}"]}],
                         ['e3', {:text=>text}],
                         ['e4', {:text=>"(#{unread})"}]
                       ])
      else
        t.item_style_set(item, 0, 's1')
        t.item_complex(item,
                       [
                         ['e1', {:image=>@images["outlook-#{img}"]}],
                         ['e2', {:text=>text}]
                       ])
      end
    end
    t.item_lastchild(parentList[depth], item)
    depth += 1
    parentList[depth] = item
  }
end
