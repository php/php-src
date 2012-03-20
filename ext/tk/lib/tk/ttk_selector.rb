#
#  ttk_selector
#
######################################
#  toplevel classes/modules
module Tk
  @TOPLEVEL_ALIAS_TABLE[:Ttk] = {
    :TkButton       => 'tkextlib/tile/tbutton',

    :TkCheckbutton  => 'tkextlib/tile/tcheckbutton',
    :TkCheckButton  => 'tkextlib/tile/tcheckbutton',

    # :TkDialog       => 'tkextlib/tile/dialog',

    :TkEntry        => 'tkextlib/tile/tentry',

    :TkCombobox     => 'tkextlib/tile/tcombobox',

    :TkFrame        => 'tkextlib/tile/tframe',

    :TkLabel        => 'tkextlib/tile/tlabel',

    :TkLabelframe   => 'tkextlib/tile/tlabelframe',
    :TkLabelFrame   => 'tkextlib/tile/tlabelframe',

    :TkMenubutton   => 'tkextlib/tile/tmenubutton',
    :TkMenuButton   => 'tkextlib/tile/tmenubutton',

    :TkNotebook     => 'tkextlib/tile/tnotebook',

    # :TkPaned        => 'tkextlib/tile/tpaned',
    :TkPanedwindow  => 'tkextlib/tile/tpaned',
    :TkPanedWindow  => 'tkextlib/tile/tpaned',

    :TkProgressbar  => 'tkextlib/tile/tprogressbar',

    :TkRadiobutton  => 'tkextlib/tile/tradiobutton',
    :TkRadioButton  => 'tkextlib/tile/tradiobutton',

    :TkScale        => 'tkextlib/tile/tscale',
    # :TkProgress     => 'tkextlib/tile/tscale',

    :TkScrollbar    => 'tkextlib/tile/tscrollbar',
    :TkXScrollbar   => 'tkextlib/tile/tscrollbar',
    :TkYScrollbar   => 'tkextlib/tile/tscrollbar',

    :TkSeparator    => 'tkextlib/tile/tseparator',

    :TkSizeGrip     => 'tkextlib/tile/sizegrip',
    :TkSizegrip     => 'tkextlib/tile/sizegrip',

    # :TkSquare       => 'tkextlib/tile/tsquare',

    :TkTreeview     => 'tkextlib/tile/treeview',
  }

  # @TOPLEVEL_ALIAS_TABLE[:Tile] = @TOPLEVEL_ALIAS_TABLE[:Ttk]
  Tk.__create_widget_set__(:Tile, :Ttk)

  ############################################
  #  depend on the version of Tcl/Tk
  major, minor, type, patchlevel = TclTkLib.get_version

  #  ttk::spinbox is supported on Tcl/Tk8.6b1 or later
  if ([major,minor,type,patchlevel] <=>
        [8,6,TclTkLib::RELEASE_TYPE::BETA,1]) >= 0
    @TOPLEVEL_ALIAS_TABLE[:Ttk].update(
      :TkSpinbox => 'tkextlib/tile/tspinbox'
    )
  end

  ################################################
  # register some Ttk widgets as default
  # (Ttk is a standard library on Tcl/Tk8.5+)
  @TOPLEVEL_ALIAS_TABLE[:Ttk].each{|sym, file|
    #unless Tk::TOPLEVEL_ALIASES.autoload?(sym) || Tk::TOPLEVEL_ALIASES.const_defined?(sym)
    #   @TOPLEVEL_ALIAS_OWNER[sym] = :Ttk
    #   Tk::TOPLEVEL_ALIASES.autoload(sym, file)
    #end
    Tk.__regist_toplevel_aliases__(:Ttk, file, sym)
  }

  ################################################

  # @TOPLEVEL_ALIAS_SETUP_PROC[:Tile] =
  #   @TOPLEVEL_ALIAS_SETUP_PROC[:Ttk] = proc{|mod|
  #   unless Tk.autoload?(:Tile) || Tk.const_defined?(:Tile)
  #     Object.autoload :Ttk, 'tkextlib/tile'
  #     Tk.autoload :Tile, 'tkextlib/tile'
  #   end
  # }
  Tk.__toplevel_alias_setup_proc__(:Ttk, :Tile){|mod|
    unless Tk.autoload?(:Tile) || Tk.const_defined?(:Tile)
      Object.autoload :Ttk, 'tkextlib/tile'
      Tk.autoload :Tile, 'tkextlib/tile'
    end
  }
end
