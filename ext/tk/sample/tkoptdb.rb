#!/usr/bin/env ruby
#
#  sample script of TkOptionDB
#
#  If 'LANG' environment variable's value is started by 'ja',
#  then read Japanese resource data and display Japanese button text.
#  In other case, read English resource data and display English text.
#
require "tk"

if __FILE__ == $0 || !TkCore::INTERP.safe?
  if ENV['LANG'] =~ /^ja/
    # read Japanese resource
    TkOptionDB.read_with_encoding(File.expand_path('resource.ja',
                                                   File.dirname(__FILE__)),
                                  'utf-8')
  else
    # read English resource
    TkOptionDB.readfile(File.expand_path('resource.en',
                                         File.dirname(__FILE__)))
  end
end

# 'show_msg' and 'bye_msg' procedures can be defined on BTN_CMD resource.
# Those procedures are called under $SAFE==2
cmd = TkOptionDB.new_proc_class(:BTN_CMD, [:show_msg, :bye_msg], 3) {
  # If you want to check resource string (str),
  # please define __check_proc_string__(str) like this.
  class << self
    def __check_proc_string__(str)
      print "($SAFE=#{$SAFE} >>>) check!! str.tainted?::#{str.tainted?}"
      str.untaint
      print "==>#{str.tainted?} (<<< $SAFE=#{$SAFE}): "
      str
    end
    private :__check_proc_string__
  end
}

# following two frame widgets use same database entry
TkFrame.new(:class=>'BtnFrame'){|f|
  pack(:padx=>5, :pady=>5)
  TkButton.new(:parent=>f, :widgetname=>'hello'){
    command proc{
      print "($SAFE=#{$SAFE} >>>) : "
      cmd.show_msg(TkOptionDB.inspect)
      print "(<<< $SAFE=#{$SAFE})"
    }
    pack(:fill=>:x, :padx=>10, :pady=>10)
  }
  TkButton.new(:command=>proc{
                 print "($SAFE=#{$SAFE} >>>) : "
                 cmd.bye_msg
                 print "(<<< $SAFE=#{$SAFE} ) : "
                 exit
               },
               :parent=>f, :widgetname=>'quit'){
    pack(:fill=>:x, :padx=>10, :pady=>10)
  }
}

class BtnFrame < TkFrame; end
BtnFrame.new{|f|
  pack(:padx=>5, :pady=>5)
  TkButton.new(:parent=>f, :widgetname=>'hello'){
    command proc{
      print "($SAFE=#{$SAFE} >>>) : "
      cmd.show_msg(TkOptionDB.inspect)
      print "(<<< $SAFE=#{$SAFE})"
    }
    pack(:fill=>:x, :padx=>10, :pady=>10)
  }
  TkButton.new(:command=>proc{
                 print "($SAFE=#{$SAFE} >>>) : "
                 cmd.bye_msg
                 print "(<<< $SAFE=#{$SAFE})"
                 exit
               },
               :parent=>f, :widgetname=>'quit'){
    pack(:fill=>:x, :padx=>10, :pady=>10)
  }
}

# if unknown class, use default option values
TkFrame.new(:class=>'BtnFrame2'){|f|
  pack(:padx=>5, :pady=>5)
  TkButton.new(:parent=>f, :widgetname=>'hello'){
    command proc{
      print "($SAFE=#{$SAFE} >>>) : "
      cmd.show_msg(TkOptionDB.inspect)
      print "(<<< $SAFE=#{$SAFE})"
    }
    pack(:fill=>:x, :padx=>10, :pady=>10)
  }
  TkButton.new(:command=>proc{
                 print "($SAFE=#{$SAFE} >>>) : "
                 cmd.bye_msg
                 print "(<<< $SAFE=#{$SAFE})"
                 exit
               },
               :parent=>f, :widgetname=>'quit'){
    pack(:fill=>:x, :padx=>10, :pady=>10)
  }
}

Tk.mainloop
