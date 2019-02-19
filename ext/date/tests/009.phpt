--TEST--
strftime() and gmstrftime() tests
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') die('skip posix only test.');
if (!function_exists('strftime')) die("skip, strftime not available");
?>
--FILE--
<?php
date_default_timezone_set('Asia/Jerusalem');

$t = mktime(0,0,0, 6, 27, 2006);

var_dump(strftime(""));
var_dump(strftime("%a %A %b %B %c %C %d %D %e %g %G %h %H %I %j %m %M %n %p %r %R %S %t %T %u %U %V %W %w %x %X %y %Y %Z %z %%", $t));
var_dump(strftime("%%q %%a", $t));
var_dump(strftime("%q", $t));
var_dump(strftime("blah", $t));

var_dump(gmstrftime(""));
var_dump(gmstrftime("%a %A %b %B %c %C %d %D %e %g %G %h %H %I %j %m %M %n %p %r %R %S %t %T %u %U %V %W %w %x %X %y %Y %Z %z %%", $t));
var_dump(gmstrftime("%%q %%a", $t));
var_dump(gmstrftime("%q", $t));
var_dump(gmstrftime("blah", $t));

echo "Done\n";
?>
--EXPECTF--
bool(false)
string(%d) "Tue Tuesday Jun June Tue Jun 27 00:00:00 2006 %s
%s %"
string(5) "%q %a"
string(%d) "%s"
string(4) "blah"
bool(false)
string(%d) "Mon Monday Jun June Mon Jun 26 21:00:00 2006 %s
%s %"
string(5) "%q %a"
string(%d) "%s"
string(4) "blah"
Done
