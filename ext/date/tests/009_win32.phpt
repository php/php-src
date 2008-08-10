--TEST--
strftime() and gmstrftime() tests
--INI--
date.timezone=Asia/Jerusalem
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die('skip only windows test.');
if (!function_exists('strftime')) die("skip, strftime not available");
?>
--FILE--
<?php

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
?>
--EXPECTF--	
Warning: strftime() expects at least 1 parameter, 0 given in C:\Users\pierre\Documents\php-sdk\vc9\x86\php_5_3\ext\date\tests\009_win32.php on line 5
bool(false)
bool(false)
string(147) "Tue Tuesday Jun June 06/27/06 00:00:00 27 00 12 178 06 00 AM 00 26 26 2 06/27/06 00:00:00 06 2006 W. Europe Daylight Time W. Europe Daylight Time %"
string(5) "%q %a"
string(4) "blah"

Warning: gmstrftime() expects at least 1 parameter, 0 given in C:\Users\pierre\Documents\php-sdk\vc9\x86\php_5_3\ext\date\tests\009_win32.php on line 15
bool(false)
bool(false)
string(146) "Mon Monday Jun June 06/26/06 21:00:00 26 21 09 177 06 00 PM 00 26 26 1 06/26/06 21:00:00 06 2006 W. Europe Standard Time W. Europe Standard Time %"
string(5) "%q %a"
string(4) "blah"
Done
