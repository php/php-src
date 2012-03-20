%# floor2.rb
#
# This demonstration script creates a canvas widet that displays the
# floorplan for DEC's Western Research Laboratory.
#
# floorDisplay widget demo (called by 'widget')
#

# floorDisplay2 --
# Recreate the floorplan display in the canvas given by "w".  The
# floor given by "active" is displayed on top with its office structure
# visible.
#
# Arguments:
# w -           Name of the canvas window.
# active -      Number of active floor (1, 2, or 3).

def floorDisplay2(w,active)
  return if $activeFloor2 == active

  w.delete('all')
  $activeFloor2 = active

  # First go through the three floors, displaying the backgrounds for
  # each floor.

  floor2_bg1(w,$floor2_colors['bg1'],$floor2_colors['outline1'])
  floor2_bg2(w,$floor2_colors['bg2'],$floor2_colors['outline2'])
  floor2_bg3(w,$floor2_colors['bg3'],$floor2_colors['outline3'])

  # Raise the background for the active floor so that it's on top.

  w.raise("floor#{active}")

  # Create a dummy item just to mark this point in the display list,
  # so we can insert highlights here.

  w.create(TkcRectangle,0,100,1,101,'fill'=>'','outline'=>'','tags'=>'marker')

  # Add the walls and labels for the active floor, along with
  # transparent polygons that define the rooms on the floor.
  # Make sure that the room polygons are on top.

  $floorLabels2.clear
  $floorItems2.clear
  send("floor2_fg#{active}", w, $floor2_colors['offices'])
  w.raise('room')

  # Offset the floors diagonally from each other.

  w.move('floor1', '2c', '2c')
  w.move('floor2', '1c', '1c')

  # Create items for the room entry and its label.
  w.create(TkcWindow, 600, 100, 'anchor'=>'w', 'window'=>$floor2_entry)
  w.create(TkcText, 600, 100, 'anchor'=>'e', 'text'=>"Room: ")
  w['scrollregion'] = w.bbox('all')
end

# newRoom2 --
# This method is invoked whenever the mouse enters a room
# in the floorplan.  It changes tags so that the current room is
# highlighted.
#
# Arguments:
# w  -          The name of the canvas window.

def newRoom2(w)
  id = w.find_withtag('current')[0]
  $currentRoom2.value = $floorLabels2[id.id] if id != ""
  Tk.update(true)
end

# roomChanged2 --
# This method is invoked whenever the currentRoom variable changes.
# It highlights the current room and unhighlights any previous room.
#
# Arguments:
# w -           The canvas window displaying the floorplan.
# args -        Not used.

def roomChanged2(w,*args)
  w.delete('highlight')
  item = $floorItems2[$currentRoom2.value]
  return if item == nil
  new = TkcPolygon.new(w, *(w.coords(item)))
  new.configure('fill'=>$floor2_colors['active'], 'tags'=>'highlight')
  w.raise(new, 'marker')
end

# floor2_bg1 --
# This method represents part of the floorplan database.  When
# invoked, it instantiates the background information for the first
# floor.
#
# Arguments:
# w -           The canvas window.
# fill -        Fill color to use for the floor's background.
# outline -     Color to use for the floor's outline.

def floor2_bg1(w,fill,outline)
  w.create(TkcPolygon,347,80,349,82,351,84,353,85,363,92,375,99,386,104,
                 386,129,398,129,398,162,484,162,484,129,559,129,559,133,725,
                 133,725,129,802,129,802,389,644,389,644,391,559,391,559,327,
                 508,327,508,311,484,311,484,278,395,278,395,288,400,288,404,
                 288,409,290,413,292,418,297,421,302,422,309,421,318,417,325,
                 411,330,405,332,397,333,344,333,340,334,336,336,335,338,332,
                 342,331,347,332,351,334,354,336,357,341,359,340,360,335,363,
                 331,365,326,366,304,366,304,355,258,355,258,387,60,387,60,391,
                 0,391,0,337,3,337,3,114,8,114,8,25,30,25,30,5,93,5,98,5,104,7,
                 110,10,116,16,119,20,122,28,123,32,123,68,220,68,220,34,221,
                 22,223,17,227,13,231,8,236,4,242,2,246,0,260,0,283,1,300,5,
                 321,14,335,22,348,25,365,29,363,39,358,48,352,56,337,70,
                 344,76,347,80, 'tags'=>['floor1','bg'], 'fill'=>fill)
  w.create(TkcLine,386,129,398,129, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,258,355,258,387, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,60,387,60,391, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,0,337,0,391, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,60,391,0,391, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,3,114,3,337, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,258,387,60,387, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,484,162,398,162, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,398,162,398,129, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,484,278,484,311, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,484,311,508,311, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,508,327,508,311, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,559,327,508,327, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,644,391,559,391, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,644,389,644,391, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,559,129,484,129, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,484,162,484,129, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,725,133,559,133, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,559,129,559,133, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,725,129,802,129, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,802,389,802,129, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,3,337,0,337, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,559,391,559,327, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,802,389,644,389, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,725,133,725,129, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,8,25,8,114, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,8,114,3,114, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,30,25,8,25, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,484,278,395,278, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,30,25,30,5, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,93,5,30,5, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,98,5,93,5, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,104,7,98,5, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,110,10,104,7, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,116,16,110,10, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,119,20,116,16, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,122,28,119,20, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,123,32,122,28, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,123,68,123,32, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,220,68,123,68, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,386,129,386,104, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,386,104,375,99, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,375,99,363,92, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,353,85,363,92, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,220,68,220,34, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,337,70,352,56, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,352,56,358,48, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,358,48,363,39, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,363,39,365,29, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,365,29,348,25, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,348,25,335,22, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,335,22,321,14, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,321,14,300,5, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,300,5,283,1, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,283,1,260,0, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,260,0,246,0, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,246,0,242,2, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,242,2,236,4, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,236,4,231,8, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,231,8,227,13, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,223,17,227,13, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,221,22,223,17, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,220,34,221,22, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,340,360,335,363, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,335,363,331,365, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,331,365,326,366, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,326,366,304,366, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,304,355,304,366, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,395,288,400,288, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,404,288,400,288, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,409,290,404,288, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,413,292,409,290, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,418,297,413,292, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,421,302,418,297, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,422,309,421,302, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,421,318,422,309, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,421,318,417,325, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,417,325,411,330, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,411,330,405,332, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,405,332,397,333, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,397,333,344,333, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,344,333,340,334, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,340,334,336,336, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,336,336,335,338, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,335,338,332,342, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,331,347,332,342, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,332,351,331,347, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,334,354,332,351, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,336,357,334,354, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,341,359,336,357, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,341,359,340,360, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,395,288,395,278, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,304,355,258,355, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,347,80,344,76, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,344,76,337,70, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,349,82,347,80, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,351,84,349,82, 'fill'=>outline, 'tags'=>['floor1','bg'])
  w.create(TkcLine,353,85,351,84, 'fill'=>outline, 'tags'=>['floor1','bg'])
end

# floor2_bg2 --
# This method represents part of the floorplan database.  When
# invoked, it instantiates the background information for the first
# floor.
#
# Arguments:
# w -           The canvas window.
# fill -        Fill color to use for the floor's background.
# outline -     Color to use for the floor's outline.

def floor2_bg2(w,fill,outline)
  w.create(TkcPolygon,559,129,484,129,484,162,398,162,398,129,315,129,
                 315,133,176,133,176,129,96,129,96,133,3,133,3,339,0,339,0,391,
                 60,391,60,387,258,387,258,329,350,329,350,311,395,311,395,280,
                 484,280,484,311,508,311,508,327,558,327,558,391,644,391,644,
                 367,802,367,802,129,725,129,725,133,559,133,559,129,
                 'tags'=>['floor2','bg'], 'fill'=>fill)
  w.create(TkcLine,350,311,350,329, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,398,129,398,162, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,802,367,802,129, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,802,129,725,129, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,725,133,725,129, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,559,129,559,133, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,559,133,725,133, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,484,162,484,129, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,559,129,484,129, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,802,367,644,367, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,644,367,644,391, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,644,391,558,391, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,558,327,558,391, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,558,327,508,327, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,508,327,508,311, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,484,311,508,311, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,484,280,484,311, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,398,162,484,162, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,484,280,395,280, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,395,280,395,311, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,258,387,60,387, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,3,133,3,339, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,3,339,0,339, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,60,391,0,391, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,0,339,0,391, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,60,387,60,391, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,258,329,258,387, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,350,329,258,329, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,395,311,350,311, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,398,129,315,129, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,176,133,315,133, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,176,129,96,129, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,3,133,96,133, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,315,133,315,129, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,176,133,176,129, 'fill'=>outline, 'tags'=>['floor2','bg'])
  w.create(TkcLine,96,133,96,129, 'fill'=>outline, 'tags'=>['floor2','bg'])
end

# floor2_bg3 --
# This method represents part of the floorplan database.  When
# invoked, it instantiates the background information for the first
# floor.
#
# Arguments:
# w -           The canvas window.
# fill -        Fill color to use for the floor's background.
# outline -     Color to use for the floor's outline.

def floor2_bg3(w,fill,outline)
  w.create(TkcPolygon,159,300,107,300,107,248,159,248,159,129,96,129,96,
                 133,21,133,21,331,0,331,0,391,60,391,60,370,159,370,159,300,
                 'tags'=>['floor3','bg'], 'fill'=>fill)
  w.create(TkcPolygon,258,370,258,329,350,329,350,311,399,311,399,129,
                 315,129,315,133,176,133,176,129,159,129,159,370,258,370,
                 'tags'=>['floor3','bg'], 'fill'=>fill)
  w.create(TkcLine,96,133,96,129, 'fill'=>outline, 'tags'=>['floor3','bg'])
  w.create(TkcLine,176,129,96,129, 'fill'=>outline, 'tags'=>['floor3','bg'])
  w.create(TkcLine,176,129,176,133, 'fill'=>outline, 'tags'=>['floor3','bg'])
  w.create(TkcLine,315,133,176,133, 'fill'=>outline, 'tags'=>['floor3','bg'])
  w.create(TkcLine,315,133,315,129, 'fill'=>outline, 'tags'=>['floor3','bg'])
  w.create(TkcLine,399,129,315,129, 'fill'=>outline, 'tags'=>['floor3','bg'])
  w.create(TkcLine,399,311,399,129, 'fill'=>outline, 'tags'=>['floor3','bg'])
  w.create(TkcLine,399,311,350,311, 'fill'=>outline, 'tags'=>['floor3','bg'])
  w.create(TkcLine,350,329,350,311, 'fill'=>outline, 'tags'=>['floor3','bg'])
  w.create(TkcLine,350,329,258,329, 'fill'=>outline, 'tags'=>['floor3','bg'])
  w.create(TkcLine,258,370,258,329, 'fill'=>outline, 'tags'=>['floor3','bg'])
  w.create(TkcLine,60,370,258,370, 'fill'=>outline, 'tags'=>['floor3','bg'])
  w.create(TkcLine,60,370,60,391, 'fill'=>outline, 'tags'=>['floor3','bg'])
  w.create(TkcLine,60,391,0,391, 'fill'=>outline, 'tags'=>['floor3','bg'])
  w.create(TkcLine,0,391,0,331, 'fill'=>outline, 'tags'=>['floor3','bg'])
  w.create(TkcLine,21,331,0,331, 'fill'=>outline, 'tags'=>['floor3','bg'])
  w.create(TkcLine,21,331,21,133, 'fill'=>outline, 'tags'=>['floor3','bg'])
  w.create(TkcLine,96,133,21,133, 'fill'=>outline, 'tags'=>['floor3','bg'])
  w.create(TkcLine,107,300,159,300,159,248,107,248,107,300,
              'fill'=>outline, 'tags'=>['floor3','bg'])
end

# floor2_fg1 --
# This method represents part of the floorplan database.  When
# invoked, it instantiates the foreground information for the first
# floor (office outlines and numbers).
#
# Arguments:
# w -           The canvas window.
# color -       Color to use for drawing foreground information.

def floor2_fg1(w,color)
  i = TkcPolygon.new(w,375,246,375,172,341,172,341,246,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '101'
  $floorItems2['101'] = i
  w.create(TkcText,358,209, 'text'=>'101', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,307,240,339,240,339,206,307,206,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = 'Pub Lift1'
  $floorItems2['Pub Lift1'] = i
  w.create(TkcText,323,223, 'text'=>'Pub Lift1', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,339,205,307,205,307,171,339,171,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = 'Priv Lift1'
  $floorItems2['Priv Lift1'] = i
  w.create(TkcText,323,188, 'text'=>'Priv Lift1', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,42,389,42,337,1,337,1,389,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '110'
  $floorItems2['110'] = i
  w.create(TkcText,21.5,363, 'text'=>'110', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,59,389,59,385,90,385,90,337,44,337,44,389,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '109'
  $floorItems2['109'] = i
  w.create(TkcText,67,363, 'text'=>'109', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,51,300,51,253,6,253,6,300,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '111'
  $floorItems2['111'] = i
  w.create(TkcText,28.5,276.5, 'text'=>'111', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,98,248,98,309,79,309,79,248,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '117B'
  $floorItems2['117B'] = i
  w.create(TkcText,88.5,278.5, 'text'=>'117B', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,51,251,51,204,6,204,6,251,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '112'
  $floorItems2['112'] = i
  w.create(TkcText,28.5,227.5, 'text'=>'112', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,6,156,51,156,51,203,6,203,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '113'
  $floorItems2['113'] = i
  w.create(TkcText,28.5,179.5, 'text'=>'113', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,85,169,79,169,79,192,85,192,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '117A'
  $floorItems2['117A'] = i
  w.create(TkcText,82,180.5, 'text'=>'117A', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,77,302,77,168,53,168,53,302,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '117'
  $floorItems2['117'] = i
  w.create(TkcText,65,235, 'text'=>'117', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,51,155,51,115,6,115,6,155,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '114'
  $floorItems2['114'] = i
  w.create(TkcText,28.5,135, 'text'=>'114', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,95,115,53,115,53,168,95,168,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '115'
  $floorItems2['115'] = i
  w.create(TkcText,74,141.5, 'text'=>'115', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,87,113,87,27,10,27,10,113,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '116'
  $floorItems2['116'] = i
  w.create(TkcText,48.5,70, 'text'=>'116', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,89,91,128,91,128,113,89,131,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '118'
  $floorItems2['118'] = i
  w.create(TkcText,108.5,102, 'text'=>'118', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,178,128,178,132,216,132,216,91,
                     163,91,163,112,149,112,149,128,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '120'
  $floorItems2['120'] = i
  w.create(TkcText,189.5,111.5, 'text'=>'120', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,79,193,87,193,87,169,136,169,136,192,
                     156,192,156,169,175,169,175,246,79,246,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '122'
  $floorItems2['122'] = i
  w.create(TkcText,131,207.5, 'text'=>'122', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,138,169,154,169,154,191,138,191,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '121'
  $floorItems2['121'] = i
  w.create(TkcText,146,180, 'text'=>'121', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,99,300,126,300,126,309,99,309,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '106A'
  $floorItems2['106A'] = i
  w.create(TkcText,112.5,304.5, 'text'=>'106A', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,128,299,128,309,150,309,150,248,99,248,99,299,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '105'
  $floorItems2['105'] = i
  w.create(TkcText,124.5,278.5, 'text'=>'105', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,174,309,174,300,152,300,152,309,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '106B'
  $floorItems2['106B'] = i
  w.create(TkcText,163,304.5, 'text'=>'106B', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,176,299,176,309,216,309,216,248,152,248,152,299,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '104'
  $floorItems2['104'] = i
  w.create(TkcText,184,278.5, 'text'=>'104', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,138,385,138,337,91,337,91,385,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '108'
  $floorItems2['108'] = i
  w.create(TkcText,114.5,361, 'text'=>'108', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,256,337,140,337,140,385,256,385,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '107'
  $floorItems2['107'] = i
  w.create(TkcText,198,361, 'text'=>'107', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,300,353,300,329,260,329,260,353,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = 'Smoking'
  $floorItems2['Smoking'] = i
  w.create(TkcText,280,341, 'text'=>'Smoking', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,314,135,314,170,306,170,306,246,177,246,177,135,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '123'
  $floorItems2['123'] = i
  w.create(TkcText,245.5,190.5, 'text'=>'123', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,217,248,301,248,301,326,257,326,257,310,217,310,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '103'
  $floorItems2['103'] = i
  w.create(TkcText,259,287, 'text'=>'103', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,396,188,377,188,377,169,316,169,316,131,396,131,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '124'
  $floorItems2['124'] = i
  w.create(TkcText,356,150, 'text'=>'124', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,397,226,407,226,407,189,377,189,377,246,397,246,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '125'
  $floorItems2['125'] = i
  w.create(TkcText,392,217.5, 'text'=>'125', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,399,187,409,187,409,207,474,207,474,164,399,164,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '126'
  $floorItems2['126'] = i
  w.create(TkcText,436.5,185.5, 'text'=>'126', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,409,209,409,229,399,229,399,253,
                     486,253,486,239,474,239,474,209,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '127'
  $floorItems2['127'] = i
  w.create(TkcText,436.5,'231', 'text'=>'127', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,501,164,501,174,495,174,495,188,
                     490,188,490,204,476,204,476,164,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = 'MShower'
  $floorItems2['MShower'] = i
  w.create(TkcText,488.5,'184', 'text'=>'MShower', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,497,176,513,176,513,204,492,204,492,190,497,190,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = 'Closet'
  $floorItems2['Closet'] = i
  w.create(TkcText,502.5,190, 'text'=>'Closet', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,476,237,476,206,513,206,513,254,488,254,488,237,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = 'WShower'
  $floorItems2['WShower'] = i
  w.create(TkcText,494.5,230, 'text'=>'WShower', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,486,131,558,131,558,135,724,135,724,166,
                     697,166,697,275,553,275,531,254,515,254,
                     515,174,503,174,503,161,486,161,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '130'
  $floorItems2['130'] = i
  w.create(TkcText,638.5,205, 'text'=>'130', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,308,242,339,242,339,248,342,248,
                     342,246,397,246,397,276,393,276,
                     393,309,300,309,300,248,308,248,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '102'
  $floorItems2['102'] = i
  w.create(TkcText,367.5,278.5, 'text'=>'102', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,397,255,486,255,486,276,397,276,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '128'
  $floorItems2['128'] = i
  w.create(TkcText,441.5,265.5, 'text'=>'128', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,510,309,486,309,486,255,530,255,
                     552,277,561,277,561,325,510,325,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '129'
  $floorItems2['129'] = i
  w.create(TkcText,535.5,293, 'text'=>'129', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,696,281,740,281,740,387,642,387,
                     642,389,561,389,561,277,696,277,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '133'
  $floorItems2['133'] = i
  w.create(TkcText,628.5,335, 'text'=>'133', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,742,387,742,281,800,281,800,387,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '132'
  $floorItems2['132'] = i
  w.create(TkcText,771,334, 'text'=>'132', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,800,168,800,280,699,280,699,168,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '134'
  $floorItems2['134'] = i
  w.create(TkcText,749.5,224, 'text'=>'134', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,726,131,726,166,800,166,800,131,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '135'
  $floorItems2['135'] = i
  w.create(TkcText,763,148.5, 'text'=>'135', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,340,360,335,363,331,365,326,366,304,366,
                     304,312,396,312,396,288,400,288,404,288,
                     409,290,413,292,418,297,421,302,422,309,
                     421,318,417,325,411,330,405,332,397,333,
                     344,333,340,334,336,336,335,338,332,342,
                     331,347,332,351,334,354,336,357,341,359,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = 'Ramona Stair'
  $floorItems2['Ramona Stair'] = i
  w.create(TkcText,368,323, 'text'=>'Ramona Stair', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,30,23,30,5,93,5,98,5,104,7,110,10,116,16,119,20,
                     122,28,123,32,123,68,220,68,220,87,90,87,90,23,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = 'University Stair'
  $floorItems2['University Stair'] = i
  w.create(TkcText,155,77.5, 'text'=>'University Stair', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,282,37,295,40,312,49,323,56,337,70,352,56,
                     358,48,363,39,365,29,348,25,335,22,321,14,
                     300,5,283,1,260,0,246,0,242,2,236,4,231,8,
                     227,13,223,17,221,22,220,34,260,34,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = 'Plaza Stair'
  $floorItems2['Plaza Stair'] = i
  w.create(TkcText,317.5,28.5, 'text'=>'Plaza Stair', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,220,34,260,34,282,37,295,40,312,49,
                     323,56,337,70,350,83,365,94,377,100,
                     386,104,386,128,220,128,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = 'Plaza Deck'
  $floorItems2['Plaza Deck'] = i
  w.create(TkcText,303,81, 'text'=>'Plaza Deck', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,257,336,77,336,6,336,6,301,77,301,77,310,257,310,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '106'
  $floorItems2['106'] = i
  w.create(TkcText,131.5,318.5, 'text'=>'106', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  i = TkcPolygon.new(w,146,110,162,110,162,91,130,91,130,115,95,115,
                     95,128,114,128,114,151,157,151,157,153,112,153,
                     112,130,97,130,97,168,175,168,175,131,146,131,
                     'fill'=>'', 'tags'=>['floor1','room'])
  $floorLabels2[i.id] = '119'
  $floorItems2['119'] = i
  w.create(TkcText,143.5,133, 'text'=>'119', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor1','label'])
  w.create(TkcLine,155,191,155,189, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,155,177,155,169, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,96,129,96,169, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,78,169,176,169, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,176,247,176,129, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,340,206,307,206, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,340,187,340,170, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,340,210,340,201, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,340,247,340,224, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,340,241,307,241, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,376,246,376,170, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,307,247,307,170, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,376,170,307,170, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,315,129,315,170, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,147,129,176,129, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,202,133,176,133, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,398,129,315,129, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,258,352,258,387, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,60,387,60,391, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,0,337,0,391, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,60,391,0,391, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,3,114,3,337, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,258,387,60,387, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,52,237,52,273, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,52,189,52,225, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,52,140,52,177, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,395,306,395,311, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,531,254,398,254, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,475,178,475,238, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,502,162,398,162, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,398,129,398,188, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,383,188,376,188, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,408,188,408,194, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,398,227,398,254, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,408,227,398,227, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,408,222,408,227, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,408,206,408,210, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,408,208,475,208, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,484,278,484,311, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,484,311,508,311, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,508,327,508,311, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,559,327,508,327, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,644,391,559,391, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,644,389,644,391, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,514,205,475,205, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,496,189,496,187, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,559,129,484,129, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,484,162,484,129, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,725,133,559,133, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,559,129,559,133, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,725,149,725,167, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,725,129,802,129, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,802,389,802,129, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,739,167,802,167, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,396,188,408,188, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,0,337,9,337, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,58,337,21,337, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,43,391,43,337, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,105,337,75,337, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,91,387,91,337, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,154,337,117,337, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,139,387,139,337, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,227,337,166,337, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,258,337,251,337, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,258,328,302,328, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,302,355,302,311, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,395,311,302,311, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,484,278,395,278, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,395,294,395,278, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,473,278,473,275, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,473,256,473,254, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,533,257,531,254, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,553,276,551,274, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,698,276,553,276, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,559,391,559,327, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,802,389,644,389, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,741,314,741,389, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,698,280,698,167, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,707,280,698,280, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,802,280,731,280, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,741,280,741,302, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,698,167,727,167, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,725,137,725,129, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,514,254,514,175, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,496,175,514,175, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,502,175,502,162, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,475,166,475,162, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,496,176,496,175, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,491,189,496,189, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,491,205,491,189, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,487,238,475,238, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,487,240,487,238, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,487,252,487,254, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,315,133,304,133, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,256,133,280,133, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,78,247,270,247, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,307,247,294,247, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,214,133,232,133, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,217,247,217,266, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,217,309,217,291, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,217,309,172,309, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,154,309,148,309, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,175,300,175,309, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,151,300,175,300, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,151,247,151,309, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,78,237,78,265, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,78,286,78,309, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,106,309,78,309, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,130,309,125,309, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,99,309,99,247, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,127,299,99,299, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,127,309,127,299, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,155,191,137,191, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,137,169,137,191, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,78,171,78,169, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,78,190,78,218, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,86,192,86,169, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,86,192,78,192, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,52,301,3,301, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,52,286,52,301, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,52,252,3,252, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,52,203,3,203, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,3,156,52,156, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,8,25,8,114, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,63,114,3,114, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,75,114,97,114, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,108,114,129,114, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,129,114,129,89, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,52,114,52,128, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,132,89,88,89, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,88,25,88,89, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,88,114,88,89, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,218,89,144,89, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,147,111,147,129, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,162,111,147,111, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,162,109,162,111, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,162,96,162,89, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,218,89,218,94, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,218,89,218,119, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,8,25,88,25, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,258,337,258,328, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,113,129,96,129, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,302,355,258,355, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,386,104,386,129, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,377,100,386,104, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,365,94,377,100, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,350,83,365,94, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,337,70,350,83, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,337,70,323,56, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,312,49,323,56, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,295,40,312,49, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,282,37,295,40, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,260,34,282,37, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,253,34,260,34, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,386,128,386,104, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,113,152,156,152, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,113,152,156,152, 'fill'=>color, 'tags'=>['floor1','wall'])
  w.create(TkcLine,113,152,113,129, 'fill'=>color, 'tags'=>['floor1','wall'])
end

# floor2_fg2 --
# This method represents part of the floorplan database.  When
# invoked, it instantiates the foreground information for the second
# floor (office outlines and numbers).
#
# Arguments:
# w -           The canvas window.
# color -       Color to use for drawing foreground information.

def floor2_fg2(w,color)
  i = TkcPolygon.new(w,748,188,755,188,755,205,758,205,758,222,
                     800,222,800,168,748,168,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '238'
  $floorItems2['238'] = i
  w.create(TkcText,774,195, 'text'=>'238', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,726,188,746,188,746,166,800,166,800,131,726,131,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '237'
  $floorItems2['237'] = i
  w.create(TkcText,763,148.5, 'text'=>'237', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,497,187,497,204,559,204,559,324,641,324,
                     643,324,643,291,641,291,641,205,696,205,
                     696,291,694,291,694,314,715,314,715,291,
                     715,205,755,205,755,190,724,190,724,187,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '246'
  $floorItems2['246'] = i
  w.create(TkcText,600,264, 'text'=>'246', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,694,279,643,279,643,314,694,314,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '247'
  $floorItems2['247'] = i
  w.create(TkcText,668.5,296.5, 'text'=>'247', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,232,250,308,250,308,242,339,242,339,246,
                     397,246,397,255,476,255,476,250,482,250,559,250,
                     559,274,482,274,482,278,396,278,396,274,232,274,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '202'
  $floorItems2['202'] = i
  w.create(TkcText,285.5,260, 'text'=>'202', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,53,228,53,338,176,338,233,338,233,196,
                     306,196,306,180,175,180,175,169,156,169,
                     156,196,176,196,176,228,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '206'
  $floorItems2['206'] = i
  w.create(TkcText,143,267, 'text'=>'206', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,51,277,6,277,6,338,51,338,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '212'
  $floorItems2['212'] = i
  w.create(TkcText,28.5,307.5, 'text'=>'212', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,557,276,486,276,486,309,510,309,510,325,557,325,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '245'
  $floorItems2['245'] = i
  w.create(TkcText,521.5,300.5, 'text'=>'245', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,560,389,599,389,599,326,560,326,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '244'
  $floorItems2['244'] = i
  w.create(TkcText,579.5,357.5, 'text'=>'244', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,601,389,601,326,643,326,643,389,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '243'
  $floorItems2['243'] = i
  w.create(TkcText,622,357.5, 'text'=>'243', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,688,316,645,316,645,365,688,365,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '242'
  $floorItems2['242'] = i
  w.create(TkcText,666.5,340.5, 'text'=>'242', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,802,367,759,367,759,226,802,226,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = 'Barbecue Deck'
  $floorItems2['Barbecue Deck'] = i
  w.create(TkcText,780.5,296.5, 'text'=>'Barbecue Deck', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,755,262,755,314,717,314,717,262,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '240'
  $floorItems2['240'] = i
  w.create(TkcText,736,288, 'text'=>'240', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,755,316,689,316,689,365,755,365,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '241'
  $floorItems2['241'] = i
  w.create(TkcText,722,340.5, 'text'=>'241', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,755,206,717,206,717,261,755,261,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '239'
  $floorItems2['239'] = i
  w.create(TkcText,736,233.5, 'text'=>'239', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,695,277,643,277,643,206,695,206,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '248'
  $floorItems2['248'] = i
  w.create(TkcText,669,241.5, 'text'=>'248', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,676,135,676,185,724,185,724,135,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '236'
  $floorItems2['236'] = i
  w.create(TkcText,700,160, 'text'=>'236', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,675,135,635,135,635,145,628,145,628,185,675,185,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '235'
  $floorItems2['235'] = i
  w.create(TkcText,651.5,160, 'text'=>'235', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,626,143,633,143,633,135,572,135,
                     572,143,579,143,579,185,626,185,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '234'
  $floorItems2['234'] = i
  w.create(TkcText,606,160, 'text'=>'234', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,557,135,571,135,571,145,578,145,
                     578,185,527,185,527,131,557,131,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '233'
  $floorItems2['233'] = i
  w.create(TkcText,552.5,158, 'text'=>'233', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,476,249,557,249,557,205,476,205,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '230'
  $floorItems2['230'] = i
  w.create(TkcText,516.5,227, 'text'=>'230', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,476,164,486,164,486,131,525,131,525,185,476,185,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '232'
  $floorItems2['232'] = i
  w.create(TkcText,500.5,158, 'text'=>'232', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,476,186,495,186,495,204,476,204,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '229'
  $floorItems2['229'] = i
  w.create(TkcText,485.5,195, 'text'=>'229', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,474,207,409,207,409,187,399,187,399,164,474,164,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '227'
  $floorItems2['227'] = i
  w.create(TkcText,436.5,185.5, 'text'=>'227', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,399,228,399,253,474,253,474,209,409,209,409,228,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '228'
  $floorItems2['228'] = i
  w.create(TkcText,436.5,231, 'text'=>'228', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,397,246,397,226,407,226,407,189,377,189,377,246,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '226'
  $floorItems2['226'] = i
  w.create(TkcText,392,217.5, 'text'=>'226', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,377,169,316,169,316,131,397,131,397,188,377,188,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '225'
  $floorItems2['225'] = i
  w.create(TkcText,356.5,150, 'text'=>'225', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,234,198,306,198,306,249,234,249,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '224'
  $floorItems2['224'] = i
  w.create(TkcText,270,223.5, 'text'=>'224', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,270,179,306,179,306,170,314,170,314,135,270,135,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '223'
  $floorItems2['223'] = i
  w.create(TkcText,292,157, 'text'=>'223', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,268,179,221,179,221,135,268,135,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '222'
  $floorItems2['222'] = i
  w.create(TkcText,244.5,157, 'text'=>'222', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,177,179,219,179,219,135,177,135,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '221'
  $floorItems2['221'] = i
  w.create(TkcText,198,157, 'text'=>'221', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,299,327,349,327,349,284,341,284,341,276,299,276,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '204'
  $floorItems2['204'] = i
  w.create(TkcText,324,301.5, 'text'=>'204', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,234,276,297,276,297,327,257,327,257,338,234,338,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '205'
  $floorItems2['205'] = i
  w.create(TkcText,265.5,307, 'text'=>'205', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,256,385,256,340,212,340,212,385,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '207'
  $floorItems2['207'] = i
  w.create(TkcText,234,362.5, 'text'=>'207', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,210,340,164,340,164,385,210,385,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '208'
  $floorItems2['208'] = i
  w.create(TkcText,187,362.5, 'text'=>'208', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,115,340,162,340,162,385,115,385,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '209'
  $floorItems2['209'] = i
  w.create(TkcText,138.5,362.5, 'text'=>'209', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,89,228,89,156,53,156,53,228,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '217'
  $floorItems2['217'] = i
  w.create(TkcText,71,192, 'text'=>'217', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,89,169,97,169,97,190,89,190,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '217A'
  $floorItems2['217A'] = i
  w.create(TkcText,93,179.5, 'text'=>'217A', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,89,156,89,168,95,168,95,135,53,135,53,156,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '216'
  $floorItems2['216'] = i
  w.create(TkcText,71,145.5, 'text'=>'216', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,51,179,51,135,6,135,6,179,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '215'
  $floorItems2['215'] = i
  w.create(TkcText,28.5,157, 'text'=>'215', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,51,227,6,227,6,180,51,180,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '214'
  $floorItems2['214'] = i
  w.create(TkcText,28.5,203.5, 'text'=>'214', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,51,275,6,275,6,229,51,229,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '213'
  $floorItems2['213'] = i
  w.create(TkcText,28.5,252, 'text'=>'213', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,114,340,67,340,67,385,114,385,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '210'
  $floorItems2['210'] = i
  w.create(TkcText,90.5,362.5, 'text'=>'210', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,59,389,59,385,65,385,65,340,1,340,1,389,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '211'
  $floorItems2['211'] = i
  w.create(TkcText,33,364.5, 'text'=>'211', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,393,309,350,309,350,282,342,282,342,276,393,276,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '203'
  $floorItems2['203'] = i
  w.create(TkcText,367.5,292.5, 'text'=>'203', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,99,191,91,191,91,226,174,226,174,198,
                     154,198,154,192,109,192,109,169,99,169,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '220'
  $floorItems2['220'] = i
  w.create(TkcText,132.5,208.5, 'text'=>'220', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,339,205,307,205,307,171,339,171,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = 'Priv Lift2'
  $floorItems2['Priv Lift2'] = i
  w.create(TkcText,323,188, 'text'=>'Priv Lift2', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,307,240,339,240,339,206,307,206,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = 'Pub Lift 2'
  $floorItems2['Pub Lift 2'] = i
  w.create(TkcText,323,223, 'text'=>'Pub Lift 2', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,175,168,97,168,97,131,175,131,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '218'
  $floorItems2['218'] = i
  w.create(TkcText,136,149.5, 'text'=>'218', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,154,191,111,191,111,169,154,169,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '219'
  $floorItems2['219'] = i
  w.create(TkcText,132.5,180, 'text'=>'219', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  i = TkcPolygon.new(w,375,246,375,172,341,172,341,246,
                     'fill'=>'', 'tags'=>['floor2','room'])
  $floorLabels2[i.id] = '201'
  $floorItems2['201'] = i
  w.create(TkcText,358,209, 'text'=>'201', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor2','label'])
  w.create(TkcLine,641,186,678,186, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,757,350,757,367, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,634,133,634,144, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,634,144,627,144, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,572,133,572,144, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,572,144,579,144, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,398,129,398,162, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,174,197,175,197, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,175,197,175,227, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,757,206,757,221, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,396,188,408,188, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,727,189,725,189, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,747,167,802,167, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,747,167,747,189, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,755,189,739,189, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,769,224,757,224, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,802,224,802,129, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,802,129,725,129, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,725,189,725,129, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,725,186,690,186, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,676,133,676,186, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,627,144,627,186, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,629,186,593,186, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,579,144,579,186, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,559,129,559,133, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,725,133,559,133, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,484,162,484,129, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,559,129,484,129, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,526,129,526,186, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,540,186,581,186, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,528,186,523,186, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,511,186,475,186, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,496,190,496,186, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,496,205,496,202, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,475,205,527,205, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,558,205,539,205, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,558,205,558,249, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,558,249,475,249, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,662,206,642,206, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,695,206,675,206, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,695,278,642,278, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,642,291,642,206, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,695,291,695,206, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,716,208,716,206, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,757,206,716,206, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,757,221,757,224, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,793,224,802,224, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,757,262,716,262, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,716,220,716,264, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,716,315,716,276, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,757,315,703,315, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,757,325,757,224, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,757,367,644,367, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,689,367,689,315, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,647,315,644,315, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,659,315,691,315, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,600,325,600,391, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,627,325,644,325, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,644,391,644,315, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,615,325,575,325, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,644,391,558,391, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,563,325,558,325, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,558,391,558,314, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,558,327,508,327, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,558,275,484,275, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,558,302,558,275, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,508,327,508,311, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,484,311,508,311, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,484,275,484,311, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,475,208,408,208, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,408,206,408,210, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,408,222,408,227, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,408,227,398,227, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,398,227,398,254, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,408,188,408,194, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,383,188,376,188, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,398,188,398,162, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,398,162,484,162, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,475,162,475,254, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,398,254,475,254, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,484,280,395,280, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,395,311,395,275, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,307,197,293,197, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,278,197,233,197, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,233,197,233,249, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,307,179,284,179, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,233,249,278,249, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,269,179,269,133, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,220,179,220,133, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,155,191,110,191, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,90,190,98,190, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,98,169,98,190, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,52,133,52,165, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,52,214,52,177, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,52,226,52,262, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,52,274,52,276, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,234,275,234,339, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,226,339,258,339, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,211,387,211,339, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,214,339,177,339, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,258,387,60,387, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,3,133,3,339, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,165,339,129,339, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,117,339,80,339, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,68,339,59,339, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,0,339,46,339, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,60,391,0,391, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,0,339,0,391, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,60,387,60,391, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,258,329,258,387, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,350,329,258,329, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,395,311,350,311, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,398,129,315,129, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,176,133,315,133, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,176,129,96,129, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,3,133,96,133, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,66,387,66,339, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,115,387,115,339, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,163,387,163,339, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,234,275,276,275, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,288,275,309,275, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,298,275,298,329, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,341,283,350,283, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,321,275,341,275, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,375,275,395,275, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,315,129,315,170, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,376,170,307,170, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,307,250,307,170, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,376,245,376,170, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,340,241,307,241, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,340,245,340,224, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,340,210,340,201, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,340,187,340,170, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,340,206,307,206, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,293,250,307,250, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,271,179,238,179, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,226,179,195,179, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,176,129,176,179, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,182,179,176,179, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,174,169,176,169, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,162,169,90,169, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,96,169,96,129, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,175,227,90,227, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,90,190,90,227, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,52,179,3,179, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,52,228,3,228, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,52,276,3,276, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,155,177,155,169, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,110,191,110,169, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,155,189,155,197, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,350,283,350,329, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,162,197,155,197, 'fill'=>color, 'tags'=>['floor2','wall'])
  w.create(TkcLine,341,275,341,283, 'fill'=>color, 'tags'=>['floor2','wall'])
end

# floor2_fg3 --
# This method represents part of the floorplan database.  When
# invoked, it instantiates the foreground information for the third
# floor (office outlines and numbers).
#
# Arguments:
# w -           The canvas window.
# color -       Color to use for drawing foreground information.

def floor2_fg3(w,color)
  i = TkcPolygon.new(w,89,228,89,180,70,180,70,228,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '316'
  $floorItems2['316'] = i
  w.create(TkcText,79.5,204, 'text'=>'316', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,115,368,162,368,162,323,115,323,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '309'
  $floorItems2['309'] = i
  w.create(TkcText,138.5,345.5, 'text'=>'309', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,164,323,164,368,211,368,211,323,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '308'
  $floorItems2['308'] = i
  w.create(TkcText,187.5,345.5, 'text'=>'308', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,256,368,212,368,212,323,256,323,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '307'
  $floorItems2['307'] = i
  w.create(TkcText,234,345.5, 'text'=>'307', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,244,276,297,276,297,327,260,327,260,321,244,321,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '305'
  $floorItems2['305'] = i
  w.create(TkcText,270.5,301.5, 'text'=>'305', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,251,219,251,203,244,203,244,219,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '324B'
  $floorItems2['324B'] = i
  w.create(TkcText,247.5,211, 'text'=>'324B', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,251,249,244,249,244,232,251,232,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '324A'
  $floorItems2['324A'] = i
  w.create(TkcText,247.5,240.5, 'text'=>'324A', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,223,135,223,179,177,179,177,135,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '320'
  $floorItems2['320'] = i
  w.create(TkcText,200,157, 'text'=>'320', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,114,368,114,323,67,323,67,368,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '310'
  $floorItems2['310'] = i
  w.create(TkcText,90.5,345.5, 'text'=>'310', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,23,277,23,321,68,321,68,277,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '312'
  $floorItems2['312'] = i
  w.create(TkcText,45.5,299, 'text'=>'312', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,23,229,68,229,68,275,23,275,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '313'
  $floorItems2['313'] = i
  w.create(TkcText,45.5,252, 'text'=>'313', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,68,227,23,227,23,180,68,180,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '314'
  $floorItems2['314'] = i
  w.create(TkcText,40.5,203.5, 'text'=>'314', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,95,179,95,135,23,135,23,179,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '315'
  $floorItems2['315'] = i
  w.create(TkcText,59,157, 'text'=>'315', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,99,226,99,204,91,204,91,226,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '316B'
  $floorItems2['316B'] = i
  w.create(TkcText,95,215, 'text'=>'316B', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,91,202,99,202,99,180,91,180,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '316A'
  $floorItems2['316A'] = i
  w.create(TkcText,95,191, 'text'=>'316A', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,97,169,109,169,109,192,154,192,154,198,
                     174,198,174,226,101,226,101,179,97,179,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '319'
  $floorItems2['319'] = i
  w.create(TkcText,141.5,209, 'text'=>'319', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,65,368,58,368,58,389,1,389,1,333,23,333,23,323,65,323,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '311'
  $floorItems2['311'] = i
  w.create(TkcText,29.5,361, 'text'=>'311', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,154,191,111,191,111,169,154,169,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '318'
  $floorItems2['318'] = i
  w.create(TkcText,132.5,180, 'text'=>'318', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,175,168,97,168,97,131,175,131,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '317'
  $floorItems2['317'] = i
  w.create(TkcText,136,149.5, 'text'=>'317', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,274,194,274,221,306,221,306,194,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '323'
  $floorItems2['323'] = i
  w.create(TkcText,290,207.5, 'text'=>'323', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,306,222,274,222,274,249,306,249,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '325'
  $floorItems2['325'] = i
  w.create(TkcText,290,235.5, 'text'=>'325', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,263,179,224,179,224,135,263,135,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '321'
  $floorItems2['321'] = i
  w.create(TkcText,243.5,157, 'text'=>'321', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,314,169,306,169,306,192,273,192,
                     264,181,264,135,314,135,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '322'
  $floorItems2['322'] = i
  w.create(TkcText,293.5,163.5, 'text'=>'322', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,307,240,339,240,339,206,307,206,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = 'Pub Lift3'
  $floorItems2['Pub Lift3'] = i
  w.create(TkcText,323,223, 'text'=>'Pub Lift3', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,339,205,307,205,307,171,339,171,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = 'Priv Lift3'
  $floorItems2['Priv Lift3'] = i
  w.create(TkcText,323,188, 'text'=>'Priv Lift3', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,350,284,376,284,376,276,397,276,397,309,350,309,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '303'
  $floorItems2['303'] = i
  w.create(TkcText,373.5,292.5, 'text'=>'303', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,272,203,272,249,252,249,252,230,
                     244,230,244,221,252,221,252,203,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '324'
  $floorItems2['324'] = i
  w.create(TkcText,262,226, 'text'=>'324', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,299,276,299,327,349,327,349,284,341,284,341,276,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '304'
  $floorItems2['304'] = i
  w.create(TkcText,324,301.5, 'text'=>'304', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,375,246,375,172,341,172,341,246,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '301'
  $floorItems2['301'] = i
  w.create(TkcText,358,209, 'text'=>'301', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,397,246,377,246,377,185,397,185,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '327'
  $floorItems2['327'] = i
  w.create(TkcText,387,215.5, 'text'=>'327', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,316,131,316,169,377,169,377,185,397,185,397,131,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '326'
  $floorItems2['326'] = i
  w.create(TkcText,365.5,150, 'text'=>'326', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,308,251,242,251,242,274,342,274,342,282,375, 282,
                     375,274,397,274,397,248,339,248,339,242,308,242,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '302'
  $floorItems2['302'] = i
  w.create(TkcText,319.5,261, 'text'=>'302', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  i = TkcPolygon.new(w,70,321,242,321,242,200,259,200,259,203,272,203,
                     272,193,263,180,242,180,175,180,175,169,156,169,
                     156,196,177,196,177,228,107,228,70,228,70,275,107,275,
                     107,248,160,248,160,301,107,301,107,275,70,275,
                     'fill'=>'', 'tags'=>['floor3','room'])
  $floorLabels2[i.id] = '306'
  $floorItems2['306'] = i
  w.create(TkcText,200.5,284.5, 'text'=>'306', 'fill'=>color,
              'anchor'=>'c', 'tags'=>['floor3','label'])
  w.create(TkcLine,341,275,341,283, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,162,197,155,197, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,396,247,399,247, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,399,129,399,311, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,258,202,243,202, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,350,283,350,329, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,251,231,243,231, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,243,220,251,220, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,243,250,243,202, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,155,197,155,190, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,110,192,110,169, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,155,192,110,192, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,155,177,155,169, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,176,197,176,227, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,69,280,69,274, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,21,276,69,276, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,69,262,69,226, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,21,228,69,228, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,21,179,75,179, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,69,179,69,214, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,90,220,90,227, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,90,204,90,202, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,90,203,100,203, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,90,187,90,179, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,90,227,176,227, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,100,179,100,227, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,100,179,87,179, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,96,179,96,129, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,162,169,96,169, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,173,169,176,169, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,182,179,176,179, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,176,129,176,179, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,195,179,226,179, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,224,133,224,179, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,264,179,264,133, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,238,179,264,179, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,273,207,273,193, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,273,235,273,250, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,273,224,273,219, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,273,193,307,193, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,273,222,307,222, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,273,250,307,250, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,384,247,376,247, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,340,206,307,206, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,340,187,340,170, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,340,210,340,201, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,340,247,340,224, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,340,241,307,241, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,376,247,376,170, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,307,250,307,170, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,376,170,307,170, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,315,129,315,170, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,376,283,366,283, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,376,283,376,275, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,399,275,376,275, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,341,275,320,275, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,341,283,350,283, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,298,275,298,329, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,308,275,298,275, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,243,322,243,275, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,243,275,284,275, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,258,322,226,322, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,212,370,212,322, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,214,322,177,322, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,163,370,163,322, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,165,322,129,322, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,84,322,117,322, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,71,322,64,322, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,115,322,115,370, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,66,322,66,370, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,52,322,21,322, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,21,331,0,331, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,21,331,21,133, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,96,133,21,133, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,176,129,96,129, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,315,133,176,133, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,315,129,399,129, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,399,311,350,311, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,350,329,258,329, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,258,322,258,370, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,60,370,258,370, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,60,370,60,391, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,0,391,0,331, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,60,391,0,391, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,307,250,307,242, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,273,250,307,250, 'fill'=>color, 'tags'=>['floor3','wall'])
  w.create(TkcLine,258,250,243,250, 'fill'=>color, 'tags'=>['floor3','wall'])
end

# Below is the "main program" that creates the floorplan demonstration.

# toplevel widget
if defined?($floor2_demo) && $floor2_demo
  $floor2_demo.destroy
  $floor2_demo = nil
end

# demo toplevel widget
$floor2_demo = TkToplevel.new {|w|
  title("Floorplan Canvas Demonstration 2")
  iconname("Floorplan2")
  positionWindow(w)
  geometry('+20+20')
  minsize(100,100)
}

base_frame = TkFrame.new($floor2_demo).pack(:fill=>:both, :expand=>true)

# label
TkLabel.new(base_frame, 'font'=>$font, 'wraplength'=>'8i', 'justify'=>'left',
            'text'=>"This window contains a canvas widget showing the floorplan of Digital Equipment Corporation's Western Research Laboratory.  It has three levels.  At any given time one of the levels is active, meaning that you can see its room structure.  To activate a level, click the left mouse button anywhere on it.  As the mouse moves over the active level, the room under the mouse lights up and its room number appears in the \"Room:\" entry.  You can also type a room number in the entry and the room will light up."){
  pack('side'=>'top')
}

# frame
$floor2_buttons = TkFrame.new(base_frame) {|frame|
  TkButton.new(frame) {
    text 'Dismiss'
    command proc{
      tmppath = $floor2_demo
      $floor2_demo = nil
      tmppath.destroy
    }
  }.pack('side'=>'left', 'expand'=>'yes')

  TkButton.new(frame) {
    text 'Show Code'
    command proc{showCode 'floor2'}
  }.pack('side'=>'left', 'expand'=>'yes')
}
$floor2_buttons.pack('side'=>'bottom', 'fill'=>'x', 'pady'=>'2m')

#
$floorLabels2 = {}
$floorItems2 = {}

# canvas
if $tk_version =~ /^4\.[01]/
  $floor2_canvas_frame = TkFrame.new(base_frame,'bd'=>2,'relief'=>'sunken',
                                    'highlightthickness'=>2)
  $floor2_canvas = TkCanvas.new($floor2_canvas_frame,
                               'width'=>900, 'height'=>500, 'borderwidth'=>0,
                               'highlightthickness'=>0) {|c|
    TkScrollbar.new(base_frame, 'orient'=>'horiz',
                    'command'=>proc{|*args| c.xview(*args)}){|hs|
      c.xscrollcommand(proc{|first,last| hs.set first,last})
      pack('side'=>'bottom', 'fill'=>'x')
    }
    TkScrollbar.new(base_frame, 'command'=>proc{|*args| c.yview(*args)}){|vs|
      c.yscrollcommand(proc{|first,last| vs.set first,last})
      pack('side'=>'right', 'fill'=>'y')
    }
  }
  $floor2_canvas_frame.pack('side'=>'top','fill'=>'both', 'expand'=>'yes')
  $floor2_canvas.pack('expand'=>'yes', 'fill'=>'both')

else
  TkFrame.new(base_frame) {|f|
    pack('side'=>'top', 'fill'=>'both', 'expand'=>'yes')

    h = TkScrollbar.new(f, 'highlightthickness'=>0, 'orient'=>'horizontal')
    v = TkScrollbar.new(f, 'highlightthickness'=>0, 'orient'=>'vertical')

    TkFrame.new(f, 'bd'=>2, 'relief'=>'sunken') {|f1|
      $floor2_canvas = TkCanvas.new(f1, 'width'=>900, 'height'=>500,
                                   'borderwidth'=>0,
                                   'highlightthickness'=>0) {
        xscrollcommand(proc{|first,last| h.set first,last})
        yscrollcommand(proc{|first,last| v.set first,last})
        pack('expand'=>'yes', 'fill'=>'both')
      }
      grid('padx'=>1, 'pady'=>1, 'row'=>0, 'column'=>0,
           'rowspan'=>1, 'columnspan'=>1, 'sticky'=>'news')
    }

    v.grid('padx'=>1, 'pady'=>1, 'row'=>0, 'column'=>1,
           'rowspan'=>1, 'columnspan'=>1, 'sticky'=>'news')
    h.grid('padx'=>1, 'pady'=>1, 'row'=>1, 'column'=>0,
           'rowspan'=>1, 'columnspan'=>1, 'sticky'=>'news')

    TkGrid.rowconfigure(f, 0, 'weight'=>1, 'minsize'=>0)
    TkGrid.columnconfigure(f, 0, 'weight'=>1, 'minsize'=>0)

    pack('expand'=>'yes', 'fill'=>'both', 'padx'=>1, 'pady'=>1)

    v.command(proc{|*args| $floor2_canvas.yview(*args)})
    h.command(proc{|*args| $floor2_canvas.xview(*args)})
  }
end


# Create an entry for displaying and typing in current room.

$currentRoom2 = TkVariable.new
$floor2_entry = TkEntry.new($floor2_canvas, 'width'=>10, 'relief'=>'sunken',
                           'bd'=>2, 'textvariable'=>$currentRoom2)

# Choose colors, then fill in the floorplan.

$floor2_colors = {}
if TkWinfo.depth($floor2_canvas) > 1
  $floor2_colors['bg1'] = '#a9c1da'
  $floor2_colors['outline1'] = '#77889a'
  $floor2_colors['bg2'] = '#9ab0c6'
  $floor2_colors['outline2'] = '#687786'
  $floor2_colors['bg3'] = '#8ba0b3'
  $floor2_colors['outline3'] = '#596673'
  $floor2_colors['offices'] = 'Black'
  $floor2_colors['active'] = '#c4d1df'
else
  $floor2_colors['bg1'] = 'white'
  $floor2_colors['outline1'] = 'black'
  $floor2_colors['bg2'] = 'white'
  $floor2_colors['outline2'] = 'black'
  $floor2_colors['bg3'] = 'white'
  $floor2_colors['outline3'] = 'black'
  $floor2_colors['offices'] = 'Black'
  $floor2_colors['active'] = 'black'
end

$activeFloor2 = ''
floorDisplay2 $floor2_canvas,3

# Set up event bindings for canvas:

$floor2_canvas.itembind('floor1', '1', proc{floorDisplay2 $floor2_canvas,1})
$floor2_canvas.itembind('floor2', '1', proc{floorDisplay2 $floor2_canvas,2})
$floor2_canvas.itembind('floor3', '1', proc{floorDisplay2 $floor2_canvas,3})
$floor2_canvas.itembind('room', 'Enter', proc{newRoom2 $floor2_canvas})
$floor2_canvas.itembind('room', 'Leave', proc{$currentRoom2.value = ''})
$floor2_canvas.bind('2', proc{|x,y| $floor2_canvas.scan_mark x,y}, '%x %y')
$floor2_canvas.bind('B2-Motion',
                   proc{|x,y| $floor2_canvas.scan_dragto x,y}, '%x %y')
$floor2_canvas.bind('Destroy', proc{$currentRoom2.unset})
$currentRoom2.value = ''
$currentRoom2.trace('w',proc{roomChanged2 $floor2_canvas})
