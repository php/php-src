--TEST--
Test strftime() function : usage variation - Checking time related formats which are not supported on Windows.
--SKIPIF--
<?php
if (strtoupper(substr(PHP_OS, 0, 3)) != 'WIN') {
    die("skip Test is valid for Windows");
}
?>
--FILE--
<?php
/* Prototype  : string strftime(string format [, int timestamp])
 * Description: Format a local time/date according to locale settings 
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */

echo "*** Testing strftime() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
setlocale(LC_ALL, "en_US");
date_default_timezone_set("Asia/Calcutta");
$timestamp = mktime(8, 8, 8, 8, 8, 2008);

//array of values to iterate over
$inputs = array(
	  'Time in a.m/p.m notation' => "%r",
	  'Time in 24 hour notation' => "%R",
	  'Current time %H:%M:%S format' => "%T",
);

// loop through each element of the array for timestamp

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
	  var_dump( strftime($value) );
	  var_dump( strftime($value, $timestamp) );
}	  

?>
===DONE===
--EXPECTF--
*** Testing strftime() : usage variation ***

--Time in a.m/p.m notation--
bool(false)
bool(false)

--Time in 24 hour notation--
bool(false)
bool(false)

--Current time %H:%M:%S format--
bool(false)
bool(false)
===DONE===
