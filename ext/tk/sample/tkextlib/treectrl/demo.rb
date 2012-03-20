#!/usr/bin/env ruby

require 'tk'
require 'tkextlib/treectrl'

$ScriptDir = File.dirname(File.expand_path(__FILE__))

$HasColumnCreate = Tk::TreeCtrl::HasColumnCreateCommand

$Version_1_1_OrLater = (TkPackage.vcompare(Tk::TreeCtrl.package_version, '1.1') >= 0)

#if Hash.instance_methods.include?(:key)
if TkCore::WITH_RUBY_VM  ### Ruby 1.9 !!!!
  # ruby 1.9.x --> use Hash#key
  # Because Hash#index show warning "Hash#index is deprecated; use Hash#key".
else
  # ruby 1.8.x --> use Hash#index
  class Hash
    alias key index
  end
end

class TkTreeCtrl_demo
  def initialize(dir)
    @ScriptDir = dir || '.'

    @thisPlatform = Tk::PLATFORM['platform']
    if @thisPlatform == 'unix' && Tk.windowingsystem == 'aqua'
      @thisPlatform = 'macosx'
    end

    @RandomN = [500]

    @images = Hash.new
    @sel_images = Hash.new

    @popup = Hash.new
    @mTree = Hash.new
    @mHeader = Hash.new

    @non_clear_list = []

    @demoCmd = Hash.new
    @demoFile = Hash.new

    # Get default colors
    w = TkListbox.new
    @SystemButtonFace    = w[:highlightbackground]
    @SystemHighlight     = w[:selectbackground]
    @SystemHighlightText = w[:selectforeground]
    w.destroy

    ####################

    make_source_window()
    make_menubar()
    make_main_window()

    if $Version_1_1_OrLater
      begin
        @tree2[:backgroundimage]
        @has_bgimg = true
      rescue
        @has_bgimg = false
      end
    else
      @has_bgimg = false
    end

    ####################

    make_list_popup()
    make_header_popup()

    init_pics('sky')

    ####################

    @tree2.bind('ButtonPress-3',
                proc{|w, x, y, rootx, rooty|
                  show_list_popup(w, x, y, rootx, rooty)
                }, '%W %x %y %X %Y')

    # Allow "scan" bindings
    if @thisPlatform == 'windows'
      @tree2.bind_remove('Control-ButtonPress-3')
    end

    ####################

    init_demo_scripts_module()
    load_demo_scripts()
    init_demo_list()

    ####################

    @tree1.notify_bind(@tree1, 'Selection',
                       proc{|c, t|
                         if c == 1
                           item = t.selection_get[0]
                           demo_set(@demoCmd[item], @demoFile[item])
                         end
                       }, '%c %T')

    # When one item is selected in the demo list, display the styles in
    # that item.
    # See DemoClear for why the tag "DontDelete" is used
    @tree2.notify_bind('DontDelete', 'Selection',
                       proc{|c, t|
                         display_styles_in_item(t.selection_get[0]) if c == 1
                       }, '%c %T')
  end

  ##########################

  def init_pics(*args)
    args.each{|pat|
      unless TkImage.names.find{|img| (name = @images.key(img)) && File.fnmatch(pat, name)}
        Dir.glob(File.join(@ScriptDir, 'pics', "#{pat}.gif")).each{|file|
          name = File.basename(file, '.gif')
          img = TkPhotoImage.new(:file=>file)
          @images[name] = img
          @sel_images[name] = TkPhotoImage.new
          @sel_images[name].copy(img)
          Tk::TreeCtrl.image_tint(@sel_images[name], @SystemHighlight, 128)
        }
      end
    }
  end

  ##########################

  private

  def make_menubar
    menuspec = [
      [['File']]
    ]
    if Tk::PLATFORM['platform'] != 'unix'
      TkConsole.create
      TkConsole.eval('.console conf -height 8')
      menuspec[0] << ['Console', proc{
          if TkComm.bool(TkConsole.eval('winfo ismapped .'))
            TkConsole.hide
          else
            TkConsole.show
          end
        }]
    end
    menuspec[0] << ['View Source', proc{toggle_source_window()}]
    menuspec[0] << ['Quit', proc{exit}]
    Tk.root.add_menubar(menuspec)
  end

  def make_source_window
    @src_top = TkToplevel.new
    f = TkFrame.new(@src_top, :borderwidth=>0)
    case @thisPlatform
    when 'unix'
      font = TkFont.new(['Courier', -12])
    else
      font = TkFont.new(['Courier', 9])
    end

    @src_txt = TkText.new(f, :font=>font, :tabs=>font.measure('1234'),
                          :wrap=>:none)
    xscr = @src_txt.xscrollbar(TkScrollbar.new(f))
    yscr = @src_txt.yscrollbar(TkScrollbar.new(f))

    f.pack(:expand=>true, :fill=>:both)
    f.grid_columnconfigure(0, :weight=>1)
    f.grid_rowconfigure(0, :weight=>1)
    @src_txt.grid(:row=>0, :column=>0, :sticky=>:news)
    xscr.grid(:row=>1, :column=>0, :sticky=>:we)
    yscr.grid(:row=>0, :column=>1, :sticky=>:ns)

    @src_top.protocol('WM_DELETE_WINDOW', proc{@src_top.withdraw})
    @src_top.geometry('-0+0')
    @src_top.withdraw
  end

  def show_source(file)
    @src_top.title("Demo Source: #{file}")
    @src_txt.value = IO.read(File.join(@ScriptDir, file))
    @src_txt.set_insert('1.0')
  end

  def toggle_source_window
    if @src_top.winfo_mapped?
      @src_top.withdraw
    else
      @src_top.deiconify
    end
  end

  def tree_plus_scrollbars_in_a_frame(parent, h, v)
    f = TkFrame.new(parent, :borderwidth=>1, :relief=>:sunken)
    case @thisPlatform
    when 'unix'
      font = TkFont.new(['Helvetica', -12])
    else
      # There is a bug on my Win98 box with Tk_MeasureChars() and
      # MS Sans Serif 8.
      font = TkFont.new(['MS Sans', 8])
    end

    tree = Tk::TreeCtrl.new(f, :highlightthickness=>0,
                            :borderwidth=>0, :font=>font)
    tree[:xscrollincrement] = 20
    tree.debug_configure(:enable=>false, :display=>false)

    if h
      h_scr = TkScrollbar.new(f, :orient=>:horizontal,
                              :command=>proc{|*args| tree.xview(*args)})
      tree.notify_bind(h_scr, 'Scroll-x',
                       proc{|w, l, u| w.set(l, u)}, '%W %l %u')
      h_scr.bind('ButtonPress-1', proc{tree.set_focus})
    end

    if v
      v_scr = TkScrollbar.new(f, :orient=>:vertical,
                              :command=>proc{|*args| tree.yview(*args)})
      tree.notify_bind(v_scr, 'Scroll-y',
                       proc{|w, l, u| w.set(l, u)}, '%W %l %u')
      v_scr.bind('ButtonPress-1', proc{tree.set_focus})
    end

    f.grid_columnconfigure(0, :weight=>1)
    f.grid_rowconfigure(0, :weight=>1)
    tree.grid(:row=>0, :column=>0, :sticky=>:news)
    h_scr.grid(:row=>1, :column=>0, :sticky=>:we) if h
    v_scr.grid(:row=>0, :column=>1, :sticky=>:ns) if v

    [f, tree]
  end

  def make_main_window
    Tk.root.title('Tk::TreeCtrl Demo')

    case @thisPlatform
    when 'macosx'
      Tk.root.geometry('+40+40')
    else
      Tk.root.geometry('+0+30')
    end

    pane1 = TkPanedWindow.new(:orient=>:vertical, :borderwidth=>0)
    pane2 = TkPanedWindow.new(:orient=>:horizontal, :borderwidth=>0)

    # Tree + scrollbar: demos
    f1, @tree1 = tree_plus_scrollbars_in_a_frame(nil, false, true)
    @tree1.configure(:showbuttons=>false, :showlines=>:false,
                    :showroot=>false, :height=>100)
    if $HasColumnCreate
      @tree1.column_create(:text=>'List of Demos',
                           :expand=>true, :button=>false)
    else
      @tree1.column_configure(0, :text=>'List of Demos',
                              :expand=>true, :button=>false)
    end

    # Tree + scrollbar: styles + elements in list
    f4, @tree4 = tree_plus_scrollbars_in_a_frame(nil, false, true)
    @tree4.configure(:showroot=>false, :height=>140)
    if $HasColumnCreate
      @tree4.column_create(:text=>'Elements and Styles',
                           :expand=>true, :button=>false)
    else
      @tree4.column_configure(0, :text=>'Elements and Styles',
                              :expand=>true, :button=>false)
    end

    # Tree + scrollbar: styles + elements in selected item
    f3, @tree3 = tree_plus_scrollbars_in_a_frame(nil, false, true)
    @tree3.configure(:showroot=>false)
    if $HasColumnCreate
      @tree3.column_create(:text=>'Styles in Item',
                           :expand=>true, :button=>false)
    else
      @tree3.column_configure(0, :text=>'Styles in Item',
                              :expand=>true, :button=>false)
    end

    pane1.add(f1, f4, f3, :height=>150)
    pane1.pack(:expand=>true, :fill=>:both)

    # Frame on right
    f2_base = TkFrame.new

    # Tree + scrollbars
    f2, @tree2 = tree_plus_scrollbars_in_a_frame(f2_base, true, true)
    @tree2.configure(:indent=>19)
    @tree2.debug_configure(:enable=>false, :display=>true,
                           :erasecolor=>'pink', :displaydelay=>30)

    # Give it a big border to debug drawing
    @tree2.configure(:borderwidth=>6, :relief=>:ridge, :highlightthickness=>3)

    f2_base.grid_columnconfigure(0, :weight=>1)
    f2_base.grid_rowconfigure(0, :weight=>1)
    f2.grid(:row=>0, :column=>0, :sticky=>:news, :pady=>0)

    pane2.add(pane1, :width=>200)
    pane2.add(f2_base, :width=>450)

    pane2.pack(:expand=>true, :fill=>:both)

    ###
    # A treectrl widget can generate the following built-in events:
    # <ActiveItem> called when the active item changes
    # <Collapse-before> called before an item is closed
    # <Collapse-after> called after an item is closed
    # <Expand-before> called before an item is opened
    # <Expand-after> called after an item is opened
    # <Selection> called when items are added to or removed from the selection
    # <Scroll-x> called when horizontal scroll position changes
    # <Scroll-y> called when vertical scroll position changes
    #
    # The application programmer can define custom events to be
    # generated by the "T notify generate" command. The following events
    # are generated by the example bindings.

    @tree2.notify_install_event('Header')
    @tree2.notify_install_detail('Header', 'invoke')

    @tree2.notify_install_event('Drag')
    @tree2.notify_install_detail('Drag', 'begin')
    @tree2.notify_install_detail('Drag', 'end')
    @tree2.notify_install_detail('Drag', 'receive')

    @tree2.notify_install_event('Edit')
    @tree2.notify_install_detail('Edit', 'accept')
  end

  def make_list_popup
    @popup[:bgimg] = TkVariable.new
    @popup[:bgmode] = TkVariable.new
    @popup[:debug] = Hash.new{|h, k| h[k] = TkVariable.new}
    @popup[:doublebuffer] = TkVariable.new
    @popup[:linestyle] = TkVariable.new
    @popup[:orient] = TkVariable.new
    @popup[:selectmode] = TkVariable.new
    @popup[:show] = Hash.new{|h, k| h[k] = TkVariable.new}

    menuspec = [
      [ 'Collapse', [], nil, '', {:menu_config=>{:tearoff=>false}} ],

      [ 'Expand',   [], nil, '', {:menu_config=>{:tearoff=>false}} ]
    ]

    # if $Version_1_1_OrLater
    if @has_bgimg
      menuspec << \
      [ 'Background Image',
        [
          [ 'none', [@popup[:bgimg], 'none'], nil, '',
            {:command=>proc{@tree2.backgroundimage = ''}} ],
          [ 'sky',  [@popup[:bgimg], 'sky'],  nil, '',
            {:command=>proc{
                @tree2.backgroundimage = @images[@popup[:bgimg].value]}} ]
        ],
        nil, '', {:menu_config=>{:tearoff=>false}}
      ]
    end

    menuspec.concat([
      [ 'Background Mode',
        %w(column index row visindex).collect{|val|
          [ val, [@popup[:bgmode], val] , nil, '',
            {:command=>proc{@tree2.backgroundmode = @popup[:bgmode].value}} ]
        },
        nil, '', {:menu_config=>{:tearoff=>false}}
      ],

      [ 'Debug',
        [
          [ 'Data',    @popup[:debug][:data],    nil, '',
            {:command=>proc{
                @tree2.debug_configure(:data=>@popup[:debug][:data].value)
              }
            } ],
          [ 'Display', @popup[:debug][:display], nil, '',
            {:command=>proc{
                @tree2.debug_configure(:display=>@popup[:debug][:display].value)
              }
            } ],
          [ 'Enable',  @popup[:debug][:enable],  nil, '',
            {:command=>proc{
                @tree2.debug_configure(:enable=>@popup[:debug][:enable].value)
              }
            } ]
        ],
        nil, '', {:menu_config=>{:tearoff=>false}}
      ],

      [ 'Buffering',
        [
          [ 'none',   [@popup[:doublebuffer], 'none'],   nil, '',
            {:command=>proc{
                @tree2.doublebuffer = @popup[:doublebuffer].value
              }
            } ],
          [ 'item',   [@popup[:doublebuffer], 'item'],   nil, '',
            {:command=>proc{
                @tree2.doublebuffer = @popup[:doublebuffer].value
              }
            } ],
          [ 'window', [@popup[:doublebuffer], 'window'], nil, '',
            {:command=>proc{
                @tree2.doublebuffer = @popup[:doublebuffer].value
              }
            } ]
        ],
        nil, '', {:menu_config=>{:tearoff=>false}}
      ],

      [ 'Line style',
        [
          [ 'dot',   [@popup[:linestyle], 'dot'],   nil, '',
            {:command=>proc{@tree2.linestyle = @popup[:linestyle].value}} ],
          [ 'solid', [@popup[:linestyle], 'solid'], nil, '',
            {:command=>proc{@tree2.linestyle = @popup[:linestyle].value}} ]
        ],
        nil, '', {:menu_config=>{:tearoff=>false}}
      ],

      [ 'Orient',
        [
          [ 'Horizontal', [@popup[:orient], 'horizontal'], nil, '',
            {:command=>proc{@tree2.orient = @popup[:orient].value}} ],
          [ 'Vertical',   [@popup[:orient], 'vertical'],   nil, '',
            {:command=>proc{@tree2.orient = @popup[:orient].value}} ]
        ],
        nil, '', {:menu_config=>{:tearoff=>false}}
      ],

      [ 'Selectmode',
        %w(list browse extended multiple single).collect{|val|
          [ val, [@popup[:selectmode], val] , nil, '',
            {:command=>proc{@tree2.selectmode = @popup[:selectmode].value}} ]
        },
        nil, '', {:menu_config=>{:tearoff=>false}}
      ],

      [ 'Show',
        [
          [ 'Buttons', @popup[:show][:buttons], nil, '',
            {:command=>proc{
                @tree2.showbuttons = @popup[:show][:buttons].value
              }
            } ],
          [ 'Header', @popup[:show][:header], nil, '',
            {:command=>proc{
                @tree2.showheader = @popup[:show][:header].value
              }
            } ],
          [ 'Lines', @popup[:show][:lines], nil, '',
            {:command=>proc{
                @tree2.showlines = @popup[:show][:lines].value
              }
            } ],
          [ 'Root', @popup[:show][:root], nil, '',
            {:command=>proc{
                @tree2.showroot = @popup[:show][:root].value
              }
            } ],
          [ 'Root Button', @popup[:show][:rootbutton], nil, '',
            {:command=>proc{
                @tree2.showrootbutton = @popup[:show][:rootbutton].value
              }
            } ]
        ],
        nil, '', {:menu_config=>{:tearoff=>false}}
      ],

      [ 'Visible', [], nil, '', {:menu_config=>{:tearoff=>false}} ]
    ])

    m = TkMenu.new_menuspec(menuspec, @tree2, false)
    @non_clear_list << m
    @mTree[@tree2] = m
  end

  def show_list_popup(w, x, y, rootx, rooty)
    id = w.identify(x, y)
    unless id.empty?
      if id[0] == 'header'
        col = id[1]
        @popup[:column].value = col
        @popup[:arrow].value = w.column_cget(col, :arrow)
        @popup[:arrowside].value = w.column_cget(col, :arrowside)
        @popup[:arrowgravity].value = w.column_cget(col, :arrowgravity)
        @popup[:expand].value = w.column_cget(col, :expand)
        @popup[:squeeze].value = w.column_cget(col, :squeeze)
        @popup[:justify].value = w.column_cget(col, :justify)
        @mHeader[w].popup(rootx, rooty)
        return
      end
    end

    m = @mTree[w].entrycget('Collapse', :menu)
    m.delete(0, :end)
    if $Version_1_1_OrLater
      m.add_command(:label=>'All', :command=>proc{w.item_collapse(:all)})
    else
      m.add_command(:label=>'All', :command=>proc{w.collapse(:all)})
    end
    unless id.empty?
      if id[0] == 'item'
        item = id[1]
        if $Version_1_1_OrLater
          m.add_command(:label=>"Item #{item}",
                        :command=>proc{w.item_collapse(item)})
          m.add_command(:label=>"Item #{item} (recurse)",
                        :command=>proc{w.item_collapse_recurse(item)})
        else
          m.add_command(:label=>"Item #{item}",
                        :command=>proc{w.collapse(item)})
          m.add_command(:label=>"Item #{item} (recurse)",
                        :command=>proc{w.collapse_recurse(item)})
        end
      end
    end

    m = @mTree[w].entrycget('Expand', :menu)
    m.delete(0, :end)
    if $Version_1_1_OrLater
      m.add_command(:label=>'All', :command=>proc{w.item_expand(:all)})
    else
      m.add_command(:label=>'All', :command=>proc{w.expand(:all)})
    end
    unless id.empty?
      if id[0] == 'item'
        item = id[1]
        if $Version_1_1_OrLater
          m.add_command(:label=>"Item #{item}",
                        :command=>proc{w.item_expand(item)})
          m.add_command(:label=>"Item #{item} (recurse)",
                        :command=>proc{w.item_expand_recurse(item)})
        else
          m.add_command(:label=>"Item #{item}",
                        :command=>proc{w.expand(item)})
          m.add_command(:label=>"Item #{item} (recurse)",
                        :command=>proc{w.expand_recurse(item)})
        end
      end
    end

    [:data, :display, :enable].each{|k|
      @popup[:debug][k].value = w.debug_cget(k)
    }
    # if $Version_1_1_OrLater
    if @has_bgimg
      @popup[:bgimg].value = @images.key(w[:backgroundimage])
    end
    @popup[:bgmode].value = w[:backgroundmode]
    @popup[:doublebuffer].value = w[:doublebuffer]
    @popup[:linestyle].value = w[:linestyle]
    @popup[:orient].value = w[:orient]
    @popup[:selectmode].value = w[:selectmode]
    @popup[:show][:buttons].value = w[:showbuttons]
    @popup[:show][:header].value = w[:showheader]
    @popup[:show][:lines].value = w[:showlines]
    @popup[:show][:root].value = w[:showroot]
    @popup[:show][:rootbutton].value = w[:showrootbutton]

    m = @mTree[w].entrycget('Visible', :menu)
    m.delete(0, :end)
    @popup[:visible] = []
    (0...(w.numcolumns)).each{|i|
      @popup[:visible][i] = TkVariable.new(w.column_cget(i, :visible))
      txt = w.column_cget(i, :text)
      img_name = w.column_cget(i, :image)
      img_name = @images.key(img_name) if img_name.kind_of?(TkImage)
      m.add_checkbutton(:variable=>@popup[:visible][i],
                        :label=>"Column #{i} \"#{txt}\" [#{img_name}]",
                        :command=>proc{w.column_configure(i, :visible=>@popup[:visible][i].value)})
    }

    @mTree[w].popup(rootx, rooty)
  end

  def make_header_popup
    @popup[:column] = TkVariable.new unless @popup[:column]
    @popup[:arrow] = TkVariable.new
    @popup[:arrowside] = TkVariable.new
    @popup[:arrowgravity] = TkVariable.new
    @popup[:expand] = TkVariable.new
    @popup[:squeeze] = TkVariable.new
    @popup[:justify] = TkVariable.new

    menuspec = [
      [ 'Arrow',
        [
          [ 'None', [@popup[:arrow], 'none'], nil, '',
            {:command=>proc{
                @tree2.column_configure(@popup[:column].value, :arrow=>:none)
              }
            } ],
          [ 'Up', [@popup[:arrow], 'up'], nil, '',
            {:command=>proc{
                @tree2.column_configure(@popup[:column].value, :arrow=>:up)
              }
            } ],
          [ 'Down', [@popup[:arrow], 'down'], nil, '',
            {:command=>proc{
                @tree2.column_configure(@popup[:column].value, :arrow=>:down)
              }
            } ],

          '---',

          [ 'Side Left', [@popup[:arrowside], 'left'], nil, '',
            {:command=>proc{
                @tree2.column_configure(@popup[:column].value,
                                        :arrowside=>:left)
              }
            } ],
          [ 'Side Right', [@popup[:arrowside], 'right'], nil, '',
            {:command=>proc{
                @tree2.column_configure(@popup[:column].value,
                                        :arrowside=>:right)
              }
            } ],

          '---',

          [ 'Gravity Left', [@popup[:arrowgravity], 'left'], nil, '',
            {:command=>proc{
                @tree2.column_configure(@popup[:column].value,
                                        :arrowgravity=>:left)
              }
            } ],
          [ 'Gravity Right', [@popup[:arrowgravity], 'right'], nil, '',
            {:command=>proc{
                @tree2.column_configure(@popup[:column].value,
                                        :arrowgravity=>:right)
              }
            } ],
        ],
        nil, '', {:menu_config=>{:tearoff=>false}} ],

      [ 'Expand',  @popup[:expand],  nil, '',
        {:command=>proc{
            @tree2.column_configure(@popup[:column].value,
                                    :expand=>@popup[:expand].value)
          }
        } ],

      [ 'Squeeze', @popup[:squeeze], nil, '',
        {:command=>proc{
            @tree2.column_configure(@popup[:column].value,
                                    :squeeze=>@popup[:squeeze].value)
          }
        } ],

      [ 'Justify',
        [
          [ 'Left', [@popup[:justify], 'left'], nil, '',
            {:command=>proc{
                @tree2.column_configure(@popup[:column].value, :justify=>:left)
              }
            } ],
          [ 'Center', [@popup[:justify], 'center'], nil, '',
            {:command=>proc{
                @tree2.column_configure(@popup[:column].value,
                                        :justify=>:center)
              }
            } ],
          [ 'Right', [@popup[:justify], 'right'], nil, '',
            {:command=>proc{
                @tree2.column_configure(@popup[:column].value,
                                        :justify=>:right)
              }
            } ]
        ],
        nil, '', {:menu_config=>{:tearoff=>false}} ]
    ]

    m = TkMenu.new_menuspec(menuspec, @tree2, false)
    @non_clear_list << m
    @mHeader[@tree2] = m
  end

  ###########################

  def init_demo_scripts_module
    @demo_scripts = Module.new

    master = self

    has_bgimg = @has_bgimg

    scriptDir = @ScriptDir

    thisPlatform = @thisPlatform

    randomN = @RandomN

    images = @images
    sel_images = @sel_images

    systemButtonFace = @SystemButtonFace
    systemHighlight  = @SystemHighlight
    systemHighlightText = @SystemHighlightText

    def master._pub_display_styles_in_item(item)
      display_styles_in_item(item)
    end
    proc_disp_styles_in_item = proc{|item|
      master._pub_display_styles_in_item(item)
    }

    @demo_scripts.instance_eval{
      @master = master

      @has_bgimg = has_bgimg

      @display_styles_in_item = proc_disp_styles_in_item

      @Priv = TkVarAccess.new('::TreeCtrl::Priv')

      @ScriptDir = scriptDir

      @thisPlatform = thisPlatform

      @RandomN = randomN

      @images = images
      @sel_images = sel_images

      @SystemButtonFace = systemButtonFace
      @SystemHighlight  = systemHighlight
      @SystemHighlightText = systemHighlightText
    }

    class << @demo_scripts
      def _get_binding
        binding
      end
      private :_get_binding

      def load_demo(file)
        puts "load \"#{file}\"" if $DEBUG
        begin
          eval(IO.readlines(file).join, _get_binding())
        rescue Exception => e
          bt = e.backtrace

          if bt[0] =~ /^([^:]+):(\d+):/
            errline = $2.to_i
          else
            raise e
          end

          if bt[1] =~ /^([^:]+):(\d+):/
            bt.unshift("#{file}:#{errline - $2.to_i + 1}")
            raise e
          else
            raise e
          end
        end
      end

      def init_pics(*args)
        @master.init_pics(*args)
      end
    end
  end

  def load_demo_scripts
    # demo sources
    [
      'bitmaps',
      'explorer',
      'help',
      'imovie',
      'layout',
      'mailwasher',
      'outlook-folders',
      'outlook-newgroup',
      'random',
      'www-options'
    ].each{|f|
      @demo_scripts.load_demo(File.join(@ScriptDir, "#{f}.rb"))
    }
  end

  ###########################

  def init_demo_list
    @tree1.element_create('e1', :text,
                          :fill=>[@SystemHighlightText, ['selected', 'focus']])
    @tree1.element_create('e2', :rect, :showfocus=>true,
                          :fill=>[
                            @SystemHighlight, ['selected', 'focus'],
                            'gray', ['selected', '!focus'],
                          ])
    @tree1.style_create('s1')
    @tree1.style_elements('s1', ['e2', 'e1'])

    # Tk listbox has linespace + 1 height
    @tree1.style_layout('s1', 'e2', :union=>['e1'],
                        :ipadx=>2, :ipady=>[0, 1], :iexpand=>:e)

    if $Version_1_1_OrLater
      @tree1.defaultstyle = 's1'
    end

    ###
    [
      ["Random #{@RandomN[0]} Items", :demoRandom, 'random.rb'],
      ["Random #{@RandomN[0]} Items, Button Images", :demoRandom2, 'random.rb'],
      ["Outlook Express (Folders)", :demoOutlookFolders, 'outlook-folders.rb'],
      ["Outlook Express (Newsgroup)", :demoOutlookNewsgroup, 'outlook-newgroup.rb'],
      ["Explorer (Details)", :demoExplorerDetails, 'explorer.rb'],
      ["Explorer (List)", :demoExplorerList, 'explorer.rb'],
      ["Explorer (Large icons)", :demoExplorerLargeIcons, 'explorer.rb'],
      ["Explorer (Small icons)", :demoExplorerSmallIcons, 'explorer.rb'],
      ["Internet Options", :demoInternetOptions, 'www-options.rb'],
      ["Help Contents", :demoHelpContents, 'help.rb'],
      ["Layout", :demoLayout, 'layout.rb'],
      ["MailWasher", :demoMailWasher, 'mailwasher.rb'],
      ["Bitmaps", :demoBitmaps, 'bitmaps.rb'],
      ["iMovie", :demoIMovie, 'imovie.rb']
    ].each{|label, cmd, file|
      item = @tree1.item_create
      @tree1.item_lastchild(:root, item)
      unless $Version_1_1_OrLater
        @tree1.item_style_set(item, 0, 's1')
      end
      @tree1.item_text(item, 0, label)
      @demoCmd[item] = cmd
      @demoFile[item] = file
    }

    @tree1.yview_moveto(0.0)
  end

  def demo_set(cmd, file)
    demo_clear()
    clicks = Tk::Clock.clicks
    @demo_scripts.__send__(cmd, @tree2)
    clicks = Tk::Clock.clicks - clicks
    puts "set list in #{'%.2g'%(clicks/1000000.0)} seconds (#{clicks} clicks)"
    @tree2.xview_moveto(0)
    @tree2.yview_moveto(0)
    Tk.update
    display_styles_in_list()
    show_source(file)
  end

  def display_styles_in_list
    # Create elements and styles the first time this is called
    if @tree4.style_names.empty?
      @tree4.element_create('e1', :text,
                            :fill=>[@SystemHighlightText,['selected','focus']])
      @tree4.element_create('e2', :text,
                            :fill=>[
                              @SystemHighlightText, ['selected','focus'],
                              '', ['selected','!focus'],
                              'blue', []
                            ])
      @tree4.element_create('e3', :rect, :showfocus=>true,
                            :fill=>[
                              @SystemHighlight, ['selected','focus'],
                              'gray', ['selected', '!focus']
                            ])

      @tree4.style_create('s1')
      @tree4.style_elements('s1', ['e3', 'e1'])
      @tree4.style_layout('s1', 'e3', :union=>['e1'], :ipadx=>1, :ipady=>[0,1])

      @tree4.style_create('s2')
      @tree4.style_elements('s2', ['e3', 'e1', 'e2'])
      @tree4.style_layout('s2', 'e1', :padx=>[0,4])
      @tree4.style_layout('s2', 'e3', :union=>['e1', 'e2'],
                          :ipadx=>1, :ipady=>[0,1])
    end

    # Clear the list
    @tree4.item_delete(:all)

    # One item for each element in the demo list
    @tree2.element_names.sort.each{|elem|
      if $Version_1_1_OrLater
        item = @tree4.item_create(:button=>true)
        @tree4.item_collapse(item)
      else
        item = @tree4.item_create
        @tree4.item_hasbutton(item, true)
        @tree4.collapse(item)
      end
      @tree4.item_style_set(item, 0, 's1')
      @tree4.item_text(item, 0,
                       "Element #{elem} (#{@tree2.element_type(elem)})")

      # One item for each configuration option for this element
      @tree2.element_configinfo(elem).each{|name, x, y, default, current|
        item2 = @tree4.item_create

        if default == current
          @tree4.item_style_set(item2, 0, 's1')
          @tree4.item_complex(item2, [
                                ['e1', {:text=>"#{name} #{current.inspect}"}]
                              ])
        else
          @tree4.item_style_set(item2, 0, 's2')
          @tree4.item_complex(item2, [
                                ['e1', {:text=>name}],
                                ['e2', {:text=>current.inspect}]
                              ])
        end

        @tree4.item_lastchild(item, item2)
      }

      @tree4.item_lastchild(:root, item)
    }

    # One item for each style in the demo list
    @tree2.style_names.sort.each{|sty|
      if $Version_1_1_OrLater
        item = @tree4.item_create(:button=>true)
        @tree4.item_collapse(item)
      else
        item = @tree4.item_create
        @tree4.item_hasbutton(item, true)
        @tree4.collapse(item)
      end
      @tree4.item_style_set(item, 0, 's1')
      @tree4.item_text(item, 0, "Style #{sty}")

      # One item for each element in the style
      @tree2.style_elements(sty).each{|elem|
        if $Version_1_1_OrLater
          item2 = @tree4.item_create(:button=>true)
          @tree4.item_collapse(item2)
        else
          item2 = @tree4.item_create
          @tree4.item_hasbutton(item2, true)
          @tree4.collapse(item2)
        end
        @tree4.item_style_set(item2, 0, 's1')
        @tree4.item_text(item2, 0,
                         "Element #{elem} (#{@tree2.element_type(elem)})")

        # One item for each layout option for this element in this style
        @tree2.style_layout(sty, elem).each{|k, v|
          item3 = @tree4.item_create
          unless $Version_1_1_OrLater
            @tree4.item_hasbutton(item3, false)
          end
          @tree4.item_style_set(item3, 0, 's1')
          @tree4.item_text(item3, 0, "#{k} #{v.inspect}")
          @tree4.item_lastchild(item2, item3)
        }

        @tree4.item_lastchild(item, item2)
      }

      @tree4.item_lastchild(:root, item)
    }

    @tree4.xview_moveto(0)
    @tree4.yview_moveto(0)
  end

  def display_styles_in_item(item)
    @tree3.column_configure(0, :text=>"Styles in item #{@tree2.index(item)}")

    # Create elements and styles the first time this is called
    if @tree3.style_names.empty?
      @tree3.element_create('e1', :text,
                            :fill=>[@SystemHighlightText,['selected','focus']])
      @tree3.element_create('e2', :text,
                            :fill=>[
                              @SystemHighlightText, ['selected','focus'],
                              '', ['selected','!focus'],
                              'blue', []
                            ])
      @tree3.element_create('e3', :rect, :showfocus=>true,
                            :fill=>[
                              @SystemHighlight, ['selected','focus'],
                              'gray', ['selected', '!focus']
                            ])

      @tree3.style_create('s1')
      @tree3.style_elements('s1', ['e3', 'e1'])
      @tree3.style_layout('s1', 'e3', :union=>['e1'], :ipadx=>1, :ipady=>[0,1])

      @tree3.style_create('s2')
      @tree3.style_elements('s2', ['e3', 'e1', 'e2'])
      @tree3.style_layout('s2', 'e1', :padx=>[0,4])
      @tree3.style_layout('s2', 'e3', :union=>['e1', 'e2'],
                          :ipadx=>1, :ipady=>[0,1])
    end
    # Clear the list
    @tree3.item_delete(:all)

    # One item for each item-column
    column = 0
    @tree2.item_style_set(item).each{|sty|
      item2 = @tree3.item_create
      if $Version_1_1_OrLater
        @tree3.item_collapse(item2)
      else
        @tree3.collapse(item2)
      end
      @tree3.item_style_set(item2, 0, 's1')
      @tree3.item_element_configure(item2, 0, 'e1',
                                    :text=>"Column #{column}: Style #{sty}")

      button = false

      # One item for each element in this style
      unless sty.to_s.empty?
        @tree2.item_style_elements(item, column).each{|elem|
          button = true
          if $Version_1_1_OrLater
            item3 = @tree3.item_create(:button=>true)
          else
            item3 = @tree3.item_create
            @tree3.item_hasbutton(item3, true)
          end
          if $Version_1_1_OrLater
            @tree3.item_collapse(item3)
          else
            @tree3.collapse(item3)
          end
          @tree3.item_style_set(item3, 0, 's1')
          @tree3.item_element_configure(item3, 0, 'e1',
                                        :text=>"Element #{elem} (#{@tree2.element_type(elem)})")

          # One item for each configuration option in this element
          @tree2.item_element_configinfo(item, column, elem) \
          .each{|name, x, y, default, current|
            item4 = @tree3.item_create
            masterDefault = @tree2.element_cget(elem, name)
            sameAsMaster = (masterDefault == current)
            if !sameAsMaster && current == ''
              sameAsMaster = true
              current = masterDefault
            end

            if sameAsMaster
              @tree3.item_style_set(item4, 0, 's1')
              @tree3.item_complex(item4, [
                                    ['e1',
                                      {:text=>"#{name} #{current.inspect}"}]
                                  ])
            else
              @tree3.item_style_set(item4, 0, 's2')
              @tree3.item_complex(item4, [
                                    ['e1', {:text=>name}],
                                    ['e2', {:text=>current.inspect}]
                                  ])
            end
            @tree3.item_lastchild(item3, item4)
          }
          @tree3.item_lastchild(item2, item3)
        }
        if $Version_1_1_OrLater
          @tree3.item_configure(item2, :button=>true) if button
        else
          @tree3.item_hasbutton(item2, true) if button
        end
      end
      @tree3.item_lastchild(:root, item2)
      column += 1
    }

    @tree3.xview_moveto(0)
    @tree3.yview_moveto(0)
  end

  def demo_clear
    # Clear the demo list
    @tree2.item_delete(:all)

    # Clear all bindings on the demo list added by the previous demo.
    # This is why DontDelete is used for the <Selection> binding.
    @tree2.notify_bindinfo(@tree2).each{|ev|
      @tree2.notify_bind_remove(@tree2, ev)
    }

    # Clear all run-time states
    @tree2.state_names.each{|st| @tree2.state_undefine(st) }

    # Clear the styles-in-item list
    @tree3.item_delete(:all)

    # Delete columns in demo list
    while (@tree2.numcolumns > 0)
      @tree2.column_delete(0)
    end

    # Delete all styles in demo list
    @tree2.style_delete(*(@tree2.style_names))

    # Delete all elements in demo list
    @tree2.element_delete(*(@tree2.element_names))

    if $Version_1_1_OrLater
      @tree2.item_configure(:root, :button=>false)
      @tree2.item_expand(:root)
    else
      @tree2.item_hasbutton(:root, false)
      @tree2.expand(:root)
    end

    # Restore some happy defaults to the demo list
    # if $Version_1_1_OrLater
    if @has_bgimg
      @tree2.configure(:orient=>:vertical, :wrap=>'',
                       :xscrollincrement=>0, :yscrollincrement=>0,
                       :itemheight=>0, :showheader=>true,
                       :background=>'white', :scrollmargin=>0,
                       :xscrolldelay=>50, :yscrolldelay=>50,
                       :openbuttonimage=>'', :closedbuttonimage=>'',
                       :backgroundmode=>:row, :treecolumn=>0, :indent=>19,
                       :defaultstyle=>'', :backgroundimage=>'')
    else
      @tree2.configure(:orient=>:vertical, :wrap=>'',
                       :xscrollincrement=>0, :yscrollincrement=>0,
                       :itemheight=>0, :showheader=>true,
                       :background=>'white', :scrollmargin=>0,
                       :xscrolldelay=>50, :yscrolldelay=>50,
                       :openbuttonimage=>'', :closedbuttonimage=>'',
                       :backgroundmode=>:row, :treecolumn=>0, :indent=>19)
    end

    # Restore default bindings to the demo list
    @tree2.bindtags = [ @tree2, Tk::TreeCtrl, @tree2.winfo_toplevel, :all ]

    @tree2.winfo_children.each{|w|
      w.destroy unless @non_clear_list.include?(w)
    }
  end
end

TkTreeCtrl_demo.new($ScriptDir)

##############################################

def cursor_window(top = nil)
  top.destroy if top.kind_of?(TkWindow) && top.winfo_exist?
  top = TkToplevel.new(:title=>'Cursor Window')

  c = TkCanvas.new(top, :background=>'white',
                   :width=>50*10, :highlightthickness=>0,
                   :borderwidth=>0).pack(:expand=>true, :fill=>:both)
  cursors = %w(
	X_cursor
	arrow
	based_arrow_down
	based_arrow_up
	boat
	bogosity
	bottom_left_corner
	bottom_right_corner
	bottom_side
	bottom_tee
	box_spiral
	center_ptr
	circle
	clock
	coffee_mug
	cross
	cross_reverse
	crosshair
	diamond_cross
	dot
	dotbox
	double_arrow
	draft_large
	draft_small
	draped_box
	exchange
	fleur
	gobbler
	gumby
	hand1
	hand2
	heart
	icon
	iron_cross
	left_ptr
	left_side
	left_tee
	leftbutton
	ll_angle
	lr_angle
	man
	middlebutton
	mouse
	pencil
	pirate
	plus
	question_arrow
	right_ptr
	right_side
	right_tee
	rightbutton
	rtl_logo
	sailboat
	sb_down_arrow
	sb_h_double_arrow
	sb_left_arrow
	sb_right_arrow
	sb_up_arrow
	sb_v_double_arrow
	shuttle
	sizing
	spider
	spraycan
	star
	target
	tcross
	top_left_arrow
	top_left_corner
	top_right_corner
	top_side
	top_tee
	trek
	ul_angle
	umbrella
	ur_angle
	watch
	xterm
    )

  orig_cursor = c.cursor
  col = 0
  row = 0

  cursors.each{|cur|
    x = col * 50
    y = row * 40

    begin
      c.cursor = cur

      r = TkcRectangle.new(c, x, y, x+50, y+40,
                           :fill=>'gray90', :outline=>'black', :width=>2)
      t = TkcText.new(c, x+50/2, y+4, :text=>cur, :anchor=>:n, :width=>42)

      col += 1
      if col >= 10
        col = 0
        row += 1
      end

      r.bind('Enter', proc{c.cursor = cur; r.fill = 'linen'})
      r.bind('Leave', proc{c.cursor = ''; r.fill = 'gray90'})

      t.bind('Enter', proc{c.cursor = cur})
      t.bind('Leave', proc{c.cursor = ''})
    rescue
      c.cursor = orig_cursor
    end
  }

  c.cursor = orig_cursor
  c.height = (row + 1) * 40
end

cursor_window()

##############################################

# A little screen magnifier for X11
if Tk::PLATFORM['platform'] == 'unix' && Tk.windowingsystem != 'aqua'
  def show_loupe(setting=nil)
    loupe = (setting.kind_of?(Hash))? setting: {}
    loupe[:zoom] = 3 unless loupe[:zoom]
    loupe[:x] = 0 unless loupe[:x]
    loupe[:y] = 0 unless loupe[:y]
    loupe[:auto] = true unless loupe[:auto]
    loupe[:delay] = 500 unless loupe[:delay]
    loupe[:image] =
      TkPhotoImage.new(:width=>150, :height=>150) unless loupe[:image]

    top = TkToplevel.new(:geometry=>'-0+30',
                         :title=>'A little screen magnifier for X11')
    TkLabel.new(top, :image=>loupe[:image]).pack

    TkTimer.new(proc{loupe[:delay]}, -1, proc{
                  x, y = TkWinfo.pointerxy(Tk.root)
                  if loupe[:auto] || loupe[:x] != x || loupe[:y] != y
                    w = loupe[:image].width
                    h = loupe[:image].height
                    Tk::TreeCtrl.loupe(loupe[:image], x, y, w, h, loupe[:zoom])
                    loupe[:x] = x
                    loupe[:y] = y
                  end
                }).start
  end

  show_loupe()
end

##############################################

Tk.mainloop
