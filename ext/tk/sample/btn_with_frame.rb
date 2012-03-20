require 'tk'

class Button_with_Frame < TkButton
  def create_self(keys)
    @frame = TkFrame.new('widgetname'=>@path, 'background'=>'yellow')
    install_win(@path) # create new @path which is a daughter of old @path
    super(keys)
    TkPack(@path, :padx=>7, :pady=>7)
    @epath = @frame.path
  end
  def epath
    @epath
  end
end

Button_with_Frame.new(:text=>'QUIT', :command=>proc{exit}) {
  pack(:padx=>15, :pady=>5)
}

Tk.mainloop
