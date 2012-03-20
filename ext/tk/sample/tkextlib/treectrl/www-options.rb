def demoInternetOptions (t)
  @Option = TkVarAccess.new_hash('::Option')

  height = t.font.metrics(:linespace) + 2
  height = 18 if height < 18
  t.configure(:showroot=>false, :showbuttons=>false, :showlines=>false,
              :itemheight=>height, :selectmode=>:browse)

  init_pics('internet-*')

  if $HasColumnCreate
    t.column_create(:text=>'Internet Options')
  else
    t.column_configure(0, :text=>'Internet Options')
  end

  t.state_define('check')
  t.state_define('radio')
  t.state_define('on')

  t.element_create('e1', :image, :image=>[
                     @images['internet-check-on'],  ['check', 'on'],
                     @images['internet-check-off'], ['check'],
                     @images['internet-radio-on'],  ['radio', 'on'],
                     @images['internet-radio-off'], ['radio']
                   ])
  t.element_create('e2', :text,
                   :fill=>[@SystemHighlightText, ['selected', 'focus']])
  t.element_create('e3', :rect, :showfocus=>true,
                   :fill=>[@SystemHighlight, ['selected', 'focus']])

  s = t.style_create('s1')
  t.style_elements(s, ['e3', 'e1', 'e2'])
  t.style_layout(s, 'e1', :padx=>[0,4], :expand=>:ns)
  t.style_layout(s, 'e2', :expand=>:ns)
  t.style_layout(s, 'e3', :union=>['e2'], :iexpand=>:ns, :ipadx=>2)

  parentList = [:root, '', '', '', '', '', '']
  parent = :root
  [
    [0, :print, "Printing", "", ""],
        [1, :off, "Print background colors and images", "o1", ""],
    [0, :search, "Search from Address bar", "", ""],
        [1, :search, "When searching", "", ""],
            [2, :off, "Display results, and go to the most likely sites",
                                                                 "o2", "r1"],
            [2, :off, "Do not search from the Address bar", "o3", "r1"],
            [2, :off, "Just display the results in the main window",
                                                                 "o4", "r1"],
            [2, :on, "Just go to the most likely site", "o5", "r1"],
    [0, :security, "Security", "", ""],
        [1, :on, "Check for publisher's certificate revocation", "o5", ""],
        [1, :off, "Check for server certificate revocation (requires restart)",
                                                                 "o6", ""]
  ].each{|depth, setting, text, option, group|
    item = t.item_create()
    t.item_style_set(item, 0, 's1')
    t.item_element_configure(item, 0, 'e2', :text=>text)
    @Option[:option, item] = option
    @Option[:group, item] = group
    if setting == :on || setting == :off
      @Option[:setting, item] = setting
      if group == ''
        t.item_state_set(item, 'check')
        if setting == :on
          t.item_state_set(item, 'on')
        end
      else
        if setting == :on
          @Option[:current, group] = item
          t.item_state_set(item, 'on')
        end
        t.item_state_set(item, 'radio')
      end
    else
      t.item_element_configure(item, 0, 'e1',
                               :image=>@images["internet-#{setting}"])
    end
    t.item_lastchild(parentList[depth], item)
    depth += 1
    parentList[depth] = item
  }

  treeCtrlOption = TkBindTag.new
  treeCtrlOption.bind('Double-ButtonPress-1', proc{|w, x, y|
                        Tk::TreeCtrl::BindCallback.doubleButton1(w, x, y)
                      }, '%W %x %y')
  treeCtrlOption.bind('ButtonPress-1', proc{|w, x, y|
                        optionButton1(w, x, y)
                        Tk.callback_break
                      }, '%W %x %y')
  treeCtrlOption.bind('Button1-Motion', proc{|w, x, y|
                        optionMotion1(w, x, y)
                        Tk.callback_break
                      }, '%W %x %y')
  treeCtrlOption.bind('Button1-Leave', proc{|w, x, y|
                        optionLeave1(w, x, y)
                        Tk.callback_break
                      }, '%W %x %y')
  treeCtrlOption.bind('ButtonRelease-1', proc{|w, x, y|
                        optionRelease1(w, x, y)
                        Tk.callback_break
                      }, '%W %x %y')

  t.bindtags = [
    t, treeCtrlOption, Tk::TreeCtrl, t.winfo_toplevel, TkBindTag::ALL
  ]
end

def optionButton1(w, x, y)
  w.set_focus
  id = w.identify(x, y)
  if id[0] == 'header'
    Tk::TreeCtrl::BindCallback.buttonPress1(w, x, y)
  elsif id.empty?
    @Priv['buttonMode'] = ''
  else
    @Priv['buttonMode'] = ''
    item = id[1]
    w.selection_modify(item, :all)
    w.activate(item)
    return if @Option[:option, item] == ''
    group = @Option[:group, item]
    if group == ''
      # a checkbutton
      w.item_state_set(item, '~on')
      if @Option[:setting, item] == 'on'
        setting = :off
      else
        setting = :on
      end
      @Option[:setting, item] = setting
    else
      # a radiobutton
      current = @Option[:current, group]
      return if current == item.to_s
      w.item_state_set(current, '!on')
      w.item_state_set(item, 'on')
      @Option[:setting, item] = :on
      @Option[:current, group] = item
    end
  end
end

# Alternate implementation that doesn't rely on run-time styles
def demoInternetOptions_2(t)
  height = t.font.metrics(:linespace) + 2
  height = 18 if height < 18
  t.configure(:showroot=>false, :showbuttons=>false, :showlines=>false,
              :itemheight=>height, :selectmode=>:browse)

  init_pics('internet-*')

  t.column_configure(0, :text=>'Internet Options')

  t.element_create('e1', :image)
  t.element_create('e2', :text,
                   :fill=>[@SystemHighlightText, ['selected', 'focus']])
  t.element_create('e3', :rect, :showfocus=>true,
                   :fill=>[@SystemHighlight, ['selected', 'focus']])

  s = t.style_create('s1')
  t.style_elements('s1', ['e3', 'e1', 'e2'])
  t.style_layout(s, 'e1', :padx=>[0,4], :expand=>:ns)
  t.style_layout(s, 'e2', :expand=>:ns)
  t.style_layout(s, 'e3', :union=>['e2'], :iexpand=>:ns, :ipadx=>2)

  parentList = [:root, '', '', '', '', '', '']
  parent = :root
  [
    [0, :print, "Printing", "", ""],
        [1, :off, "Print background colors and images", "o1", ""],
    [0, :search, "Search from Address bar", "", ""],
        [1, :search, "When searching", "", ""],
            [2, :off, "Display results, and go to the most likely sites",
                                                                 "o2", "r1"],
            [2, :off, "Do not search from the Address bar", "o3", "r1"],
            [2, :off, "Just display the results in the main window",
                                                                 "o4", "r1"],
            [2, :on, "Just go to the most likely site", "o5", "r1"],
    [0, :security, "Security", "", ""],
        [1, :on, "Check for publisher's certificate revocation", "o5", ""],
        [1, :off, "Check for server certificate revocation (requires restart)",
                                                                 "o6", ""]
  ].each{|depth, setting, text, option, group|
    item = t.item_create()
    t.item_style_set(item, 0, 's1')
    t.item_element_configure(item, 0, 'e2', :text=>text)
    @Option[:option, item] = option
    @Option[:group, item] = group
    if setting == :on || setting == :off
      @Option[:setting, item] = setting
      if group == ''
        img = @images["internet-check-#{setting}"]
        t.item_element_configure(item, 0, 'e1', :image=>img)
      else
        if setting == :on
          @Option[:current, group] = item
        end
        img = @images["internet-radio-#{setting}"]
        t.item_element_configure(item, 0, 'e1', :image=>img)
      end
    else
      t.item_element_configure(item, 0, 'e1',
                               :image=>@images["internet-#{setting}"])
    end
    t.item_lastchild(parentList[depth], item)
    depth += 1
    parentList[depth] = item
  }

  treeCtrlOption = TkBindTag.new
  treeCtrlOption.bind('Double-ButtonPress-1', proc{|w, x, y|
                        Tk::TreeCtrl::BindCallback.doubleButton1(w, x, y)
                      }, '%W %x %y')
  treeCtrlOption.bind('ButtonPress-1', proc{|w, x, y|
                        optionButton1(w, x, y)
                        Tk.callback_break
                      }, '%W %x %y')
  treeCtrlOption.bind('Button1-Motion', proc{|w, x, y|
                        optionMotion1(w, x, y)
                        Tk.callback_break
                      }, '%W %x %y')
  treeCtrlOption.bind('Button1-Leave', proc{|w, x, y|
                        optionLeave1(w, x, y)
                        Tk.callback_break
                      }, '%W %x %y')
  treeCtrlOption.bind('ButtonRelease-1', proc{|w, x, y|
                        optionRelease1(w, x, y)
                        Tk.callback_break
                      }, '%W %x %y')

  t.bindtags = [
    t, treeCtrlOption, Tk::TreeCtrl, t.winfo_toplevel, TkBindTag::ALL
  ]
end

def optionButton1_2(w, x, y)
  w.set_focus
  id = w.identify(x, y)
  if id[0] == 'header'
    Tk::TreeCtrl::BindCallback.buttonPress1(w, x, y)
  elsif id.empty?
    @Priv['buttonMode'] = ''
  else
    @Priv['buttonMode'] = ''
    item = id[1]
    w.selection_modify(item, :all)
    w.activate(item)
    return if @Option[:option, item] == ''
    group = @Option[:group, item]
    if group == ''
      # a checkbutton
      if @Option[:setting, item] == 'on'
        setting = :off
      else
        setting = :on
      end
      w.item_element_configure(item, 0, 'e1',
                               :image=>@images["internet-check-#{setting}"])
      @Option[:setting, item] = setting
    else
      # a radiobutton
      current = @Option[:current, group]
      return if current == item.to_s
      w.item_element_configure(current, 0, 'e1',
                               :image=>@images["internet-radio-off"])
      w.item_element_configure(item, 0, 'e1',
                               :image=>@images["internet-radio-on"])
      @Option[:setting, item] = :on
      @Option[:current, group] = item
    end
  end
end

def optionMotion1(w, x, y)
  case @Priv['buttonMode']
  when 'resize', 'header'
    Tk::TreeCtrl::BindCallback.motion1(w, x, y)
  end
end

def optionLeave1(w, x, y)
  # This is called when I do ButtonPress-1 on Unix for some reason,
  # and buttonMode is undefined.
  begin
    mode = @Priv['buttonMode']
  rescue
  else
    case mode
    when 'header'
      t.column_configure(@Priv['column'], :sunken=>false)
    end
  end
end

def optionRelease1(w, x, y)
  case @Priv['buttonMode']
  when 'resize', 'header'
    Tk::TreeCtrl::BindCallback.release1(w, x, y)
  end
  @Priv['buttonMode'] = ''
end
