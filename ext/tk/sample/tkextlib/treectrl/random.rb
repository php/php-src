#
def random_N
  @RandomN[0] || 500
end

#
# Demo: random N items
#
def demoRandom(t)
  init_pics('folder-*', 'small-*')

  height = t.font.metrics(:linespace)
  height = 18 if height < 18
  t.configure(:itemheight=>height, :selectmode=>:extended,
              :showroot=>true, :showrootbutton=>true, :showbuttons=>true,
              :showlines=>true, :scrollmargin=>16,
              :xscrolldelay=>[500, 50], :yscrolldelay=>[500, 50])

  if $Version_1_1_OrLater
    t.column_create(:expand=>true, :text=>'Item',
                    :itembackground=>['#e0e8f0', []], :tag=>'item')
    t.column_create(:text=>'Parent', :justify=>:center,
                    :itembackground=>['gray90', []], :tag=>'parent')
    t.column_create(:text=>'Depth', :justify=>:center,
                    :itembackground=>['linen', []], :tag=>'depth')
  else # TreeCtrl 1.0
    t.column_configure(0, :expand=>true, :text=>'Item',
                       :itembackground=>['#e0e8f0', []], :tag=>'item')
    t.column_configure(1, :text=>'Parent', :justify=>:center,
                       :itembackground=>['gray90', []], :tag=>'parent')
    t.column_configure(2, :text=>'Depth', :justify=>:center,
                       :itembackground=>['linen', []], :tag=>'depth')
  end

  t.element_create('e1', :image, :image=>[
                     @images['folder-open'], ['open'],
                     @images['folder-closed'], []
                   ])
  t.element_create('e2', :image, :image=>@images['small-file'])
  t.element_create('e3', :text,
                   :fill=>[@SystemHighlightText, ['selected', 'focus']])
  t.element_create('e4', :text, :fill=>'blue')
  t.element_create('e6', :text)
  t.element_create('e5', :rect, :showfocus=>true,
                   :fill=>[
                     @SystemHighlight, ['selected', 'focus'],
                     'gray', ['selected', '!focus']
                   ])

  s = t.style_create('s1')
  t.style_elements(s, ['e5', 'e1', 'e3', 'e4'])
  t.style_layout(s, 'e1', :padx=>[0,4], :expand=>:ns)
  t.style_layout(s, 'e3', :padx=>[0,4], :expand=>:ns)
  t.style_layout(s, 'e4', :padx=>[0,6], :expand=>:ns)
  t.style_layout(s, 'e5', :union=>['e3'], :iexpand=>:ns, :ipadx=>2)

  s = t.style_create('s2')
  t.style_elements(s, ['e5', 'e2', 'e3'])
  t.style_layout(s, 'e2', :padx=>[0,4], :expand=>:ns)
  t.style_layout(s, 'e3', :padx=>[0,4], :expand=>:ns)
  t.style_layout(s, 'e5', :union=>['e3'], :iexpand=>:ns, :ipadx=>2)

  s = t.style_create('s3')
  t.style_elements(s, ['e6'])
  t.style_layout(s, 'e6', :padx=>6, :expand=>:ns)

  @Priv[:sensitive, t] = [
    [:item, 's1',  'e5', 'e1', 'e3'],
    [:item, 's2',  'e5', 'e2', 'e3']
  ]
  @Priv[:dragimage, t] = [
    [:item, 's1',  'e1', 'e3'],
    [:item, 's2',  'e2', 'e3']
  ]

  clicks = Tk::Clock.clicks
  items = [ t.index(:root) ]
  (1...(random_N())).each{|i|
    item_i = t.item_create
    item_j = nil
    loop {
      j = rand(i)
      item_j = items[j]
      break if t.depth(item_j) < 5
    }
    if $Version_1_1_OrLater
      t.item_collapse(item_i) if rand(2) == 0
    else # TreeCtrl 1.0
      t.collapse(item_i) if rand(2) == 0
    end
    if rand(2) == 0
      t.item_lastchild(item_j, item_i)
    else
      t.item_firstchild(item_j, item_i)
    end
    items << item_i
  }
  puts "created #{random_N() - 1} items in #{Tk::Clock.clicks - clicks} clicks"

  clicks = Tk::Clock.clicks
  (0...(random_N())).each{|i|
    item_i = items[i]
    numChildren = t.item_numchildren(item_i)
    if numChildren > 0
      if $Version_1_1_OrLater
        t.item_configure(item_i, :button=>true)
      else # TreeCtrl 1.0
        t.item_hasbutton(item_i, true)
      end
      t.item_style_set(item_i, 0, 's1', 1, 's3', 2, 's3')
      t.item_complex(item_i,
                     [ ['e3', {:text=>"Item #{i}"}],
                       ['e4', {:text=>"(#{numChildren})"}] ],
                     [ ['e6', {:text=>"#{t.item_parent(item_i)}"}] ],
                     [ ['e6', {:text=>"#{t.depth(item_i)}"}] ])
    else
      t.item_style_set(item_i, 1, 's3', 2, 's3', 0, 's2')
      t.item_complex(item_i,
                     [ ['e3', {:text=>"Item #{i}"}] ],
                     [ ['e6', {:text=>"#{t.item_parent(item_i)}"}] ],
                     [ ['e6', {:text=>"#{t.depth(item_i)}"}] ])
    end
  }
  puts "configured #{random_N()} items in #{Tk::Clock.clicks - clicks} clicks"

  treeCtrlRandom = TkBindTag.new

  treeCtrlRandom.bind('Double-ButtonPress-1',
                      proc{|w, x, y|
                        Tk::TreeCtrl::BindCallback.doubleButton1(w, x, y)
                        Tk.callback_break
                      }, '%W %x %y')

  treeCtrlRandom.bind('Control-ButtonPress-1',
                      proc{|w, x, y|
                        @Priv['selectMode'] = :toggle
                        randomButton1(w, x, y)
                        Tk.callback_break
                      }, '%W %x %y')

  treeCtrlRandom.bind('Shift-ButtonPress-1',
                      proc{|w, x, y|
                        @Priv['selectMode'] = :add
                        randomButton1(w, x, y)
                        Tk.callback_break
                      }, '%W %x %y')

  treeCtrlRandom.bind('ButtonPress-1',
                      proc{|w, x, y|
                        @Priv['selectMode'] = :set
                        randomButton1(w, x, y)
                        Tk.callback_break
                      }, '%W %x %y')

  treeCtrlRandom.bind('Button1-Motion',
                      proc{|w, x, y|
                        randomMotion1(w, x, y)
                        Tk.callback_break
                      }, '%W %x %y')

  treeCtrlRandom.bind('Button1-Leave',
                      proc{|w, x, y|
                        randomLeave1(w, x, y)
                        Tk.callback_break
                      }, '%W %x %y')

  treeCtrlRandom.bind('ButtonRelease-1',
                      proc{|w, x, y|
                        randomRelease1(w, x, y)
                        Tk.callback_break
                      }, '%W %x %y')

  t.bindtags = [ t, treeCtrlRandom, Tk::TreeCtrl, t.winfo_toplevel, :all ]
end

def randomButton1(t, x, y)
  t.set_focus
  id = t.identify(x, y)
  puts id.inspect
  @Priv['buttonMode'] = ''

  # Click outside any item
  if id.empty?
    t.selection_clear

  # Click in header
  elsif id[0] == 'header'
    Tk::TreeCtrl::BindCallback.buttonPress1(t, x, y)

  # Click in item
  else
    where, item, arg1, arg2, arg3, arg4 = id
    case arg1
    when 'button'
      if $Version_1_1_OrLater
        t.item_toggle(item)
      else # TreeCtrl 1.0
        t.toggle(item)
      end

    when 'line'
      if $Version_1_1_OrLater
        t.item_toggle(arg2)
      else # TreeCtrl 1.0
        t.toggle(arg2)
      end

    when 'column'
      ok = false
      # Clicked an element
      if id.length == 6
        column = id[3]
        e = id[5]
        @Priv.list_element(:sensitive, t).each{|lst|
          c, s, *eList = TkComm.simplelist(lst)
          next if column != t.column_index(c)
          next if t.item_style_set(item, c) != s
          next if eList.find{|le| le == e} == nil
          ok = true
          break
        }
      end
      unless ok
        t.selection_clear
        return
      end

      @Priv[:drag, :motion] = 0
      @Priv[:drag, :x] = t.canvasx(x)
      @Priv[:drag, :y] = t.canvasy(y)
      @Priv[:drop] = ''

      if @Priv['selectMode'] == 'add'
          Tk::TreeCtrl::BindCallback.beginExtend(t, item)
      elsif @Priv['selectMode'] == 'toggle'
          Tk::TreeCtrl::BindCallback.beginToggle(t, item)
      elsif ! t.selection_includes(item)
          Tk::TreeCtrl::BindCallback.beginSelect(t, item)
      end
      t.activate(item)

      if t.selection_includes(item)
        @Priv['buttonMode'] = 'drag'
      end
    end
  end
end

def randomMotion1(t, x, y)
  case @Priv['buttonMode']
  when 'resize', 'header'
    Tk::TreeCtrl::BindCallback.motion1(t, x, y)
  when 'drag'
    randomAutoScanCheck(t, x, y)
    randomMotion(t, x, y)
  end
end

def randomMotion(t, x, y)
  case @Priv['buttonMode']
  when 'resize', 'header'
    Tk::TreeCtrl::BindCallback.motion1(t, x, y)

  when 'drag'
    # Detect initial mouse movement
    unless @Priv.bool_element(:drag, :motion)
      @Priv[:selection] = t.selection_get
      @Priv[:drop] = ''
      t.dragimage_clear
      # For each selected item, add 2nd and 3rd elements of
      # column "item" to the dragimage
      @Priv.list_element(:selection).each{|i|
        @Priv.list_element(:dragimage,t).each{|lst|
          c, s, *eList = TkComm.simplelist(lst)
          if t.item_style_set(i, c) == s
            t.dragimage_add(i, c, *eList)
          end
        }
      }
      @Priv[:drag,:motion] = true
    end

    # Find the item under the cursor
    cursor = 'X_cursor'
    drop = ''
    id = t.identify(x, y)
    ok = false
    if !id.empty? && id[0] == 'item' && id.length == 6
      item = id[1]
      column = id[3]
      e = id[5]
      @Priv.list_element(:sensitive,t).each{|lst|
        c, s, *eList = TkComm.simplelist(lst)
        next if column != t.column_index(c)
        next if t.item_style_set(item, c) != s
        next unless eList.find{|val| val.to_s == e.to_s}
        ok = true
        break
      }
      ok = true if @Priv.list_element(:sensitive,t).find{|val| TkComm.simplelist(val).index(e)}
    end

    if ok
      # If the item is not in the pre-drag selection
      # (i.e. not being dragged) see if we can drop on it
      unless @Priv.list_element(:selection).find{|val| val.to_s == item.to_s}
        drop = item
        # We can drop if dragged item isn't an ancestor
        @Priv.list_element(:selection).each{|item2|
          if t.item_isancestor(item2, item)
            drop = ''
            break
          end
        }
        if drop != ''
          x1, y1, x2, y2 = t.item_bbox(drop)
          if y < y1 + 3
            cursor = 'top_side'
            @Priv[:drop,:pos] = 'prevsibling'
          elsif y >= y2 - 3
            cursor = 'bottom_side'
            @Priv[:drop,:pos] = 'nextsibling'
          else
            cursor = ''
            @Priv[:drop,:pos] = 'lastchild'
          end
        end
      end
    end

    t[:cursor] = cursor if t[:cursor] != cursor

    # Select the item under the cursor (if any) and deselect
    # the previous drop-item (if any)
    t.selection_modify(drop, @Priv[:drop])
    @Priv[:drop] = drop

    # Show the dragimage in its new position
    x = t.canvasx(x) - @Priv.numeric_element(:drag,:x)
    y = t.canvasx(y) - @Priv.numeric_element(:drag,:y)
    t.dragimage_offset(x, y)
    t.dragimage_configure(:visible=>true)
  end
end

def randomLeave1(t, x, y)
  # This is called when I do ButtonPress-1 on Unix for some reason,
  # and buttonMode is undefined.
  return unless @Priv.exist?('buttonMode')
  case @Priv['buttonMode']
  when 'header'
    Tk::TreeCtrl::BindCallback.leave1(t, x, y)
  end
end

def randomRelease1(t, x, y)
  case @Priv['buttonMode']
  when 'resize', 'header'
    Tk::TreeCtrl::BindCallback.release1(t, x, y)
  when 'drag'
    Tk::TreeCtrl::BindCallback.autoScanCancel(t)
    t.dragimage_configure(:visible=>false)
    t.selection_modify('', @Priv[:drop])
    t[:cursor] = ''
    if @Priv[:drop] != ''
      randomDrop(t, @Priv[:drop], @Priv.list_element(:selection),
                 @Priv[:drop, :pos])
    end
  end
  @Priv['buttonMode'] = ''
end

def randomDrop(t, target, src, pos)
  parentList = []
  case pos
  when 'lastchild'
    parent = target
  when 'prevsibling'
    parent = t.item_parent(target)
  when 'nextsibling'
    parent = t.item_parent(target)
  end
  src.each{|item|
    # Ignore any item whose ancestor is also selected
    ignore = false
    t.item_ancestors(item).each{|ancestor|
      if src.find{|val| val.to_s == ancestor.to_s}
        ignore = true
        break
      end
    }
    next if ignore

    # Update the old parent of this moved item later
    unless parentList.find{|val| val.to_s == item.to_s}
      parentList << t.item_parent(item)
    end

    # Add to target
    t.__send__("item_#{pos}", target, item)

    # Update text: parent
    t.item_element_configure(item, 'parent', 'e6', :text=>parent)

    # Update text: depth
    t.item_element_configure(item, 'depth', 'e6', :text=>t.depth(item))

    # Recursively update text: depth
    itemList = []
    item = t.item_firstchild(item)
    itemList << item if item != ''

    while item = itemList.pop
      t.item_element_configure(item, 'depth', 'e6', :text=>t.depth(item))

      item2 = t.item_nextsibling(item)
      itemList << item2 if item2 != ''

      item2 = t.item_firstchild(item)
      itemList << item2 if item2 != ''
    end
  }

  # Update items that lost some children
  parentList.each{|item|
    numChildren = t.item_numchildren(item)
    if numChildren == 0
      if $Version_1_1_OrLater
        t.item_configure(item, :button=>false)
      else # TreeCtrl 1.0
        t.item_hasbutton(item, false)
      end
      t.item_style_map(item, 'item', 's2', ['e3', 'e3'])
    else
      t.item_element_configure(item, 'item', 'e4', :text=>"(#{numChildren})")
    end
  }

  # Update the target that gained some children
  if t.item_style_set(parent, 0) != 's1'
    if $Version_1_1_OrLater
      t.item_configure(parent, :button=>true)
    else # TreeCtrl 1.0
      t.item_hasbutton(parent, true)
    end
    t.item_style_map(parent, 'item', 's1', ['e3', 'e3'])
  end
  numChildren = t.item_numchildren(parent)
  t.item_element_configure(parent, 'item', 'e4', :text=>"(#{numChildren})")
end

# Same as TreeCtrl::AutoScanCheck, but calls RandomMotion and
# RandomAutoScanCheckAux
def randomAutoScanCheck(t, x, y)
  x1, y1, x2, y2 = t.contentbox
  margin = t.winfo_pixels(t.scrollmargin)
  if x < x1 + margin || x >= x2 - margin || y < y1 + margin || y >= y2 - margin
    if ! @Priv.exist?(:autoscan, :afterId, t)
      if y >= y2 - margin
        t.yview(:scroll, 1, :units)
        delay = t.yscrolldelay
      elsif y < y1 + margin
        t.yview(:scroll, -1, :units)
        delay = t.yscrolldelay
      elsif x >= x2 - margin
        t.xview(:scroll, 1, :units)
        delay = t.xscrolldelay
      elsif x < x1 + margin
        t.xview(:scroll, -1, :units)
        delay = t.xscrolldelay
      end
      if @Priv.exist?(:autoscan, :scanning, t)
        delay = delay[1] if delay.kind_of?(Array)
      else
        delay = delay[0] if delay.kind_of?(Array)
        @Priv[:autoscan, :scanning, t] = true
      end
      case @Priv['buttonMode']
      when 'drag', 'marquee'
        randomMotion(t, x, y)
      end
      @Priv[:autoscan, :afterId, t] =
        Tk.after(delay, proc{ randomAutoScanCheckAux(t) })
    end
    return
  end
  Tk::TreeCtrl::BindCallback.autoScanCancel(t)
end

def randomAutoScanCheckAux(t)
  @Priv.unset(:autoscan, :afterId, t)
  x = t.winfo_pointerx - t.winfo_rootx
  y = t.winfo_pointery - t.winfo_rooty
  randomAutoScanCheck(t, x, y)
end

#
# Demo: random N items, button images
#
def demoRandom2(t)
  demoRandom(t)

  init_pics('mac-*')

  t.configure(:openbuttonimage=>@images['mac-collapse'],
              :closedbuttonimage=>@images['mac-expand'],
              :showlines=>false)
end
