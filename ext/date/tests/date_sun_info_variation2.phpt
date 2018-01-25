--TEST--
Test date_sun_info() function : error variations - Pass unexpected values for latitude argument
--FILE--
<?php
/* Prototype  : array date_sun_info ( int $time , float $latitude , float $longitude )
 * Description:  Returns an array with information about sunset/sunrise and twilight begin/end.
 * Source code: ext/standard/data/php_date.c
 */

date_default_timezone_set("UTC");

echo "*** Testing date_sun_info() : usage variations ***\n";

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// heredoc string
$heredoc = <<<EOT
abc
xyz
EOT;

// get a resource variable
$fp = fopen(__FILE__, "r");

$inputs = array(
       // int data
/*1*/  0,
       1,
       12345,
       -2345,

       // float data
/*5*/  10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
       .5,

       // null data
/*10*/ NULL,
       null,

       // boolean data
/*12*/ true,
       false,
       TRUE,
       FALSE,

       // empty data
/*16*/ "",
       '',
       array(),

       // string data
/*19*/ "abcxyz",
       'abcxyz',
       $heredoc,

       // undefined data
/*22*/ @$undefined_var,

       // unset data
/*23*/ @$unset_var,

       // resource variable
/*24*/ $fp
);

// loop through each element of $inputs to check the behaviour of date_sun_info()
$iterator = 1;
foreach($inputs as $input) {
    echo "\n-- Iteration $iterator --\n";
    var_dump(date_sun_info(strtotime("2006-12-12"), $input, 35.2333));
    $iterator++;
};

?>
===Done===
--EXPECTF--
*** Testing date_sun_info() : usage variations ***

-- Iteration 1 --
array(9) {
  ["sunrise"]=>
  int(1165894138)
  ["sunset"]=>
  int(1165937784)
  ["transit"]=>
  int(1165915961)
  ["civil_twilight_begin"]=>
  int(1165892795)
  ["civil_twilight_end"]=>
  int(1165939127)
  ["nautical_twilight_begin"]=>
  int(1165891226)
  ["nautical_twilight_end"]=>
  int(1165940696)
  ["astronomical_twilight_begin"]=>
  int(1165889650)
  ["astronomical_twilight_end"]=>
  int(1165942271)
}

-- Iteration 2 --
array(9) {
  ["sunrise"]=>
  int(1165894240)
  ["sunset"]=>
  int(1165937681)
  ["transit"]=>
  int(1165915961)
  ["civil_twilight_begin"]=>
  int(1165892898)
  ["civil_twilight_end"]=>
  int(1165939024)
  ["nautical_twilight_begin"]=>
  int(1165891330)
  ["nautical_twilight_end"]=>
  int(1165940591)
  ["astronomical_twilight_begin"]=>
  int(1165889758)
  ["astronomical_twilight_end"]=>
  int(1165942164)
}

-- Iteration 3 --
array(9) {
  ["sunrise"]=>
  bool(true)
  ["sunset"]=>
  bool(true)
  ["transit"]=>
  int(1165915961)
  ["civil_twilight_begin"]=>
  bool(true)
  ["civil_twilight_end"]=>
  bool(true)
  ["nautical_twilight_begin"]=>
  int(1165883368)
  ["nautical_twilight_end"]=>
  int(1165948554)
  ["astronomical_twilight_begin"]=>
  int(1165890281)
  ["astronomical_twilight_end"]=>
  int(1165941641)
}

-- Iteration 4 --
array(9) {
  ["sunrise"]=>
  int(1165894072)
  ["sunset"]=>
  int(1165937850)
  ["transit"]=>
  int(1165915961)
  ["civil_twilight_begin"]=>
  int(1165895418)
  ["civil_twilight_end"]=>
  int(1165936504)
  ["nautical_twilight_begin"]=>
  int(1165896984)
  ["nautical_twilight_end"]=>
  int(1165934938)
  ["astronomical_twilight_begin"]=>
  int(1165898549)
  ["astronomical_twilight_end"]=>
  int(1165933372)
}

-- Iteration 5 --
array(9) {
  ["sunrise"]=>
  int(1165895221)
  ["sunset"]=>
  int(1165936701)
  ["transit"]=>
  int(1165915961)
  ["civil_twilight_begin"]=>
  int(1165893858)
  ["civil_twilight_end"]=>
  int(1165938064)
  ["nautical_twilight_begin"]=>
  int(1165892278)
  ["nautical_twilight_end"]=>
  int(1165939643)
  ["astronomical_twilight_begin"]=>
  int(1165890706)
  ["astronomical_twilight_end"]=>
  int(1165941215)
}

-- Iteration 6 --
array(9) {
  ["sunrise"]=>
  int(1165893046)
  ["sunset"]=>
  int(1165938875)
  ["transit"]=>
  int(1165915961)
  ["civil_twilight_begin"]=>
  int(1165891669)
  ["civil_twilight_end"]=>
  int(1165940253)
  ["nautical_twilight_begin"]=>
  int(1165890044)
  ["nautical_twilight_end"]=>
  int(1165941878)
  ["astronomical_twilight_begin"]=>
  int(1165888392)
  ["astronomical_twilight_end"]=>
  int(1165943530)
}

-- Iteration 7 --
array(9) {
  ["sunrise"]=>
  int(1165894138)
  ["sunset"]=>
  int(1165937784)
  ["transit"]=>
  int(1165915961)
  ["civil_twilight_begin"]=>
  int(1165892795)
  ["civil_twilight_end"]=>
  int(1165939127)
  ["nautical_twilight_begin"]=>
  int(1165891226)
  ["nautical_twilight_end"]=>
  int(1165940696)
  ["astronomical_twilight_begin"]=>
  int(1165889650)
  ["astronomical_twilight_end"]=>
  int(1165942271)
}

-- Iteration 8 --
array(9) {
  ["sunrise"]=>
  int(1165894138)
  ["sunset"]=>
  int(1165937784)
  ["transit"]=>
  int(1165915961)
  ["civil_twilight_begin"]=>
  int(1165892795)
  ["civil_twilight_end"]=>
  int(1165939127)
  ["nautical_twilight_begin"]=>
  int(1165891226)
  ["nautical_twilight_end"]=>
  int(1165940696)
  ["astronomical_twilight_begin"]=>
  int(1165889650)
  ["astronomical_twilight_end"]=>
  int(1165942271)
}

-- Iteration 9 --
array(9) {
  ["sunrise"]=>
  int(1165894189)
  ["sunset"]=>
  int(1165937733)
  ["transit"]=>
  int(1165915961)
  ["civil_twilight_begin"]=>
  int(1165892846)
  ["civil_twilight_end"]=>
  int(1165939075)
  ["nautical_twilight_begin"]=>
  int(1165891278)
  ["nautical_twilight_end"]=>
  int(1165940643)
  ["astronomical_twilight_begin"]=>
  int(1165889704)
  ["astronomical_twilight_end"]=>
  int(1165942217)
}

-- Iteration 10 --
array(9) {
  ["sunrise"]=>
  int(1165894138)
  ["sunset"]=>
  int(1165937784)
  ["transit"]=>
  int(1165915961)
  ["civil_twilight_begin"]=>
  int(1165892795)
  ["civil_twilight_end"]=>
  int(1165939127)
  ["nautical_twilight_begin"]=>
  int(1165891226)
  ["nautical_twilight_end"]=>
  int(1165940696)
  ["astronomical_twilight_begin"]=>
  int(1165889650)
  ["astronomical_twilight_end"]=>
  int(1165942271)
}

-- Iteration 11 --
array(9) {
  ["sunrise"]=>
  int(1165894138)
  ["sunset"]=>
  int(1165937784)
  ["transit"]=>
  int(1165915961)
  ["civil_twilight_begin"]=>
  int(1165892795)
  ["civil_twilight_end"]=>
  int(1165939127)
  ["nautical_twilight_begin"]=>
  int(1165891226)
  ["nautical_twilight_end"]=>
  int(1165940696)
  ["astronomical_twilight_begin"]=>
  int(1165889650)
  ["astronomical_twilight_end"]=>
  int(1165942271)
}

-- Iteration 12 --
array(9) {
  ["sunrise"]=>
  int(1165894240)
  ["sunset"]=>
  int(1165937681)
  ["transit"]=>
  int(1165915961)
  ["civil_twilight_begin"]=>
  int(1165892898)
  ["civil_twilight_end"]=>
  int(1165939024)
  ["nautical_twilight_begin"]=>
  int(1165891330)
  ["nautical_twilight_end"]=>
  int(1165940591)
  ["astronomical_twilight_begin"]=>
  int(1165889758)
  ["astronomical_twilight_end"]=>
  int(1165942164)
}

-- Iteration 13 --
array(9) {
  ["sunrise"]=>
  int(1165894138)
  ["sunset"]=>
  int(1165937784)
  ["transit"]=>
  int(1165915961)
  ["civil_twilight_begin"]=>
  int(1165892795)
  ["civil_twilight_end"]=>
  int(1165939127)
  ["nautical_twilight_begin"]=>
  int(1165891226)
  ["nautical_twilight_end"]=>
  int(1165940696)
  ["astronomical_twilight_begin"]=>
  int(1165889650)
  ["astronomical_twilight_end"]=>
  int(1165942271)
}

-- Iteration 14 --
array(9) {
  ["sunrise"]=>
  int(1165894240)
  ["sunset"]=>
  int(1165937681)
  ["transit"]=>
  int(1165915961)
  ["civil_twilight_begin"]=>
  int(1165892898)
  ["civil_twilight_end"]=>
  int(1165939024)
  ["nautical_twilight_begin"]=>
  int(1165891330)
  ["nautical_twilight_end"]=>
  int(1165940591)
  ["astronomical_twilight_begin"]=>
  int(1165889758)
  ["astronomical_twilight_end"]=>
  int(1165942164)
}

-- Iteration 15 --
array(9) {
  ["sunrise"]=>
  int(1165894138)
  ["sunset"]=>
  int(1165937784)
  ["transit"]=>
  int(1165915961)
  ["civil_twilight_begin"]=>
  int(1165892795)
  ["civil_twilight_end"]=>
  int(1165939127)
  ["nautical_twilight_begin"]=>
  int(1165891226)
  ["nautical_twilight_end"]=>
  int(1165940696)
  ["astronomical_twilight_begin"]=>
  int(1165889650)
  ["astronomical_twilight_end"]=>
  int(1165942271)
}

-- Iteration 16 --

Warning: date_sun_info() expects parameter 2 to be float, string given in %s on line %d
bool(false)

-- Iteration 17 --

Warning: date_sun_info() expects parameter 2 to be float, string given in %s on line %d
bool(false)

-- Iteration 18 --

Warning: date_sun_info() expects parameter 2 to be float, array given in %s on line %d
bool(false)

-- Iteration 19 --

Warning: date_sun_info() expects parameter 2 to be float, string given in %s on line %d
bool(false)

-- Iteration 20 --

Warning: date_sun_info() expects parameter 2 to be float, string given in %s on line %d
bool(false)

-- Iteration 21 --

Warning: date_sun_info() expects parameter 2 to be float, string given in %s on line %d
bool(false)

-- Iteration 22 --
array(9) {
  ["sunrise"]=>
  int(1165894138)
  ["sunset"]=>
  int(1165937784)
  ["transit"]=>
  int(1165915961)
  ["civil_twilight_begin"]=>
  int(1165892795)
  ["civil_twilight_end"]=>
  int(1165939127)
  ["nautical_twilight_begin"]=>
  int(1165891226)
  ["nautical_twilight_end"]=>
  int(1165940696)
  ["astronomical_twilight_begin"]=>
  int(1165889650)
  ["astronomical_twilight_end"]=>
  int(1165942271)
}

-- Iteration 23 --
array(9) {
  ["sunrise"]=>
  int(1165894138)
  ["sunset"]=>
  int(1165937784)
  ["transit"]=>
  int(1165915961)
  ["civil_twilight_begin"]=>
  int(1165892795)
  ["civil_twilight_end"]=>
  int(1165939127)
  ["nautical_twilight_begin"]=>
  int(1165891226)
  ["nautical_twilight_end"]=>
  int(1165940696)
  ["astronomical_twilight_begin"]=>
  int(1165889650)
  ["astronomical_twilight_end"]=>
  int(1165942271)
}

-- Iteration 24 --

Warning: date_sun_info() expects parameter 2 to be float, resource given in %s on line %d
bool(false)
===Done===
