#
# tkmultilistframe.rb : multiple listbox widget on scrollable frame
#                       by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'

class TkMultiListFrame < TkListbox
  include TkComposite

  #   lbox_height : height of listboxes (pixel)
  #   title_info  : array [ [<title_string>,<init_width>], ... ]
  #   keys        : hash {<option>=><value>, ... }
  def initialize_composite(lbox_height, title_info, keys={})
    # argument check
    if (! title_info.kind_of? Array) or (title_info.size < 2)
      raise
    end

    # mode
    @keep_minsize = true
    @show_each_hscr = true
    @show_win_hscr = true

    # init arrays
    @base_list  = []
    @rel_list   = []
    @title_list = []
    @title_cmd  = []
    @lbox_list  = []
    @hscr_list  = []

    # decide total width
    @lbox_total = title_info.size
    @width_total = 0
    title_info.each{|title, width, cmd|
      @width_total += width.to_f
      @title_cmd << cmd
    }

    # rel-table of label=>index
    @name_index = {}

    # size definition
    @window_width = @width_total
    @sash = 5
    @scrbar_width = 15
    @scrbar_border = 3
    @lbox_border = 1
    @title_border = 3
    @h_l_thick = 0

    # init status
    @mode = :title

    # virtical scrollbar
=begin
    @v_scroll = TkScrollbar.new(@frame, 'highlightthickness'=>@h_l_thick,
                                'borderwidth'=>@scrbar_border,
                                'orient'=>'vertical', 'width'=>@scrbar_width)
=end
    @v_scroll = TkYScrollbar.new(@frame, 'highlightthickness'=>@h_l_thick,
                                 'borderwidth'=>@scrbar_border,
                                 'width'=>@scrbar_width)

    # horizontal scrollbar
=begin
    @h_scroll = TkScrollbar.new(@frame, 'highlightthickness'=>@h_l_thick,
                                'borderwidth'=>@scrbar_border,
                                'orient'=>'horizontal', 'width'=>@scrbar_width)
=end
    @h_scroll = TkXScrollbar.new(@frame, 'highlightthickness'=>@h_l_thick,
                                 'borderwidth'=>@scrbar_border,
                                 'width'=>@scrbar_width)

    # create base flames
    @c_title = TkCanvas.new(@frame, 'highlightthickness'=>@h_l_thick,
                            'width'=>@window_width)
    @f_title = TkFrame.new(@c_title, 'width'=>@width_total)
    @w_title = TkcWindow.new(@c_title, 0, 0,
                             'window'=>@f_title, 'anchor'=>'nw')

    @c_lbox  = TkCanvas.new(@frame, 'highlightthickness'=>@h_l_thick,
                            'width'=>@window_width)
    @f_lbox  = TkFrame.new(@c_lbox, 'width'=>@width_total)
    @w_lbox  = TkcWindow.new(@c_lbox, 0, 0, 'window'=>@f_lbox, 'anchor'=>'nw')

    @c_hscr  = TkCanvas.new(@frame, 'highlightthickness'=>@h_l_thick,
                            'width'=>@window_width)
    @f_hscr  = TkFrame.new(@c_hscr, 'width'=>@width_total)
    @w_hscr  = TkcWindow.new(@c_hscr, 0, 0, 'window'=>@f_hscr, 'anchor'=>'nw')

    # create each listbox
    sum = 0.0
    @rel_list << sum/@width_total
    title_info.each_with_index{|(label, width), idx|
      # set relation between label and index
      if @name_index.include?(label)
        @name_index[label] << idx
      else
        @name_index[label] = [idx]
      end

      # calculate relative positioning
      sum += width
      @rel_list << sum/@width_total

      # title field
      f = TkFrame.new(@f_title, 'width'=>width)
      base = [f]

      title = TkLabel.new(f, 'text'=>label, 'borderwidth'=>@title_border,
                          'relief'=>'raised', 'highlightthickness'=>@h_l_thick)
      title_binding(title, idx)
      title.pack('fill'=>'x')

      @title_list << title

      f.place('relx'=>@rel_list[idx], 'y'=>0, 'anchor'=>'nw', 'width'=>1,
              'relheight'=>1.0,
              'relwidth'=>@rel_list[idx+1] - @rel_list[idx])

      # listbox field
      f = TkFrame.new(@f_lbox, 'width'=>width)
      base << f
      @lbox_list << TkListbox.new(f, 'highlightthickness'=>@h_l_thick,
                                  'borderwidth'=>@lbox_border
                                  ).pack('fill'=>'both', 'expand'=>true)
      f.place('relx'=>@rel_list[idx], 'y'=>0, 'anchor'=>'nw', 'width'=>1,
              'relwidth'=>@rel_list[idx+1] - @rel_list[idx], 'relheight'=>1.0)

      # scrollbar field
      f = TkFrame.new(@f_hscr, 'width'=>width)
      base << f
=begin
      @hscr_list << TkScrollbar.new(f, 'orient'=>'horizontal',
                                    'width'=>@scrbar_width,
                                    'borderwidth'=>@scrbar_border,
                                    'highlightthickness'=>@h_l_thick
                                    ).pack('fill'=>'x', 'anchor'=>'w')
=end
      @hscr_list << TkXScrollbar.new(f, 'width'=>@scrbar_width,
                                     'borderwidth'=>@scrbar_border,
                                     'highlightthickness'=>@h_l_thick
                                    ).pack('fill'=>'x', 'anchor'=>'w')
      f.place('relx'=>@rel_list[idx], 'y'=>0, 'anchor'=>'nw', 'width'=>1,
              'relwidth'=>@rel_list[idx+1] - @rel_list[idx])

=begin
      @lbox_list[idx].xscrollcommand proc{|first, last|
        @hscr_list[idx].set first, last
      }
      @hscr_list[idx].command proc{|*args| @lbox_list[idx].xview *args}
=end
      @lbox_list[idx].xscrollbar(@hscr_list[idx])

      # add new base
      @base_list << base
    }

    # pad
    # @f_title_pad = TkFrame.new(@frame)
    @f_title_pad = TkFrame.new(@frame, 'relief'=>'raised',
                               'borderwidth'=>@title_border,
                               'highlightthickness'=>@h_l_thick)

    @f_scr_pad = TkFrame.new(@frame, 'relief'=>'sunken',
                             'borderwidth'=>1,
                             'highlightthickness'=>@h_l_thick)

    # height check
    title_height = 0
    @title_list.each{|w|
      h = w.winfo_reqheight
      title_height = h if title_height < h
    }

    hscr_height = 0
    @hscr_list.each{|w|
      h = w.winfo_reqheight
      hscr_height = h if hscr_height < h
    }

    @f_title.height title_height
    @f_lbox.height lbox_height
    @f_hscr.height hscr_height

    # set control procedure for virtical scroll
=begin
    @lbox_list.each{|lbox|
      lbox.yscrollcommand proc{|first, last|
        @v_scroll.set first, last
      }
    }
    @v_scroll.command proc{|*args| @lbox_list.each{|lbox| lbox.yview *args} }
=end
    @v_scroll.assign(*@lbox_list)

    # set control procedure for horizoncal scroll
=begin
    @c_title.xscrollcommand proc{|first, last|
      @h_scroll.set first, last
    }
    @c_lbox.xscrollcommand proc{|first, last|
      @h_scroll.set first, last
    }
    @c_hscr.xscrollcommand proc{|first, last|
      @h_scroll.set first, last
    }
    @h_scroll.command proc{|*args|
      @c_title.xview *args
      @c_lbox.xview *args
      @c_hscr.xview *args if @show_each_hscr
    }
=end
    @h_scroll.assign(@c_title, @c_lbox, @c_hscr)

    # binding for listboxes
    @lbox_mode = {}
    @lbox_mode['browse']   = browse_mode_bindtag
    @lbox_mode['single']   = single_mode_bindtag
    @lbox_mode['extended'] = extended_mode_bindtag
    @lbox_mode['multiple'] = multiple_mode_bindtag
    @current_mode = 'browse'
    @lbox_list.each_with_index{|l, idx|
      l.bind('Shift-Key-Left',
             proc{|w| focus_shift(w, -1); Tk.callback_break}, '%W')
      l.bind('Shift-Key-Right',
             proc{|w| focus_shift(w, 1); Tk.callback_break}, '%W')

      l.bind('Button-2', proc{|x, y|
               @lbox_mark_x = x
               @lbox_list.each{|lbox| lbox.scan_mark(x, y)}
             }, '%x %y')
      l.bind('B2-Motion', proc{|x, y|
               @lbox_list.each{|lbox| lbox.scan_dragto(@lbox_mark_x, y)}
               l.scan_dragto(x, y)
             }, '%x %y')

      l.bindtags(l.bindtags.unshift(@lbox_mode[@current_mode]))
    }

    bbox = @w_title.bbox
    @c_title.height(bbox[3])
    @c_title.scrollregion(bbox)

    bbox = @w_lbox.bbox
    @c_lbox.height(bbox[3])
    @c_lbox.scrollregion(bbox)

    if @show_each_hscr
      bbox = @w_hscr.bbox
      @c_hscr.height(bbox[3])
      @c_hscr.scrollregion(bbox)
    end

    # alignment
    TkGrid.rowconfigure(@frame, 0, 'weight'=>0)
    TkGrid.rowconfigure(@frame, 1, 'weight'=>1)
    TkGrid.rowconfigure(@frame, 2, 'weight'=>0)
    TkGrid.rowconfigure(@frame, 3, 'weight'=>0)
    TkGrid.columnconfigure(@frame, 0, 'weight'=>1)
    TkGrid.columnconfigure(@frame, 1, 'weight'=>0)
    TkGrid.columnconfigure(@frame, 2, 'weight'=>0)
    @v_scroll.grid('row'=>1, 'column'=>2, 'sticky'=>'ns')
    @c_title.grid('row'=>0, 'column'=>0, 'sticky'=>'news')
    @f_title_pad.grid('row'=>0, 'column'=>2, 'sticky'=>'news')
    @c_lbox.grid('row'=>1, 'column'=>0, 'sticky'=>'news')
    @c_hscr.grid('row'=>2, 'column'=>0, 'sticky'=>'ew') if @show_each_hscr
    @h_scroll.grid('row'=>3, 'column'=>0, 'sticky'=>'ew') if @show_win_hscr
    @f_scr_pad.grid('row'=>2, 'rowspan'=>2, 'column'=>2, 'sticky'=>'news')

    # binding for 'Configure' event
    @c_lbox.bind('Configure',
                 proc{|height, width| reconstruct(height, width)},
                 '%h %w')

    # set default receiver of method calls
    @path = @lbox_list[0].path

    # configure options
    keys = {} unless keys
    keys = _symbolkey2str(keys)

    # 'mode' option of listboxes
    sel_mode = keys.delete('mode')
    mode(sel_mode) if sel_mode

    # 'scrollbarwidth' option == 'width' option of scrollbars
    width = keys.delete('scrollbarwidth')
    scrollbarwidth(width) if width

    # options for listbox titles
    title_font = keys.delete('titlefont')
    titlefont(title_font) if title_font

    title_fg = keys.delete('titleforeground')
    titleforeground(title_fg) if title_fg

    title_bg = keys.delete('titlebackground')
    titlebackground(title_bg) if title_bg

    # set receivers for configure methods
    delegate('DEFAULT', *@lbox_list)
    delegate('activebackground', @v_scroll, @h_scroll, *@hscr_list)
    delegate('troughcolor', @v_scroll, @h_scroll, *@hscr_list)
    delegate('repeatdelay', @v_scroll, @h_scroll, *@hscr_list)
    delegate('repeatinterval', @v_scroll, @h_scroll, *@hscr_list)
    delegate('borderwidth', @frame)
    delegate('width', @c_lbox, @c_title, @c_hscr)
    delegate('relief', @frame)

    # configure
    configure(keys) if keys.size > 0
  end
  private :initialize_composite

  # set 'mode' option of listboxes
  def mode(sel_mode)
    @lbox_list.each{|l|
      tags = l.bindtags
      tags = tags - [ @lbox_mode[@current_mode] ]
      l.bindtags(tags.unshift(@lbox_mode[sel_mode]))
      @current_mode = sel_mode
    }
  end

  # keep_minsize?
  def keep_minsize?
    @keep_minsize
  end
  def keep_minsize(bool)
    @keep_minsize = bool
  end

  # each hscr
  def show_each_hscr
    @show_each_hscr = true
    @c_hscr.grid('row'=>2, 'column'=>0, 'sticky'=>'ew')
  end
  def hide_each_hscr
    @show_each_hscr = false
    @c_hscr.ungrid
  end

  # window hscroll
  def show_win_hscr
    @show_win_hscr = true
    @h_scroll.grid('row'=>3, 'column'=>0, 'sticky'=>'ew')
  end
  def hide_win_hscr
    @show_each_hscr = false
    @h_scroll.ungrid
  end

  # set scrollbar width
  def scrollbarwidth(width)
    @scrbar_width = width
    @v_scroll['width'] = @scrbar_width
    @h_scroll['width'] = @scrbar_width
    @hscr_list.each{|hscr| hscr['width'] = @scrbar_width}
    self
  end

  # set scrollbar border
  def scrollbarborder(width)
    @scrbar_border = width
    @v_scroll['border'] = @scrbar_border
    @h_scroll['border'] = @scrbar_border
    @hscr_list.each{|hscr| hscr['border'] = @scrbar_border}
    self
  end

  # set listbox borders
  def listboxborder(width)
    @lbox_border = width
    @lbox_list.each{|w| w['border'] = @lbox_border}
    self
  end

  # set listbox relief
  def listboxrelief(relief)
    @lbox_list.each{|w| w['relief'] = relief}
    self
  end

  # set title borders
  def titleborder(width)
    @title_border = width
    @f_title_pad['border'] = @title_border
    @title_list.each{|label| label['border'] = @title_border}
    self
  end

  # set title font
  def titlefont(font)
    @title_list.each{|label| label['font'] = font}
    title_height = 0
    @title_list.each{|w|
      h = w.winfo_reqheight
      title_height = h if title_height < h
    }
    @f_title.height title_height
    bbox = @w_title.bbox
    @c_title.height(bbox[3])
    @c_title.scrollregion(bbox)
    self
  end

  # set title foreground color
  def titleforeground(fg)
    @title_list.each{|label| label['foreground'] = fg}
    self
  end

  # set title background color
  def titlebackground(bg)
    @f_title_pad['background'] = bg
    @title_list.each{|label| label['background'] = bg}
    self
  end

  # set title cmds
  def titlecommand(idx, cmd=Proc.new)
    @title_cmd[idx] = cmd
  end

  # call title cmds
  def titleinvoke(idx)
    @title_cmd[idx].call if @title_cmd[idx]
  end

  # get label widgets of listbox titles
  def titlelabels(*indices)
    @title_list[*indices]
  end

  # get listbox widgets
  def columns(*indices)
    @lbox_list[*indices]
  end

  def activate(idx)
    @lbox_list.each{|lbox| lbox.activate(idx)}
  end

  def bbox(idx)
    @lbox_list.collect{|lbox| lbox.bbox(idx)}
  end

  def delete(*idx)
    @lbox_list.collect{|lbox| lbox.delete(*idx)}
  end

  def get(*idx)
    if idx.size == 1
      @lbox_list.collect{|lbox| lbox.get(*idx)}
    else
      list = @lbox_list.collect{|lbox| lbox.get(*idx)}
      result = []
      list[0].each_with_index{|line, index|
        result << list.collect{|lines| lines[index]}
      }
      result
    end
  end

  def _line_array_to_hash(line)
    result = {}
    @name_index.each_pair{|label, indices|
      if indices.size == 1
        result[label] = line[indices[0]]
      else
        result[label] = indices.collect{|index| line[index]}
      end
    }
    result
  end
  private :_line_array_to_hash

  def get_by_hash(*idx)
    get_result = get(*idx)
    if idx.size == 1
      _line_array_to_hash(get_result)
    else
      get_result.collect{|line| _line_array_to_hash(line)}
    end
  end

  def insert(idx, *lines)
    lbox_ins = []
    (0..@lbox_list.size).each{lbox_ins << []}

    lines.each{|line|
      if line.kind_of? Hash
        array = []
        @name_index.each_pair{|label, indices|
          if indices.size == 1
            array[indices[0]] = line[label]
          else
            if line[label].kind_of? Array
              indices.each_with_index{|index, num|
                array[index] = line[label][num]
              }
            else
              array[indices[0]] = line[label]
            end
          end
        }
        line = array
      end

      @name_index.each_pair{|label, indices|
        if indices.size == 1
          lbox_ins[indices[0]] << line[indices[0]]
        else
          indices.each{|index| lbox_ins[index] << line[index]}
        end
      }
    }

    @lbox_list.each_with_index{|lbox, index|
      lbox.insert(idx, *lbox_ins[index]) if lbox_ins[index]
    }
  end

  def selection_anchor(index)
    @lbox_list.each{|lbox| lbox.selection_anchor(index)}
  end

  def selection_clear(first, last=None)
    @lbox_list.each{|lbox| lbox.selection_clear(first, last=None)}
  end

  def selection_set(first, last=None)
    @lbox_list.each{|lbox| lbox.selection_set(first, last=None)}
  end

  ###########################################
  private

  def reconstruct(height, width)
    if @keep_minsize && width <= @width_total
      @f_title.width(@width_total)
      @f_lbox.width(@width_total)
      @f_hscr.width(@width_total) if @show_each_hscr
      @window_width = @width_total
    else
      @f_title.width(width)
      @f_lbox.width(width)
      @f_hscr.width(width) if @show_each_hscr
      @window_width = width
    end

    @f_lbox.height(height)

    @c_title.scrollregion(@w_title.bbox)
    @c_lbox.scrollregion(@w_lbox.bbox)
    @c_hscr.scrollregion(@w_hscr.bbox) if @show_each_hscr

    (0..(@rel_list.size - 2)).each{|idx|
      title, lbox, hscr = @base_list[idx]
      title.place('relwidth'=>@rel_list[idx+1] - @rel_list[idx])
      lbox.place('relwidth'=>@rel_list[idx+1] - @rel_list[idx],
                 'relheight'=>1.0)
      hscr.place('relwidth'=>@rel_list[idx+1] - @rel_list[idx])
    }
  end

  def resize(x)
    idx = @sel_sash
    return if idx == 0

    # adjustment of relative positioning
    delta = (x - @x) / @frame_width
    if delta < @rel_list[idx-1] - @rel_list[idx] + (2*@sash/@frame_width)
      delta = @rel_list[idx-1] - @rel_list[idx] + (2*@sash/@frame_width)
    elsif delta > @rel_list[idx+1] - @rel_list[idx] - (2*@sash/@frame_width)
      delta = @rel_list[idx+1] - @rel_list[idx] - (2*@sash/@frame_width)
    end
    @rel_list[idx] += delta

    # adjustment of leftside widget of the sash
    title, lbox, hscr = @base_list[idx - 1]
    title.place('relwidth'=>@rel_list[idx] - @rel_list[idx-1])
    lbox.place('relwidth'=>@rel_list[idx] - @rel_list[idx-1], 'relheight'=>1.0)
    hscr.place('relwidth'=>@rel_list[idx] - @rel_list[idx-1])

    # adjustment of rightside widget of the sash
    title, lbox, hscr = @base_list[idx]
    title.place('relwidth'=>@rel_list[idx+1] - @rel_list[idx],
                'relx'=>@rel_list[idx])
    lbox.place('relwidth'=>@rel_list[idx+1] - @rel_list[idx],
               'relx'=>@rel_list[idx], 'relheight'=>1.0)
    hscr.place('relwidth'=>@rel_list[idx+1] - @rel_list[idx],
               'relx'=>@rel_list[idx])

    # update reference position
    @x = x
  end

  def motion_cb(w, x, idx)
    if x <= @sash && idx > 0
      w.cursor 'sb_h_double_arrow'
      @mode = :sash
      @sel_sash = idx
    elsif x >= w.winfo_width - @sash && idx < @lbox_total - 1
      w.cursor 'sb_h_double_arrow'
      @mode = :sash
      @sel_sash = idx + 1
    else
      w.cursor ""
      @mode = :title
      @sel_sash = 0
    end
  end

  def title_binding(title, index)
    title.bind('Motion', proc{|w, x, idx| motion_cb(w, x, idx.to_i)},
               "%W %x #{index}")

    title.bind('Enter', proc{|w, x, idx| motion_cb(w, x, idx.to_i)},
               "%W %x #{index}")

    title.bind('Leave', proc{|w| w.cursor ""}, "%W")

    title.bind('Button-1',
               proc{|w, x|
                 if @mode == :sash
                   @x = x
                   @frame_width = TkWinfo.width(@f_title).to_f
                 else
                   title.relief 'sunken'
                 end
               },
               '%W %X')

    title.bind('ButtonRelease-1',
               proc{|w, x, idx|
                 i = idx.to_i
                 if @mode == :title && @title_cmd[i].kind_of?(Proc)
                   @title_cmd[i].call
                 end
                 title.relief 'raised'
                 motion_cb(w,x,i)
               },
               "%W %x #{index}")

    title.bind('B1-Motion', proc{|x| resize(x) if @mode == :sash}, "%X")
  end

  #################################
  def browse_mode_bindtag
    t = TkBindTag.new
    t.bind('Button-1',
           proc{|w, y| w.focus; select_line(w, w.nearest(y))}, '%W %y')
    t.bind('B1-Motion', proc{|w, y| select_line(w, w.nearest(y))}, '%W %y')

    t.bind('Shift-Button-1',
           proc{|w, y| active_line(w, w.nearest(y))}, '%W %y')

    t.bind('Key-Up', proc{|w| select_shift(w, -1)}, '%W')
    t.bind('Key-Down', proc{|w| select_shift(w, 1)}, '%W')

    t.bind('Control-Home', proc{|w| select_line(w, 0)}, '%W')
    t.bind('Control-End', proc{|w| select_line(w, 'end')}, '%W')

    t.bind('space', proc{|w| select_line(w, w.index('active').to_i)}, '%W')
    t.bind('Select', proc{|w| select_line(w, w.index('active').to_i)}, '%W')
    t.bind('Control-slash',
           proc{|w| select_line(w, w.index('active').to_i)}, '%W')

    t
  end

  ########################
  def single_mode_bindtag
    t = TkBindTag.new
    t.bind('Button-1',
           proc{|w, y| w.focus; select_only(w, w.nearest(y))}, '%W %y')
    t.bind('ButtonRelease-1',
           proc{|w, y| active_line(w, w.nearest(y))}, '%W %y')

    t.bind('Shift-Button-1',
           proc{|w, y| active_line(w, w.nearest(y))}, '%W %y')

    t.bind('Key-Up', proc{|w| select_shift(w, -1)}, '%W')
    t.bind('Key-Down', proc{|w| select_shift(w, 1)}, '%W')

    t.bind('Control-Home', proc{|w| select_line(w, 0)}, '%W')
    t.bind('Control-End', proc{|w| select_line(w, 'end')}, '%W')

    t.bind('space', proc{|w| select_line(w, w.index('active').to_i)}, '%W')
    t.bind('Select', proc{|w| select_line(w, w.index('active').to_i)}, '%W')
    t.bind('Control-slash',
           proc{|w| select_line(w, w.index('active').to_i)}, '%W')
    t.bind('Control-backslash',
           proc{@lbox_list.each{|l| l.selection_clear(0, 'end')}})

    t
  end

  ########################
  def extended_mode_bindtag
    t = TkBindTag.new
    t.bind('Button-1',
           proc{|w, y| w.focus; select_only(w, w.nearest(y))}, '%W %y')
    t.bind('B1-Motion', proc{|w, y| select_range(w, w.nearest(y))}, '%W %y')

    t.bind('ButtonRelease-1',
           proc{|w, y| active_line(w, w.nearest(y))}, '%W %y')

    t.bind('Shift-Button-1',
           proc{|w, y| select_range(w, w.nearest(y))}, '%W %y')
    t.bind('Shift-B1-Motion',
           proc{|w, y| select_range(w, w.nearest(y))}, '%W %y')

    t.bind('Control-Button-1',
           proc{|w, y| select_toggle(w, w.nearest(y))}, '%W %y')

    t.bind('Control-B1-Motion',
           proc{|w, y| select_drag(w, w.nearest(y))}, '%W %y')

    t.bind('Key-Up', proc{|w| active_shift(w, -1)}, '%W')
    t.bind('Key-Down', proc{|w| active_shift(w, 1)}, '%W')

    t.bind('Shift-Up', proc{|w| select_expand(w, -1)}, '%W')
    t.bind('Shift-Down', proc{|w| select_expand(w, 1)}, '%W')

    t.bind('Control-Home', proc{|w| select_line2(w, 0)}, '%W')
    t.bind('Control-End', proc{|w| select_line2(w, 'end')}, '%W')

    t.bind('Control-Shift-Home', proc{|w| select_range(w, 0)}, '%W')
    t.bind('Control-Shift-End', proc{|w| select_range(w, 'end')}, '%W')

    t.bind('space', proc{|w| select_active(w)}, '%W')
    t.bind('Select', proc{|w| select_active(w)}, '%W')
    t.bind('Control-slash', proc{|w| select_all}, '%W')
    t.bind('Control-backslash', proc{|w| clear_all}, '%W')

    t
  end

  ########################
  def multiple_mode_bindtag
    t = TkBindTag.new
    t.bind('Button-1',
           proc{|w, y| w.focus; select_line3(w, w.nearest(y))}, '%W %y')
    t.bind('ButtonRelease-1',
           proc{|w, y| active_line(w, w.nearest(y))}, '%W %y')

    t.bind('Key-Up', proc{|w| active_shift(w, -1)}, '%W')
    t.bind('Key-Down', proc{|w| active_shift(w, 1)}, '%W')

    t.bind('Control-Home', proc{|w| select_line2(w, 0)}, '%W')
    t.bind('Control-End', proc{|w| select_line2(w, 'end')}, '%W')

    t.bind('Control-Shift-Home', proc{|w| active_line(w, 0)}, '%W')
    t.bind('Control-Shift-End', proc{|w| active_line(w, 'end')}, '%W')

    t.bind('space', proc{|w| select_active(w)}, '%W')
    t.bind('Select', proc{|w| select_active(w)}, '%W')
    t.bind('Control-slash', proc{|w| select_all}, '%W')
    t.bind('Control-backslash', proc{|w| clear_all}, '%W')

    t
  end

  ########################
  def active_line(w, idx)
    @lbox_list.each{|l| l.activate(idx)}
  end

  def select_only(w, idx)
    @lbox_list.each{|l|
      l.selection_clear(0, 'end')
      l.selection_anchor(idx)
      l.selection_set('anchor')
    }
  end

  def select_range(w, idx)
    @lbox_list.each{|l|
      l.selection_clear(0, 'end')
      l.selection_set('anchor', idx)
    }
  end

  def select_toggle(w, idx)
    st = w.selection_includes(idx)
    @lbox_list.each{|l|
      l.selection_anchor(idx)
      if st == 1
        l.selection_clear(idx)
      else
        l.selection_set(idx)
      end
    }
  end

  def select_drag(w, idx)
    st = w.selection_includes('anchor')
    @lbox_list.each{|l|
      if st == 1
        l.selection_set('anchor', idx)
      else
        l.selection_clear('anchor', idx)
      end
    }
  end

  def select_line(w, idx)
    @lbox_list.each{|l|
      l.selection_clear(0, 'end')
      l.activate(idx)
      l.selection_anchor(idx)
      l.selection_set('anchor')
    }
    w.selection_set('anchor')
  end

  def select_line2(w, idx)
    @lbox_list.each{|l|
      l.activate(idx)
      l.selection_anchor(idx)
      l.selection_set('anchor')
    }
  end

  def select_line3(w, idx)
    @lbox_list.each{|l|
      l.selection_set(idx)
    }
  end

  def select_active(w)
    idx = l.activate(idx)
    @lbox_list.each{|l|
      l.selection_set(idx)
    }
  end

  def select_expand(w, dir)
    idx = w.index('active').to_i + dir
    if idx < 0
      idx = 0
    elsif idx >= w.size
      idx = w.size - 1
    end
    @lbox_list.each{|l|
      l.activate(idx)
      l.selection_set(idx)
    }
  end

  def active_shift(w, dir)
    idx = w.index('active').to_i + dir
    if idx < 0
      idx = 0
    elsif idx >= w.size
      idx = w.size - 1
    end
    @lbox_list.each{|l|
      l.activate(idx)
      l.selection_anchor(idx)
    }
  end

  def select_shift(w, dir)
    idx = w.index('anchor').to_i + dir
    if idx < 0
      idx = 0
    elsif idx >= w.size
      idx = w.size - 1
    end
    @lbox_list.each{|l|
      l.selection_clear(0, 'end')
      l.activate(idx)
      l.selection_anchor(idx)
      l.selection_set('anchor')
    }
  end

  def select_all
    @lbox_list.each{|l|
      l.selection_set(0, 'end')
    }
  end

  def clear_all
    @lbox_list.each{|l|
      l.selection_clear(0, 'end')
    }
  end

  def focus_shift(w, dir)
    idx = @lbox_list.index(w) + dir
    return if idx < 0
    return if idx >= @lbox_list.size
    @lbox_list[idx].focus
  end
  ########################
end

################################################
# test
################################################
if __FILE__ == $0
  l = TkMultiListFrame.new(nil, 200,
                           [ ['L1', 200, proc{p 'click L1'}],
                             ['L2', 100],
                             ['L3', 200] ],
                           'width'=>350,
                           #'titleforeground'=>'yellow',
                           'titleforeground'=>'white',
                           #'titlebackground'=>'navy',
                           'titlebackground'=>'blue',
                           'titlefont'=>'courier'
                           ).pack('fill'=>'both', 'expand'=>true)
  l.insert('end', [1,2,3])
  l.insert('end', [4,5,6])
  l.insert('end', [4,5,6], [4,5,6])
  l.insert('end', ['aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa',
                   'bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb',
                   'cccccccccccccccccccccccccccccccccccccccccccccccccccc'])
  l.insert('end', [1,2,3])
  l.insert('end', [4,5,6], [4,5,6])
  l.insert('end', ['aaaaaaaaaaaaaaa','bbbbbbbbbbbbbb','ccccccccccccccccc'])
  l.insert('end', [1,2,3])
  l.insert('end', [4,5,6], [4,5,6])
  l.insert('end', ['aaaaaaaaaaaaaaa','bbbbbbbbbbbbbb','ccccccccccccccccc'])
  l.insert('end', [1,2,3])
  l.insert('end', [4,5,6], [4,5,6])
  l.insert('end', ['aaaaaaaaaaaaaaa','bbbbbbbbbbbbbb','ccccccccccccccccc'])
  l.insert('end', [1,2,3])
  l.insert('end', [4,5,6], [4,5,6])
  p l.columns(1)
  p l.columns(1..3)
  p l.columns(1,2)
  Tk.mainloop
end
