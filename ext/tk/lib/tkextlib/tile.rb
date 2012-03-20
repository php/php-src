#
#  Tile theme engin (tile widget set) support
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#

require 'tk'
require 'tk/ttk_selector'

# call setup script for general 'tkextlib' libraries
require 'tkextlib/setup.rb'

# library directory
require 'tkextlib/tile/setup.rb'

# load package
# TkPackage.require('tile', '0.4')
# TkPackage.require('tile', '0.6')
# TkPackage.require('tile', '0.7')
if Tk::TK_MAJOR_VERSION > 8 ||
    (Tk::TK_MAJOR_VERSION == 8 && Tk::TK_MINOR_VERSION >= 5)
  begin
    TkPackage.require('tile') # for compatibility (version check of 'tile')
  rescue RuntimeError
    # ignore, even if cannot find package 'tile'
  end
  pkgname = 'Ttk'
else
  pkgname = 'tile'
end

begin
  verstr = TkPackage.require(pkgname)
rescue RuntimeError
  # define dummy methods
  module Tk
    module Tile
      CANNOT_FIND_PACKAGE = true
      def self.const_missing(sym)
        TkPackage.require(PACKAGE_NAME)
      end
      def self.method_missing(*args)
        TkPackage.require(PACKAGE_NAME)
      end
    end
  end
  Tk.__cannot_find_tk_package_for_widget_set__(:Ttk, pkgname)
  if pkgname == 'Ttk'
    verstr = Tk::TK_PATCHLEVEL  # dummy
  else
    verstr = '0.7'  # dummy
  end
end

ver = verstr.split('.')
if ver[0].to_i == 0
  # Tile extension package
  if ver[1].to_i <= 4
    # version 0.4 or former
    module Tk
      module Tile
        USE_TILE_NAMESPACE = true
        USE_TTK_NAMESPACE  = false
        TILE_SPEC_VERSION_ID = 0
      end
    end
  elsif ver[1].to_i <= 6
    # version 0.5 -- version 0.6
    module Tk
      module Tile
        USE_TILE_NAMESPACE = true
        USE_TTK_NAMESPACE  = true
        TILE_SPEC_VERSION_ID = 5
      end
    end
  elsif ver[1].to_i <= 7
    module Tk
      module Tile
        USE_TILE_NAMESPACE = false
        USE_TTK_NAMESPACE  = true
        TILE_SPEC_VERSION_ID = 7
      end
    end
  else
    # version 0.8 or later
    module Tk
      module Tile
        USE_TILE_NAMESPACE = false
        USE_TTK_NAMESPACE  = true
        TILE_SPEC_VERSION_ID = 8
      end
    end
  end

  module Tk::Tile
    PACKAGE_NAME = 'tile'.freeze
  end
else
  # Ttk package merged Tcl/Tk core (Tcl/Tk 8.5+)
  module Tk
    module Tile
      USE_TILE_NAMESPACE = false
      USE_TTK_NAMESPACE  = true
      TILE_SPEC_VERSION_ID = 8

      PACKAGE_NAME = 'Ttk'.freeze
    end
  end
end

# autoload
module Tk
  module Tile
    TkComm::TkExtlibAutoloadModule.unshift(self)

    def self.package_name
      PACKAGE_NAME
    end

    def self.package_version
      begin
        TkPackage.require(PACKAGE_NAME)
      rescue
        ''
      end
    end

    def self.__Import_Tile_Widgets__!
      warn 'Warning: "Tk::Tile::__Import_Tile_Widgets__!" is obsolete.' <<
           ' To control default widget set, use "Tk.default_widget_set = :Ttk"'
      Tk.tk_call('namespace', 'import', '-force', 'ttk::*')
    end

    def self.__define_LoadImages_proc_for_compatibility__!
      # Ttk 8.5 (Tile 0.8) lost 'LoadImages' utility procedure.
      # So, some old scripts doen't work, because those scripts use the
      # procedure to define local styles.
      # Of course, rewriting such Tcl/Tk scripts isn't difficult for
      # Tcl/Tk users. However, it may be troublesome for Ruby/Tk users
      # who use such Tcl/Tk scripts as it is.
      # This method may help Ruby/Tk users who don't want to modify old
      # Tcl/Tk scripts for the latest version of Ttk (Tile) extension.
      # This method defines a comaptible 'LoadImages' procedure on the
      # Tcl/Tk interpreter working under Ruby/Tk.
      # Please give attention to use this method. It may conflict with
      # some definitions on Tcl/Tk scripts.
      klass_name = self.name
      proc_name = 'LoadImages'
      if Tk::Tile::USE_TTK_NAMESPACE
        ns_list = ['::tile']
        if Tk.info(:commands, "::ttk::#{proc_name}").empty?
          ns_list << '::ttk'
        end
      else # Tk::Tile::USE_TILE_NAMESPACE
        ns_list = ['::ttk']
        if Tk.info(:commands, "::tile::#{proc_name}").empty?
          ns_list << '::tile'
        end
      end

      ns_list.each{|ns|
        cmd = "#{ns}::#{proc_name}"
        unless Tk.info(:commands, cmd).empty?
          #fail RuntimeError, "can't define '#{cmd}' command (already exist)"

          # do nothing !!!
          warn "Warning: can't define '#{cmd}' command (already exist)" if $DEBUG
          next
        end
        TkNamespace.eval(ns){
          TkCore::INTERP.add_tk_procs(proc_name, 'imgdir {patterns {*.gif}}',
                                      <<-'EOS')
            foreach pattern $patterns {
              foreach file [glob -directory $imgdir $pattern] {
                set img [file tail [file rootname $file]]
                if {![info exists images($img)]} {
                  set images($img) [image create photo -file $file]
                }
              }
            }
            return [array get images]
          EOS
        }
      }
    end

    def self.load_images(imgdir, pat=nil)
      if Tk::Tile::TILE_SPEC_VERSION_ID < 8
        if Tk::Tile::USE_TTK_NAMESPACE
          cmd = '::ttk::LoadImages'
        else # Tk::Tile::USE_TILE_NAMESPACE
          cmd = '::tile::LoadImages'
        end
        pat ||= TkComm::None
        images = Hash[*TkComm.simplelist(Tk.tk_call(cmd, imgdir, pat))]
        images.keys.each{|k|
          images[k] = TkPhotoImage.new(:imagename=>images[k],
                                       :without_creating=>true)
        }
      else ## TILE_SPEC_VERSION_ID >= 8
        pat ||= '*.gif'
        if pat.kind_of?(Array)
          pat_list = pat
        else
          pat_list = [ pat ]
        end
        Dir.chdir(imgdir){
          pat_list.each{|pat|
            Dir.glob(pat).each{|f|
              img = File.basename(f, '.*')
              unless TkComm.bool(Tk.info('exists', "images(#{img})"))
                Tk.tk_call('set', "images(#{img})",
                           Tk.tk_call('image', 'create', 'photo', '-file', f))
              end
            }
          }
        }
        images = Hash[*TkComm.simplelist(Tk.tk_call('array', 'get', 'images'))]
        images.keys.each{|k|
          images[k] = TkPhotoImage.new(:imagename=>images[k],
                                       :without_creating=>true)
        }
      end

      images
    end

    def self.style(*args)
      args.map!{|arg| TkComm._get_eval_string(arg)}.join('.')
    end

    def self.themes(glob_ptn = nil)
      if TILE_SPEC_VERSION_ID < 8 && Tk.info(:commands, '::ttk::themes').empty?
        fail RuntimeError, 'not support glob option' if glob_ptn
        cmd = ['::tile::availableThemes']
      else
        glob_ptn = '*' unless glob_ptn
        cmd = ['::ttk::themes', glob_ptn]
      end

      begin
        TkComm.simplelist(Tk.tk_call_without_enc(*cmd))
      rescue
        TkComm.simplelist(Tk.tk_call('lsearch', '-all', '-inline',
                                     Tk::Tile::Style.theme_names,
                                     glob_ptn))
      end
    end

    def self.set_theme(theme)
      if TILE_SPEC_VERSION_ID < 8 && Tk.info(:commands, '::ttk::setTheme').empty?
        cmd = '::tile::setTheme'
      else
        cmd = '::ttk::setTheme'
      end

      begin
        Tk.tk_call_without_enc(cmd, theme)
      rescue
        Tk::Tile::Style.theme_use(theme)
      end
    end

    module KeyNav
      if Tk::Tile::TILE_SPEC_VERSION_ID < 8
        def self.enableMnemonics(w)
          Tk.tk_call('::keynav::enableMnemonics', w)
        end
        def self.defaultButton(w)
          Tk.tk_call('::keynav::defaultButton', w)
        end
      else # dummy
        def self.enableMnemonics(w)
          ""
        end
        def self.defaultButton(w)
          ""
        end
      end
    end

    module Font
      Default      = 'TkDefaultFont'
      Text         = 'TkTextFont'
      Heading      = 'TkHeadingFont'
      Caption      = 'TkCaptionFont'
      Tooltip      = 'TkTooltipFont'

      Fixed        = 'TkFixedFont'
      Menu         = 'TkMenuFont'
      SmallCaption = 'TkSmallCaptionFont'
      Icon         = 'TkIconFont'

      TkFont::SYSTEM_FONT_NAMES.add [
        'TkDefaultFont', 'TkTextFont', 'TkHeadingFont',
        'TkCaptionFont', 'TkTooltipFont', 'TkFixedFont',
        'TkMenuFont', 'TkSmallCaptionFont', 'TkIconFont'
      ]
    end

    module ParseStyleLayout
      def _style_layout(lst)
        ret = []
        until lst.empty?
          sub = [lst.shift]
          keys = {}

          until lst.empty?
            if lst[0][0] == ?-
              k = lst.shift[1..-1]
              children = lst.shift
              children = _style_layout(children) if children.kind_of?(Array)
              keys[k] = children
            else
              break
            end
          end

          sub << keys unless keys.empty?
          ret << sub
        end
        ret
      end
      private :_style_layout
    end

    module TileWidget
      include Tk::Tile::ParseStyleLayout

      def __val2ruby_optkeys  # { key=>proc, ... }
        # The method is used to convert a opt-value to a ruby's object.
        # When get the value of the option "key", "proc.call(value)" is called.
        super().update('style'=>proc{|v| _style_layout(list(v))})
      end
      private :__val2ruby_optkeys

      def ttk_instate(state, script=nil, &b)
        if script
          tk_send('instate', state, script)
        elsif b
          tk_send('instate', state, Proc.new(&b))
        else
          bool(tk_send('instate', state))
        end
      end
      alias tile_instate ttk_instate

      def ttk_state(state=nil)
        if state
          tk_send('state', state)
        else
          list(tk_send('state'))
        end
      end
      alias tile_state ttk_state

      def ttk_identify(x, y)
        ret = tk_send_without_enc('identify', x, y)
        (ret.empty?)? nil: ret
      end
      alias tile_identify ttk_identify

      # remove instate/state/identify method
      # to avoid the conflict with widget options
      if Tk.const_defined?(:USE_OBSOLETE_TILE_STATE_METHOD) && Tk::USE_OBSOLETE_TILE_STATE_METHOD
        alias instate  ttk_instate
        alias state    ttk_state
        alias identify ttk_identify
      end
    end

    ######################################

    autoload :TButton,       'tkextlib/tile/tbutton'
    autoload :Button,        'tkextlib/tile/tbutton'

    autoload :TCheckButton,  'tkextlib/tile/tcheckbutton'
    autoload :CheckButton,   'tkextlib/tile/tcheckbutton'
    autoload :TCheckbutton,  'tkextlib/tile/tcheckbutton'
    autoload :Checkbutton,   'tkextlib/tile/tcheckbutton'

    autoload :Dialog,        'tkextlib/tile/dialog'

    autoload :TEntry,        'tkextlib/tile/tentry'
    autoload :Entry,         'tkextlib/tile/tentry'

    autoload :TCombobox,     'tkextlib/tile/tcombobox'
    autoload :Combobox,      'tkextlib/tile/tcombobox'

    autoload :TFrame,        'tkextlib/tile/tframe'
    autoload :Frame,         'tkextlib/tile/tframe'

    autoload :TLabelframe,   'tkextlib/tile/tlabelframe'
    autoload :Labelframe,    'tkextlib/tile/tlabelframe'
    autoload :TLabelFrame,   'tkextlib/tile/tlabelframe'
    autoload :LabelFrame,    'tkextlib/tile/tlabelframe'

    autoload :TLabel,        'tkextlib/tile/tlabel'
    autoload :Label,         'tkextlib/tile/tlabel'

    autoload :TMenubutton,   'tkextlib/tile/tmenubutton'
    autoload :Menubutton,    'tkextlib/tile/tmenubutton'
    autoload :TMenuButton,   'tkextlib/tile/tmenubutton'
    autoload :MenuButton,    'tkextlib/tile/tmenubutton'

    autoload :TNotebook,     'tkextlib/tile/tnotebook'
    autoload :Notebook,      'tkextlib/tile/tnotebook'

    autoload :TPaned,        'tkextlib/tile/tpaned'
    autoload :Paned,         'tkextlib/tile/tpaned'
    autoload :PanedWindow,   'tkextlib/tile/tpaned'
    autoload :Panedwindow,   'tkextlib/tile/tpaned'

    autoload :TProgressbar,  'tkextlib/tile/tprogressbar'
    autoload :Progressbar,   'tkextlib/tile/tprogressbar'

    autoload :TRadioButton,  'tkextlib/tile/tradiobutton'
    autoload :RadioButton,   'tkextlib/tile/tradiobutton'
    autoload :TRadiobutton,  'tkextlib/tile/tradiobutton'
    autoload :Radiobutton,   'tkextlib/tile/tradiobutton'

    autoload :TScale,        'tkextlib/tile/tscale'
    autoload :Scale,         'tkextlib/tile/tscale'
    autoload :TProgress,     'tkextlib/tile/tscale'
    autoload :Progress,      'tkextlib/tile/tscale'

    autoload :TScrollbar,    'tkextlib/tile/tscrollbar'
    autoload :Scrollbar,     'tkextlib/tile/tscrollbar'
    autoload :XScrollbar,    'tkextlib/tile/tscrollbar'
    autoload :YScrollbar,    'tkextlib/tile/tscrollbar'

    autoload :TSeparator,    'tkextlib/tile/tseparator'
    autoload :Separator,     'tkextlib/tile/tseparator'

    autoload :TSpinbox,      'tkextlib/tile/tspinbox'
    autoload :Spinbox,       'tkextlib/tile/tspinbox'

    autoload :TSquare,       'tkextlib/tile/tsquare'
    autoload :Square,        'tkextlib/tile/tsquare'

    autoload :SizeGrip,      'tkextlib/tile/sizegrip'
    autoload :Sizegrip,      'tkextlib/tile/sizegrip'

    autoload :Treeview,      'tkextlib/tile/treeview'

    autoload :Style,         'tkextlib/tile/style'
  end
end

Ttk = Tk::Tile
