#
# Demo: Bitmaps
#
def demoBitmaps(t)
  #if $Version_1_1_OrLater
  if @has_bgimg
    t.configure(:showroot=>false, :showbuttons=>false, :showlines=>false,
                :selectmode=>:browse, :orient=>:horizontal, :wrap=>'5 items',
                :showheader=>false, :backgroundimage=>@images['sky'])
  else
    t.configure(:showroot=>false, :showbuttons=>false, :showlines=>false,
                :selectmode=>:browse, :orient=>:horizontal, :wrap=>'5 items',
                :showheader=>false)
  end

  if $HasColumnCreate
    t.column_create(:itembackground=>['gray90', []])
  else
    t.column_configure(0, :itembackground=>['gray90', []])
  end

  t.element_create('elemTxt', :text,
                   :fill=>[@SystemHighlightText, ['selected', 'focus']])
  t.element_create('elemSelTxt', :rect, :showfocus=>true,
                   :fill=>[@SystemHighlight, ['selected', 'focus']])
  t.element_create('elemSelBmp', :rect, :outlinewidth=>4,
                   :outline=>[@SystemHighlight, ['selected', 'focus']])
  t.element_create('elemBmp', :bitmap,
                   :foreground=>[@SystemHighlight, ['selected', 'focus']],
                   :background=>'linen',
                   :bitmap=>['question' ['selected']])

  s = t.style_create('STYLE', :orient=>:vertical)
  t.style_elements(s, ['elemSelBmp', 'elemBmp', 'elemSelTxt', 'elemTxt'])
  t.style_layout(s, 'elemSelBmp', :union=>'elemBmp', :ipadx=>6, :ipady=>6)
  t.style_layout(s, 'elemBmp',    :pady=>[0, 6], :expand=>:we)
  t.style_layout(s, 'elemSelTxt', :union=>'elemTxt', :ipadx=>2)
  t.style_layout(s, 'elemTxt',    :expand=>:we)

  # Set default item style
  if $Version_1_1_OrLater
    t.defaultstyle = [s]
  end

  bitmap_names = %w(error gray75 gray50 gray25 gray12
                    hourglass info questhead question warning)

  bitmap_names.each{|name|
    i = t.item_create
    unless $Version_1_1_OrLater
      t.item_style_set(i, 0, s)
    end
    t.item_text(i, 0, name)
    t.item_element_configure(i, 0, 'elemBmp', :bitmap=>name)
    t.item_lastchild(:root, i)
  }

  bitmap_names.each{|name|
    i = t.item_create
    t.item_style_set(i, 0, s)
    t.item_text(i, 0, name)
    if true
      t.item_element_configure(i, 0, 'elemBmp', :bitmap=>name,
                               :foreground=>['brown', ''],
                               :background=>['', ''])
    else
      t.item_element_configure(i, 0, 'elemBmp', :bitmap=>name,
                               :foreground=>[
                                 @SystemHighlight, ['selected', 'focus'],
                                 'brown', []
                               ],
                               :background=>['', []])
    end
    t.item_lastchild(:root, i)
  }
end
