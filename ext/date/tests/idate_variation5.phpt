--TEST--
Test idate() function : usage variation - Passing supported Time format characters to format argument.
--FILE--
<?php
/* Prototype  : int idate(string format [, int timestamp])
 * Description: Format a local time/date as integer
 * Source code: ext/date/php_date.c
 * Alias to functions:
 */

echo "*** Testing idate() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
date_default_timezone_set("Asia/Calcutta");

//array of values to iterate over
$inputs = array(

      'Internet Time' => 'B',
	  '12 hour format' => 'h',
	  '24 hour format' => 'H',
	  'Minutes' => 'i',
	  'DST Activated' => 'I',
	  'Seconds' => 's',
	  'Seconds since Unix Epoch' => 'U',
	  'Time zone offset' => 'Z'
);

// loop through each element of the array for timestamp
foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( idate($value) );
};
?>
===DONE===
--EXPECTF--
*** Testing idate() : usage variation ***

--Internet Time--
int(%d)

--12 hour format--
int(%d)

--24 hour format--
int(%d)

--Minutes--
int(%d)

--DST Activated--
int(%d)

--Seconds--
int(%d)

--Seconds since Unix Epoch--
int(%d)

--Time zone offset--
int(%d)
===DONE===
