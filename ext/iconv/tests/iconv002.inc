<?php
/*
Expected output:
&#97;&#98;&#99;&#100;
abcd
*/

   $s = unpack("V*", iconv("ascii","UCS-4LE", "abcd"));
   foreach($s as $c) { print "&#$c;"; } print "\n";

   $s = pack("NNNN", 97, 98, 99, 100);
   $q = iconv("UCS-4BE", "ascii", $s);
   print $q; print "\n";
?>
