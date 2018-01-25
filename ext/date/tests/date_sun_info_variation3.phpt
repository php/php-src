--TEST--
Test date_sun_info() function : usage variations - Pass unexpected values for longitude argument
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
    var_dump(date_sun_info(strtotime("2006-12-12"), 31.7667, $input));
    $iterator++;
};

?>
===Done===
--EXPECTF--
*** Testing date_sun_info() : usage variations ***

-- Iteration 1 --
array(9) {
  ["sunrise"]=>
  int(1165906221)
  ["sunset"]=>
  int(1165942618)
  ["transit"]=>
  int(1165924420)
  ["civil_twilight_begin"]=>
  int(1165904616)
  ["civil_twilight_end"]=>
  int(1165944223)
  ["nautical_twilight_begin"]=>
  int(1165902793)
  ["nautical_twilight_end"]=>
  int(1165946046)
  ["astronomical_twilight_begin"]=>
  int(1165901011)
  ["astronomical_twilight_end"]=>
  int(1165947828)
}

-- Iteration 2 --
array(9) {
  ["sunrise"]=>
  int(1165905981)
  ["sunset"]=>
  int(1165942378)
  ["transit"]=>
  int(1165924179)
  ["civil_twilight_begin"]=>
  int(1165904376)
  ["civil_twilight_end"]=>
  int(1165943983)
  ["nautical_twilight_begin"]=>
  int(1165902553)
  ["nautical_twilight_end"]=>
  int(1165945806)
  ["astronomical_twilight_begin"]=>
  int(1165900771)
  ["astronomical_twilight_end"]=>
  int(1165947588)
}

-- Iteration 3 --
array(9) {
  ["sunrise"]=>
  int(1165879309)
  ["sunset"]=>
  int(1165917937)
  ["transit"]=>
  int(1165898623)
  ["civil_twilight_begin"]=>
  int(1165877787)
  ["civil_twilight_end"]=>
  int(1165919460)
  ["nautical_twilight_begin"]=>
  int(1165876041)
  ["nautical_twilight_end"]=>
  int(1165921205)
  ["astronomical_twilight_begin"]=>
  int(1165874319)
  ["astronomical_twilight_end"]=>
  int(1165922928)
}

-- Iteration 4 --
array(9) {
  ["sunrise"]=>
  int(1165864467)
  ["sunset"]=>
  int(1165900749)
  ["transit"]=>
  int(1165882608)
  ["civil_twilight_begin"]=>
  int(1165862856)
  ["civil_twilight_end"]=>
  int(1165902359)
  ["nautical_twilight_begin"]=>
  int(1165861029)
  ["nautical_twilight_end"]=>
  int(1165904187)
  ["astronomical_twilight_begin"]=>
  int(1165859242)
  ["astronomical_twilight_end"]=>
  int(1165905973)
}

-- Iteration 5 --
array(9) {
  ["sunrise"]=>
  int(1165903700)
  ["sunset"]=>
  int(1165940097)
  ["transit"]=>
  int(1165921899)
  ["civil_twilight_begin"]=>
  int(1165902095)
  ["civil_twilight_end"]=>
  int(1165941702)
  ["nautical_twilight_begin"]=>
  int(1165900272)
  ["nautical_twilight_end"]=>
  int(1165943525)
  ["astronomical_twilight_begin"]=>
  int(1165898490)
  ["astronomical_twilight_end"]=>
  int(1165945308)
}

-- Iteration 6 --
array(9) {
  ["sunrise"]=>
  int(1165908743)
  ["sunset"]=>
  int(1165945138)
  ["transit"]=>
  int(1165926940)
  ["civil_twilight_begin"]=>
  int(1165907137)
  ["civil_twilight_end"]=>
  int(1165946743)
  ["nautical_twilight_begin"]=>
  int(1165905315)
  ["nautical_twilight_end"]=>
  int(1165948566)
  ["astronomical_twilight_begin"]=>
  int(1165903532)
  ["astronomical_twilight_end"]=>
  int(1165950349)
}

-- Iteration 7 --
array(9) {
  ["sunrise"]=>
  int(1165920008)
  ["sunset"]=>
  int(1165970177)
  ["transit"]=>
  int(1165945092)
  ["civil_twilight_begin"]=>
  int(1165918353)
  ["civil_twilight_end"]=>
  int(1165971832)
  ["nautical_twilight_begin"]=>
  int(1165916371)
  ["nautical_twilight_end"]=>
  int(1165973814)
  ["astronomical_twilight_begin"]=>
  int(1165914258)
  ["astronomical_twilight_end"]=>
  int(1165975927)
}

-- Iteration 8 --
array(9) {
  ["sunrise"]=>
  int(1165906221)
  ["sunset"]=>
  int(1165942618)
  ["transit"]=>
  int(1165924420)
  ["civil_twilight_begin"]=>
  int(1165904616)
  ["civil_twilight_end"]=>
  int(1165944223)
  ["nautical_twilight_begin"]=>
  int(1165902793)
  ["nautical_twilight_end"]=>
  int(1165946046)
  ["astronomical_twilight_begin"]=>
  int(1165901011)
  ["astronomical_twilight_end"]=>
  int(1165947828)
}

-- Iteration 9 --
array(9) {
  ["sunrise"]=>
  int(1165906101)
  ["sunset"]=>
  int(1165942498)
  ["transit"]=>
  int(1165924300)
  ["civil_twilight_begin"]=>
  int(1165904496)
  ["civil_twilight_end"]=>
  int(1165944103)
  ["nautical_twilight_begin"]=>
  int(1165902673)
  ["nautical_twilight_end"]=>
  int(1165945926)
  ["astronomical_twilight_begin"]=>
  int(1165900891)
  ["astronomical_twilight_end"]=>
  int(1165947708)
}

-- Iteration 10 --
array(9) {
  ["sunrise"]=>
  int(1165906221)
  ["sunset"]=>
  int(1165942618)
  ["transit"]=>
  int(1165924420)
  ["civil_twilight_begin"]=>
  int(1165904616)
  ["civil_twilight_end"]=>
  int(1165944223)
  ["nautical_twilight_begin"]=>
  int(1165902793)
  ["nautical_twilight_end"]=>
  int(1165946046)
  ["astronomical_twilight_begin"]=>
  int(1165901011)
  ["astronomical_twilight_end"]=>
  int(1165947828)
}

-- Iteration 11 --
array(9) {
  ["sunrise"]=>
  int(1165906221)
  ["sunset"]=>
  int(1165942618)
  ["transit"]=>
  int(1165924420)
  ["civil_twilight_begin"]=>
  int(1165904616)
  ["civil_twilight_end"]=>
  int(1165944223)
  ["nautical_twilight_begin"]=>
  int(1165902793)
  ["nautical_twilight_end"]=>
  int(1165946046)
  ["astronomical_twilight_begin"]=>
  int(1165901011)
  ["astronomical_twilight_end"]=>
  int(1165947828)
}

-- Iteration 12 --
array(9) {
  ["sunrise"]=>
  int(1165905981)
  ["sunset"]=>
  int(1165942378)
  ["transit"]=>
  int(1165924179)
  ["civil_twilight_begin"]=>
  int(1165904376)
  ["civil_twilight_end"]=>
  int(1165943983)
  ["nautical_twilight_begin"]=>
  int(1165902553)
  ["nautical_twilight_end"]=>
  int(1165945806)
  ["astronomical_twilight_begin"]=>
  int(1165900771)
  ["astronomical_twilight_end"]=>
  int(1165947588)
}

-- Iteration 13 --
array(9) {
  ["sunrise"]=>
  int(1165906221)
  ["sunset"]=>
  int(1165942618)
  ["transit"]=>
  int(1165924420)
  ["civil_twilight_begin"]=>
  int(1165904616)
  ["civil_twilight_end"]=>
  int(1165944223)
  ["nautical_twilight_begin"]=>
  int(1165902793)
  ["nautical_twilight_end"]=>
  int(1165946046)
  ["astronomical_twilight_begin"]=>
  int(1165901011)
  ["astronomical_twilight_end"]=>
  int(1165947828)
}

-- Iteration 14 --
array(9) {
  ["sunrise"]=>
  int(1165905981)
  ["sunset"]=>
  int(1165942378)
  ["transit"]=>
  int(1165924179)
  ["civil_twilight_begin"]=>
  int(1165904376)
  ["civil_twilight_end"]=>
  int(1165943983)
  ["nautical_twilight_begin"]=>
  int(1165902553)
  ["nautical_twilight_end"]=>
  int(1165945806)
  ["astronomical_twilight_begin"]=>
  int(1165900771)
  ["astronomical_twilight_end"]=>
  int(1165947588)
}

-- Iteration 15 --
array(9) {
  ["sunrise"]=>
  int(1165906221)
  ["sunset"]=>
  int(1165942618)
  ["transit"]=>
  int(1165924420)
  ["civil_twilight_begin"]=>
  int(1165904616)
  ["civil_twilight_end"]=>
  int(1165944223)
  ["nautical_twilight_begin"]=>
  int(1165902793)
  ["nautical_twilight_end"]=>
  int(1165946046)
  ["astronomical_twilight_begin"]=>
  int(1165901011)
  ["astronomical_twilight_end"]=>
  int(1165947828)
}

-- Iteration 16 --

Warning: date_sun_info() expects parameter 3 to be float, string given in %s on line %d
bool(false)

-- Iteration 17 --

Warning: date_sun_info() expects parameter 3 to be float, string given in %s on line %d
bool(false)

-- Iteration 18 --

Warning: date_sun_info() expects parameter 3 to be float, array given in %s on line %d
bool(false)

-- Iteration 19 --

Warning: date_sun_info() expects parameter 3 to be float, string given in %s on line %d
bool(false)

-- Iteration 20 --

Warning: date_sun_info() expects parameter 3 to be float, string given in %s on line %d
bool(false)

-- Iteration 21 --

Warning: date_sun_info() expects parameter 3 to be float, string given in %s on line %d
bool(false)

-- Iteration 22 --
array(9) {
  ["sunrise"]=>
  int(1165906221)
  ["sunset"]=>
  int(1165942618)
  ["transit"]=>
  int(1165924420)
  ["civil_twilight_begin"]=>
  int(1165904616)
  ["civil_twilight_end"]=>
  int(1165944223)
  ["nautical_twilight_begin"]=>
  int(1165902793)
  ["nautical_twilight_end"]=>
  int(1165946046)
  ["astronomical_twilight_begin"]=>
  int(1165901011)
  ["astronomical_twilight_end"]=>
  int(1165947828)
}

-- Iteration 23 --
array(9) {
  ["sunrise"]=>
  int(1165906221)
  ["sunset"]=>
  int(1165942618)
  ["transit"]=>
  int(1165924420)
  ["civil_twilight_begin"]=>
  int(1165904616)
  ["civil_twilight_end"]=>
  int(1165944223)
  ["nautical_twilight_begin"]=>
  int(1165902793)
  ["nautical_twilight_end"]=>
  int(1165946046)
  ["astronomical_twilight_begin"]=>
  int(1165901011)
  ["astronomical_twilight_end"]=>
  int(1165947828)
}

-- Iteration 24 --

Warning: date_sun_info() expects parameter 3 to be float, resource given in %s on line %d
bool(false)
===Done===
