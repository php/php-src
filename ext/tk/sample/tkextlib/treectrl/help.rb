#
# Demo: Help contents
#
def demoHelpContents(t)
  height = t.font.metrics(:linespace)
  height = 18 if height < 18
  t.configure(:showroot=>false, :showbuttons=>false, :showlines=>false,
              :itemheight=>height, :selectmode=>:browse)

  init_pics('help-*')

  if $Version_1_1_OrLater
    t.column_create(:text=>'Help Contents')
  else # TreeCtrl 1.0
    t.column_configure(0, :text=>'Help Contents')
  end

  # Define a new item state
  t.state_define('mouseover')

  t.element_create('e1', :image, :image=>@images['help-page'])
  t.element_create('e2', :image, :image=>[
                     @images['help-book-open'], ['open'],
                     @images['help-book-closed'], [],
                   ])
  t.element_create('e3', :text,
                   :font=>[t.font.dup.underline(true), ['mouseover']],
                   :fill=>[
                     @SystemHighlightText, ['selected', 'focus'],
                     'blue', ['mouseover']
                   ])
  t.element_create('e4', :rect, :showfocus=>true,
                   :fill=>[@SystemHighlight, ['selected', 'focus']])

  # book
  s = t.style_create('s1')
  t.style_elements(s, ['e4', 'e1', 'e3'])
  t.style_layout(s, 'e1', :padx=>[0,4], :expand=>:ns)
  t.style_layout(s, 'e3', :expand=>:ns)
  t.style_layout(s, 'e4', :union=>['e3'], :iexpand=>:ns, :ipadx=>2)

  # page
  s = t.style_create('s2')
  t.style_elements(s, ['e4', 'e2', 'e3'])
  t.style_layout(s, 'e2', :padx=>[0,4], :expand=>:ns)
  t.style_layout(s, 'e3', :expand=>:ns)
  t.style_layout(s, 'e4', :union=>['e3'], :iexpand=>:ns, :ipadx=>2)

  parentList = [:root, '', '', '', '', '', '']
  parent = :root
  [
     [0, 's1', "Welcome to Help"],
     [0, 's2', "Introducing Windows 98"],
        [1, 's2', "How to Use Help"],
           [2, 's1', "Find a topic"],
           [2, 's1', "Get more out of help"],
        [1, 's2', "Register Your Software"],
           [2, 's1', "Registering Windows 98 online"],
        [1, 's2', "What's New in Windows 98"],
           [2, 's1', "Innovative, easy-to-use features"],
           [2, 's1', "Improved reliability"],
           [2, 's1', "A faster operating system"],
           [2, 's1', "True Web integration"],
           [2, 's1', "More entertaining and fun"],
        [1, 's2', "If You're New to Windows 98"],
           [2, 's2', "Tips for Macintosh Users"],
              [3, 's1', "Why does the mouse have two buttons?"]
  ].each{|depth, style, text|
    item = t.item_create
    t.item_style_set(item, 0, style)
    t.item_element_configure(item, 0, 'e3', :text=>text)
    if $Version_1_1_OrLater
      t.item_collapse(item)
    else # TreeCtrl 1.0
      t.collapse(item)
    end
    t.item_lastchild(parentList[depth], item)
    depth += 1
    parentList[depth] = item
  }

  treeCtrlHelp = TkBindTag.new

  treeCtrlHelp.bind('Double-ButtonPress-1',
                    proc{|w, x, y|
                      if w.identify(x, y)[0] == 'header'
                        Tk::TreeCtrl::BindCallback.doubleButton1(w, x, y)
                      else
                        helpButton1(w, x, y)
                      end
                      Tk.callback_break
                    }, '%W %x %y')

  treeCtrlHelp.bind('ButtonPress-1',
                    proc{|w, x, y|
                      helpButton1(w, x, y)
                      Tk.callback_break
                    }, '%W %x %y')

  treeCtrlHelp.bind('Button1-Motion',
                    proc{|w, x, y|
                      helpMotion1(w, x, y)
                      Tk.callback_break
                    }, '%W %x %y')

  treeCtrlHelp.bind('Button1-Leave',
                    proc{|w, x, y|
                      helpLeave1(w, x, y)
                      Tk.callback_break
                    }, '%W %x %y')

  treeCtrlHelp.bind('ButtonRelease-1',
                    proc{|w, x, y|
                      helpRelease1(w, x, y)
                      Tk.callback_break
                    }, '%W %x %y')

  treeCtrlHelp.bind('Motion', proc{|w, x, y| helpMotion(w, x, y) }, '%W %x %y')

  treeCtrlHelp.bind('Leave', proc{|w, x, y| helpMotion(w, x, y) }, '%W %x %y')

  treeCtrlHelp.bind('KeyPress-Return',
                    proc{|w, x, y|
                      if w.selection_get.length == 1
                        if $Version_1_1_OrLater
                          w.item_toggle(w.selection_get[0])
                        else # TreeCtrl 1.0
                          w.toggle(w.selection_get[0])
                        end
                      end
                      Tk.callback_break
                    }, '%W %x %y')

  @Priv[:help, :prev] = ''

  t.bindtags = [ t, treeCtrlHelp, Tk::TreeCtrl, t.winfo_toplevel, :all ]
end

# This is an alternate implementation that does not define a new item state
# to change the appearance of the item under the cursor.
def demoHelpContents2(t)
  height = t.font.metrics(:linespace)
  height = 18 if height < 18
  t.configure(:showroot=>false, :showbuttons=>false, :showlines=>false,
              :itemheight=>height, :selectmode=>:browse)

  init_pics('help-*')

  if $Version_1_1_OrLater
    t.column_create(:text=>'Help Contents')
  else # TreeCtrl 1.0
    t.column_configure(0, :text=>'Help Contents')
  end

  t.element_create('e1', :image, :image=>@images['help-page'])
  t.element_create('e2', :image, :image=>[
                     @images['help-book-open'], ['open'],
                     @images['help-book-closed'], [],
                   ])
  t.element_create('e3', :text,
                   :fill=>[
                     @SystemHighlightText, ['selected', 'focus'],
                     'blue', []
                   ])
  t.element_create('e4', :rect, :showfocus=>true,
                   :fill=>[@SystemHighligh, ['selected', 'focus']])
  t.element_create('e5', :text, :font=>t.font.dup.underline(true),
                   :fill=>[
                     @SystemHighlightText, ['selected', 'focus'],
                     'blue', []
                   ])

  # book
  s = t.style_create('s1')
  t.style_elements(s, ['e4', 'e1', 'e3'])
  t.style_layout(s, 'e1', :padx=>[0,4], :expand=>:ns)
  t.style_layout(s, 'e3', :expand=>:ns)
  t.style_layout(s, 'e4', :union=>['e3'], :iexpand=>:ns, :ipadx=>2)

  # page
  s = t.style_create('s2')
  t.style_elements(s, ['e4', 'e2', 'e3'])
  t.style_layout(s, 'e2', :padx=>[0,4], :expand=>:ns)
  t.style_layout(s, 'e3', :expand=>:ns)
  t.style_layout(s, 'e4', :union=>['e3'], :iexpand=>:ns, :ipadx=>2)

  # book (focus)
  s = t.style_create('s1.f')
  t.style_elements(s, ['e4', 'e1', 'e5'])
  t.style_layout(s, 'e1', :padx=>[0,4], :expand=>:ns)
  t.style_layout(s, 'e5', :expand=>:ns)
  t.style_layout(s, 'e4', :union=>['e5'], :iexpand=>:ns, :ipadx=>2)

  # page (focus)
  s = t.style_create('s2')
  t.style_elements(s, ['e4', 'e2', 'e5'])
  t.style_layout(s, 'e2', :padx=>[0,4], :expand=>:ns)
  t.style_layout(s, 'e5', :expand=>:ns)
  t.style_layout(s, 'e4', :union=>['e5'], :iexpand=>:ns, :ipadx=>2)

  parentList = [:root, '', '', '', '', '', '']
  parent = :root
  [
     [0, 's1', "Welcome to Help"],
     [0, 's2', "Introducing Windows 98"],
        [1, 's2', "How to Use Help"],
           [2, 's1' "Find a topic"],
           [2, 's1', "Get more out of help"],
        [1, 's2', "Register Your Software"],
           [2, 's1', "Registering Windows 98 online"],
        [1, 's2', "What's New in Windows 98"],
           [2, 's1', "Innovative, easy-to-use features"],
           [2, 's1', "Improved reliability"],
           [2, 's1', "A faster operating system"],
           [2, 's1', "True Web integration"],
           [2, 's1', "More entertaining and fun"],
        [1, 's2', "If You're New to Windows 98"],
           [2, 's2', "Tips for Macintosh Users"],
              [3, 's1', "Why does the mouse have two buttons?"]
  ].each{|depth, style, text|
    item = t.item_create
    t.item_style_set(item, 0, style)
    t.item_element_configure(item, 0, 'e3', :text=>text)
    if $Version_1_1_OrLater
      t.item_collapse(item)
    else # TreeCtrl 1.0
      t.collapse(item)
    end
    t.item_lastchild(parentList[depth], item)
    depth += 1
    parentList[depth] = item
  }

  treeCtrlHelp = TkBindTag.new

  treeCtrlHelp.bind('Double-ButtonPress-1',
                    proc{|w, x, y|
                      if w.identify(x, y)[0] == 'header'
                        Tk::TreeCtrl::BindCallback.doubleButton1(w, x, y)
                      else
                        helpButton1(w, x, y)
                      end
                      Tk.callback_break
                    }, '%W %x %y')

  treeCtrlHelp.bind('ButtonPress-1',
                    proc{|w, x, y|
                      helpButton1(w, x, y)
                      Tk.callback_break
                    }, '%W %x %y')

  treeCtrlHelp.bind('Button1-Motion',
                    proc{|w, x, y|
                      helpMotion1(w, x, y)
                      Tk.callback_break
                    }, '%W %x %y')

  treeCtrlHelp.bind('Button1-Leave',
                    proc{|w, x, y|
                      helpLeave1(w, x, y)
                      Tk.callback_break
                    }, '%W %x %y')

  treeCtrlHelp.bind('ButtonRelease-1',
                    proc{|w, x, y|
                      helpRelease1(w, x, y)
                      Tk.callback_break
                    }, '%W %x %y')

  treeCtrlHelp.bind('Motion', proc{|w, x, y| helpMotion(w, x, y) }, '%W %x %y')

  treeCtrlHelp.bind('Leave', proc{|w, x, y| helpMotion(w, x, y) }, '%W %x %y')

  treeCtrlHelp.bind('KeyPress-Return',
                    proc{|w, x, y|
                      if w.selection_get.length == 1
                        w.item_toggle(w.selection_get[0])
                      end
                      Tk.callback_break
                    }, '%W %x %y')

  @Priv[:help, :prev] = ''

  t.bindtags = [ t, treeCtrlHelp, Tk::TreeCtrl, t.winfo_toplevel, :all ]
end

def helpButton1(w, x, y)
  w.set_focus
  id = w.identify(x, y)
  @Priv['buttonMode'] = ''
  if id[0] == 'header'
    Tk::TreeCtrl::BindCallback.buttonPress1(w, x, y)
  elsif id[0] == 'item'
    item = id[1]
    # didn't click an element
    return if id.length != 6
    if w.selection_includes(item)
      w.toggle(item)
      return
    end
    if w.selection_get.length > 0
      item2 = w.selection_get[0]
      if $Version_1_1_OrLater
        w.item_collapse(item2)
      else # TreeCtrl 1.0
        w.collapse(item2)
      end
      w.item_ancestors(item2).each{|i|
        if $Version_1_1_OrLater
          w.item_collapse(i) if w.compare(item, '!=', i)
        else # TreeCtrl 1.0
          w.collapse(i) if w.compare(item, '!=', i)
        end
      }
    end
    w.activate(item)
    if $Version_1_1_OrLater
      w.item_ancestors(item).each{|i|
        w.item_expand(i)
      }
      w.item_toggle(item)
    else # TreeCtrl 1.0
      w.expand(*(w.item_ancestors(item)))
      w.toggle(item)
    end
    w.selection_modify(item, :all)
  end
end

def helpMotion1(w, x, y)
  case @Priv['buttonMode']
  when 'resize', 'header'
    Tk::TreeCtrl::BindCallback.motion1(w, x, y)
  end
end

def helpLeave1(w, x, y)
  # This is called when I do ButtonPress-1 on Unix for some reason,
  # and buttonMode is undefined.
  return unless @Priv.exist?('buttonMode')
  case @Priv['buttonMode']
  when 'header'
    w.column_configure(@Priv['column'], :sunken=>false)
  end
end

def helpRelease1(w, x, y)
  case @Priv['buttonMode']
  when 'resize', 'header'
    Tk::TreeCtrl::BindCallback.release1(w, x, y)
  end
  @Priv['buttonMode'] = ''
end

def helpMotion(w, x, y)
  id = w.identify(x, y)
  if id.empty?
  elsif id[0] == 'header'
  elsif id[0] == 'item'
    item = id[1]
    if id.length == 6
      if @Priv[:help, :prev] != TkComm._get_eval_string(item)
        if @Priv[:help, :prev] != ''
          w.item_state_set(@Priv[:help, :prev], '!mouseover')
        end
        w.item_state_set(item, 'mouseover')
        @Priv[:help, :prev] = item
      end
      return
    end
  end
  if @Priv[:help, :prev] != ''
    w.item_state_set(@Priv[:help, :prev], '!mouseover')
    @Priv[:help, :prev] = ''
  end
end

# Alternate implementation doesn't rely on mouseover state
def helpMotion2(w, x, y)
  id = w.identify(x, y)
  if id[0] == 'header'
  elsif !id.empty?
    item = id[1]
    if id.kength == 6
      if @Priv[:help, :prev] != TkComm._get_eval_string(item)
        if @Priv[:help, :prev] != ''
          style = w.item_style_set(@Priv[:help, :prev], 0)
          style.sub!(/\.f$/, '')
          w.item_style_map(@Priv[:help, :prev], 0, style, ['e5', 'e3'])
        end
        style = w.item_style_set(item, 0)
        w.item_style_map(item, 0, style + '.f', ['e3', 'e5'])
        @Priv[:help, :prev] = item
      end
      return
    end
  end
  if @Priv[:help, :prev] != ''
    style = w.item_style_set(@Priv[:help, :prev], 0)
    style.sub!(/\.f$/, '')
    w.item_style_map(@Priv[:help, :prev], 0, style, ['e5', 'e3'])
    @Priv[:help, :prev] = ''
  end
end
