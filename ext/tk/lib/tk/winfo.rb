#
# tk/winfo.rb : methods for winfo command
#
module TkWinfo
end

require 'tk'

module TkWinfo
  include Tk
  extend Tk

  TkCommandNames = ['winfo'.freeze].freeze

  def TkWinfo.atom(name, win=nil)
    if win
      number(tk_call_without_enc('winfo', 'atom', '-displayof', win,
                                 _get_eval_enc_str(name)))
    else
      number(tk_call_without_enc('winfo', 'atom', _get_eval_enc_str(name)))
    end
  end
  def winfo_atom(name)
    TkWinfo.atom(name, self)
  end

  def TkWinfo.atomname(id, win=nil)
    if win
      _fromUTF8(tk_call_without_enc('winfo', 'atomname',
                                    '-displayof', win, id))
    else
      _fromUTF8(tk_call_without_enc('winfo', 'atomname', id))
    end
  end
  def winfo_atomname(id)
    TkWinfo.atomname(id, self)
  end

  def TkWinfo.cells(win)
    number(tk_call_without_enc('winfo', 'cells', win))
  end
  def winfo_cells
    TkWinfo.cells self
  end

  def TkWinfo.children(win)
    list(tk_call_without_enc('winfo', 'children', win))
  end
  def winfo_children
    TkWinfo.children self
  end

  def TkWinfo.classname(win)
    tk_call_without_enc('winfo', 'class', win)
  end
  def winfo_classname
    TkWinfo.classname self
  end
  alias winfo_class winfo_classname

  def TkWinfo.colormapfull(win)
     bool(tk_call_without_enc('winfo', 'colormapfull', win))
  end
  def winfo_colormapfull
    TkWinfo.colormapfull self
  end

  def TkWinfo.containing(rootX, rootY, win=nil)
    if win
      window(tk_call_without_enc('winfo', 'containing',
                                 '-displayof', win, rootX, rootY))
    else
      window(tk_call_without_enc('winfo', 'containing', rootX, rootY))
    end
  end
  def winfo_containing(x, y)
    TkWinfo.containing(x, y, self)
  end

  def TkWinfo.depth(win)
    number(tk_call_without_enc('winfo', 'depth', win))
  end
  def winfo_depth
    TkWinfo.depth self
  end

  def TkWinfo.exist?(win)
    bool(tk_call_without_enc('winfo', 'exists', win))
  end
  def winfo_exist?
    TkWinfo.exist? self
  end

  def TkWinfo.fpixels(win, dist)
    number(tk_call_without_enc('winfo', 'fpixels', win, dist))
  end
  def winfo_fpixels(dist)
    TkWinfo.fpixels self, dist
  end

  def TkWinfo.geometry(win)
    tk_call_without_enc('winfo', 'geometry', win)
  end
  def winfo_geometry
    TkWinfo.geometry self
  end

  def TkWinfo.height(win)
    number(tk_call_without_enc('winfo', 'height', win))
  end
  def winfo_height
    TkWinfo.height self
  end

  def TkWinfo.id(win)
    tk_call_without_enc('winfo', 'id', win)
  end
  def winfo_id
    TkWinfo.id self
  end

  def TkWinfo.interps(win=nil)
    if win
      #tk_split_simplelist(tk_call_without_enc('winfo', 'interps',
      #                                        '-displayof', win))
      tk_split_simplelist(tk_call_without_enc('winfo', 'interps',
                                              '-displayof', win),
                          false, true)
    else
      #tk_split_simplelist(tk_call_without_enc('winfo', 'interps'))
      tk_split_simplelist(tk_call_without_enc('winfo', 'interps'),
                          false, true)
    end
  end
  def winfo_interps
    TkWinfo.interps self
  end

  def TkWinfo.mapped?(win)
    bool(tk_call_without_enc('winfo', 'ismapped', win))
  end
  def winfo_mapped?
    TkWinfo.mapped? self
  end

  def TkWinfo.manager(win)
    tk_call_without_enc('winfo', 'manager', win)
  end
  def winfo_manager
    TkWinfo.manager self
  end

  def TkWinfo.appname(win)
    tk_call('winfo', 'name', win)
  end
  def winfo_appname
    TkWinfo.appname self
  end

  def TkWinfo.parent(win)
    window(tk_call_without_enc('winfo', 'parent', win))
  end
  def winfo_parent
    TkWinfo.parent self
  end

  def TkWinfo.widget(id, win=nil)
    if win
      window(tk_call_without_enc('winfo', 'pathname', '-displayof', win, id))
    else
      window(tk_call_without_enc('winfo', 'pathname', id))
    end
  end
  def winfo_widget(id)
    TkWinfo.widget id, self
  end

  def TkWinfo.pixels(win, dist)
    number(tk_call_without_enc('winfo', 'pixels', win, dist))
  end
  def winfo_pixels(dist)
    TkWinfo.pixels self, dist
  end

  def TkWinfo.reqheight(win)
    number(tk_call_without_enc('winfo', 'reqheight', win))
  end
  def winfo_reqheight
    TkWinfo.reqheight self
  end

  def TkWinfo.reqwidth(win)
    number(tk_call_without_enc('winfo', 'reqwidth', win))
  end
  def winfo_reqwidth
    TkWinfo.reqwidth self
  end

  def TkWinfo.rgb(win, color)
    list(tk_call_without_enc('winfo', 'rgb', win, color))
  end
  def winfo_rgb(color)
    TkWinfo.rgb self, color
  end

  def TkWinfo.rootx(win)
    number(tk_call_without_enc('winfo', 'rootx', win))
  end
  def winfo_rootx
    TkWinfo.rootx self
  end

  def TkWinfo.rooty(win)
    number(tk_call_without_enc('winfo', 'rooty', win))
  end
  def winfo_rooty
    TkWinfo.rooty self
  end

  def TkWinfo.screen(win)
    tk_call('winfo', 'screen', win)
  end
  def winfo_screen
    TkWinfo.screen self
  end

  def TkWinfo.screencells(win)
    number(tk_call_without_enc('winfo', 'screencells', win))
  end
  def winfo_screencells
    TkWinfo.screencells self
  end

  def TkWinfo.screendepth(win)
    number(tk_call_without_enc('winfo', 'screendepth', win))
  end
  def winfo_screendepth
    TkWinfo.screendepth self
  end

  def TkWinfo.screenheight (win)
    number(tk_call_without_enc('winfo', 'screenheight', win))
  end
  def winfo_screenheight
    TkWinfo.screenheight self
  end

  def TkWinfo.screenmmheight(win)
    number(tk_call_without_enc('winfo', 'screenmmheight', win))
  end
  def winfo_screenmmheight
    TkWinfo.screenmmheight self
  end

  def TkWinfo.screenmmwidth(win)
    number(tk_call_without_enc('winfo', 'screenmmwidth', win))
  end
  def winfo_screenmmwidth
    TkWinfo.screenmmwidth self
  end

  def TkWinfo.screenvisual(win)
    tk_call_without_enc('winfo', 'screenvisual', win)
  end
  def winfo_screenvisual
    TkWinfo.screenvisual self
  end

  def TkWinfo.screenwidth(win)
    number(tk_call_without_enc('winfo', 'screenwidth', win))
  end
  def winfo_screenwidth
    TkWinfo.screenwidth self
  end

  def TkWinfo.server(win)
    tk_call('winfo', 'server', win)
  end
  def winfo_server
    TkWinfo.server self
  end

  def TkWinfo.toplevel(win)
    window(tk_call_without_enc('winfo', 'toplevel', win))
  end
  def winfo_toplevel
    TkWinfo.toplevel self
  end

  def TkWinfo.visual(win)
    tk_call_without_enc('winfo', 'visual', win)
  end
  def winfo_visual
    TkWinfo.visual self
  end

  def TkWinfo.visualid(win)
    tk_call_without_enc('winfo', 'visualid', win)
  end
  def winfo_visualid
    TkWinfo.visualid self
  end

  def TkWinfo.visualsavailable(win, includeids=false)
    if includeids
      list(tk_call_without_enc('winfo', 'visualsavailable',
                               win, "includeids"))
    else
      list(tk_call_without_enc('winfo', 'visualsavailable', win))
    end
  end
  def winfo_visualsavailable(includeids=false)
    TkWinfo.visualsavailable self, includeids
  end

  def TkWinfo.vrootheight(win)
    number(tk_call_without_enc('winfo', 'vrootheight', win))
  end
  def winfo_vrootheight
    TkWinfo.vrootheight self
  end

  def TkWinfo.vrootwidth(win)
    number(tk_call_without_enc('winfo', 'vrootwidth', win))
  end
  def winfo_vrootwidth
    TkWinfo.vrootwidth self
  end

  def TkWinfo.vrootx(win)
    number(tk_call_without_enc('winfo', 'vrootx', win))
  end
  def winfo_vrootx
    TkWinfo.vrootx self
  end

  def TkWinfo.vrooty(win)
    number(tk_call_without_enc('winfo', 'vrooty', win))
  end
  def winfo_vrooty
    TkWinfo.vrooty self
  end

  def TkWinfo.width(win)
    number(tk_call_without_enc('winfo', 'width', win))
  end
  def winfo_width
    TkWinfo.width self
  end

  def TkWinfo.x(win)
    number(tk_call_without_enc('winfo', 'x', win))
  end
  def winfo_x
    TkWinfo.x self
  end

  def TkWinfo.y(win)
    number(tk_call_without_enc('winfo', 'y', win))
  end
  def winfo_y
    TkWinfo.y self
  end

  def TkWinfo.viewable(win)
    bool(tk_call_without_enc('winfo', 'viewable', win))
  end
  def winfo_viewable
    TkWinfo.viewable self
  end

  def TkWinfo.pointerx(win)
    number(tk_call_without_enc('winfo', 'pointerx', win))
  end
  def winfo_pointerx
    TkWinfo.pointerx self
  end

  def TkWinfo.pointery(win)
    number(tk_call_without_enc('winfo', 'pointery', win))
  end
  def winfo_pointery
    TkWinfo.pointery self
  end

  def TkWinfo.pointerxy(win)
    list(tk_call_without_enc('winfo', 'pointerxy', win))
  end
  def winfo_pointerxy
    TkWinfo.pointerxy self
  end
end
