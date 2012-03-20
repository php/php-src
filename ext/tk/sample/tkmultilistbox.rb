#
# tkmultilistbox.rb : multiple listbox widget
#                       by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'

class TkMultiListbox < TkListbox
  include TkComposite

  #   lbox_height : height of listboxes (pixel)
  #   title_info  : array [ [<title_string>,<init_width>], ... ]
  #   keys        : hash {<option>=><value>, ... }
  def initialize_composite(lbox_height, title_info, keys={})
    # argument check
    if (! title_info.kind_of? Array) or (title_info.size < 2)
      raise
    end

    # decide total width
    @width_total = 0
    title_info.each{|title, width| @width_total += width.to_f}

    # virtical scrollbar
    @v_scroll = TkScrollbar.new(@frame, 'orient'=>'vertical')

    # init arrays
    @base_list = []
    @rel_list   = []
    @title_list = []
    @lbox_list  = []
    @hscr_list  = []

    # rel-table of label=>index
    @name_index = {}

    # create base flames
    @f_title = TkFrame.new(@frame, 'width'=>@width_total)
    @f_lbox  = TkFrame.new(@frame,
                           'width'=>@width_total, 'height'=>lbox_height)
    @f_hscr  = TkFrame.new(@frame, 'width'=>@width_total,
                           'height'=>@v_scroll.cget('width') +
                                     2 * @v_scroll.cget('borderwidth'))

    # dummy label to keep the hight of title space
    TkLabel.new(@f_title, 'text'=>' ').pack

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
      @title_list << TkLabel.new(f, 'text'=>label).pack('fill'=>'x')
      f.place('relx'=>@rel_list[idx], 'y'=>0, 'anchor'=>'nw', 'width'=>-6,
              'relwidth'=>@rel_list[idx+1] - @rel_list[idx])

      # listbox field
      f = TkFrame.new(@f_lbox, 'width'=>width)
      base << f
      @lbox_list << TkListbox.new(f).pack('fill'=>'both', 'expand'=>true)
      f.place('relx'=>@rel_list[idx], 'y'=>0, 'anchor'=>'nw', 'width'=>-4,
              'relwidth'=>@rel_list[idx+1] - @rel_list[idx], 'relheight'=>1.0)

      # scrollbar field
      f = TkFrame.new(@f_hscr, 'width'=>width)
      base << f
      @hscr_list << TkScrollbar.new(f, 'orient'=>'horizontal') .
                                              pack('fill'=>'x', 'anchor'=>'w')
      f.place('relx'=>@rel_list[idx], 'y'=>0, 'anchor'=>'nw', 'width'=>-4,
              'relwidth'=>@rel_list[idx+1] - @rel_list[idx])

      @lbox_list[idx].xscrollcommand proc{|first, last|
        @hscr_list[idx].set first, last
      }
      @hscr_list[idx].command proc{|*args| @lbox_list[idx].xview *args}

      # add new base
      @base_list << base
    }

    # create tab
    @tab_list = [nil]
    (1..(@rel_list.size - 2)).each{|idx|
      tab = TkFrame.new(@f_title, 'cursor'=>'sb_h_double_arrow',
                        'width'=>6, 'borderwidth'=>2, 'relief'=>'raised')
      @tab_list << tab
      tab.place('relx'=>@rel_list[idx], 'anchor'=>'ne', 'relheight'=>0.95)
      tab.bind('Button-1',
               proc{|x| @x = x; @frame_width = TkWinfo.width(@f_title).to_f},
               '%X')
      tab.bind('B1-Motion', proc{|x, idx| resize(x, idx.to_i)}, "%X #{idx}")
    }

    # set control procedure for virtical scroll
    @lbox_list.each{|lbox|
      lbox.yscrollcommand proc{|first, last|
        @v_scroll.set first, last
      }
    }
    @v_scroll.command proc{|*args| @lbox_list.each{|lbox| lbox.yview *args} }

    # binding for listboxes
    @mode = {}
    @mode['browse']   = browse_mode_bindtag
    @mode['single']   = single_mode_bindtag
    @mode['extended'] = extended_mode_bindtag
    @mode['multiple'] = multiple_mode_bindtag
    @current_mode = 'browse'
    @lbox_list.each{|l|
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

      l.bindtags(l.bindtags.unshift(@mode[@current_mode]))
    }

    # alignment
    TkGrid.rowconfigure(@frame, 0, 'weight'=>0)
    TkGrid.rowconfigure(@frame, 1, 'weight'=>1)
    TkGrid.rowconfigure(@frame, 2, 'weight'=>0)
    TkGrid.columnconfigure(@frame, 0, 'weight'=>1)
    TkGrid.columnconfigure(@frame, 1, 'weight'=>0)
    @v_scroll.grid('row'=>1, 'column'=>1, 'sticky'=>'ns')
    @f_title.grid('row'=>0, 'column'=>0, 'sticky'=>'news')
    @f_lbox.grid('row'=>1, 'column'=>0, 'sticky'=>'news')
    @f_hscr.grid('row'=>2, 'column'=>0, 'sticky'=>'ew')

    # binding for 'Configure' event
    @frame.bind('Configure', proc{reconstruct})

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
    title_fg = keys.delete('titleforeground')
    title_bg = keys.delete('titlebackground')
    if title_font or title_fg or title_bg
      titleconfig(title_font, title_fg, title_bg)
    end

    # set receivers for configure methods
    delegate('DEFAULT', *@lbox_list)
    delegate('activebackground', @v_scroll, *@hscr_list)
    delegate('troughcolor', @v_scroll, *@hscr_list)
    delegate('repeatdelay', @v_scroll, *@hscr_list)
    delegate('repeatinterval', @v_scroll, *@hscr_list)
    delegate('borderwidth', @frame)
    delegate('relief', @frame)

    # configure
    configure(keys) if keys.size > 0
  end
  private :initialize_composite

  # set 'mode' option of listboxes
  def mode(sel_mode)
    @lbox_list.each{|l|
      tags = l.bindtags
      tags = tags - [ @mode[@current_mode] ]
      l.bindtags(tags.unshift(@mode[sel_mode]))
      @current_mode = sel_mode
    }
  end

  # set scrollbar width
  def scrollbarwidth(width)
    @v_scroll['width'] = width
    @hscr_list.each{|hscr| hscr['width'] = width}
    @f_hscr['height'] = width + 2 * @v_scroll.cget('borderwidth')
  end

  # set options of titles
  def titleconfig(font, fg, bg)
    keys = {}
    keys['font'] = font if font
    keys['foreground'] = fg if fg
    keys['background'] = bg if bg
    @title_list.each{|label| label.configure(keys)}
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

  def reconstruct
    (0..(@rel_list.size - 2)).each{|idx|
      title, lbox, hscr = @base_list[idx]
      title.place('relwidth'=>@rel_list[idx+1] - @rel_list[idx])
      lbox.place('relwidth'=>@rel_list[idx+1] - @rel_list[idx],
                 'relheight'=>1.0)
      hscr.place('relwidth'=>@rel_list[idx+1] - @rel_list[idx])

      tab = @tab_list[idx]
      tab.place('relx'=>@rel_list[idx]) if tab
    }
  end

  def resize(x, idx)
    # adjustment of relative positioning
    delta = (x - @x) / @frame_width
    if delta < @rel_list[idx-1] - @rel_list[idx] + 0.02
      delta = @rel_list[idx-1] - @rel_list[idx] + 0.02
    elsif delta > @rel_list[idx+1] - @rel_list[idx] - 0.02
      delta = @rel_list[idx+1] - @rel_list[idx] - 0.02
    end
    @rel_list[idx] += delta

    # adjustment of leftside widget of the tab
    title, lbox, hscr = @base_list[idx - 1]
    title.place('relwidth'=>@rel_list[idx] - @rel_list[idx-1])
    lbox.place('relwidth'=>@rel_list[idx] - @rel_list[idx-1], 'relheight'=>1.0)
    hscr.place('relwidth'=>@rel_list[idx] - @rel_list[idx-1])

    # adjustment of the tab position
    @tab_list[idx].place('relx'=>@rel_list[idx])

    # adjustment of rightside widget of the tab
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
  f = TkFrame.new(nil, 'width'=>300,
                  'height'=>200).pack('fill'=>'both', 'expand'=>'true')
  #f = TkFrame.new.pack('fill'=>'both', 'expand'=>'true')
  l = TkMultiListbox.new(f, 150,
                         [ ['L1', 100],
                           ['L2', 200],
                           ['L3', 50] ],
                         'titlefont'=>'courier',
                         'titleforeground'=>'yellow',
                         'titlebackground'=>'navy'
                         ).pack('fill'=>'both', 'expand'=>true)
  l.insert('end', [1,2,3])
  l.insert('end', [4,5,6])
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
