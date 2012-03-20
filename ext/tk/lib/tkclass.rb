#
#               tkclass.rb - Tk classes
#                       Date: 2000/11/27 09:23:36
#                       by Yukihiro Matsumoto <matz@caelum.co.jp>
#
#                       $Id$

require "tk"

TopLevel = TkToplevel
Frame = TkFrame
Label = TkLabel
Button = TkButton
Radiobutton = TkRadioButton
Checkbutton = TkCheckButton
Message = TkMessage
Entry = TkEntry
Spinbox = TkSpinbox
Text = TkText
Scale = TkScale
Scrollbar = TkScrollbar
Listbox = TkListbox
Menu = TkMenu
Menubutton = TkMenubutton
Canvas = TkCanvas
Arc = TkcArc
Bitmap = TkcBitmap
Line = TkcLine
Oval = TkcOval
Polygon = TkcPolygon
Rectangle = TkcRectangle
TextItem = TkcText
WindowItem = TkcWindow
BitmapImage = TkBitmapImage
PhotoImage = TkPhotoImage
Selection = TkSelection
Winfo = TkWinfo
Pack = TkPack
Grid = TkGrid
Place = TkPlace
Variable = TkVariable
Font = TkFont
VirtualEvent = TkVirtualEvent

def Mainloop
  Tk.mainloop
end
