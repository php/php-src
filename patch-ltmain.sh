*** ltmain.sh.orig	Wed Oct  6 15:10:21 1999
--- ltmain.sh	Wed Oct  6 15:11:33 1999
***************
*** 1955,1960 ****
--- 1955,1961 ----
  		    done
  	      done
  	      if test -n "$a_deplib" ; then
+ 		newdeplibs="$newdeplibs $a_deplib"
  		droppeddeps=yes
  		echo
  		echo "*** Warning: This library needs some functionality provided by $a_deplib."
