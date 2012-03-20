#!/usr/bin/env ruby
require 'tk'
require 'tkextlib/iwidgets'

def get_files(file)
  dir = (file.empty?)? ENV['HOME'] : TkComm._fromUTF8(file)
  Dir.chdir(dir) rescue return ''
  Dir['*'].sort.collect{|f|
    [TkComm._toUTF8(File.join(dir, f)), TkComm._toUTF8(f)]
  }
end

Tk::Iwidgets::Hierarchy.new(:querycommand=>proc{|arg| get_files(arg.node)},
                            :visibleitems=>'30x15',
                            :labeltext=>ENV['HOME']).pack(:side=>:left,
                                                          :expand=>true,
                                                          :fill=>:both)

# Tk::Iwidgets::Hierarchy.new(:querycommand=>[proc{|n| get_files(n)}, '%n'],
#                           :visibleitems=>'30x15',
#                           :labeltext=>ENV['HOME']).pack(:side=>:left,
#                                                         :expand=>true,
#                                                         :fill=>:both)

Tk.mainloop
