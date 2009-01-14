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
  int(1165906242)
  ["sunset"]=>
  int(1165942625)
  ["transit"]=>
  int(1165924434)
  ["civil_twilight_begin"]=>
  int(1165904636)
  ["civil_twilight_end"]=>
  int(1165944231)
  ["nautical_twilight_begin"]=>
  int(1165902813)
  ["nautical_twilight_end"]=>
  int(1165946054)
  ["astronomical_twilight_begin"]=>
  int(1165901030)
  ["astronomical_twilight_end"]=>
  int(1165947838)
}

-- Iteration 2 --
array(9) {
  ["sunrise"]=>
  int(1165906002)
  ["sunset"]=>
  int(1165942385)
  ["transit"]=>
  int(1165924194)
  ["civil_twilight_begin"]=>
  int(1165904396)
  ["civil_twilight_end"]=>
  int(1165943991)
  ["nautical_twilight_begin"]=>
  int(1165902573)
  ["nautical_twilight_end"]=>
  int(1165945814)
  ["astronomical_twilight_begin"]=>
  int(1165900789)
  ["astronomical_twilight_end"]=>
  int(1165947598)
}

-- Iteration 3 --
array(9) {
  ["sunrise"]=>
  int(1165879335)
  ["sunset"]=>
  int(1165917916)
  ["transit"]=>
  int(1165898625)
  ["civil_twilight_begin"]=>
  int(1165877811)
  ["civil_twilight_end"]=>
  int(1165919440)
  ["nautical_twilight_begin"]=>
  int(1165876064)
  ["nautical_twilight_end"]=>
  int(1165921187)
  ["astronomical_twilight_begin"]=>
  int(1165874341)
  ["astronomical_twilight_end"]=>
  int(1165922910)
}

-- Iteration 4 --
array(9) {
  ["sunrise"]=>
  int(1165864483)
  ["sunset"]=>
  int(1165900762)
  ["transit"]=>
  int(1165882623)
  ["civil_twilight_begin"]=>
  int(1165862873)
  ["civil_twilight_end"]=>
  int(1165902372)
  ["nautical_twilight_begin"]=>
  int(1165861045)
  ["nautical_twilight_end"]=>
  int(1165904200)
  ["astronomical_twilight_begin"]=>
  int(1165859259)
  ["astronomical_twilight_end"]=>
  int(1165905987)
}

-- Iteration 5 --
array(9) {
  ["sunrise"]=>
  int(1165903721)
  ["sunset"]=>
  int(1165940105)
  ["transit"]=>
  int(1165921913)
  ["civil_twilight_begin"]=>
  int(1165902115)
  ["civil_twilight_end"]=>
  int(1165941711)
  ["nautical_twilight_begin"]=>
  int(1165900292)
  ["nautical_twilight_end"]=>
  int(1165943534)
  ["astronomical_twilight_begin"]=>
  int(1165898508)
  ["astronomical_twilight_end"]=>
  int(1165945317)
}

-- Iteration 6 --
array(9) {
  ["sunrise"]=>
  int(1165908763)
  ["sunset"]=>
  int(1165945146)
  ["transit"]=>
  int(1165926954)
  ["civil_twilight_begin"]=>
  int(1165907157)
  ["civil_twilight_end"]=>
  int(1165946752)
  ["nautical_twilight_begin"]=>
  int(1165905334)
  ["nautical_twilight_end"]=>
  int(1165948575)
  ["astronomical_twilight_begin"]=>
  int(1165903551)
  ["astronomical_twilight_end"]=>
  int(1165950358)
}

-- Iteration 7 --
array(9) {
  ["sunrise"]=>
  int(1165919856)
  ["sunset"]=>
  int(1165969985)
  ["transit"]=>
  int(1165944920)
  ["civil_twilight_begin"]=>
  int(1165918203)
  ["civil_twilight_end"]=>
  int(1165971638)
  ["nautical_twilight_begin"]=>
  int(1165916223)
  ["nautical_twilight_end"]=>
  int(1165973617)
  ["astronomical_twilight_begin"]=>
  int(1165914116)
  ["astronomical_twilight_end"]=>
  int(1165975725)
}

-- Iteration 8 --
array(9) {
  ["sunrise"]=>
  int(1165906242)
  ["sunset"]=>
  int(1165942625)
  ["transit"]=>
  int(1165924434)
  ["civil_twilight_begin"]=>
  int(1165904636)
  ["civil_twilight_end"]=>
  int(1165944231)
  ["nautical_twilight_begin"]=>
  int(1165902813)
  ["nautical_twilight_end"]=>
  int(1165946054)
  ["astronomical_twilight_begin"]=>
  int(1165901030)
  ["astronomical_twilight_end"]=>
  int(1165947838)
}

-- Iteration 9 --
array(9) {
  ["sunrise"]=>
  int(1165906122)
  ["sunset"]=>
  int(1165942505)
  ["transit"]=>
  int(1165924314)
  ["civil_twilight_begin"]=>
  int(1165904516)
  ["civil_twilight_end"]=>
  int(1165944111)
  ["nautical_twilight_begin"]=>
  int(1165902693)
  ["nautical_twilight_end"]=>
  int(1165945934)
  ["astronomical_twilight_begin"]=>
  int(1165900910)
  ["astronomical_twilight_end"]=>
  int(1165947718)
}

-- Iteration 10 --
array(9) {
  ["sunrise"]=>
  int(1165906242)
  ["sunset"]=>
  int(1165942625)
  ["transit"]=>
  int(1165924434)
  ["civil_twilight_begin"]=>
  int(1165904636)
  ["civil_twilight_end"]=>
  int(1165944231)
  ["nautical_twilight_begin"]=>
  int(1165902813)
  ["nautical_twilight_end"]=>
  int(1165946054)
  ["astronomical_twilight_begin"]=>
  int(1165901030)
  ["astronomical_twilight_end"]=>
  int(1165947838)
}

-- Iteration 11 --
array(9) {
  ["sunrise"]=>
  int(1165906242)
  ["sunset"]=>
  int(1165942625)
  ["transit"]=>
  int(1165924434)
  ["civil_twilight_begin"]=>
  int(1165904636)
  ["civil_twilight_end"]=>
  int(1165944231)
  ["nautical_twilight_begin"]=>
  int(1165902813)
  ["nautical_twilight_end"]=>
  int(1165946054)
  ["astronomical_twilight_begin"]=>
  int(1165901030)
  ["astronomical_twilight_end"]=>
  int(1165947838)
}

-- Iteration 12 --
array(9) {
  ["sunrise"]=>
  int(1165906002)
  ["sunset"]=>
  int(1165942385)
  ["transit"]=>
  int(1165924194)
  ["civil_twilight_begin"]=>
  int(1165904396)
  ["civil_twilight_end"]=>
  int(1165943991)
  ["nautical_twilight_begin"]=>
  int(1165902573)
  ["nautical_twilight_end"]=>
  int(1165945814)
  ["astronomical_twilight_begin"]=>
  int(1165900789)
  ["astronomical_twilight_end"]=>
  int(1165947598)
}

-- Iteration 13 --
array(9) {
  ["sunrise"]=>
  int(1165906242)
  ["sunset"]=>
  int(1165942625)
  ["transit"]=>
  int(1165924434)
  ["civil_twilight_begin"]=>
  int(1165904636)
  ["civil_twilight_end"]=>
  int(1165944231)
  ["nautical_twilight_begin"]=>
  int(1165902813)
  ["nautical_twilight_end"]=>
  int(1165946054)
  ["astronomical_twilight_begin"]=>
  int(1165901030)
  ["astronomical_twilight_end"]=>
  int(1165947838)
}

-- Iteration 14 --
array(9) {
  ["sunrise"]=>
  int(1165906002)
  ["sunset"]=>
  int(1165942385)
  ["transit"]=>
  int(1165924194)
  ["civil_twilight_begin"]=>
  int(1165904396)
  ["civil_twilight_end"]=>
  int(1165943991)
  ["nautical_twilight_begin"]=>
  int(1165902573)
  ["nautical_twilight_end"]=>
  int(1165945814)
  ["astronomical_twilight_begin"]=>
  int(1165900789)
  ["astronomical_twilight_end"]=>
  int(1165947598)
}

-- Iteration 15 --
array(9) {
  ["sunrise"]=>
  int(1165906242)
  ["sunset"]=>
  int(1165942625)
  ["transit"]=>
  int(1165924434)
  ["civil_twilight_begin"]=>
  int(1165904636)
  ["civil_twilight_end"]=>
  int(1165944231)
  ["nautical_twilight_begin"]=>
  int(1165902813)
  ["nautical_twilight_end"]=>
  int(1165946054)
  ["astronomical_twilight_begin"]=>
  int(1165901030)
  ["astronomical_twilight_end"]=>
  int(1165947838)
}

-- Iteration 16 --

Warning: date_sun_info() expects parameter 3 to be double, string given in %s on line %d
bool(false)

-- Iteration 17 --

Warning: date_sun_info() expects parameter 3 to be double, string given in %s on line %d
bool(false)

-- Iteration 18 --

Warning: date_sun_info() expects parameter 3 to be double, array given in %s on line %d
bool(false)

-- Iteration 19 --

Warning: date_sun_info() expects parameter 3 to be double, string given in %s on line %d
bool(false)

-- Iteration 20 --

Warning: date_sun_info() expects parameter 3 to be double, string given in %s on line %d
bool(false)

-- Iteration 21 --

Warning: date_sun_info() expects parameter 3 to be double, string given in %s on line %d
bool(false)

-- Iteration 22 --
array(9) {
  ["sunrise"]=>
  int(1165906242)
  ["sunset"]=>
  int(1165942625)
  ["transit"]=>
  int(1165924434)
  ["civil_twilight_begin"]=>
  int(1165904636)
  ["civil_twilight_end"]=>
  int(1165944231)
  ["nautical_twilight_begin"]=>
  int(1165902813)
  ["nautical_twilight_end"]=>
  int(1165946054)
  ["astronomical_twilight_begin"]=>
  int(1165901030)
  ["astronomical_twilight_end"]=>
  int(1165947838)
}

-- Iteration 23 --
array(9) {
  ["sunrise"]=>
  int(1165906242)
  ["sunset"]=>
  int(1165942625)
  ["transit"]=>
  int(1165924434)
  ["civil_twilight_begin"]=>
  int(1165904636)
  ["civil_twilight_end"]=>
  int(1165944231)
  ["nautical_twilight_begin"]=>
  int(1165902813)
  ["nautical_twilight_end"]=>
  int(1165946054)
  ["astronomical_twilight_begin"]=>
  int(1165901030)
  ["astronomical_twilight_end"]=>
  int(1165947838)
}

-- Iteration 24 --

Warning: date_sun_info() expects parameter 3 to be double, resource given in %s on line %d
bool(false)
===Done===
