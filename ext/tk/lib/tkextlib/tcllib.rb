#
#  tcllib extension support
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'

# call setup script for general 'tkextlib' libraries
require 'tkextlib/setup.rb'

# call setup script
require 'tkextlib/tcllib/setup.rb'

err = ''

# package:: autoscroll
target = 'tkextlib/tcllib/autoscroll'
begin
  require target
rescue => e
  err << "\n  ['" << target << "'] "  << e.class.name << ' : ' << e.message
end

# package:: cursor
target = 'tkextlib/tcllib/cursor'
begin
  require target
rescue => e
  err << "\n  ['" << target << "'] "  << e.class.name << ' : ' << e.message
end

# package:: style
target = 'tkextlib/tcllib/style'
begin
  require target
rescue => e
  err << "\n  ['" << target << "'] "  << e.class.name << ' : ' << e.message
end

# autoload
module Tk
  module Tcllib
    TkComm::TkExtlibAutoloadModule.unshift(self)

    # package:: autoscroll
    autoload :Autoscroll,         'tkextlib/tcllib/autoscroll'

    # package:: ctext
    autoload :CText,              'tkextlib/tcllib/ctext'

    # package:: cursor
    autoload :Cursor,             'tkextlib/tcllib/cursor'

    # package:: datefield
    autoload :Datefield,          'tkextlib/tcllib/datefield'
    autoload :DateField,          'tkextlib/tcllib/datefield'

    # package:: getstring
    autoload :GetString_Dialog,   'tkextlib/tcllib/getstring'

    # package:: history
    autoload :History,            'tkextlib/tcllib/history'

    # package:: ico
    autoload :ICO,                'tkextlib/tcllib/ico'

    # package:: ipentry
    autoload :IP_Entry,           'tkextlib/tcllib/ip_entry'
    autoload :IPEntry,            'tkextlib/tcllib/ip_entry'

    # package:: khim
    autoload :KHIM,               'tkextlib/tcllib/khim'

    # package:: ntext
    autoload :Ntext,              'tkextlib/tcllib/ntext'

    # package:: Plotchart
    autoload :Plotchart,          'tkextlib/tcllib/plotchart'

    # package:: style
    autoload :Style,              'tkextlib/tcllib/style'

    # package:: swaplist
    autoload :Swaplist_Dialog,    'tkextlib/tcllib/swaplist'

    # package:: tablelist
    autoload :Tablelist,           'tkextlib/tcllib/tablelist'
    autoload :TableList,           'tkextlib/tcllib/tablelist'
    autoload :Tablelist_Tile,      'tkextlib/tcllib/tablelist_tile'
    autoload :TableList_Tile,      'tkextlib/tcllib/tablelist_tile'

    # package:: tkpiechart
    autoload :Tkpiechart,         'tkextlib/tcllib/tkpiechart'

    # package:: tooltip
    autoload :Tooltip,            'tkextlib/tcllib/tooltip'

    # package:: widget
    autoload :Widget,             'tkextlib/tcllib/widget'
  end
end

if $VERBOSE && !err.empty?
  warn("Warning: some sub-packages are failed to require : " + err)
end
