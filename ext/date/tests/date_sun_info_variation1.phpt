--TEST--
Test date_sun_info() function : error variations - Pass unexpected values for time argument
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
       12.3456789000e5,
       12.3456789000E-5,
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
    var_dump(date_sun_info($input,  31.7667, 35.2333));
    $iterator++;
};

?>
===Done===
--EXPECTF--
*** Testing date_sun_info() : usage variations ***

-- Iteration 1 --
array(9) {
  ["sunrise"]=>
  int(16748)
  ["sunset"]=>
  int(53182)
  ["transit"]=>
  int(34965)
  ["civil_twilight_begin"]=>
  int(15145)
  ["civil_twilight_end"]=>
  int(54786)
  ["nautical_twilight_begin"]=>
  int(13324)
  ["nautical_twilight_end"]=>
  int(56607)
  ["astronomical_twilight_begin"]=>
  int(11542)
  ["astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 2 --
array(9) {
  ["sunrise"]=>
  int(16748)
  ["sunset"]=>
  int(53182)
  ["transit"]=>
  int(34965)
  ["civil_twilight_begin"]=>
  int(15145)
  ["civil_twilight_end"]=>
  int(54786)
  ["nautical_twilight_begin"]=>
  int(13324)
  ["nautical_twilight_end"]=>
  int(56607)
  ["astronomical_twilight_begin"]=>
  int(11542)
  ["astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 3 --
array(9) {
  ["sunrise"]=>
  int(16748)
  ["sunset"]=>
  int(53182)
  ["transit"]=>
  int(34965)
  ["civil_twilight_begin"]=>
  int(15145)
  ["civil_twilight_end"]=>
  int(54786)
  ["nautical_twilight_begin"]=>
  int(13324)
  ["nautical_twilight_end"]=>
  int(56607)
  ["astronomical_twilight_begin"]=>
  int(11542)
  ["astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 4 --
array(9) {
  ["sunrise"]=>
  int(-69665)
  ["sunset"]=>
  int(-33260)
  ["transit"]=>
  int(-51462)
  ["civil_twilight_begin"]=>
  int(-71269)
  ["civil_twilight_end"]=>
  int(-31655)
  ["nautical_twilight_begin"]=>
  int(-73092)
  ["nautical_twilight_end"]=>
  int(-29832)
  ["astronomical_twilight_begin"]=>
  int(-74874)
  ["astronomical_twilight_end"]=>
  int(-28050)
}

-- Iteration 5 --
array(9) {
  ["sunrise"]=>
  int(16748)
  ["sunset"]=>
  int(53182)
  ["transit"]=>
  int(34965)
  ["civil_twilight_begin"]=>
  int(15145)
  ["civil_twilight_end"]=>
  int(54786)
  ["nautical_twilight_begin"]=>
  int(13324)
  ["nautical_twilight_end"]=>
  int(56607)
  ["astronomical_twilight_begin"]=>
  int(11542)
  ["astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 6 --
array(9) {
  ["sunrise"]=>
  int(-69665)
  ["sunset"]=>
  int(-33260)
  ["transit"]=>
  int(-51462)
  ["civil_twilight_begin"]=>
  int(-71269)
  ["civil_twilight_end"]=>
  int(-31655)
  ["nautical_twilight_begin"]=>
  int(-73092)
  ["nautical_twilight_end"]=>
  int(-29832)
  ["astronomical_twilight_begin"]=>
  int(-74874)
  ["astronomical_twilight_end"]=>
  int(-28050)
}

-- Iteration 7 --
array(9) {
  ["sunrise"]=>
  int(1226363)
  ["sunset"]=>
  int(1263468)
  ["transit"]=>
  int(1244916)
  ["civil_twilight_begin"]=>
  int(1224788)
  ["civil_twilight_end"]=>
  int(1265044)
  ["nautical_twilight_begin"]=>
  int(1222993)
  ["nautical_twilight_end"]=>
  int(1266839)
  ["astronomical_twilight_begin"]=>
  int(1221233)
  ["astronomical_twilight_end"]=>
  int(1268599)
}

-- Iteration 8 --
array(9) {
  ["sunrise"]=>
  int(16748)
  ["sunset"]=>
  int(53182)
  ["transit"]=>
  int(34965)
  ["civil_twilight_begin"]=>
  int(15145)
  ["civil_twilight_end"]=>
  int(54786)
  ["nautical_twilight_begin"]=>
  int(13324)
  ["nautical_twilight_end"]=>
  int(56607)
  ["astronomical_twilight_begin"]=>
  int(11542)
  ["astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 9 --
array(9) {
  ["sunrise"]=>
  int(16748)
  ["sunset"]=>
  int(53182)
  ["transit"]=>
  int(34965)
  ["civil_twilight_begin"]=>
  int(15145)
  ["civil_twilight_end"]=>
  int(54786)
  ["nautical_twilight_begin"]=>
  int(13324)
  ["nautical_twilight_end"]=>
  int(56607)
  ["astronomical_twilight_begin"]=>
  int(11542)
  ["astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 10 --
array(9) {
  ["sunrise"]=>
  int(16748)
  ["sunset"]=>
  int(53182)
  ["transit"]=>
  int(34965)
  ["civil_twilight_begin"]=>
  int(15145)
  ["civil_twilight_end"]=>
  int(54786)
  ["nautical_twilight_begin"]=>
  int(13324)
  ["nautical_twilight_end"]=>
  int(56607)
  ["astronomical_twilight_begin"]=>
  int(11542)
  ["astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 11 --
array(9) {
  ["sunrise"]=>
  int(16748)
  ["sunset"]=>
  int(53182)
  ["transit"]=>
  int(34965)
  ["civil_twilight_begin"]=>
  int(15145)
  ["civil_twilight_end"]=>
  int(54786)
  ["nautical_twilight_begin"]=>
  int(13324)
  ["nautical_twilight_end"]=>
  int(56607)
  ["astronomical_twilight_begin"]=>
  int(11542)
  ["astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 12 --
array(9) {
  ["sunrise"]=>
  int(16748)
  ["sunset"]=>
  int(53182)
  ["transit"]=>
  int(34965)
  ["civil_twilight_begin"]=>
  int(15145)
  ["civil_twilight_end"]=>
  int(54786)
  ["nautical_twilight_begin"]=>
  int(13324)
  ["nautical_twilight_end"]=>
  int(56607)
  ["astronomical_twilight_begin"]=>
  int(11542)
  ["astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 13 --
array(9) {
  ["sunrise"]=>
  int(16748)
  ["sunset"]=>
  int(53182)
  ["transit"]=>
  int(34965)
  ["civil_twilight_begin"]=>
  int(15145)
  ["civil_twilight_end"]=>
  int(54786)
  ["nautical_twilight_begin"]=>
  int(13324)
  ["nautical_twilight_end"]=>
  int(56607)
  ["astronomical_twilight_begin"]=>
  int(11542)
  ["astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 14 --
array(9) {
  ["sunrise"]=>
  int(16748)
  ["sunset"]=>
  int(53182)
  ["transit"]=>
  int(34965)
  ["civil_twilight_begin"]=>
  int(15145)
  ["civil_twilight_end"]=>
  int(54786)
  ["nautical_twilight_begin"]=>
  int(13324)
  ["nautical_twilight_end"]=>
  int(56607)
  ["astronomical_twilight_begin"]=>
  int(11542)
  ["astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 15 --
array(9) {
  ["sunrise"]=>
  int(16748)
  ["sunset"]=>
  int(53182)
  ["transit"]=>
  int(34965)
  ["civil_twilight_begin"]=>
  int(15145)
  ["civil_twilight_end"]=>
  int(54786)
  ["nautical_twilight_begin"]=>
  int(13324)
  ["nautical_twilight_end"]=>
  int(56607)
  ["astronomical_twilight_begin"]=>
  int(11542)
  ["astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 16 --

Warning: date_sun_info() expects parameter 1 to be long, string given in %s on line %d
bool(false)

-- Iteration 17 --

Warning: date_sun_info() expects parameter 1 to be long, string given in %s on line %d
bool(false)

-- Iteration 18 --

Warning: date_sun_info() expects parameter 1 to be long, array given in %s on line %d
bool(false)

-- Iteration 19 --

Warning: date_sun_info() expects parameter 1 to be long, string given in %s on line %d
bool(false)

-- Iteration 20 --

Warning: date_sun_info() expects parameter 1 to be long, string given in %s on line %d
bool(false)

-- Iteration 21 --

Warning: date_sun_info() expects parameter 1 to be long, string given in %s on line %d
bool(false)

-- Iteration 22 --
array(9) {
  ["sunrise"]=>
  int(16748)
  ["sunset"]=>
  int(53182)
  ["transit"]=>
  int(34965)
  ["civil_twilight_begin"]=>
  int(15145)
  ["civil_twilight_end"]=>
  int(54786)
  ["nautical_twilight_begin"]=>
  int(13324)
  ["nautical_twilight_end"]=>
  int(56607)
  ["astronomical_twilight_begin"]=>
  int(11542)
  ["astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 23 --
array(9) {
  ["sunrise"]=>
  int(16748)
  ["sunset"]=>
  int(53182)
  ["transit"]=>
  int(34965)
  ["civil_twilight_begin"]=>
  int(15145)
  ["civil_twilight_end"]=>
  int(54786)
  ["nautical_twilight_begin"]=>
  int(13324)
  ["nautical_twilight_end"]=>
  int(56607)
  ["astronomical_twilight_begin"]=>
  int(11542)
  ["astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 24 --

Warning: date_sun_info() expects parameter 1 to be long, resource given in %s on line %d
bool(false)
===Done===