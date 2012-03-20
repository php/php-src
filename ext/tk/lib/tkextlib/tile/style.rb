#
#  style commands
#                               by Hidetoshi NAGAI (nagai@ai.kyutech.ac.jp)
#
require 'tk'
require 'tkextlib/tile.rb'

module Tk
  module Tile
    module Style
    end
  end
end

module Tk::Tile::Style
  extend TkCore
end

class << Tk::Tile::Style
  if Tk::Tile::TILE_SPEC_VERSION_ID < 8
    TkCommandNames = ['style'.freeze].freeze

    # --- Tk::Tile::Style.__define_wrapper_proc_for_compatibility__! ---
    # On Ttk (Tile) extension, 'style' command has imcompatible changes
    # depend on the version of the extention. It requires modifying the
    # Tcl/Tk scripts to define local styles. The rule for modification
    # is a simple one. But, if users want to keep compatibility between
    # versions of the extension, they will have to contrive to do that.
    # It may be troublesome, especially for Ruby/Tk users.
    # This method may help such work. This method make some definitions
    # on the Tcl/Tk interpreter to work with different version of style
    # command format. Please give attention to use this method. It may
    # conflict with some definitions on Tcl/Tk scripts.
    if Tk::Tile::TILE_SPEC_VERSION_ID < 7
      def __define_wrapper_proc_for_compatibility__!
        __define_themes_and_setTheme_proc__!

        unless Tk.info(:commands, '::ttk::style').empty?
          # fail RuntimeError,
          #      "can't define '::ttk::style' command (already exist)"

          # do nothing !!!
          warn "Warning: can't define '::ttk::style' command (already exist)" if $DEBUG
          return
        end
        TkCore::INTERP.add_tk_procs('::ttk::style', 'args', <<-'EOS')
        if [string equal [lrange $args 0 1] {element create}] {
          if [string equal [lindex $args 3] image] {
            set spec [lindex $args 4]
            set map  [lrange $spec 1 end]
            if [llength $map] {
              # return [eval [concat [list ::style element create [lindex $args 2] image [lindex $spec 0] -map $map] [lrange $args 5 end]]]
              return [uplevel 1 [list ::style element create [lindex $args 2] image [lindex $spec 0] -map $map] [lrange $args 5 end]]
            }
          }
        }
        # return [eval "::style $args"]
        return [uplevel 1 ::style $args]
      EOS
        #########################
      end
    else ### TILE_SPEC_VERSION_ID == 7
      def __define_wrapper_proc_for_compatibility__!
        __define_themes_and_setTheme_proc__!

        unless Tk.info(:commands, '::ttk::style').empty?
          # fail RuntimeError,
          #     "can't define '::ttk::style' command (already exist)"

          # do nothing !!!
          warn "Warning: can't define '::ttk::style' command (already exist)" if $DEBUG
          return
        end
        TkCore::INTERP.add_tk_procs('::ttk::style', 'args', <<-'EOS')
        if [string equal [lrange $args 0 1] {element create}] {
          if [string equal [lindex $args 3] image] {
            set spec [lindex $args 4]
            set map  [lrange $spec 1 end]
            if [llength $map] {
              # return [eval [concat [list ::style element create [lindex $args 2] image [lindex $spec 0] -map $map] [lrange $args 5 end]]]
              return [uplevel 1 [list ::style element create [lindex $args 2] image [lindex $spec 0] -map $map] [lrange $args 5 end]]]
            }
          }
        } elseif [string equal [lindex $args 0] default] {
          # return [eval "::style [lreplace $args 0 0 configure]"]
          return [uplevel 1 ::style [lreplace $args 0 0 configure]]
        }
        # return [eval "::style $args"]
        return [uplevel 1 ::style $args]
      EOS
        #########################
      end
    end
  else ### TILE_SPEC_VERSION_ID >= 8
    TkCommandNames = ['::ttk::style'.freeze].freeze

    def __define_wrapper_proc_for_compatibility__!
      __define_themes_and_setTheme_proc__!

      unless Tk.info(:commands, '::style').empty?
        # fail RuntimeError, "can't define '::style' command (already exist)"

        # do nothing !!!
        warn "Warning: can't define '::style' command (already exist)" if $DEBUG
        return
      end
      TkCore::INTERP.add_tk_procs('::style', 'args', <<-'EOS')
        if [string equal [lrange $args 0 1] {element create}] {
          if [string equal [lindex $args 3] image] {
            set name [lindex $args 4]
            set opts [lrange $args 5 end]
            set idx [lsearch $opts -map]
            if {$idx >= 0 && [expr $idx % 2 == 0]} {
             # return [eval [concat [list ::ttk::style element create [lindex $args 2] image [concat $name [lindex $opts [expr $idx + 1]]]] [lreplace $opts $idx [expr $idx + 1]]]]
              return [uplevel 1 [list ::ttk::style element create [lindex $args 2] image [concat $name [lindex $opts [expr $idx + 1]]]] [lreplace $opts $idx [expr $idx + 1]]]
            }
          }
        } elseif [string equal [lindex $args 0] default] {
          # return [eval "::ttk::style [lreplace $args 0 0 configure]"]
          return [uplevel 1 ::ttk::style [lreplace $args 0 0 configure]]
        }
        # return [eval "::ttk::style $args"]
        return [uplevel 1 ::ttk::style $args]
      EOS
      #########################
    end
  end

  def __define_themes_and_setTheme_proc__!
    TkCore::INTERP.add_tk_procs('::ttk::themes', '{ptn *}', <<-'EOS')
      #set themes [list]
      set themes [::ttk::style theme names]
      foreach pkg [lsearch -inline -all -glob [package names] ttk::theme::$ptn] {
          set theme [namespace tail $pkg]
          if {[lsearch -exact $themes $theme] < 0} {
              lappend themes $theme
          }
      }
      foreach pkg [lsearch -inline -all -glob [package names] tile::theme::$ptn] {
          set theme [namespace tail $pkg]
          if {[lsearch -exact $themes $theme] < 0} {
              lappend themes $theme
          }
      }
      return $themes
    EOS
    #########################
    TkCore::INTERP.add_tk_procs('::ttk::setTheme', 'theme', <<-'EOS')
      variable currentTheme
      if {[lsearch -exact [::ttk::style theme names] $theme] < 0} {
          package require [lsearch -inline -regexp [package names] (ttk|tile)::theme::$theme]
      }
      ::ttk::style theme use $theme
      set currentTheme $theme
    EOS
  end
  private :__define_themes_and_setTheme_proc__!

  def configure(style=nil, keys=nil)
    if style.kind_of?(Hash)
      keys = style
      style = nil
    end
    style = '.' unless style

    if Tk::Tile::TILE_SPEC_VERSION_ID < 7
      sub_cmd = 'default'
    else
      sub_cmd = 'configure'
    end

    if keys && keys != None
      tk_call(TkCommandNames[0], sub_cmd, style, *hash_kv(keys))
    else
      tk_call(TkCommandNames[0], sub_cmd, style)
    end
  end
  alias default configure

  def map(style=nil, keys=nil)
    if style.kind_of?(Hash)
      keys = style
      style = nil
    end
    style = '.' unless style

    if keys && keys != None
      if keys.kind_of?(Hash)
        tk_call(TkCommandNames[0], 'map', style, *hash_kv(keys))
      else
        simplelist(tk_call(TkCommandNames[0], 'map', style, '-' << keys.to_s))
      end
    else
      ret = {}
      Hash[*(simplelist(tk_call(TkCommandNames[0], 'map', style)))].each{|k, v|
        ret[k[1..-1]] = list(v)
      }
      ret
    end
  end
  alias map_configure map

  def map_configinfo(style=nil, key=None)
    style = '.' unless style
    map(style, key)
  end

  def map_default_configinfo(key=None)
    map('.', key)
  end

  def lookup(style, opt, state=None, fallback_value=None)
    tk_call(TkCommandNames[0], 'lookup', style,
            '-' << opt.to_s, state, fallback_value)
  end

  include Tk::Tile::ParseStyleLayout

  def layout(style=nil, spec=nil)
    if style.kind_of?(Hash)
      spec = style
      style = nil
    end
    style = '.' unless style

    if spec
      tk_call(TkCommandNames[0], 'layout', style, spec)
    else
      _style_layout(list(tk_call(TkCommandNames[0], 'layout', style)))
    end
  end

  def element_create(name, type, *args)
    if type == 'image' || type == :image
      element_create_image(name, *args)
    elsif type == 'vsapi' || type == :vsapi
      element_create_vsapi(name, *args)
    else
      tk_call(TkCommandNames[0], 'element', 'create', name, type, *args)
    end
  end

  def element_create_image(name, *args)
    fail ArgumentError, 'Must supply a base image' unless (spec = args.shift)
    if (opts = args.shift)
      if opts.kind_of?(Hash)
        opts = _symbolkey2str(opts)
      else
        fail ArgumentError, 'bad option'
      end
    end
    fail ArgumentError, 'too many arguments' unless args.empty?

    if spec.kind_of?(Array)
      # probably, command format is tile 0.8+ (Tcl/Tk8.5+) style
      if Tk::Tile::TILE_SPEC_VERSION_ID >= 8
        if opts
          tk_call(TkCommandNames[0],
                  'element', 'create', name, 'image', spec, opts)
        else
          tk_call(TkCommandNames[0], 'element', 'create', name, 'image', spec)
        end
      else
        fail ArgumentError, 'illegal arguments' if opts.key?('map')
        base = spec.shift
        opts['map'] = spec
        tk_call(TkCommandNames[0],
                'element', 'create', name, 'image', base, opts)
      end
    else
      # probably, command format is tile 0.7.8 or older style
      if Tk::Tile::TILE_SPEC_VERSION_ID >= 8
        spec = [spec, *(opts.delete('map'))] if opts.key?('map')
      end
      if opts
        tk_call(TkCommandNames[0],
                'element', 'create', name, 'image', spec, opts)
      else
        tk_call(TkCommandNames[0], 'element', 'create', name, 'image', spec)
      end
    end
  end

  def element_create_vsapi(name, class_name, part_id, *args)
    # supported on Tcl/Tk 8.6 or later

    # argument check
    if (state_map = args.shift || None)
      if state_map.kind_of?(Hash)
        opts = _symbolkey2str(state_map)
        state_map = None
      end
    end
    opts = args.shift || None
    fail ArgumentError, "too many arguments" unless args.empty?

    # define a Microsoft Visual Styles element
    tk_call(TkCommandNames[0], 'element', 'create', name, 'vsapi',
            class_name, part_id, state_map, opts)
  end

  def element_names()
    list(tk_call(TkCommandNames[0], 'element', 'names'))
  end

  def element_options(elem)
    simplelist(tk_call(TkCommandNames[0], 'element', 'options', elem))
  end

  def theme_create(name, keys=nil)
    name = name.to_s
    if keys && keys != None
      tk_call(TkCommandNames[0], 'theme', 'create', name, *hash_kv(keys))
    else
      tk_call(TkCommandNames[0], 'theme', 'create', name)
    end
    name
  end

  def theme_settings(name, cmd=nil, &b)
    name = name.to_s
    cmd = Proc.new(&b) if !cmd && b
    tk_call(TkCommandNames[0], 'theme', 'settings', name, cmd)
    name
  end

  def theme_names()
    list(tk_call(TkCommandNames[0], 'theme', 'names'))
  end

  def theme_use(name)
    name = name.to_s
    tk_call(TkCommandNames[0], 'theme', 'use', name)
    name
  end
end
