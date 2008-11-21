--TEST--
Test gmmktime() function : usage variation - Passing positive and negetive float values to arguments.
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

	  'float 12.3456789000e10' => 12.3456789000e10,
      'float -12.3456789000e10' => -12.3456789000e10,
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

--float 12.3456789000e10--
(bool|int)\((false|444445658554088)\)
(bool|int)\((false|7408625522408)\)
(bool|int)\((false|124674971880)\)
(bool|int)\((false|324659998242749288)\)
(bool|int)\((false|3906586568967811688)\)

--float -12.3456789000e10--
(bool|int)\((false|-444443222245912)\)
(bool|int)\((false|-7406189157592)\)
int\((-929300768|-122238606120)\)
(bool|int)\((false|-324659995848460312)\)
(bool|int)\((false|-3906586693265644312)\)

--float -10.5--
int\(1218118088\)
int\(1218181808\)
int\(1218182870\)
int\(1170922088\)
(bool|int)\((false|-62465356312)\)
===DONE===
