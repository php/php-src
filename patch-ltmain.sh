--- /usr/local/share/libtool/ltmain.sh	Wed Jul 14 14:46:28 1999
+++ ltmain.sh	Tue Oct  5 19:01:41 1999
@@ -1955,6 +1955,7 @@
 		    done
 	      done
 	      if test -n "$a_deplib" ; then
+	      newdeplibs="$newdeplibs $a_deplib"
 		droppeddeps=yes
 		echo
 		echo "*** Warning: This library needs some functionality provided by $a_deplib."
