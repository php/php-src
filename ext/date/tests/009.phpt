--TEST--
strftime() and gmstrftime() tests
--INI--
date.timezone=Asia/Jerusalem
--FILE--
<?php

$t = mktime(0,0,0, 6, 27, 2006);

var_dump(strftime());

var_dump(strftime(""));
var_dump(strftime("%a %A %b %B %c %C %d %D %e %g %G %h %H %I %j %m %M %n %p %r %R %S %t %T %u %U %V %W %w %x %X %y %Y %Z %z %%", $t));
var_dump(strftime("%%q %%a", $t));
var_dump(strftime("%q", $t));
var_dump(strftime("blah", $t));

var_dump(gmstrftime());

var_dump(gmstrftime(""));
var_dump(gmstrftime("%a %A %b %B %c %C %d %D %e %g %G %h %H %I %j %m %M %n %p %r %R %S %t %T %u %U %V %W %w %x %X %y %Y %Z %z %%", $t));
var_dump(gmstrftime("%%q %%a", $t));
var_dump(gmstrftime("%q", $t));
var_dump(gmstrftime("blah", $t));

echo "Done\n";
?>
--EXPECTF--	
Warning: strftime() expects at least 1 parameter, 0 given in %s on line %d
bool(false)
bool(false)
string(179) "Tue Tuesday Jun June Tue Jun 27 00:00:00 2006 20 27 06/27/06 27 06 2006 Jun 00 12 178 06 00 
 AM 12:00:00 AM 00:00 00 	 00:00:00 2 26 26 26 2 06/27/06 00:00:00 06 2006 IDT +0300 %"
string(5) "%q %a"
string(1) "q"
string(4) "blah"

Warning: gmstrftime() expects at least 1 parameter, 0 given in %s on line %d
bool(false)
bool(false)
string(178) "Mon Monday Jun June Mon Jun 26 21:00:00 2006 20 26 06/26/06 26 06 2006 Jun 21 09 177 06 00 
 PM 09:00:00 PM 21:00 00 	 21:00:00 1 26 26 26 1 06/26/06 21:00:00 06 2006 GMT +0000 %"
string(5) "%q %a"
string(1) "q"
string(4) "blah"
Done
--UEXPECTF--
Warning: strftime() expects at least 1 parameter, 0 given in %s on line %d
bool(false)
bool(false)
unicode(179) "Tue Tuesday Jun June Tue Jun 27 00:00:00 2006 20 27 06/27/06 27 06 2006 Jun 00 12 178 06 00 
 AM 12:00:00 AM 00:00 00 	 00:00:00 2 26 26 26 2 06/27/06 00:00:00 06 2006 IDT +0300 %"
unicode(5) "%q %a"
unicode(1) "q"
unicode(4) "blah"

Warning: gmstrftime() expects at least 1 parameter, 0 given in %s on line %d
bool(false)
bool(false)
unicode(178) "Mon Monday Jun June Mon Jun 26 21:00:00 2006 20 26 06/26/06 26 06 2006 Jun 21 09 177 06 00 
 PM 09:00:00 PM 21:00 00 	 21:00:00 1 26 26 26 1 06/26/06 21:00:00 06 2006 GMT +0000 %"
unicode(5) "%q %a"
unicode(1) "q"
unicode(4) "blah"
Done
