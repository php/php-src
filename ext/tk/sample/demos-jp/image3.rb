# -*- coding: utf-8 -*-
# image3.rb
#
# This demonstration script creates a simple collection of widgets
# that allow you to select and view images in a Tk label.
#
# widget demo 'load image' (called by 'widget')
#

# toplevel widget
if defined?($image3_demo) && $image3_demo
  $image3_demo.destroy
  $image3_demo = nil
end

# demo toplevel widget
$image3_demo = TkToplevel.new {|w|
  title('Image Demonstration #3')
  iconname("Image3")
  positionWindow(w)
}

base_frame = TkFrame.new($image3_demo).pack(:fill=>:both, :expand=>true)

#
def loadDir3(w)
  w.delete(0,'end')
  Dir.glob([$dirName,'*'].join(File::Separator)).sort.each{|f|
    w.insert('end',File.basename(f))
  }
end

# selectAndLoadDir3 --
# This procedure pops up a dialog to ask for a directory to load into
# the listobx and (if the user presses OK) reloads the directory
# listbox from the directory named in the demo's entry.
#
# Arguments:
# w -                   Name of the toplevel window of the demo.
def selectAndLoadDir3(w, lbox)
  dir = Tk.chooseDirectory(:initialdir=>$dirName.value,
                           :parent=>w, :mustexist=>true)
  if dir.length > 0
    $dirName.value = dir
    loadDir3(lbox)
  end
end

def loadImage3(w,x,y)
  $image3a.file([$dirName, w.get("@#{x},#{y}")].join(File::Separator))
end


# label
msg = TkLabel.new(base_frame) {
  font $font
  wraplength '4i'
  justify 'left'
  text "このデモではTkの photo image を使用して画像を 見ることができます。最初にエントリ内ににディレクトリ名を入れて下さい。次に下のリストボックスにこのディレクトリをロードするため、リターンを押してください。その後、画像を選択するためにリストボックスの中のファイル名をダブルクリックして下さい。"
}
msg.pack('side'=>'top')

# frame
TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    #text '了解'
    text '閉じる'
    command proc{
      tmppath = $image3_demo
      $image3_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'コード参照'
    command proc{showCode 'image3'}
  }.pack('side'=>'left', 'expand'=>'yes')

}.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

# variable
$dirName = TkVariable.new([$demo_dir,'..','images'].join(File::Separator))

# image
begin
  $image3a.delete
rescue
end
$image3a = TkPhotoImage.new

#
image3_f = TkFrame.new(base_frame).pack(:fill=>:both, :expand=>true)

image3_df = TkLabelFrame.new(base_frame, :text=>'ディレクトリ:')

image3_ff = TkLabelFrame.new(base_frame, :text=>'ファイル:',
                             :padx=>'2m', :pady=>'2m')
image3_lbx = TkListbox.new(image3_ff, :width=>20, :height=>10) {
  pack(:side=>:left, :fill=>:y, :expand=>true)
  yscrollbar(TkScrollbar.new(image3_ff).pack(:side=>:left, :fill=>:y,
                                             :expand=>true))
  insert(0, *(%w(earth.gif earthris.gif teapot.ppm)))
  bind('Double-1', proc{|x,y| loadImage3(self, x, y)}, '%x %y')
}

image3_ent = TkEntry.new(image3_df, :width=>30, :textvariable=>$dirName){
  pack(:side=>:left, :fill=>:both, :padx=>'2m', :pady=>'2m', :expand=>true)
  bind('Return', proc{loadDir3(image3_lbx)})
}

TkButton.new(image3_df, :pady=>0, :padx=>'2m', :text=>"ディレクトリ選択",
             :command=>proc{selectAndLoadDir3(image3_ent, image3_lbx)}) {
  pack(:side=>:left, :fill=>:y, :padx=>[0, '2m'], :pady=>'2m')
}

image3_if = TkLabelFrame.new(base_frame, :text=>'イメージ:') {|f|
  # TkLabel.new(f, :image=>$image3a).pack(:padx=>'2m', :pady=>'2m')
  Tk::Label.new(f, :image=>$image3a).pack(:padx=>'2m', :pady=>'2m')
}

Tk.grid(image3_df,  '-',
        :sticky=>:ew, :padx=>'1m', :pady=>'1m', :in=>image3_f)
Tk.grid(image3_ff, image3_if,
        :sticky=>:nw, :padx=>'1m', :pady=>'1m', :in=>image3_f)
TkGrid.columnconfigure(image3_f, 1, :weight=>1)

