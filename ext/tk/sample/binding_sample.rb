#!/usr/bin/env ruby

require 'tk'

class Button_clone < TkLabel
  def initialize(*args)
    @command = nil

    if args[-1].kind_of?(Hash)
      keys = _symbolkey2str(args.pop)
      @command = keys.delete('command')

      keys['highlightthickness'] = 1 unless keys.key?('highlightthickness')
      keys['padx'] = '3m' unless keys.key?('padx')
      keys['pady'] = '1m' unless keys.key?('pady')
      keys['relief'] = 'raised' unless keys.key?('relief')

      args.push(keys)
    end

    super(*args)

    @press = false

    self.bind('Enter', proc{self.background(self.activebackground)})
    self.bind('Leave', proc{
                @press = false
                self.background(self.highlightbackground)
                self.relief('raised')
              })

    self.bind('ButtonPress-1', proc{@press = true; self.relief('sunken')})
    self.bind('ButtonRelease-1', proc{
                self.relief('raised')
                @command.call if @press && @command
                @press = false
              })
  end

  def command(cmd = Proc.new)
    @command = cmd
  end

  def invoke
    if @command
      @command.call
    else
      ''
    end
  end
end

TkLabel.new(:text=><<EOT).pack
This is a sample of 'event binding'.
The first button is a normal button widget.
And the second one is a normal label widget
but with some bindings like a button widget.
EOT

lbl = TkLabel.new(:foreground=>'red').pack(:pady=>3)

v = TkVariable.new(0)

TkFrame.new{|f|
  TkLabel.new(f, :text=>'click count : ').pack(:side=>:left)
  TkLabel.new(f, :textvariable=>v).pack(:side=>:left)
}.pack

TkButton.new(:text=>'normal Button widget',
             :command=>proc{
               puts 'button is clicked!!'
               lbl.text 'button is clicked!!'
               v.numeric += 1
             }){
  pack(:fill=>:x, :expand=>true)
}

Button_clone.new(:text=>'Label with Button binding',
                 :command=>proc{
                   puts 'label is clicked!!'
                   lbl.text 'label is clicked!!'
                   v.numeric += 1
                 }){
  pack(:fill=>:x, :expand=>true)
}

Tk.mainloop
