--TEST--
Test strptime() function : basic functionality
--SKIPIF--
<?php
	if (!function_exists('strptime')) {
		die("skip - strptime() function not available in this build");
	}
        if(PHP_OS == 'Darwin') die("skip - strptime() behaves differently on Darwin");
?>
--FILE--
<?php
/* Prototype  : array strptime  ( string $date  , string $format  )
 * Description:  Parse a time/date generated with strftime()
 * Source code: ext/standard/datetime.c
*/

$orig = setlocale(LC_ALL, 'C');
date_default_timezone_set("GMT");

echo "*** Testing strptime() : basic functionality ***\n";

$input = "10:00:00 AM July 2 1963";
$tstamp = strtotime($input);

$str = strftime("%r %B%e %Y %Z", $tstamp);
var_dump(strptime($str, '%H:%M:%S %p %B %d %Y'));

$str = strftime("%T %D", $tstamp);
var_dump(strptime($str, '%H:%M:%S %m/%d/%y'));

$str = strftime("%A %B %e %R", $tstamp);
var_dump(strptime($str, '%A %B %e %R'));

setlocale(LC_ALL, $orig);
?>
===DONE===
--EXPECT--
*** Testing strptime() : basic functionality ***
array(9) {
  ["tm_sec"]=>
  int(0)
  ["tm_min"]=>
  int(0)
  ["tm_hour"]=>
  int(10)
  ["tm_mday"]=>
  int(2)
  ["tm_mon"]=>
  int(6)
  ["tm_year"]=>
  int(63)
  ["tm_wday"]=>
  int(2)
  ["tm_yday"]=>
  int(182)
  ["unparsed"]=>
  string(4) " GMT"
}
array(9) {
  ["tm_sec"]=>
  int(0)
  ["tm_min"]=>
  int(0)
  ["tm_hour"]=>
  int(10)
  ["tm_mday"]=>
  int(2)
  ["tm_mon"]=>
  int(6)
  ["tm_year"]=>
  int(163)
  ["tm_wday"]=>
  int(1)
  ["tm_yday"]=>
  int(182)
  ["unparsed"]=>
  string(0) ""
}
array(9) {
  ["tm_sec"]=>
  int(0)
  ["tm_min"]=>
  int(0)
  ["tm_hour"]=>
  int(10)
  ["tm_mday"]=>
  int(2)
  ["tm_mon"]=>
  int(6)
  ["tm_year"]=>
  int(0)
  ["tm_wday"]=>
  int(2)
  ["tm_yday"]=>
  int(182)
  ["unparsed"]=>
  string(0) ""
}
===DONE===
