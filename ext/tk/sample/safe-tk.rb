#!/usr/bin/env ruby
# This script is a sample of MultiTkIp class

require "multi-tk"

###############################

TkLabel.new(:text=>'This is the Default Master Ipnterpreter').pack(:padx=>5, :pady=>3)
TkButton.new(:text=>'QUIT', :command=>proc{exit}).pack(:pady=>3)
TkFrame.new(:borderwidth=>2, :height=>3,
            :relief=>:sunken).pack(:fill=>:x, :expand=>true,
                                   :padx=>10, :pady=>7)

safe0_p = proc{|*args| p args}

###############################

puts "---- create a safe slave IP with Ruby's safe-level == 1 ----------"
ip = MultiTkIp.new_safe_slave(1){|*args| safe0_p["safe_slave safe_level == #{$SAFE}", args]}

puts "\n---- create procs ----------"
puts 'x = proc{p [\'proc x\', "$SAFE==#{$SAFE}"]; exit}'
#x = proc{p ['proc x', "$SAFE==#{$SAFE}"]; exit}
x = proc{p ['proc x', "$SAFE==#{$SAFE}"]; exit}
TkLabel.new(:text=>'x = proc{p [\'proc x\', "$SAFE==#{$SAFE}"]; exit}',
            :anchor=>:w).pack(:fill=>:x)

puts 'y = proc{|label| p [\'proc y\', "$SAFE==#{$SAFE}", label]; label.text($SAFE)}'
y = proc{|label| p ['proc y', "$SAFE==#{$SAFE}", label]; label.text($SAFE)}
TkLabel.new(:text=>'y = proc{|label| p [\'proc y\', "$SAFE==#{$SAFE}", label]; label.text($SAFE)}',
            :anchor=>:w).pack(:fill=>:x)

puts 'z = proc{p [\'proc z\', "$SAFE==#{$SAFE}"]; exit}'
z = proc{p ['proc z', "$SAFE==#{$SAFE}"]; exit}
TkLabel.new(:text=>'z = proc{p [\'proc z\', "$SAFE==#{$SAFE}"]; exit}',
            :anchor=>:w).pack(:fill=>:x)

puts "\n---- call 1st eval_proc ----------"
print 'lbl = '
p lbl = ip.eval_proc{
  TkLabel.new(:text=>"1st eval_proc : $SAFE == #{$SAFE}").pack

  f = TkFrame.new.pack
  TkLabel.new(f, :text=>"$SAFE == ").pack(:side=>:left)
  # TkLabel.new(f, :text=>" (<-- 'lbl' widget is here)").pack(:side=>:right)
  l = TkLabel.new(f).pack(:side=>:right)

  TkButton.new(:text=>':command=>proc{l.text($SAFE)}',
               :command=>proc{l.text($SAFE)}).pack(:fill=>:x, :padx=>5)
  TkButton.new(:text=>':command=>x', :command=>x).pack(:fill=>:x, :padx=>5)
  TkButton.new(:text=>':command=>proc{exit}',
               :command=>proc{
                 safe0_p["'exit' is called at $SAFE=#{$SAFE}"];exit}
               ).pack(:fill=>:x, :padx=>5)
  TkFrame.new(:borderwidth=>2, :height=>3,
              :relief=>:sunken).pack(:fill=>:x, :expand=>true,
                                     :padx=>10, :pady=>7)
  l # return the label widget
}

puts "\n---- change the safe slave IP's safe-level ==> 3 ----------"
ip.safe_level = 3

puts "\n---- call 2nd eval_proc ----------"
p ip.eval_proc(proc{
                 TkLabel.new(:text=>"2nd eval_proc : $SAFE == #{$SAFE}").pack
                 f = TkFrame.new.pack
                 TkLabel.new(f, :text=>"$SAFE == ").pack(:side=>:left)
                 l = TkLabel.new(f, :text=>$SAFE).pack(:side=>:right)
                 TkButton.new(:text=>':command=>proc{l.text($SAFE)}',
                              :command=>proc{l.text($SAFE)}).pack(:fill=>:x,
                                                                  :padx=>5)
                 TkButton.new(:text=>':command=>proc{y.call(l)}',
                              :command=>proc{y.call(l)}).pack(:fill=>:x,
                                                              :padx=>5)
                 TkButton.new(:text=>':command=>proc{Proc.new(&y).call(l)}',
                              :command=>proc{
                                Proc.new(&y).call(l)
                              }).pack(:fill=>:x, :padx=>5)
                 TkButton.new(:text=>':command=>proc{MultiTkIp._proc_on_current_safelevel(y).call(l)}',
                              :command=>proc{
                                MultiTkIp._proc_on_current_safelevel(y).call(l)
                              }).pack(:fill=>:x, :padx=>5)
if false && Object.const_defined?(:RubyVM) && ::RubyVM.class == Class
                 TkButton.new(:text=>':command=>proc{Thread.new(l, &y).value}',
                              :command=>proc{
                                Thread.new(l, &y).value
                              }).pack(:fill=>:x, :padx=>5)
else
  # KNOWN BUG::
  #   Current multi-tk.rb cannot support long term threads on callbacks.
  #   Such a thread freezes the Ruby/Tk process.
end
                 TkButton.new(:text=>':command=>proc{z.call}',
                              :command=>proc{z.call}).pack(:fill=>:x, :padx=>5)
                 TkFrame.new(:borderwidth=>2, :height=>3,
                             :relief=>:sunken).pack(:fill=>:x, :expand=>true,
                                                    :padx=>10, :pady=>7)
               })

puts "\n---- call 1st and 2nd eval_str ----------"
p bind = ip.eval_str('
  TkLabel.new(:text=>"1st and 2nd eval_str : $SAFE == #{$SAFE}").pack
  f = TkFrame.new.pack
  TkLabel.new(f, :text=>"$SAFE == ").pack(:side=>:left)
  l = TkLabel.new(f, :text=>$SAFE).pack(:side=>:right)
  TkButton.new(:text=>":command=>proc{y.call(l)}",
               :command=>proc{y.call(l)}).pack(:fill=>:x, :padx=>5)
  binding
', binding)

p ip.eval_str("
  TkButton.new(:text=>':command=>proc{ l.text = $SAFE }',
               :command=>proc{ l.text = $SAFE }).pack(:fill=>:x, :padx=>5)
  TkFrame.new(:borderwidth=>2, :height=>3,
              :relief=>:sunken).pack(:fill=>:x, :expand=>true,
                                     :padx=>10, :pady=>7)
", bind)

puts "\n---- change the safe slave IP's safe-level ==> 4 ----------"
ip.safe_level = 4

puts "\n---- call 3rd and 4th eval_proc ----------"
p ip.eval_proc{
  TkLabel.new(:text=>"3rd and 4th eval_proc : $SAFE == #{$SAFE}").pack
}
p ip.eval_proc{
  TkButton.new(:text=>':command=>proc{ lbl.text = $SAFE }',
               :command=>proc{ lbl.text = $SAFE }).pack(:fill=>:x, :padx=>5)
}

puts "\n---- start event-loop ( current $SAFE == #{$SAFE} ) ----------"

Tk.mainloop
