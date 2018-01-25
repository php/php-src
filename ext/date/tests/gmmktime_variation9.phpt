--TEST--
Test gmmktime() function : usage variation - Passing positive and negative float values to arguments.
--FILE--
<?php
/* Prototype  : int gmmktime([int hour [, int min [, int sec [, int mon [, int day [, int year]]]]]])
 * Description: Get UNIX timestamp for a GMT date
 * Source code: ext/date/php_date.c
 * Alias to functions:
 */

echo "*** Testing gmmktime() : usage variation ***\n";

//Initialise variables
$hour = 8;
$min = 8;
$sec = 8;
$mon = 8;
$day = 8;
$year = 2008;

$inputs = array(

	  'float 123456' => 123456,
      'float -123456' => -123456,
      'float -10.5' => -10.5,
);

// loop through each element of the array for min
foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
	  var_dump( gmmktime($value, $min, $sec, $mon, $day, $year) );
	  var_dump( gmmktime($hour, $value, $sec, $mon, $day, $year) );
	  var_dump( gmmktime($hour, $min, $value, $mon, $day, $year) );
	  var_dump( gmmktime($hour, $min, $sec, $value, $day, $year) );
	  var_dump( gmmktime($hour, $min, $sec, $mon, $value, $value) );
}
?>
===DONE===
--EXPECTREGEX--
\*\*\* Testing gmmktime\(\) : usage variation \*\*\*

--float 123456--
int\(1662595688\)
int\(1225589768\)
int\(1218306336\)
(bool|int)\((false|325855037288)\)
(bool|int)\((false|3844412784488)\)

--float -123456--
int\(773712488\)
int\(1210775048\)
int\(1218059424\)
(bool|int)\((false|-323460834712)\)
(bool|int)\((false|-3968710530712)\)

--float -10.5--
int\(1218118088\)
int\(1218181808\)
int\(1218182870\)
int\(1170922088\)
(bool|int)\((false|-62465356312)\)
===DONE===
