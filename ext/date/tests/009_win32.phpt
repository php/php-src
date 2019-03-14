--TEST--
strftime() and gmstrftime() tests
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die('skip only windows test.');
if (false === setlocale(LC_TIME, "en-us")) die("skip, couldn't set the locale to en-us");
?>
--FILE--
<?php
date_default_timezone_set('Asia/Jerusalem');

$loc = setlocale(LC_TIME, "0");
setlocale(LC_TIME, "en-us");

$t = mktime(0,0,0, 6, 27, 2006);

var_dump(strftime());

var_dump(strftime(""));

var_dump(strftime("%a %A %b %B %c %d %H %I %j %m %M %p %S %U %W %w %x %X %y %Y %Z %z %%", $t));

var_dump(strftime("%%q %%a", $t));

var_dump(strftime("blah", $t));

var_dump(gmstrftime());

var_dump(gmstrftime(""));

var_dump(gmstrftime("%a %A %b %B %c %d %H %I %j %m %M %p %S %U %W %w %x %X %y %Y %Z %z %%", $t));

var_dump(gmstrftime("%%q %%a", $t));

var_dump(gmstrftime("blah", $t));

echo "Done\n";

setlocale(LC_TIME, $loc);
?>
--EXPECTF--
Warning: strftime() expects at least 1 parameter, 0 given in %s on line %d
bool(false)
bool(false)
string(%d) "Tue Tuesday Jun June 6/27/2006 12:00:00 AM 27 00 12 178 06 00 AM 00 26 26 2 6/27/2006 12:00:00 AM 06 2006 %s"
string(5) "%q %a"
string(4) "blah"

Warning: gmstrftime() expects at least 1 parameter, 0 given in %s on line %d
bool(false)
bool(false)
string(%d) "Mon Monday Jun June 6/26/2006 9:00:00 PM 26 21 09 177 06 00 PM 00 26 26 1 6/26/2006 9:00:00 PM 06 2006 %s"
string(5) "%q %a"
string(4) "blah"
Done
