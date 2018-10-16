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
  int(16742)
  ["sunset"]=>
  int(53161)
  ["transit"]=>
  int(34951)
  ["civil_twilight_begin"]=>
  int(15138)
  ["civil_twilight_end"]=>
  int(54765)
  ["nautical_twilight_begin"]=>
  int(13316)
  ["nautical_twilight_end"]=>
  int(56587)
  ["astronomical_twilight_begin"]=>
  int(11534)
  ["astronomical_twilight_end"]=>
  int(58369)
}

-- Iteration 2 --
array(9) {
  ["sunrise"]=>
  int(16742)
  ["sunset"]=>
  int(53161)
  ["transit"]=>
  int(34951)
  ["civil_twilight_begin"]=>
  int(15138)
  ["civil_twilight_end"]=>
  int(54765)
  ["nautical_twilight_begin"]=>
  int(13316)
  ["nautical_twilight_end"]=>
  int(56587)
  ["astronomical_twilight_begin"]=>
  int(11534)
  ["astronomical_twilight_end"]=>
  int(58369)
}

-- Iteration 3 --
array(9) {
  ["sunrise"]=>
  int(16742)
  ["sunset"]=>
  int(53161)
  ["transit"]=>
  int(34951)
  ["civil_twilight_begin"]=>
  int(15138)
  ["civil_twilight_end"]=>
  int(54765)
  ["nautical_twilight_begin"]=>
  int(13316)
  ["nautical_twilight_end"]=>
  int(56587)
  ["astronomical_twilight_begin"]=>
  int(11534)
  ["astronomical_twilight_end"]=>
  int(58369)
}

-- Iteration 4 --
array(9) {
  ["sunrise"]=>
  int(-69672)
  ["sunset"]=>
  int(-33281)
  ["transit"]=>
  int(-51476)
  ["civil_twilight_begin"]=>
  int(-71277)
  ["civil_twilight_end"]=>
  int(-31675)
  ["nautical_twilight_begin"]=>
  int(-73100)
  ["nautical_twilight_end"]=>
  int(-29852)
  ["astronomical_twilight_begin"]=>
  int(-74883)
  ["astronomical_twilight_end"]=>
  int(-28069)
}

-- Iteration 5 --
array(9) {
  ["sunrise"]=>
  int(16742)
  ["sunset"]=>
  int(53161)
  ["transit"]=>
  int(34951)
  ["civil_twilight_begin"]=>
  int(15138)
  ["civil_twilight_end"]=>
  int(54765)
  ["nautical_twilight_begin"]=>
  int(13316)
  ["nautical_twilight_end"]=>
  int(56587)
  ["astronomical_twilight_begin"]=>
  int(11534)
  ["astronomical_twilight_end"]=>
  int(58369)
}

-- Iteration 6 --
array(9) {
  ["sunrise"]=>
  int(-69672)
  ["sunset"]=>
  int(-33281)
  ["transit"]=>
  int(-51476)
  ["civil_twilight_begin"]=>
  int(-71277)
  ["civil_twilight_end"]=>
  int(-31675)
  ["nautical_twilight_begin"]=>
  int(-73100)
  ["nautical_twilight_end"]=>
  int(-29852)
  ["astronomical_twilight_begin"]=>
  int(-74883)
  ["astronomical_twilight_end"]=>
  int(-28069)
}

-- Iteration 7 --
array(9) {
  ["sunrise"]=>
  int(1226368)
  ["sunset"]=>
  int(1263442)
  ["transit"]=>
  int(1244905)
  ["civil_twilight_begin"]=>
  int(1224792)
  ["civil_twilight_end"]=>
  int(1265019)
  ["nautical_twilight_begin"]=>
  int(1222996)
  ["nautical_twilight_end"]=>
  int(1266815)
  ["astronomical_twilight_begin"]=>
  int(1221234)
  ["astronomical_twilight_end"]=>
  int(1268576)
}

-- Iteration 8 --
array(9) {
  ["sunrise"]=>
  int(16742)
  ["sunset"]=>
  int(53161)
  ["transit"]=>
  int(34951)
  ["civil_twilight_begin"]=>
  int(15138)
  ["civil_twilight_end"]=>
  int(54765)
  ["nautical_twilight_begin"]=>
  int(13316)
  ["nautical_twilight_end"]=>
  int(56587)
  ["astronomical_twilight_begin"]=>
  int(11534)
  ["astronomical_twilight_end"]=>
  int(58369)
}

-- Iteration 9 --
array(9) {
  ["sunrise"]=>
  int(16742)
  ["sunset"]=>
  int(53161)
  ["transit"]=>
  int(34951)
  ["civil_twilight_begin"]=>
  int(15138)
  ["civil_twilight_end"]=>
  int(54765)
  ["nautical_twilight_begin"]=>
  int(13316)
  ["nautical_twilight_end"]=>
  int(56587)
  ["astronomical_twilight_begin"]=>
  int(11534)
  ["astronomical_twilight_end"]=>
  int(58369)
}

-- Iteration 10 --
array(9) {
  ["sunrise"]=>
  int(16742)
  ["sunset"]=>
  int(53161)
  ["transit"]=>
  int(34951)
  ["civil_twilight_begin"]=>
  int(15138)
  ["civil_twilight_end"]=>
  int(54765)
  ["nautical_twilight_begin"]=>
  int(13316)
  ["nautical_twilight_end"]=>
  int(56587)
  ["astronomical_twilight_begin"]=>
  int(11534)
  ["astronomical_twilight_end"]=>
  int(58369)
}

-- Iteration 11 --
array(9) {
  ["sunrise"]=>
  int(16742)
  ["sunset"]=>
  int(53161)
  ["transit"]=>
  int(34951)
  ["civil_twilight_begin"]=>
  int(15138)
  ["civil_twilight_end"]=>
  int(54765)
  ["nautical_twilight_begin"]=>
  int(13316)
  ["nautical_twilight_end"]=>
  int(56587)
  ["astronomical_twilight_begin"]=>
  int(11534)
  ["astronomical_twilight_end"]=>
  int(58369)
}

-- Iteration 12 --
array(9) {
  ["sunrise"]=>
  int(16742)
  ["sunset"]=>
  int(53161)
  ["transit"]=>
  int(34951)
  ["civil_twilight_begin"]=>
  int(15138)
  ["civil_twilight_end"]=>
  int(54765)
  ["nautical_twilight_begin"]=>
  int(13316)
  ["nautical_twilight_end"]=>
  int(56587)
  ["astronomical_twilight_begin"]=>
  int(11534)
  ["astronomical_twilight_end"]=>
  int(58369)
}

-- Iteration 13 --
array(9) {
  ["sunrise"]=>
  int(16742)
  ["sunset"]=>
  int(53161)
  ["transit"]=>
  int(34951)
  ["civil_twilight_begin"]=>
  int(15138)
  ["civil_twilight_end"]=>
  int(54765)
  ["nautical_twilight_begin"]=>
  int(13316)
  ["nautical_twilight_end"]=>
  int(56587)
  ["astronomical_twilight_begin"]=>
  int(11534)
  ["astronomical_twilight_end"]=>
  int(58369)
}

-- Iteration 14 --
array(9) {
  ["sunrise"]=>
  int(16742)
  ["sunset"]=>
  int(53161)
  ["transit"]=>
  int(34951)
  ["civil_twilight_begin"]=>
  int(15138)
  ["civil_twilight_end"]=>
  int(54765)
  ["nautical_twilight_begin"]=>
  int(13316)
  ["nautical_twilight_end"]=>
  int(56587)
  ["astronomical_twilight_begin"]=>
  int(11534)
  ["astronomical_twilight_end"]=>
  int(58369)
}

-- Iteration 15 --
array(9) {
  ["sunrise"]=>
  int(16742)
  ["sunset"]=>
  int(53161)
  ["transit"]=>
  int(34951)
  ["civil_twilight_begin"]=>
  int(15138)
  ["civil_twilight_end"]=>
  int(54765)
  ["nautical_twilight_begin"]=>
  int(13316)
  ["nautical_twilight_end"]=>
  int(56587)
  ["astronomical_twilight_begin"]=>
  int(11534)
  ["astronomical_twilight_end"]=>
  int(58369)
}

-- Iteration 16 --

Warning: date_sun_info() expects parameter 1 to be int, string given in %s on line %d
bool(false)

-- Iteration 17 --

Warning: date_sun_info() expects parameter 1 to be int, string given in %s on line %d
bool(false)

-- Iteration 18 --

Warning: date_sun_info() expects parameter 1 to be int, array given in %s on line %d
bool(false)

-- Iteration 19 --

Warning: date_sun_info() expects parameter 1 to be int, string given in %s on line %d
bool(false)

-- Iteration 20 --

Warning: date_sun_info() expects parameter 1 to be int, string given in %s on line %d
bool(false)

-- Iteration 21 --

Warning: date_sun_info() expects parameter 1 to be int, string given in %s on line %d
bool(false)

-- Iteration 22 --
array(9) {
  ["sunrise"]=>
  int(16742)
  ["sunset"]=>
  int(53161)
  ["transit"]=>
  int(34951)
  ["civil_twilight_begin"]=>
  int(15138)
  ["civil_twilight_end"]=>
  int(54765)
  ["nautical_twilight_begin"]=>
  int(13316)
  ["nautical_twilight_end"]=>
  int(56587)
  ["astronomical_twilight_begin"]=>
  int(11534)
  ["astronomical_twilight_end"]=>
  int(58369)
}

-- Iteration 23 --
array(9) {
  ["sunrise"]=>
  int(16742)
  ["sunset"]=>
  int(53161)
  ["transit"]=>
  int(34951)
  ["civil_twilight_begin"]=>
  int(15138)
  ["civil_twilight_end"]=>
  int(54765)
  ["nautical_twilight_begin"]=>
  int(13316)
  ["nautical_twilight_end"]=>
  int(56587)
  ["astronomical_twilight_begin"]=>
  int(11534)
  ["astronomical_twilight_end"]=>
  int(58369)
}

-- Iteration 24 --

Warning: date_sun_info() expects parameter 1 to be int, resource given in %s on line %d
bool(false)
===Done===
