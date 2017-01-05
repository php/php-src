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
  int(1165894152)
  ["sunset"]=>
  int(1165937798)
  ["transit"]=>
  int(1165915975)
  ["civil_twilight_begin"]=>
  int(1165892809)
  ["civil_twilight_end"]=>
  int(1165939141)
  ["nautical_twilight_begin"]=>
  int(1165891239)
  ["nautical_twilight_end"]=>
  int(1165940710)
  ["astronomical_twilight_begin"]=>
  int(1165889663)
  ["astronomical_twilight_end"]=>
  int(1165942287)
}

-- Iteration 2 --
array(9) {
  ["sunrise"]=>
  int(1165894254)
  ["sunset"]=>
  int(1165937695)
  ["transit"]=>
  int(1165915975)
  ["civil_twilight_begin"]=>
  int(1165892911)
  ["civil_twilight_end"]=>
  int(1165939038)
  ["nautical_twilight_begin"]=>
  int(1165891344)
  ["nautical_twilight_end"]=>
  int(1165940606)
  ["astronomical_twilight_begin"]=>
  int(1165889771)
  ["astronomical_twilight_end"]=>
  int(1165942179)
}

-- Iteration 3 --
array(9) {
  ["sunrise"]=>
  bool(true)
  ["sunset"]=>
  bool(true)
  ["transit"]=>
  int(1165915975)
  ["civil_twilight_begin"]=>
  bool(true)
  ["civil_twilight_end"]=>
  bool(true)
  ["nautical_twilight_begin"]=>
  int(1165883331)
  ["nautical_twilight_end"]=>
  int(1165948619)
  ["astronomical_twilight_begin"]=>
  int(1165890260)
  ["astronomical_twilight_end"]=>
  int(1165941690)
}

-- Iteration 4 --
array(9) {
  ["sunrise"]=>
  int(1165894085)
  ["sunset"]=>
  int(1165937865)
  ["transit"]=>
  int(1165915975)
  ["civil_twilight_begin"]=>
  int(1165895431)
  ["civil_twilight_end"]=>
  int(1165936519)
  ["nautical_twilight_begin"]=>
  int(1165896998)
  ["nautical_twilight_end"]=>
  int(1165934952)
  ["astronomical_twilight_begin"]=>
  int(1165898564)
  ["astronomical_twilight_end"]=>
  int(1165933386)
}

-- Iteration 5 --
array(9) {
  ["sunrise"]=>
  int(1165895237)
  ["sunset"]=>
  int(1165936713)
  ["transit"]=>
  int(1165915975)
  ["civil_twilight_begin"]=>
  int(1165893873)
  ["civil_twilight_end"]=>
  int(1165938077)
  ["nautical_twilight_begin"]=>
  int(1165892293)
  ["nautical_twilight_end"]=>
  int(1165939656)
  ["astronomical_twilight_begin"]=>
  int(1165890721)
  ["astronomical_twilight_end"]=>
  int(1165941229)
}

-- Iteration 6 --
array(9) {
  ["sunrise"]=>
  int(1165893058)
  ["sunset"]=>
  int(1165938891)
  ["transit"]=>
  int(1165915975)
  ["civil_twilight_begin"]=>
  int(1165891680)
  ["civil_twilight_end"]=>
  int(1165940269)
  ["nautical_twilight_begin"]=>
  int(1165890055)
  ["nautical_twilight_end"]=>
  int(1165941895)
  ["astronomical_twilight_begin"]=>
  int(1165888402)
  ["astronomical_twilight_end"]=>
  int(1165943548)
}

-- Iteration 7 --
array(9) {
  ["sunrise"]=>
  int(1165894152)
  ["sunset"]=>
  int(1165937798)
  ["transit"]=>
  int(1165915975)
  ["civil_twilight_begin"]=>
  int(1165892809)
  ["civil_twilight_end"]=>
  int(1165939141)
  ["nautical_twilight_begin"]=>
  int(1165891239)
  ["nautical_twilight_end"]=>
  int(1165940710)
  ["astronomical_twilight_begin"]=>
  int(1165889663)
  ["astronomical_twilight_end"]=>
  int(1165942287)
}

-- Iteration 8 --
array(9) {
  ["sunrise"]=>
  int(1165894152)
  ["sunset"]=>
  int(1165937798)
  ["transit"]=>
  int(1165915975)
  ["civil_twilight_begin"]=>
  int(1165892809)
  ["civil_twilight_end"]=>
  int(1165939141)
  ["nautical_twilight_begin"]=>
  int(1165891239)
  ["nautical_twilight_end"]=>
  int(1165940710)
  ["astronomical_twilight_begin"]=>
  int(1165889663)
  ["astronomical_twilight_end"]=>
  int(1165942287)
}

-- Iteration 9 --
array(9) {
  ["sunrise"]=>
  int(1165894203)
  ["sunset"]=>
  int(1165937747)
  ["transit"]=>
  int(1165915975)
  ["civil_twilight_begin"]=>
  int(1165892860)
  ["civil_twilight_end"]=>
  int(1165939090)
  ["nautical_twilight_begin"]=>
  int(1165891292)
  ["nautical_twilight_end"]=>
  int(1165940658)
  ["astronomical_twilight_begin"]=>
  int(1165889717)
  ["astronomical_twilight_end"]=>
  int(1165942233)
}

-- Iteration 10 --
array(9) {
  ["sunrise"]=>
  int(1165894152)
  ["sunset"]=>
  int(1165937798)
  ["transit"]=>
  int(1165915975)
  ["civil_twilight_begin"]=>
  int(1165892809)
  ["civil_twilight_end"]=>
  int(1165939141)
  ["nautical_twilight_begin"]=>
  int(1165891239)
  ["nautical_twilight_end"]=>
  int(1165940710)
  ["astronomical_twilight_begin"]=>
  int(1165889663)
  ["astronomical_twilight_end"]=>
  int(1165942287)
}

-- Iteration 11 --
array(9) {
  ["sunrise"]=>
  int(1165894152)
  ["sunset"]=>
  int(1165937798)
  ["transit"]=>
  int(1165915975)
  ["civil_twilight_begin"]=>
  int(1165892809)
  ["civil_twilight_end"]=>
  int(1165939141)
  ["nautical_twilight_begin"]=>
  int(1165891239)
  ["nautical_twilight_end"]=>
  int(1165940710)
  ["astronomical_twilight_begin"]=>
  int(1165889663)
  ["astronomical_twilight_end"]=>
  int(1165942287)
}

-- Iteration 12 --
array(9) {
  ["sunrise"]=>
  int(1165894254)
  ["sunset"]=>
  int(1165937695)
  ["transit"]=>
  int(1165915975)
  ["civil_twilight_begin"]=>
  int(1165892911)
  ["civil_twilight_end"]=>
  int(1165939038)
  ["nautical_twilight_begin"]=>
  int(1165891344)
  ["nautical_twilight_end"]=>
  int(1165940606)
  ["astronomical_twilight_begin"]=>
  int(1165889771)
  ["astronomical_twilight_end"]=>
  int(1165942179)
}

-- Iteration 13 --
array(9) {
  ["sunrise"]=>
  int(1165894152)
  ["sunset"]=>
  int(1165937798)
  ["transit"]=>
  int(1165915975)
  ["civil_twilight_begin"]=>
  int(1165892809)
  ["civil_twilight_end"]=>
  int(1165939141)
  ["nautical_twilight_begin"]=>
  int(1165891239)
  ["nautical_twilight_end"]=>
  int(1165940710)
  ["astronomical_twilight_begin"]=>
  int(1165889663)
  ["astronomical_twilight_end"]=>
  int(1165942287)
}

-- Iteration 14 --
array(9) {
  ["sunrise"]=>
  int(1165894254)
  ["sunset"]=>
  int(1165937695)
  ["transit"]=>
  int(1165915975)
  ["civil_twilight_begin"]=>
  int(1165892911)
  ["civil_twilight_end"]=>
  int(1165939038)
  ["nautical_twilight_begin"]=>
  int(1165891344)
  ["nautical_twilight_end"]=>
  int(1165940606)
  ["astronomical_twilight_begin"]=>
  int(1165889771)
  ["astronomical_twilight_end"]=>
  int(1165942179)
}

-- Iteration 15 --
array(9) {
  ["sunrise"]=>
  int(1165894152)
  ["sunset"]=>
  int(1165937798)
  ["transit"]=>
  int(1165915975)
  ["civil_twilight_begin"]=>
  int(1165892809)
  ["civil_twilight_end"]=>
  int(1165939141)
  ["nautical_twilight_begin"]=>
  int(1165891239)
  ["nautical_twilight_end"]=>
  int(1165940710)
  ["astronomical_twilight_begin"]=>
  int(1165889663)
  ["astronomical_twilight_end"]=>
  int(1165942287)
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
  int(1165894152)
  ["sunset"]=>
  int(1165937798)
  ["transit"]=>
  int(1165915975)
  ["civil_twilight_begin"]=>
  int(1165892809)
  ["civil_twilight_end"]=>
  int(1165939141)
  ["nautical_twilight_begin"]=>
  int(1165891239)
  ["nautical_twilight_end"]=>
  int(1165940710)
  ["astronomical_twilight_begin"]=>
  int(1165889663)
  ["astronomical_twilight_end"]=>
  int(1165942287)
}

-- Iteration 23 --
array(9) {
  ["sunrise"]=>
  int(1165894152)
  ["sunset"]=>
  int(1165937798)
  ["transit"]=>
  int(1165915975)
  ["civil_twilight_begin"]=>
  int(1165892809)
  ["civil_twilight_end"]=>
  int(1165939141)
  ["nautical_twilight_begin"]=>
  int(1165891239)
  ["nautical_twilight_end"]=>
  int(1165940710)
  ["astronomical_twilight_begin"]=>
  int(1165889663)
  ["astronomical_twilight_end"]=>
  int(1165942287)
}

-- Iteration 24 --

Warning: date_sun_info() expects parameter 2 to be float, resource given in %s on line %d
bool(false)
===Done===
