#
# Demo: MailWasher
#
def demoMailWasher(t)
  init_pics('*checked')

  height = t.font.metrics(:linespace) + 2
  height = 18 if height < 18

  t.configure(:showroot=>false, :showrootbutton=>false, :showbuttons=>false,
              :showlines=>false, :itemheight=>height, :selectmode=>:browse,
              :xscrollincrement=>1)

  pad = 4

  if $Version_1_1_OrLater
    t.column_create(:text=>'Delete', :textpadx=>pad, :tag=>'delete')
    t.column_create(:text=>'Bounce', :textpadx=>pad, :tag=>'bounce')
    t.column_create(:text=>'Status', :width=>80, :textpadx=>pad,
                    :tag=>'status')
    t.column_create(:text=>'Size', :width=>40, :textpadx=>pad,
                    :justify=>:right, :tag=>'size')
    t.column_create(:text=>'From', :width=>140, :textpadx=>pad, :tag=>'from')
    t.column_create(:text=>'Subject', :width=>240, :textpadx=>pad,
                    :tag=>'subject')
    t.column_create(:text=>'Received', :textpadx=>pad, :arrow=>:up,
                    :arrowpad=>[4,0], :tag=>'received')
    t.column_create(:text=>'Attachments', :textpadx=>pad, :tag=>'attachments')

    t.state_define('CHECK')

    t.element_create('imgCheck', :image, :image=>[
                       @images['checked'], ['CHECK'], @images['unchecked'], []
                     ])

  else # TreeCtrl 1.0
    t.column_configure(0, :text=>'Delete', :textpadx=>pad, :tag=>'delete')
    t.column_configure(1, :text=>'Bounce', :textpadx=>pad, :tag=>'bounce')
    t.column_configure(2, :text=>'Status', :width=>80, :textpadx=>pad,
                       :tag=>'status')
    t.column_configure(3, :text=>'Size', :width=>40, :textpadx=>pad,
                       :justify=>:right, :tag=>'size')
    t.column_configure(4, :text=>'From', :width=>140, :textpadx=>pad,
                       :tag=>'from')
    t.column_configure(5, :text=>'Subject', :width=>240, :textpadx=>pad,
                       :tag=>'subject')
    t.column_configure(6, :text=>'Received', :textpadx=>pad, :arrow=>:up,
                       :arrowpad=>[4,0], :tag=>'received')
    t.column_configure(7, :text=>'Attachments', :textpadx=>pad,
                       :tag=>'attachments')

    t.element_create('imgOff', :image, :image=>@images['unchecked'])
    t.element_create('imgOn', :image, :image=>@images['checked'])
  end

  t.element_create('border', :rect, :open=>:nw, :outline=>'gray',
                   :outlinewidth=>1, :fill=>[@SystemHighlight, ['selected']])
  t.element_create('txtAny', :text, :lines=>1,
                   :fill=>[@SystemHighlightText, ['selected']])
  t.element_create('txtNone', :text, :text=>'none', :lines=>1,
                   :fill=>[@SystemHighlightText, ['selected']])
  t.element_create('txtYes', :text, :text=>'yes', :lines=>1,
                   :fill=>[@SystemHighlightText, ['selected']])
  t.element_create('txtNormal', :text, :text=>'Normal', :lines=>1,
                   :fill=>[@SystemHighlightText, ['selected'], '#006800', []])
  t.element_create('txtPossSpam', :text, :text=>'Possible Spam', :lines=>1,
                   :fill=>[@SystemHighlightText, ['selected'], '#787800', []])
  t.element_create('txtProbSpam', :text, :text=>'Probably Spam', :lines=>1,
                   :fill=>[@SystemHighlightText, ['selected'], '#FF9000', []])
  t.element_create('txtBlacklist', :text, :text=>'Blacklisted', :lines=>1,
                   :fill=>[@SystemHighlightText, ['selected'], '#FF5800', []])

  if $Version_1_1_OrLater
    s = t.style_create('styCheck')
    t.style_elements(s, ['border', 'imgCheck'])
    t.style_layout(s, 'border', :detach=>true, :iexpand=>:es)
    t.style_layout(s, 'imgCheck', :expand=>:news)
  else
    ['Off', 'On'].each{|name|
      s = t.style_create('sty' << name)
      i = 'img' << name
      t.style_elements(s, ['border', i])
      t.style_layout(s, 'border', :detach=>true, :iexpand=>:es)
      t.style_layout(s, i, :expand=>:news)
    }
  end

  pad = 4

  %w(Any None Yes Normal PossSpam ProbSpam Blacklist).each{|name|
    s = t.style_create('sty' << name)
    e = 'txt' << name
    t.style_elements(s, ['border', e])
    t.style_layout(s, 'border', :detach=>true, :iexpand=>:es)
    t.style_layout(s, e, :padx=>pad, :squeeze=>:x, :expand=>:ns)
  }

  [
    ['baldy@spammer.com',  "Your hair is thinning"],
    ['flat@spammer.com', "Your breasts are too small"],
    ['tiny@spammer.com',  "Your penis is too small"],
    ['dumbass@spammer.com', "You are not very smart"],
    ['bankrobber@spammer.com', "You need more money"],
    ['loser@spammer.com', "You need better friends"],
    ['gossip@spammer.com', "Find out what your coworkers think about you"],
    ['whoami@spammer.com', "Find out what you think about yourself"],
    ['downsized@spammer.com', "You need a better job"],
    ['poorhouse@spammer.com', "Your mortgage is a joke"],
    ['spam4ever@spammer.com', "You need more spam"]
  ].each{|frm, subj|
    item = t.item_create
    status = ['styNormal','styPossSpam','styProbSpam','styBlacklist'][rand(4)]
    attachments = ['styNone','styYes'][rand(2)]

    if $Version_1_1_OrLater
      delete = [false, true][rand(2)]
      bounce = [false, true][rand(2)]
      t.item_style_set(item,
                       0, 'styCheck', 1, 'styCheck', 2, status, 3, 'styAny',
                       4, 'styAny', 5, 'styAny', 6, 'styAny', 7, attachments)
      t.item_state_forcolumn(item, 'delete', 'CHECK') if delete
      t.item_state_forcolumn(item, 'bounce', 'CHECK') if bounce

    else # TreeCtrl 1.0
      delete = ['styOn', 'styOff'][rand(2)]
      bounce = ['styOn', 'styOff'][rand(2)]
      t.item_style_set(item,
                       0, delete, 1, bounce, 2, status, 3, 'styAny',
                       4, 'styAny', 5, 'styAny', 6, 'styAny', 7, attachments)
    end

    bytes = 512 + rand(1024 * 12)
    size = "#{bytes / 1024 + 1}KB"
    seconds = Tk::Clock.seconds - rand(100000)
    received = Tk::Clock.format(seconds, '%d/%m/%y %I:%M %p')
    t.item_text(item, 3, size, 4, frm, 5, subj, 6, received)
    t.item_lastchild(:root, item)
  }

  sortColumn = 6
  t.notify_bind(t, 'Header-invoke',
                proc{|c, w|
                  if c == sortColumn
                    if w.column_cget(sortColumn, :arrow) == 'down'
                      order = :increasing
                      arrow = :up
                    else
                      order = :decreasing
                      arrow = :down
                    end
                  else
                    if w.column_cget(sortColumn, :arrow) == 'down'
                      order = :decreasing
                      arrow = :down
                    else
                      order = :increasing
                      arrow = :up
                    end
                    w.column_configure(sortColumn, :arrow=>:none)
                    sortColumn = c
                  end
                  w.column_configure(c, :arrow=>arrow)
                  case w.column_cget(c, :tag)
                  when 'bounce', 'delete'
                    w.item_sort(:root, order,
                                {
                                  :column=>c,
                                  :command=>proc{|item1, item2|
                                    compareOnOff(w, c, item1, item2)
                                  }
                                },
                                { :column=>'subject', :dictionary=>true })
                  when 'status'
                    w.item_sort(:root, order,
                                { :column=>c, :dictionary=>true })
                  when 'from'
                    w.item_sort(:root, order,
                                { :column=>c, :dictionary=>true },
                                { :column=>'subject', :dictionary=>true })
                  when 'subject'
                    w.item_sort(:root, order,
                                { :column=>c, :dictionary=>true })
                  when 'size'
                    w.item_sort(:root, order,
                                { :column=>c, :dictionary=>true },
                                { :column=>'subject', :dictionary=>true })
                  when 'received'
                    w.item_sort(:root, order,
                                { :column=>c, :dictionary=>true },
                                { :column=>'subject', :dictionary=>true })
                  when 'attachments'
                    w.item_sort(:root, order,
                                { :column=>c, :dictionary=>true },
                                { :column=>'subject', :dictionary=>true })
                  end
                }, '%C %T')

  mailWasher = TkBindTag.new

  if $Version_1_1_OrLater
    mailWasher.bind('ButtonPress-1',
                    proc{|w, x, y|
                      id = w.identify(x, y)
                      if id.empty?
                      elsif id[0] == 'header'
                      else
                        what, item, where, arg1, arg2, arg3 = id
                        if where == 'column'
                          tag = w.column_cget(arg1, :tag)
                          if tag == 'delete' || tag == 'bounce'
                            w.item_state_forcolumn(item, arg1, '~CHECK')
                          end
                        end
                      end
                    }, '%W %x %y')
  else # TreeCtrl 1.0
    mailWasher.bind('ButtonPress-1',
                    proc{|w, x, y|
                      id = w.identify(x, y)
                      if id.empty?
                      elsif id[0] == 'header'
                      else
                        what, item, where, arg1, arg2, arg3 = id
                        if where == 'column'
                          tag = w.column_cget(arg1, :tag)
                          if tag == 'delete' || tag == 'bounce'
                            style = w.item_style_set(item, arg1)
                            if style == 'styOn'
                              style = 'styOff'
                            else
                              style = 'styOn'
                            end
                            w.item_style_set(item, arg1, style)
                            @display_styles_in_item.call(item)
                          end
                        end
                      end
                    }, '%W %x %y')
  end

  t.bindtags = [t, mailWasher, Tk::TreeCtrl, t.winfo_toplevel, TkBindTag::ALL]
end

if $Version_1_1_OrLater
  def compareOnOff(w, c, item1, item2)
    s1 = w.item_state_forcolumn(item1, c)
    s2 = w.item_state_forcolumn(item2, c)
    if (s1 == s2)
      0
    elsif (s1 == 'CHECK')
      -1
    else
      1
    end
  end

else # TreeCtrl 1.0
  def compareOnOff(w, c, item1, item2)
    s1 = w.item_style_set(item1, c)
    s2 = w.item_style_set(item2, c)
    if (s1 == s2)
      0
    elsif (s1 == 'styOff')
      -1
    else
      1
    end
  end
end
