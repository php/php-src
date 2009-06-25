--TEST--
Test strftime() function : usage variation - Checking Preferred date and time representation other than on Windows.
--SKIPIF--
<?php
if (strtoupper(substr(PHP_OS, 0, 3)) == 'WIN') {
    die("skip Test is not valid for Windows");
}
if(!setlocale(LC_ALL, "en_US.utf8", "en_US")) {
	die("skip Locale en_US.utf8 or en_US is  needed by test and is not available");
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
setlocale(LC_ALL, "en_US.utf8", "en_US");
date_default_timezone_set("Asia/Calcutta");
$timestamp = mktime(8, 8, 8, 8, 8, 2008);

//array of values to iterate over
$inputs = array(
      'Preferred date and time representation' => "%c",
	  'Preferred date representation' => "%x",
	  'Preferred time representation' => "%X",
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

--Preferred date and time representation--
string(%d) "%s %d %s %d %d:%d:%d %s %s"
string(31) "Fri 08 Aug 2008 08:08:08 AM IST"

--Preferred date representation--
string(%d) "%d/%d/%d"
string(10) "08/08/2008"

--Preferred time representation--
string(%d) "%d:%d:%d %s"
string(11) "08:08:08 AM"
===DONE===
