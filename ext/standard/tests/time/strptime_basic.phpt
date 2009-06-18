--TEST--
Test strptime() function : basic functionality 
--SKIPIF--
<?php 
	if (!function_exists('strptime')) { 
		die("skip - strptime() function not available in this build"); 
	}	 
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
var_dump(strptime($str, '%H:%M:%S %p %B %d %Y %Z'));

$str = strftime("%T %D", $tstamp);
var_dump(strptime($str, '%H:%M:%S %m/%d/%y'));

$str = strftime("%A %B %e %R", $tstamp);
var_dump(strptime($str, '%A %B %e %R'));

setlocale(LC_ALL, $orig);
?>
===DONE===
--EXPECTF--
Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %s on line %d
*** Testing strptime() : basic functionality ***
array(9) {
  [u"tm_sec"]=>
  int(0)
  [u"tm_min"]=>
  int(0)
  [u"tm_hour"]=>
  int(10)
  [u"tm_mday"]=>
  int(2)
  [u"tm_mon"]=>
  int(6)
  [u"tm_year"]=>
  int(63)
  [u"tm_wday"]=>
  int(2)
  [u"tm_yday"]=>
  int(182)
  [u"unparsed"]=>
  unicode(3) "GMT"
}
array(9) {
  [u"tm_sec"]=>
  int(0)
  [u"tm_min"]=>
  int(0)
  [u"tm_hour"]=>
  int(10)
  [u"tm_mday"]=>
  int(2)
  [u"tm_mon"]=>
  int(6)
  [u"tm_year"]=>
  int(163)
  [u"tm_wday"]=>
  int(1)
  [u"tm_yday"]=>
  int(182)
  [u"unparsed"]=>
  unicode(0) ""
}
array(9) {
  [u"tm_sec"]=>
  int(0)
  [u"tm_min"]=>
  int(0)
  [u"tm_hour"]=>
  int(10)
  [u"tm_mday"]=>
  int(2)
  [u"tm_mon"]=>
  int(6)
  [u"tm_year"]=>
  int(0)
  [u"tm_wday"]=>
  int(2)
  [u"tm_yday"]=>
  int(182)
  [u"unparsed"]=>
  unicode(0) ""
}

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %s on line %d
===DONE===
