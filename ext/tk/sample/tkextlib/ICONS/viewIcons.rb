#!/usr/bin/env ruby
#
#   viewIcons.rb
#
#     --  Display icons from icon library.
#
#     --  Copy the clicked icon data (command string of creating
#         a TkPhotoImage instance) to the clipboard.
#
require 'tk'
require 'tkextlib/ICONS'

class ViewIcons
  #####################################
  private
  #####################################
  def _create_controls
    @controls = base = TkFrame.new
    columns = TkFrame.new(base)
    line1 = TkFrame.new(base, :height=>2, :borderwidth=>1, :relief=>:sunken)
    line2 = TkFrame.new(base, :height=>2, :borderwidth=>1, :relief=>:sunken)

    lbl_library = TkLabel.new(base, :font=>@boldfont, :text=>'Library')
    lbl_groups  = TkLabel.new(base, :font=>@boldfont, :text=>'Groups')
    lbl_columns = TkLabel.new(base, :font=>@boldfont, :text=>'Columns')

    ent_library = TkEntry.new(base, :width=>50, :textvariable=>@library)
    ent_groups  = TkEntry.new(base, :width=>50, :textvariable=>@groups)

    btn_browse = TkButton.new(base, :text=>'Browse',
                                    :command=>method(:select_icons))
    btn_view   = TkButton.new(base, :text=>'View',
                                    :command=>method(:display_icons))
    btn_exit   = TkButton.new(base, :text=>'Exit', :command=>proc{exit})

    @column_btns = {}
    6.step(20, 2){|i|
      @column_btns[i] = TkButton.new(columns,
                                     :text=>i.to_s, :width=>2,
                                     :command=>proc{set_columns(i)}
                                     ).pack(:side=>:left)
    }
    @column_btns[@columns][:relief] = :sunken

    lbl_library.grid(:row=>0, :column=>0, :padx=>4)
    ent_library.grid(:row=>0, :column=>1)
    btn_browse.grid(:row=>0, :column=>2, :padx=>4, :pady=>2, :sticky=>:ew)

    line1.grid(:row=>1, :column=>0, :pady=>2, :columnspan=>3, :sticky=>:ew)

    lbl_groups.grid(:row=>2, :column=>0, :padx=>4)
    ent_groups.grid(:row=>2, :column=>1)
    btn_view.grid(:row=>2, :column=>2, :padx=>4, :pady=>2, :sticky=>:ew)

    line1.grid(:row=>3, :column=>0, :pady=>2, :columnspan=>3, :sticky=>:ew)

    lbl_columns.grid(:row=>4, :column=>0, :padx=>4)
    columns.grid(:row=>4, :column=>1, :padx=>2, :sticky=>:ew)
    btn_exit.grid(:row=>4, :column=>2, :padx=>4, :pady=>2, :sticky=>:ew)

    base.pack

    ent_library.bind('Return', method(:display_icons), '')
    ent_groups.bind('Return', method(:display_icons), '')
  end

  def _create_display
    base = TkFrame.new(:borderwidth=>2, :relief=>:sunken)

    @icons_window = icons = TkCanvas.new(base)
    xscr = icons.xscrollbar(TkScrollbar.new(base))
    yscr = icons.yscrollbar(TkScrollbar.new(base))

    icons.grid(:row=>0, :column=>0, :sticky=>:news)
    yscr.grid(:row=>0, :column=>1, :sticky=>:ns)
    xscr.grid(:row=>1, :column=>0, :sticky=>:ew)
    base.grid_columnconfigure(0, :weight=>1)
    base.grid_columnconfigure(1, :weight=>0)
    base.grid_rowconfigure(0, :weight=>1)
    base.grid_rowconfigure(1, :weight=>0)
    # yscr.pack(:side=>:right, :fill=>:y)
    # xscr.pack(:side=>:bottom, :fill=>:x)
    # icons.pack(:side=>:left, :fill=>:both, :expand=>true)

    @icons_layout = TkFrame.new(icons).pack
    TkcWindow.create(icons, 0, 0, :anchor=>:nw, :window=>@icons_layout)
    @icons_layout.bind('Configure', method(:layout_resize), '')

    base.pack(:expand=>true, :fill=>:both)
  end

  def _create_info_window
    @info_window = TkToplevel.new(:background=>'lightyellow', :borderwidth=>1,
                                  :relief=>:solid){|w|
      lbl_name = TkLabel.new(w, :text=>'Name', :background=>'lightyellow',
                             :font=>@boldfont, :justify=>:left)
      lbl_grps = TkLabel.new(w, :text=>'Groups', :background=>'lightyellow',
                             :font=>@boldfont, :justify=>:left)
      lbl_type = TkLabel.new(w, :text=>'Type', :background=>'lightyellow',
                             :font=>@boldfont, :justify=>:left)
      lbl_size = TkLabel.new(w, :text=>'Size', :background=>'lightyellow',
                             :font=>@boldfont, :justify=>:left)

      lbl_name.grid(:row=>0, :column=>0, :sticky=>:w)
      lbl_grps.grid(:row=>1, :column=>0, :sticky=>:w)
      lbl_type.grid(:row=>2, :column=>0, :sticky=>:w)
      lbl_size.grid(:row=>3, :column=>0, :sticky=>:w)

      @name = TkLabel.new(w, :background=>'lightyellow', :justify=>:left)
      @grps = TkLabel.new(w, :background=>'lightyellow', :justify=>:left)
      @type = TkLabel.new(w, :background=>'lightyellow', :justify=>:left)
      @size = TkLabel.new(w, :background=>'lightyellow', :justify=>:left)

      @name.grid(:row=>0, :column=>1, :sticky=>:w)
      @grps.grid(:row=>1, :column=>1, :sticky=>:w)
      @type.grid(:row=>2, :column=>1, :sticky=>:w)
      @size.grid(:row=>3, :column=>1, :sticky=>:w)

      def name(txt)
        @name['text'] = txt
      end
      def groups(txt)
        @grps['text'] = txt
      end
      def type(txt)
        @type['text'] = txt
      end
      def size(txt)
        @size['text'] = txt
      end

      overrideredirect(true)
      withdraw
    }
  end

  def initialize(init_path = Tk::LIBRARY)
    init_path = Tk::LIBRARY unless init_path
    init_path = File.expand_path(init_path)
    if File.directory?(init_path)
      @initial_dir  = init_path
      @initial_file = 'tkIcons'
    else
      @initial_dir  = File.dirname(init_path)
      @initial_file = File.basename(init_path)
    end

    if Tk::PLATFORM['platform'] == 'unix'
      TkOption.add('*HighlightThickness', 0)
    end

    @columns = 14
    @command = ""

    @delay_timer = nil

    dummy = TkLabel.new
    @font = dummy.font
    @boldfont = TkFont.new(@font, :weight=>:bold)
    @icons = {}
    @icon_name = {}
    @icon_info = {}

    @library = TkVariable.new(File.join(@initial_dir, @initial_file))
    @groups  = TkVariable.new('*')

    _create_controls

    _create_display

    _create_info_window

    Tk.root.title('viewIcons')
    layout_resize
    Tk.root.resizable(false, true)

    display_icons
  end

  def init_info(item, name)
    @icon_name[item] = name

    item.bind('Button-1', method(:clip_info),  '%W')
    item.bind('Enter',    method(:delay_info), '%W')
    item.bind('Leave',    method(:cancel_info), '')
  end

  def delay_info(item)
    cancel_info
    @delay_timer = TkTimer.new(200, 1, proc{ show_info(item) }).start
  end

  def cancel_info
    if @delay_timer
      @delay_timer.cancel
      @delay_timer = nil
    end
    @info_window.withdraw
  end

  def show_info(item)
    name, groups, type, size = @icon_info[@icon_name[item]]
    @info_window.name(name)
    @info_window.groups(groups)
    @info_window.type(type)
    @info_window.size(size)

    info_x = item.winfo_rootx + 10
    info_y = item.winfo_rooty + item.winfo_height

    @info_window.geometry("+#{info_x}+#{info_y}")
    @info_window.deiconify

    @info_window.raise

    @delay_timer = nil
  end

  def primary_transfer(offset, max_chars)
    @command
  end

  def lost_selection
    @command = ""
  end

  def clip_info(item)
    name = @icon_name[item]
    data_width = 60

    cmd = "#{name} = TkPhotoImage.new(:data=><<'EOD')\n"

    icon_data = Tk::ICONS.query(name, :file=>@library.value, :items=>'d')[0][0]

    icon_data.scan(/.{1,#{data_width}}/m){|s| cmd << '   ' << s << "\n"}

    cmd << "EOD\n"

    @command = cmd

    TkClipboard.clear
    TkClipboard.append(@command)

    if Tk::PLATFORM['platform'] == 'unix'
      TkSelection.handle(Tk.root, method(:primary_transfer),
                         :selection=>'PRIMARY')
      TkSelection.set_owner(Tk.root, :selection=>'PRIMARY',
                            :command=>method(:lost_selection))
    end

    Tk.bell
  end

  def layout_resize
    Tk.update
    bbox = @icons_window.bbox('all')
    width = @controls.winfo_width - @icons_window.yscrollbar.winfo_width - 8

    @icons_window.configure(:width=>width, :scrollregion=>bbox,
                            :xscrollincrement=>'0.1i',
                            :yscrollincrement=>'0.1i')
  end

  def select_icons
    new_lib = Tk.getOpenFile(:initialdir=>@initial_dir,
                             :initialfile=>'tkIcons',
                             :title=>'Select Icon Library',
                             :filetypes=>[
                               ['Icon Libraries', ['tkIcons*']],
                               ['All Files', ['*']]
                             ])

    @library.value = new_lib if new_lib.length != 0
    display_icons
  end

  def display_icons
    column = 0
    limit = @columns - 1
    row = 0

    unless File.exist?(@library.value)
      Tk.messageBox(:icon=>'warning', :message=>'File does not exist',
                    :title=>'viewIcons')
      return
    end

    cursor = Tk.root[:cursor]
    Tk.root[:cursor] = 'watch'

    Tk::ICONS.delete(@icons)

    @icons_frame.destroy if @icons_frame
    @icons_frame = TkFrame.new(@icons_layout).pack

    @icons = Tk::ICONS.create(:file=>@library.value, :group=>@groups.value)

    Tk::ICONS.query(:file=>@library.value, :group=>@groups.value).each{|inf|
      name = inf[0]
      @icon_info[name] = inf

      lbl = TkLabel.new(@icons_frame, :image=>"::icon::#{name}")
      lbl.grid(:column=>column, :row=>row, :padx=>3, :pady=>3)
      # lbl.grid_columnconfigure column

      init_info(lbl, name)

      if column == limit
        column = 0
        row += 1
      else
        column += 1
      end
    }

    Tk.root[:cursor] = cursor
  end

  def set_columns(columns)
    @columns = columns
    6.step(20, 2){|i| @column_btns[i][:relief] = :raised }
    @column_btns[@columns][:relief] = :sunken
    display_icons
  end
end

ViewIcons.new(ARGV[0])

Tk.mainloop
