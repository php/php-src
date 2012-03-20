#
#  This Tcl/Tk script is quoted from <http://wiki.tcl.tk/10615>.
#
 package require Tk
  namespace eval ::tktree {

    # Images used for open and close state of subgroups
    set ::tktree::imgcollapse [image create photo .tktreeopenbm -data {
      R0lGODdhCQAJAIAAAAEBAf///ywAAAAACQAJAAACEISPoRvG614D80x5ZXyogwIAOw==}]
    set ::tktree::imgexpand [image create photo .tktreeclosebm -data {
      R0lGODdhCQAJAIAAAAEBAf///ywAAAAACQAJAAACEYSPoRu28KCSDSJLc44s3lMAADs=}]
    ###Default images for groups and children
    set ::tktree::imgsubgroups [image create photo .tktreeimgfolder -data {
                R0lGODlhEAANAKIAANnZ2Xh4eLi4uPj4APj4+AAAAP///////yH5BAEAAAAA
                LAAAAAAQAA0AAANkCIChiqDLITgyEgi6GoIjIyMYugCBpMsaWBA0giMjIzgy
                UYBBMjIoIyODEgVBODIygiMjE1gQJIMyMjIoI1GAQSMjODIyghMFQSgjI4My
                MhJYEDSCIyMjODJRgKHLXAiApcsMmAA7}]
    set ::tktree::imgchildren [image create photo .tktreeimgfile -data {
                R0lGODlhDQAQAKIAANnZ2Xh4ePj4+Li4uAAAAP///////////yH5BAEAAAAA
                LAAAAAANABAAAANSGLoLgACBoqsRCBAoujqCASGDojtESCEihCREIjgUKLo8
                hCGCpCsySIGiy0MYIki6IoMUKLo8hCGCpCsySIGiy0MYKLo8hIGiy0MYOLo8
                SLrMCQA7}]

    #### Swtich all subgroups of a layer to open or close
    proc ::tktree::switchlayer {win opn {layer /}} {
      variable cfg
      foreach child $cfg($win,$layer:subgroups) {
        set cfg($win,$child:open) $opn
        switchlayer $win $opn $child
      }
      buildwhenidle $win
    }

    ####  will open or close the item given
    proc ::tktree::switchstate {win item} {
      set ::tktree::cfg($win,$item:open) [expr ! $::tktree::cfg($win,$item:open)]
      buildwhenidle $win
    }

    #### Select the next item up or down
    proc ::tktree::updown {win down} {
      variable cfg
      set index [lsearch -exact $cfg($win,sortlist) $cfg($win,selection)]
      if {$down} {incr index} {incr index -1}
      if {$index < 0} {set index end} elseif {$index >= [llength $cfg($win,sortlist)]} {set index 0}
      setselection $win [lindex $cfg($win,sortlist) $index]
    }

    #### left-right button binding commands
    proc ::tktree::leftright {win right} {
      variable cfg
      set item $cfg($win,selection)
      set index [lsearch -exact $cfg($win,sortlist) $item]
      set parentindex [lsearch -exact $cfg($win,sortlist) [file dirname $item]]
      if {$parentindex == -1} {set parentindex [expr $index - 1]}
      if {$cfg($win,$item:group)} {
        if {$right} {
          if {$cfg($win,$item:open)} {incr index} {set cfg($win,$item:open) 1}
        } else {
          if {$cfg($win,$item:open)} {set cfg($win,$item:open) 0} {set index $parentindex}
        }
      } else {
        if {$right} {incr index} {set index $parentindex}
      }
      if {$index < 0} {set index end} elseif {$index >= [llength $cfg($win,sortlist)]} {set index 0}
      setselection $win [lindex $cfg($win,sortlist) $index]
      buildwhenidle $win
    }

    #### will return the pathname of the item at x and y cooridinates
    proc ::tktree::labelat {win x y} {
      set x [$win canvasx $x]; set y [$win canvasy $y]
      foreach m [$win find overlapping $x $y $x $y] {
        if {[info exists ::tktree::cfg($win,tag:$m)]} {return $::tktree::cfg($win,tag:$m)}
      }
      return ""
    }

    #### will return the path of the current selection in the given tree widget
    proc ::tktree::getselection {win} {
      return $::tktree::cfg($win,selection)
    }

    #### adjust the scrollview to show the selected item as needed
    proc ::tktree::scrolladjust {win tag} {
      update
      set item [$win bbox $tag]
      set region [$win cget -scrollregion]
      foreach {axis idx1 idx2} {yview 1 3 xview 0 2} {
        set range [expr abs([lindex $region $idx2]) - abs([lindex $region $idx1])]
        set itemtop [lindex $item $idx1];  set itembot [lindex $item $idx2]
        set viewtop [expr $range * [lindex [$win $axis] 0]]
        set viewbot [expr $range * [lindex [$win $axis] 1]]
        if {$itembot > $viewbot} {$win $axis moveto [expr ($itembot. - $viewbot + $viewtop) / $range]}
        if {$itemtop < $viewtop} {$win $axis moveto [expr $itemtop. / $range]}
      }
    }

    #### will set the current selection to the given item on the given tree
    proc ::tktree::setselection {win item} {
      variable cfg
      if {![llength $cfg($win,sortlist)]} {return}
      if {$item eq ""} {set item [lindex $cfg($win,sortlist) 0]}
      if {![info exists cfg($win,$item:tag)]} {set item [lindex $cfg($win,sortlist) 0]}
      if {[$win gettags $cfg($win,$item:tag)] ne ""} {
        $win select from $cfg($win,$item:tag) 0
        $win select to $cfg($win,$item:tag) end
        set cfg($win,selection) $item
        scrolladjust $win $cfg($win,$item:tag)
      } {
        setselection $win "/[lindex $cfg($win,/:sortlist) 0]"
      }
    }

    #### will delete the item given from the tree given
    proc ::tktree::delitem {win item} {
      variable cfg
      if {$item eq "/"} {
        array unset cfg $win,* ; catch {destroy $win}
      } {
        set group [file dirname $item]
        if {$cfg($win,$item:group)} {set type subgroups} {set type children}
        set index [lsearch -exact $cfg($win,$group:$type) $item]
        set cfg($win,$group:$type) [lreplace $cfg($win,$group:$type) $index $index]
        array unset cfg $win,$item*
        buildwhenidle $win
      }
    }

    #### create a new item in the tree and rebuild the widget
    proc ::tktree::newitem {win item args} {
      variable cfg
      if {[string index $item 0] ne "/"} {set item /$item}
      if {[string index $item end] eq "/"} {
        set subgroup 1
        set type subgroups
        set item [string range $item 0 end-1]
        set cfg($win,$item:command) [list ::tktree::switchstate $win $item]
      } {
        set subgroup 0
        set type children
        set cfg($win,$item:command) {}
      }
      #Build parent group if needed
      set group [file dirname $item]
      if {![info exists cfg($win,$group:open)]} {newitem $win "$group\/"}
      lappend cfg($win,$group:$type) $item
      #Configure the new item
      set cfg($win,$item:group) $subgroup
      set cfg($win,$item:subgroups) {}
      set cfg($win,$item:children) {}
      set cfg($win,$item:sortlist) {}
      set cfg($win,$item:tags) {}
      set cfg($win,$item:open) 0
      set cfg($win,$item:image) {}
      set cfg($win,$item:textcolor) $cfg($win,textcolor)
      set cfg($win,$item:font) $cfg($win,font)
      if {$cfg($win,images)} {set cfg($win,$item:image) [eval list \$::tktree::img$type]}
      foreach {confitem confval} $args {
        switch -exact -- $confitem {
          -textcolor  {set cfg($win,$item:textcolor) $confval}
          -command    {set cfg($win,$item:command)   $confval}
          -image      {set cfg($win,$item:image)     $confval}
          -font       {set cfg($win,$item:font)      $confval}
        }
      }
      buildwhenidle $win
    }

    #### Draw the given layer of the tree on the canvas starting at xposition
    proc ::tktree::buildlayer {win layer xpos} {
      variable cfg
      #Record y positions for vertical line later on
      set ystart $cfg($win,y); set yend $cfg($win,y)
      if {$layer eq "/"} {set cfg($win,sortlist) ""}
      foreach child $cfg($win,$layer:sortlist) {
        lappend cfg($win,sortlist) $child
        #Check spacing required for images
        set imgwidth 0; set imgheight 0
        if {[string length $cfg($win,$child:image)]} {
          set imgwidth [expr  ([image width $cfg($win,$child:image)] + 2) / 2]
          set imgheight [expr ([image height $cfg($win,$child:image)] + 2) / 2]
        }
        #find X-axis points for image, horiz line, and text
        if {$imgwidth} {
          set centerX [expr $imgwidth + $xpos + 7]
          set rightX  [expr $xpos + 7]
          set textX   [expr ($imgwidth * 2) + $xpos + 10]
        } {
          set centerX [expr $xpos + 10]
          set rightX  [expr $centerX + 4]
          set textX   [expr $rightX + 1]
        }
        #Find the proper amount to increment the y axis
        set fontheight [lindex [font metrics $cfg($win,$child:font)] 5]
        set yincr [expr ($fontheight + 1) / 2]
        if {$imgheight > $yincr} {set yincr $imgheight}
        incr cfg($win,y) $yincr
        #Draw the horizonal line
        $win create line $xpos $cfg($win,y) $rightX $cfg($win,y) -fill $cfg($win,linecolor)
        set yend $cfg($win,y)
        #Draw the image, if it exists
        if {$imgwidth} {
          set it [$win create image $centerX $cfg($win,y) -image $cfg($win,$child:image)]
          $win bind $it <1> [list ::tktree::setselection $win $child]
        }
        #Draw text and store tags for reference
        set cfg($win,$child:tag) [$win create text $textX $cfg($win,y) \
            -text [file tail $child] -font $cfg($win,$child:font) -anchor w -tags x -fill $cfg($win,$child:textcolor)]
        set cfg($win,tag:$cfg($win,$child:tag)) $child
        #Command binding
        $win bind $cfg($win,$child:tag) <1> [list ::tktree::setselection $win $child]
        $win bind $cfg($win,$child:tag) <Double-1> $cfg($win,$child:command)
        #next step up on the y axis
        incr cfg($win,y) $yincr
        #If its a group, add open-close functionality
        if {$cfg($win,$child:group)} {
          if {$cfg($win,$child:open)} {set img collapse} {set img expand}
          set ocimg [$win create image $xpos [expr $cfg($win,y) - $yincr] -image [eval list \$::tktree::img$img]]
          $win bind $ocimg <1> [list ::tktree::switchstate $win $child]
          if {$cfg($win,$child:open)} {buildlayer $win $child $centerX}
        }
      }
      #Vertical line
      $win lower [$win create line $xpos [expr $ystart - 7] $xpos $yend -fill $cfg($win,linecolor)]
    }

    #### sort the layer by subgroups then children
    proc ::tktree::sortlayer {win {layer /}} {
      variable cfg
      set cfg($win,$layer:subgroups) [lsort -dictionary $cfg($win,$layer:subgroups)]
      set cfg($win,$layer:children) [lsort -dictionary $cfg($win,$layer:children)]
      set cfg($win,$layer:sortlist) [join [list $cfg($win,$layer:subgroups) $cfg($win,$layer:children)]]
      foreach group $cfg($win,$layer:subgroups) {sortlayer $win $group}
    }

    #### build the tree at the given path
    proc ::tktree::buildtree {win} {
      variable cfg
      $win delete all
      sortlayer $win
      set xpos 5
      set cfg($win,y) 5
      #Draw global expand/contract button, if needed
      if {[string length $cfg($win,/:subgroups)] && $cfg($win,expandall)} {
        set exp 0
        foreach subgroup $cfg($win,/:subgroups) {incr exp $cfg($win,$subgroup:open)}
        if {$exp} {set type collapse} {set type expand}
        set ocimg [$win create image 1 1 -image [eval list \$::tktree::img$type] -anchor w]
        $win bind $ocimg <1> [list ::tktree::switchlayer $win [expr ! $exp]]
      }
      #Build the layers and set initial selection
      buildlayer $win / $xpos
      $win config -scrollregion [$win bbox all]
      setselection $win $cfg($win,selection)
    }

    #### internal use - set up a handle to build the tree when everything is idle
    proc ::tktree::buildwhenidle {win} {
      catch {after cancel $::tktree::cfg($win,buildHandle)}
      set ::tktree::cfg($win,buildHandle) [after idle [list ::tktree::buildtree $win]]
    }

    #### will create a new tree widget at the given path
    proc ::tktree::treecreate {win args} {
      variable cfg
      #Default configuration for new tree
      set cfg($win,selection) {}
      set cfg($win,selidx) {}
      set cfg($win,/:subgroups) {}
      set cfg($win,/:children) {}
      set cfg($win,/:open) 1
      set cfg($win,images) 1
      set cfg($win,expandall) 1
      set cfg($win,linecolor)  black
      set cfg($win,textcolor)  black
      set cfg($win,font) {-family Helvetica -size 10}
      #Parse and setup custom configuration options
      set canvascfg ""
      foreach {item val} $args {
        switch -- $item {
          -linecolor            {set cfg($win,linecolor) $val}
          -textcolor            {set cfg($win,textcolor) $val}
          -font                 {set cfg($win,font) $val}
          -images               {set cfg($win,images) $val}
          -expandall            {set cfg($win,expandall) $val}
          default               {lappend canvascfg $item $val}
        }
      }
      #Build the canvas
      eval {canvas $win -takefocus 1} $canvascfg
      bind $win <Destroy> [list ::tktree::delitem $win /]
      bind $win <1>  [list focus $win]
      bind $win <Return> {eval $::tktree::cfg(%W,[::tktree::getselection %W]:command)}
      bind $win <space> {eval $::tktree::cfg(%W,[::tktree::getselection %W]:command)}
      bind $win <Up>    [list ::tktree::updown $win 0]
      bind $win <Down>    [list ::tktree::updown $win 1]
      bind $win <Left>    [list ::tktree::leftright $win 0]
      bind $win <Right>    [list ::tktree::leftright $win 1]

      #Build the tree when idle
      buildwhenidle $win
    }
 }

