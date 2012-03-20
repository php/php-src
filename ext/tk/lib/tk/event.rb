#
#  tk/event.rb - module for event
#

module TkEvent
end

########################

require 'tkutil'
require 'tk' unless Object.const_defined? :TkComm

########################

module TkEvent
  class Event < TkUtil::CallbackSubst
    module Grp
      KEY         =           0x1
      BUTTON      =           0x2
      MOTION      =           0x4
      CROSSING    =           0x8
      FOCUS       =           0x10
      EXPOSE      =           0x20
      VISIBILITY  =           0x40
      CREATE      =           0x80
      DESTROY     =           0x100
      UNMAP       =           0x200
      MAP         =           0x400
      REPARENT    =           0x800
      CONFIG      =           0x1000
      GRAVITY     =           0x2000
      CIRC        =           0x4000
      PROP        =           0x8000
      COLORMAP    =           0x10000
      VIRTUAL     =           0x20000
      ACTIVATE    =           0x40000
      MAPREQ      =           0x80000
      CONFIGREQ   =           0x100000
      RESIZEREQ   =           0x200000
      CIRCREQ     =           0x400000

      MWHEEL      =           KEY

      STRING_DATA =           0x80000000  # special flag for 'data' field

      ALL         =           0xFFFFFFFF

      KEY_BUTTON_MOTION_VIRTUAL  = (KEY|MWHEEL|BUTTON|MOTION|VIRTUAL)
      KEY_BUTTON_MOTION_CROSSING = (KEY|MWHEEL|BUTTON|MOTION|CROSSING|VIRTUAL)
    end

    type_data = [
      #-----+-------------------+------------------+-----------------------#
      #  ID |  const            |  group_flag      |  context_name         #
      #-----+-------------------+------------------+-----------------------#
      [  2,  :KeyPress,          Grp::KEY,         'KeyPress',    'Key'    ],
      [  3,  :KeyRelease,        Grp::KEY,         'KeyRelease'            ],
      [  4,  :ButtonPress,       Grp::BUTTON,      'ButtonPress', 'Button' ],
      [  5,  :ButtonRelease,     Grp::BUTTON,      'ButtonRelease'         ],
      [  6,  :MotionNotify,      Grp::MOTION,      'Motion'                ],
      [  7,  :EnterNotify,       Grp::CROSSING,    'Enter'                 ],
      [  8,  :LeaveNotify,       Grp::CROSSING,    'Leave'                 ],
      [  9,  :FocusIn,           Grp::FOCUS,       'FocusIn'               ],
      [ 10,  :FocusOut,          Grp::FOCUS,       'FocusOut'              ],
      [ 11,  :KeymapNotify,      0,                                        ],
      [ 12,  :Expose,            Grp::EXPOSE,      'Expose'                ],
      [ 13,  :GraphicsExpose,    Grp::EXPOSE,                              ],
      [ 14,  :NoExpose,          0,                                        ],
      [ 15,  :VisibilityNotify,  Grp::VISIBILITY,  'Visibility'            ],
      [ 16,  :CreateNotify,      Grp::CREATE,      'Create'                ],
      [ 17,  :DestroyNotify,     Grp::DESTROY,     'Destroy'               ],
      [ 18,  :UnmapNotify,       Grp::UNMAP,       'Unmap'                 ],
      [ 19,  :MapNotify,         Grp::MAP,         'Map'                   ],
      [ 20,  :MapRequest,        Grp::MAPREQ,      'MapRequest'            ],
      [ 21,  :ReparentNotify,    Grp::REPARENT,    'Reparent'              ],
      [ 22,  :ConfigureNotify,   Grp::CONFIG,      'Configure'             ],
      [ 23,  :ConfigureRequest,  Grp::CONFIGREQ,   'ConfigureRequest'      ],
      [ 24,  :GravityNotify,     Grp::GRAVITY,     'Gravity'               ],
      [ 25,  :ResizeRequest,     Grp::RESIZEREQ,   'ResizeRequest'         ],
      [ 26,  :CirculateNotify,   Grp::CIRC,        'Circulate'             ],
      [ 27,  :CirculateRequest,  0,                'CirculateRequest'      ],
      [ 28,  :PropertyNotify,    Grp::PROP,        'Property'              ],
      [ 29,  :SelectionClear,    0,                                        ],
      [ 30,  :SelectionRequest,  0,                                        ],
      [ 31,  :SelectionNotify,   0,                                        ],
      [ 32,  :ColormapNotify,    Grp::COLORMAP,    'Colormap'              ],
      [ 33,  :ClientMessage,     0,                                        ],
      [ 34,  :MappingNotify,     0,                                        ],
      [ 35,  :VirtualEvent,      Grp::VIRTUAL,                             ],
      [ 36,  :ActivateNotify,    Grp::ACTIVATE,    'Activate'              ],
      [ 37,  :DeactivateNotify,  Grp::ACTIVATE,    'Deactivate'            ],
      [ 38,  :MouseWheelEvent,   Grp::MWHEEL,      'MouseWheel'            ],
      [ 39,  :TK_LASTEVENT,      0,                                        ]
    ]

    module TypeNum
    end

    TYPE_NAME_TBL  = Hash.new
    TYPE_ID_TBL    = Hash.new
    TYPE_GROUP_TBL = Hash.new

    type_data.each{|id, c_name, g_flag, *t_names|
      TypeNum.const_set(c_name, id)
      t_names.each{|t_name| t_name.freeze; TYPE_NAME_TBL[t_name] = id }
      TYPE_ID_TBL[id]    = t_names
      TYPE_GROUP_TBL[id] = g_flag
    }

    TYPE_NAME_TBL.freeze
    TYPE_ID_TBL.freeze

    def self.type_id(name)
      TYPE_NAME_TBL[name.to_s]
    end

    def self.type_name(id)
      TYPE_ID_TBL[id] && TYPE_ID_TBL[id][0]
    end

    def self.group_flag(id)
      TYPE_GROUP_TBL[id] || 0
    end

    #############################################

    module StateMask
      ShiftMask      =        (1<<0)
      LockMask       =        (1<<1)
      ControlMask    =        (1<<2)
      Mod1Mask       =        (1<<3)
      Mod2Mask       =        (1<<4)
      Mod3Mask       =        (1<<5)
      Mod4Mask       =        (1<<6)
      Mod5Mask       =        (1<<7)
      Button1Mask    =        (1<<8)
      Button2Mask    =        (1<<9)
      Button3Mask    =        (1<<10)
      Button4Mask    =        (1<<11)
      Button5Mask    =        (1<<12)

      AnyModifier    =        (1<<15)

      META_MASK      =  (AnyModifier<<1)
      ALT_MASK       =  (AnyModifier<<2)
      EXTENDED_MASK  =  (AnyModifier<<3)

      CommandMask    =  Mod1Mask
      OptionMask     =  Mod2Mask
    end

    #############################################

    FIELD_FLAG = {
      # key  =>  flag
      'above'       => Grp::CONFIG,
      'borderwidth' => (Grp::CREATE|Grp::CONFIG),
      'button'      => Grp::BUTTON,
      'count'       => Grp::EXPOSE,
      'data'        => (Grp::VIRTUAL|Grp::STRING_DATA),
      'delta'       => Grp::MWHEEL,
      'detail'      => (Grp::FOCUS|Grp::CROSSING),
      'focus'       => Grp::CROSSING,
      'height'      => (Grp::EXPOSE|Grp::CONFIG),
      'keycode'     => Grp::KEY,
      'keysym'      => Grp::KEY,
      'mode'        => (Grp::CROSSING|Grp::FOCUS),
      'override'    => (Grp::CREATE|Grp::MAP|Grp::REPARENT|Grp::CONFIG),
      'place'       => Grp::CIRC,
      'root'        => (Grp::KEY_BUTTON_MOTION_VIRTUAL|Grp::CROSSING),
      'rootx'       => (Grp::KEY_BUTTON_MOTION_VIRTUAL|Grp::CROSSING),
      'rooty'       => (Grp::KEY_BUTTON_MOTION_VIRTUAL|Grp::CROSSING),
      'sendevent'   => Grp::ALL,
      'serial'      => Grp::ALL,
      'state'       => (Grp::KEY_BUTTON_MOTION_VIRTUAL|
                        Grp::CROSSING|Grp::VISIBILITY),
      'subwindow'   => (Grp::KEY_BUTTON_MOTION_VIRTUAL|Grp::CROSSING),
      'time'        => (Grp::KEY_BUTTON_MOTION_VIRTUAL|Grp::CROSSING|
                        Grp::PROP),
      'warp'        => Grp::KEY_BUTTON_MOTION_VIRTUAL,
      'width'       => (Grp::EXPOSE|Grp::CREATE|Grp::CONFIG),
      'window'      => (Grp::CREATE|Grp::UNMAP|Grp::MAP|Grp::REPARENT|
                        Grp::CONFIG|Grp::GRAVITY|Grp::CIRC),
      'when'        => Grp::ALL,
      'x'           => (Grp::KEY_BUTTON_MOTION_VIRTUAL|Grp::CROSSING|
                        Grp::EXPOSE|Grp::CREATE|Grp::CONFIG|Grp::GRAVITY|
                        Grp::REPARENT),
      'y'           => (Grp::KEY_BUTTON_MOTION_VIRTUAL|Grp::CROSSING|
                        Grp::EXPOSE|Grp::CREATE|Grp::CONFIG|Grp::GRAVITY|
                        Grp::REPARENT),
    }

    FIELD_OPERATION = {
      'root' => proc{|val|
        begin
          Tk.tk_call_without_enc('winfo', 'pathname', val)
          val
        rescue
          nil
        end
      },

      'subwindow' => proc{|val|
        begin
          Tk.tk_call_without_enc('winfo', 'pathname', val)
          val
        rescue
          nil
        end
      },

      'window' => proc{|val| nil}
    }

    #-------------------------------------------

    def valid_fields(group_flag=nil)
      group_flag = self.class.group_flag(self.type) unless group_flag

      fields = {}
      FIELD_FLAG.each{|key, flag|
        next if (flag & group_flag) == 0
        begin
          val = self.__send__(key)
        rescue
          next
        end
        # next if !val || val == '??'
        next if !val || (val == '??' && (flag & Grp::STRING_DATA))
        fields[key] = val
      }

      fields
    end

    def valid_for_generate(group_flag=nil)
      fields = valid_fields(group_flag)

      FIELD_OPERATION.each{|key, cmd|
        next unless fields.has_key?(key)
        val = FIELD_OPERATION[key].call(fields[key])
        if val
          fields[key] = val
        else
          fields.delete(key)
        end
      }

      fields
    end

    def generate(win, modkeys={})
      klass = self.class

      if modkeys.has_key?(:type) || modkeys.has_key?('type')
        modkeys = TkComm._symbolkey2str(modkeys)
        type_id = modkeys.delete('type')
      else
        type_id = self.type
      end

      type_name  = klass.type_name(type_id)
      unless type_name
        fail RuntimeError, "type_id #{type_id} is invalid"
      end

      group_flag = klass.group_flag(type_id)

      opts = valid_for_generate(group_flag)

      modkeys.each{|key, val|
        if val
          opts[key.to_s] = val
        else
          opts.delete(key.to_s)
        end
      }

      if group_flag != Grp::KEY
        Tk.event_generate(win, type_name, opts)
      else
        # If type is KEY event, focus should be set to target widget.
        # If not set, original widget will get the same event.
        # That will make infinite loop.
        w = Tk.tk_call_without_enc('focus')
        begin
          Tk.tk_call_without_enc('focus', win)
          Tk.event_generate(win, type_name, opts)
        ensure
          Tk.tk_call_without_enc('focus', w)
        end
      end
    end

    #############################################

    # [ <'%' subst-key char>, <proc type char>, <instance var (accessor) name>]
    KEY_TBL = [
      [ ?#, ?n, :serial ],
      [ ?a, ?s, :above ],
      [ ?b, ?n, :num ],
      [ ?c, ?n, :count ],
      [ ?d, ?s, :detail ],
      # ?e
      [ ?f, ?b, :focus ],
      # ?g
      [ ?h, ?n, :height ],
      [ ?i, ?s, :win_hex ],
      # ?j
      [ ?k, ?n, :keycode ],
      # ?l
      [ ?m, ?s, :mode ],
      # ?n
      [ ?o, ?b, :override ],
      [ ?p, ?s, :place ],
      # ?q
      # ?r
      [ ?s, ?x, :state ],
      [ ?t, ?n, :time ],
      # ?u
      [ ?v, ?n, :value_mask ],
      [ ?w, ?n, :width ],
      [ ?x, ?n, :x ],
      [ ?y, ?n, :y ],
      # ?z
      [ ?A, ?s, :char ],
      [ ?B, ?n, :borderwidth ],
      # ?C
      [ ?D, ?n, :wheel_delta ],
      [ ?E, ?b, :send_event ],
      # ?F
      # ?G
      # ?H
      # ?I
      # ?J
      [ ?K, ?s, :keysym ],
      # ?L
      # ?M
      [ ?N, ?n, :keysym_num ],
      # ?O
      [ ?P, ?s, :property ],
      # ?Q
      [ ?R, ?s, :rootwin_id ],
      [ ?S, ?s, :subwindow ],
      [ ?T, ?n, :type ],
      # ?U
      # ?V
      [ ?W, ?w, :widget ],
      [ ?X, ?n, :x_root ],
      [ ?Y, ?n, :y_root ],
      # ?Z
      nil
    ]

    # [ <'%' subst-key str>, <proc type char>, <instance var (accessor) name>]
    #   the subst-key string will be converted to a bytecode (128+idx).
    LONGKEY_TBL = [
      # for example, for %CTT and %CST subst-key on tkdnd-2.0
      # ['CTT', ?l, :drop_target_type],
      # ['CST', ?l, :drop_source_type],
    ]

    # [ <proc type char>, <proc/method to convert tcl-str to ruby-obj>]
    PROC_TBL = [
      [ ?n, TkComm.method(:num_or_str) ],
      [ ?s, TkComm.method(:string) ],
      [ ?b, TkComm.method(:bool) ],
      [ ?w, TkComm.method(:window) ],

      [ ?x, proc{|val|
          begin
            TkComm::number(val)
          rescue ArgumentError
            val
          end
        }
      ],

      nil
    ]

=begin
    # for Ruby m17n :: ?x --> String --> char-code ( getbyte(0) )
    KEY_TBL.map!{|inf|
      if inf.kind_of?(Array)
        inf[0] = inf[0].getbyte(0) if inf[0].kind_of?(String)
        inf[1] = inf[1].getbyte(0) if inf[1].kind_of?(String)
      end
      inf
    }

    PROC_TBL.map!{|inf|
      if inf.kind_of?(Array)
        inf[0] = inf[0].getbyte(0) if inf[0].kind_of?(String)
      end
      inf
    }
=end

    # setup tables to be used by scan_args, _get_subst_key, _get_all_subst_keys
    #
    #     _get_subst_key() and _get_all_subst_keys() generates key-string
    #     which describe how to convert callback arguments to ruby objects.
    #     When binding parameters are given, use _get_subst_key().
    #     But when no parameters are given, use _get_all_subst_keys() to
    #     create a Event class object as a callback parameter.
    #
    #     scan_args() is used when doing callback. It convert arguments
    #     ( which are Tcl strings ) to ruby objects based on the key string
    #     that is generated by _get_subst_key() or _get_all_subst_keys().
    #
    _setup_subst_table(KEY_TBL, PROC_TBL)
    # _setup_subst_table(KEY_TBL, LONGKEY_TBL, PROC_TBL) # if use longname-keys

    #
    # NOTE: The order of parameters which passed to callback procedure is
    #        <extra_arg>, <extra_arg>, ... , <subst_arg>, <subst_arg>, ...
    #

    # If you need support extra arguments given by Tcl/Tk,
    # please override _get_extra_args_tbl
    #
    #def self._get_extra_args_tbl
    #  # return an array of convert procs
    #  []
    #end

=begin
    alias button num
    alias delta  wheel_delta
    alias root   rootwin_id
    alias rootx  x_root
    alias root_x x_root
    alias rooty  y_root
    alias root_y y_root
    alias sendevent send_event
=end
    ALIAS_TBL = {
      :button    => :num,
      :data      => :detail,
      :delta     => :wheel_delta,
      :root      => :rootwin_id,
      :rootx     => :x_root,
      :root_x    => :x_root,
      :rooty     => :y_root,
      :root_y    => :y_root,
      :sendevent => :send_event,
      :window    => :widget
    }

    _define_attribute_aliases(ALIAS_TBL)

  end

  ###############################################

  def install_bind_for_event_class(klass, cmd, *args)
    extra_args_tbl = klass._get_extra_args_tbl

    if args.compact.size > 0
      args.map!{|arg| klass._sym2subst(arg)}
      args = args.join(' ')
      keys = klass._get_subst_key(args)

      if cmd.kind_of?(String)
        id = cmd
      elsif cmd.kind_of?(TkCallbackEntry)
        id = install_cmd(cmd)
      else
        id = install_cmd(proc{|*arg|
          ex_args = []
          extra_args_tbl.reverse_each{|conv| ex_args << conv.call(arg.pop)}
          begin
            TkUtil.eval_cmd(cmd, *(ex_args.concat(klass.scan_args(keys, arg))))
          rescue Exception=>e
            if TkCore::INTERP.kind_of?(TclTkIp)
              fail e
            else
              # MultiTkIp
              fail Exception, "#{e.class}: #{e.message.dup}"
            end
          end
        })
      end
    elsif cmd.respond_to?(:arity) && cmd.arity == 0  # args.size == 0
      args = ''
      if cmd.kind_of?(String)
        id = cmd
      elsif cmd.kind_of?(TkCallbackEntry)
        id = install_cmd(cmd)
      else
        id = install_cmd(proc{
                           begin
                             TkUtil.eval_cmd(cmd)
                           rescue Exception=>e
                             if TkCore::INTERP.kind_of?(TclTkIp)
                               fail e
                             else
                               # MultiTkIp
                               fail Exception, "#{e.class}: #{e.message.dup}"
                             end
                           end
                         })
      end
    else
      keys, args = klass._get_all_subst_keys

      if cmd.kind_of?(String)
        id = cmd
      elsif cmd.kind_of?(TkCallbackEntry)
        id = install_cmd(cmd)
      else
        id = install_cmd(proc{|*arg|
          ex_args = []
          extra_args_tbl.reverse_each{|conv| ex_args << conv.call(arg.pop)}
          begin
            TkUtil.eval_cmd(cmd, *(ex_args << klass.new(*klass.scan_args(keys, arg))))
          rescue Exception=>e
            if TkCore::INTERP.kind_of?(TclTkIp)
              fail e
            else
              # MultiTkIp
              fail Exception, "#{e.class}: #{e.message.dup}"
            end
          end
        })
      end
    end

    if TkCore::INTERP.kind_of?(TclTkIp)
      id + ' ' + args
    else
      # MultiTkIp
      "if {[set st [catch {#{id} #{args}} ret]] != 0} {
         if {$st == 4} {
           return -code continue $ret
         } elseif {$st == 3} {
           return -code break $ret
         } elseif {$st == 2} {
           return -code return $ret
         } elseif {[regexp {^Exception: (TkCallbackContinue: .*)$} \
                                                               $ret m msg]} {
           return -code continue $msg
         } elseif {[regexp {^Exception: (TkCallbackBreak: .*)$} $ret m msg]} {
           return -code break $msg
         } elseif {[regexp {^Exception: (TkCallbackReturn: .*)$} $ret m msg]} {
           return -code return $msg
         } elseif {[regexp {^Exception: (\\S+: .*)$} $ret m msg]} {
           return -code return $msg
         } else {
           return -code error $ret
         }
       } else {
          set ret
       }"
    end
  end

  def install_bind(cmd, *args)
    install_bind_for_event_class(TkEvent::Event, cmd, *args)
  end
end
