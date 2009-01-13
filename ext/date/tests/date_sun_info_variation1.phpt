--TEST--
Test date_sun_info() function : error variations - Pass unexpected values for time argument
--INI--
date.timezone=UTC
--FILE--
<?php
/* Prototype  : array date_sun_info ( int $time , float $latitude , float $longitude )
 * Description:  Returns an array with information about sunset/sunrise and twilight begin/end.
 * Source code: ext/standard/data/php_date.c
 */

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
  [u"sunrise"]=>
  int(16748)
  [u"sunset"]=>
  int(53182)
  [u"transit"]=>
  int(34965)
  [u"civil_twilight_begin"]=>
  int(15145)
  [u"civil_twilight_end"]=>
  int(54786)
  [u"nautical_twilight_begin"]=>
  int(13324)
  [u"nautical_twilight_end"]=>
  int(56607)
  [u"astronomical_twilight_begin"]=>
  int(11542)
  [u"astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 2 --
array(9) {
  [u"sunrise"]=>
  int(16748)
  [u"sunset"]=>
  int(53182)
  [u"transit"]=>
  int(34965)
  [u"civil_twilight_begin"]=>
  int(15145)
  [u"civil_twilight_end"]=>
  int(54786)
  [u"nautical_twilight_begin"]=>
  int(13324)
  [u"nautical_twilight_end"]=>
  int(56607)
  [u"astronomical_twilight_begin"]=>
  int(11542)
  [u"astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 3 --
array(9) {
  [u"sunrise"]=>
  int(16748)
  [u"sunset"]=>
  int(53182)
  [u"transit"]=>
  int(34965)
  [u"civil_twilight_begin"]=>
  int(15145)
  [u"civil_twilight_end"]=>
  int(54786)
  [u"nautical_twilight_begin"]=>
  int(13324)
  [u"nautical_twilight_end"]=>
  int(56607)
  [u"astronomical_twilight_begin"]=>
  int(11542)
  [u"astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 4 --
array(9) {
  [u"sunrise"]=>
  int(-69665)
  [u"sunset"]=>
  int(-33260)
  [u"transit"]=>
  int(-51462)
  [u"civil_twilight_begin"]=>
  int(-71269)
  [u"civil_twilight_end"]=>
  int(-31655)
  [u"nautical_twilight_begin"]=>
  int(-73092)
  [u"nautical_twilight_end"]=>
  int(-29832)
  [u"astronomical_twilight_begin"]=>
  int(-74874)
  [u"astronomical_twilight_end"]=>
  int(-28050)
}

-- Iteration 5 --
array(9) {
  [u"sunrise"]=>
  int(16748)
  [u"sunset"]=>
  int(53182)
  [u"transit"]=>
  int(34965)
  [u"civil_twilight_begin"]=>
  int(15145)
  [u"civil_twilight_end"]=>
  int(54786)
  [u"nautical_twilight_begin"]=>
  int(13324)
  [u"nautical_twilight_end"]=>
  int(56607)
  [u"astronomical_twilight_begin"]=>
  int(11542)
  [u"astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 6 --
array(9) {
  [u"sunrise"]=>
  int(-69665)
  [u"sunset"]=>
  int(-33260)
  [u"transit"]=>
  int(-51462)
  [u"civil_twilight_begin"]=>
  int(-71269)
  [u"civil_twilight_end"]=>
  int(-31655)
  [u"nautical_twilight_begin"]=>
  int(-73092)
  [u"nautical_twilight_end"]=>
  int(-29832)
  [u"astronomical_twilight_begin"]=>
  int(-74874)
  [u"astronomical_twilight_end"]=>
  int(-28050)
}

-- Iteration 7 --
array(9) {
  [u"sunrise"]=>
  int(1226363)
  [u"sunset"]=>
  int(1263468)
  [u"transit"]=>
  int(1244916)
  [u"civil_twilight_begin"]=>
  int(1224788)
  [u"civil_twilight_end"]=>
  int(1265044)
  [u"nautical_twilight_begin"]=>
  int(1222993)
  [u"nautical_twilight_end"]=>
  int(1266839)
  [u"astronomical_twilight_begin"]=>
  int(1221233)
  [u"astronomical_twilight_end"]=>
  int(1268599)
}

-- Iteration 8 --
array(9) {
  [u"sunrise"]=>
  int(16748)
  [u"sunset"]=>
  int(53182)
  [u"transit"]=>
  int(34965)
  [u"civil_twilight_begin"]=>
  int(15145)
  [u"civil_twilight_end"]=>
  int(54786)
  [u"nautical_twilight_begin"]=>
  int(13324)
  [u"nautical_twilight_end"]=>
  int(56607)
  [u"astronomical_twilight_begin"]=>
  int(11542)
  [u"astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 9 --
array(9) {
  [u"sunrise"]=>
  int(16748)
  [u"sunset"]=>
  int(53182)
  [u"transit"]=>
  int(34965)
  [u"civil_twilight_begin"]=>
  int(15145)
  [u"civil_twilight_end"]=>
  int(54786)
  [u"nautical_twilight_begin"]=>
  int(13324)
  [u"nautical_twilight_end"]=>
  int(56607)
  [u"astronomical_twilight_begin"]=>
  int(11542)
  [u"astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 10 --
array(9) {
  [u"sunrise"]=>
  int(16748)
  [u"sunset"]=>
  int(53182)
  [u"transit"]=>
  int(34965)
  [u"civil_twilight_begin"]=>
  int(15145)
  [u"civil_twilight_end"]=>
  int(54786)
  [u"nautical_twilight_begin"]=>
  int(13324)
  [u"nautical_twilight_end"]=>
  int(56607)
  [u"astronomical_twilight_begin"]=>
  int(11542)
  [u"astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 11 --
array(9) {
  [u"sunrise"]=>
  int(16748)
  [u"sunset"]=>
  int(53182)
  [u"transit"]=>
  int(34965)
  [u"civil_twilight_begin"]=>
  int(15145)
  [u"civil_twilight_end"]=>
  int(54786)
  [u"nautical_twilight_begin"]=>
  int(13324)
  [u"nautical_twilight_end"]=>
  int(56607)
  [u"astronomical_twilight_begin"]=>
  int(11542)
  [u"astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 12 --
array(9) {
  [u"sunrise"]=>
  int(16748)
  [u"sunset"]=>
  int(53182)
  [u"transit"]=>
  int(34965)
  [u"civil_twilight_begin"]=>
  int(15145)
  [u"civil_twilight_end"]=>
  int(54786)
  [u"nautical_twilight_begin"]=>
  int(13324)
  [u"nautical_twilight_end"]=>
  int(56607)
  [u"astronomical_twilight_begin"]=>
  int(11542)
  [u"astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 13 --
array(9) {
  [u"sunrise"]=>
  int(16748)
  [u"sunset"]=>
  int(53182)
  [u"transit"]=>
  int(34965)
  [u"civil_twilight_begin"]=>
  int(15145)
  [u"civil_twilight_end"]=>
  int(54786)
  [u"nautical_twilight_begin"]=>
  int(13324)
  [u"nautical_twilight_end"]=>
  int(56607)
  [u"astronomical_twilight_begin"]=>
  int(11542)
  [u"astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 14 --
array(9) {
  [u"sunrise"]=>
  int(16748)
  [u"sunset"]=>
  int(53182)
  [u"transit"]=>
  int(34965)
  [u"civil_twilight_begin"]=>
  int(15145)
  [u"civil_twilight_end"]=>
  int(54786)
  [u"nautical_twilight_begin"]=>
  int(13324)
  [u"nautical_twilight_end"]=>
  int(56607)
  [u"astronomical_twilight_begin"]=>
  int(11542)
  [u"astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 15 --
array(9) {
  [u"sunrise"]=>
  int(16748)
  [u"sunset"]=>
  int(53182)
  [u"transit"]=>
  int(34965)
  [u"civil_twilight_begin"]=>
  int(15145)
  [u"civil_twilight_end"]=>
  int(54786)
  [u"nautical_twilight_begin"]=>
  int(13324)
  [u"nautical_twilight_end"]=>
  int(56607)
  [u"astronomical_twilight_begin"]=>
  int(11542)
  [u"astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 16 --

Warning: date_sun_info() expects parameter 1 to be long, Unicode string given in %s on line %d
bool(false)

-- Iteration 17 --

Warning: date_sun_info() expects parameter 1 to be long, Unicode string given in %s on line %d
bool(false)

-- Iteration 18 --

Warning: date_sun_info() expects parameter 1 to be long, array given in %s on line %d
bool(false)

-- Iteration 19 --

Warning: date_sun_info() expects parameter 1 to be long, Unicode string given in %s on line %d
bool(false)

-- Iteration 20 --

Warning: date_sun_info() expects parameter 1 to be long, Unicode string given in %s on line %d
bool(false)

-- Iteration 21 --

Warning: date_sun_info() expects parameter 1 to be long, Unicode string given in %s on line %d
bool(false)

-- Iteration 22 --
array(9) {
  [u"sunrise"]=>
  int(16748)
  [u"sunset"]=>
  int(53182)
  [u"transit"]=>
  int(34965)
  [u"civil_twilight_begin"]=>
  int(15145)
  [u"civil_twilight_end"]=>
  int(54786)
  [u"nautical_twilight_begin"]=>
  int(13324)
  [u"nautical_twilight_end"]=>
  int(56607)
  [u"astronomical_twilight_begin"]=>
  int(11542)
  [u"astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 23 --
array(9) {
  [u"sunrise"]=>
  int(16748)
  [u"sunset"]=>
  int(53182)
  [u"transit"]=>
  int(34965)
  [u"civil_twilight_begin"]=>
  int(15145)
  [u"civil_twilight_end"]=>
  int(54786)
  [u"nautical_twilight_begin"]=>
  int(13324)
  [u"nautical_twilight_end"]=>
  int(56607)
  [u"astronomical_twilight_begin"]=>
  int(11542)
  [u"astronomical_twilight_end"]=>
  int(58389)
}

-- Iteration 24 --

Warning: date_sun_info() expects parameter 1 to be long, resource given in %s on line %d
bool(false)
===Done===