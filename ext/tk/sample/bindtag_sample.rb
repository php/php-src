#!/usr/bin/env ruby
require 'tk'

TkLabel.new(:text=><<EOT, :justify=>:left).pack
This is a sample of bindtags and usage of
Tk.callback_break/Tk.callback_continue.
Please check the work of following buttons
(attend the difference between before/after
 pressing the bottom button), and see the
source code.
EOT

def set_class_bind
  TkButton.bind('ButtonPress-1',
                proc{puts 'bind "ButtonPress-1" of TkButton class'})
  TkButton.bind('ButtonRelease-1',
                proc{puts 'bind "ButtonRelease-1" of TkButton class'})
end

# set root binding
r = TkRoot.new
r.bind('ButtonPress-1',   proc{puts 'bind "ButtonPress-1" of root widget'})
r.bind('ButtonRelease-1', proc{puts 'bind "ButtonRelease-1" of root widget'})

# set 'all' binding
TkBindTag::ALL.bind('ButtonPress-1',
                    proc{puts 'bind "ButtonPress-1" of the tag "all"'})
TkBindTag::ALL.bind('ButtonRelease-1',
                    proc{puts 'bind "ButtonRelease-1" of the tag "all"'})

# create buttons
b1 = TkButton.new(:text=>'button-1',
                  :command=>proc{puts "command of button-1"}).pack
b2 = TkButton.new(:text=>'button-2',
                  :command=>proc{puts "command of button-2"}).pack
b3 = TkButton.new(:text=>'button-3',
                  :command=>proc{puts "command of button-3"}).pack
b4 = TkButton.new(:text=>'button-4',
                  :command=>proc{puts "command of button-4"}).pack
b5 = TkButton.new(:text=>'button-5',
                  :command=>proc{puts "command of button-5"}).pack

# set button binding
b1.bind('ButtonPress-1',   proc{puts 'bind "ButtonPress-1" of button-1'})
b1.bind('ButtonRelease-1', proc{puts 'bind "ButtonRelease-1" of button-1'})

b2.bind('ButtonPress-1',   proc{puts 'bind "ButtonPress-1" of button-2'})
b2.bind('ButtonRelease-1', proc{puts 'bind "ButtonRelease-1" of button-2'})

b3.bind('ButtonPress-1',   proc{puts 'bind "ButtonPress-1" of button-3'})
b3.bind('ButtonRelease-1', proc{puts 'bind "ButtonRelease-1" of button-3'})

b4.bind('ButtonPress-1',   proc{puts 'bind "ButtonPress-1" of button-4'})
b4.bind('ButtonRelease-1', proc{puts 'bind "ButtonRelease-1" of button-4'})

b5.bind('ButtonPress-1',   proc{puts 'bind "ButtonPress-1" of button-5'})
b5.bind('ButtonRelease-1', proc{puts 'bind "ButtonRelease-1" of button-5'})

# create bindtag and set binding
tag1 = TkBindTag.new
tag1.bind('ButtonPress-1',   proc{puts 'bind "ButtonPress-1" of tag1'})
tag1.bind('ButtonRelease-1', proc{puts 'bind "ButtonRelease-1" of tag1'})

tag2 = TkBindTag.new
tag2.bind('ButtonPress-1',
          proc{
            puts 'bind "ButtonPress-1" of tag2'
            puts 'call Tk.callback_continue'
            Tk.callback_continue
            puts 'never see this message'
          })
tag2.bind('ButtonRelease-1',
          proc{
            puts 'bind "ButtonRelease-1" of tag2'
            puts 'call Tk.callback_continue'
            Tk.callback_continue
            puts 'never see this message'
          })

tag3 = TkBindTag.new
tag3.bind('ButtonPress-1',
          proc{
            puts 'bind "ButtonPress-1" of tag3'
            puts 'call Tk.callback_break'
            Tk.callback_break
            puts 'never see this message'
          })
tag3.bind('ButtonRelease-1',
          proc{
            puts 'bind "ButtonRelease-1" of tag3'
            puts 'call Tk.callback_break'
            Tk.callback_break
            puts 'never see this message'
          })

# set bindtags
p b1.bindtags

tags = b2.bindtags
tags[2,0] = tag1
tags[0,0] = tag1
b2.bindtags(tags)
p b2.bindtags

tags = b3.bindtags
tags[2,0] = tag2
tags[0,0] = tag2
b3.bindtags(tags)
p b3.bindtags

tags = b4.bindtags
tags[2,0] = tag3
tags[0,0] = tag3
b4.bindtags(tags)
p b4.bindtags

b5.bindtags([tag1, TkButton, tag2, b5])

# create button to set button class binding
TkButton.new(:text=>'set binding to TkButton class',
             :command=>proc{
               puts 'call "set_class_bind"'
               set_class_bind
             }).pack(:pady=>7)

# start event-loop
Tk.mainloop
