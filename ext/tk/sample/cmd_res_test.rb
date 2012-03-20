require 'tk'
TkOptionDB.readfile(File.expand_path('cmd_resource',
                                     File.dirname(__FILE__)))
f = TkFrame.new(:class=>'BtnFrame').pack
b = TkButton.new(:parent=>f, :widgetname=>'hello').pack
cmd1 = TkOptionDB.new_proc_class(b, [:show_msg, :bye_msg], 3)
cmd2 = TkOptionDB.new_proc_class(:ZZZ, [:show_msg, :bye_msg], 3, false, cmd1)
cmd3 = TkOptionDB.new_proc_class(:ZZZ, [:show_msg, :bye_msg], 3, false, b)
cmd4 = TkOptionDB.new_proc_class(:BTN_CMD, [:show_msg, :bye_msg], 3){
  def self.__check_proc_string__(str)
    "{|arg| print [arg, $SAFE].inspect, ': '; Proc.new#{str}.call(arg)}"
  end
}
cmd1.show_msg('cmd1')
cmd2.show_msg('cmd2')
cmd3.show_msg('cmd3')
cmd4.show_msg('cmd4')
