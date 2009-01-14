--TEST--
Test date_sun_info() function : error variations - PAss unexepcted valuesfor latitude argument
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
  [u"sunrise"]=>
  int(1165894152)
  [u"sunset"]=>
  int(1165937798)
  [u"transit"]=>
  int(1165915975)
  [u"civil_twilight_begin"]=>
  int(1165892809)
  [u"civil_twilight_end"]=>
  int(1165939141)
  [u"nautical_twilight_begin"]=>
  int(1165891239)
  [u"nautical_twilight_end"]=>
  int(1165940710)
  [u"astronomical_twilight_begin"]=>
  int(1165889663)
  [u"astronomical_twilight_end"]=>
  int(1165942287)
}

-- Iteration 2 --
array(9) {
  [u"sunrise"]=>
  int(1165894254)
  [u"sunset"]=>
  int(1165937695)
  [u"transit"]=>
  int(1165915975)
  [u"civil_twilight_begin"]=>
  int(1165892911)
  [u"civil_twilight_end"]=>
  int(1165939038)
  [u"nautical_twilight_begin"]=>
  int(1165891344)
  [u"nautical_twilight_end"]=>
  int(1165940606)
  [u"astronomical_twilight_begin"]=>
  int(1165889771)
  [u"astronomical_twilight_end"]=>
  int(1165942179)
}

-- Iteration 3 --
array(9) {
  [u"sunrise"]=>
  bool(true)
  [u"sunset"]=>
  bool(true)
  [u"transit"]=>
  int(1165915975)
  [u"civil_twilight_begin"]=>
  bool(true)
  [u"civil_twilight_end"]=>
  bool(true)
  [u"nautical_twilight_begin"]=>
  int(1165883331)
  [u"nautical_twilight_end"]=>
  int(1165948619)
  [u"astronomical_twilight_begin"]=>
  int(1165890260)
  [u"astronomical_twilight_end"]=>
  int(1165941690)
}

-- Iteration 4 --
array(9) {
  [u"sunrise"]=>
  int(1165894085)
  [u"sunset"]=>
  int(1165937865)
  [u"transit"]=>
  int(1165915975)
  [u"civil_twilight_begin"]=>
  int(1165895431)
  [u"civil_twilight_end"]=>
  int(1165936519)
  [u"nautical_twilight_begin"]=>
  int(1165896998)
  [u"nautical_twilight_end"]=>
  int(1165934952)
  [u"astronomical_twilight_begin"]=>
  int(1165898564)
  [u"astronomical_twilight_end"]=>
  int(1165933386)
}

-- Iteration 5 --
array(9) {
  [u"sunrise"]=>
  int(1165895237)
  [u"sunset"]=>
  int(1165936713)
  [u"transit"]=>
  int(1165915975)
  [u"civil_twilight_begin"]=>
  int(1165893873)
  [u"civil_twilight_end"]=>
  int(1165938077)
  [u"nautical_twilight_begin"]=>
  int(1165892293)
  [u"nautical_twilight_end"]=>
  int(1165939656)
  [u"astronomical_twilight_begin"]=>
  int(1165890721)
  [u"astronomical_twilight_end"]=>
  int(1165941229)
}

-- Iteration 6 --
array(9) {
  [u"sunrise"]=>
  int(1165893058)
  [u"sunset"]=>
  int(1165938891)
  [u"transit"]=>
  int(1165915975)
  [u"civil_twilight_begin"]=>
  int(1165891680)
  [u"civil_twilight_end"]=>
  int(1165940269)
  [u"nautical_twilight_begin"]=>
  int(1165890055)
  [u"nautical_twilight_end"]=>
  int(1165941895)
  [u"astronomical_twilight_begin"]=>
  int(1165888402)
  [u"astronomical_twilight_end"]=>
  int(1165943548)
}

-- Iteration 7 --
array(9) {
  [u"sunrise"]=>
  int(1165894152)
  [u"sunset"]=>
  int(1165937798)
  [u"transit"]=>
  int(1165915975)
  [u"civil_twilight_begin"]=>
  int(1165892809)
  [u"civil_twilight_end"]=>
  int(1165939141)
  [u"nautical_twilight_begin"]=>
  int(1165891239)
  [u"nautical_twilight_end"]=>
  int(1165940710)
  [u"astronomical_twilight_begin"]=>
  int(1165889663)
  [u"astronomical_twilight_end"]=>
  int(1165942287)
}

-- Iteration 8 --
array(9) {
  [u"sunrise"]=>
  int(1165894152)
  [u"sunset"]=>
  int(1165937798)
  [u"transit"]=>
  int(1165915975)
  [u"civil_twilight_begin"]=>
  int(1165892809)
  [u"civil_twilight_end"]=>
  int(1165939141)
  [u"nautical_twilight_begin"]=>
  int(1165891239)
  [u"nautical_twilight_end"]=>
  int(1165940710)
  [u"astronomical_twilight_begin"]=>
  int(1165889663)
  [u"astronomical_twilight_end"]=>
  int(1165942287)
}

-- Iteration 9 --
array(9) {
  [u"sunrise"]=>
  int(1165894203)
  [u"sunset"]=>
  int(1165937747)
  [u"transit"]=>
  int(1165915975)
  [u"civil_twilight_begin"]=>
  int(1165892860)
  [u"civil_twilight_end"]=>
  int(1165939090)
  [u"nautical_twilight_begin"]=>
  int(1165891292)
  [u"nautical_twilight_end"]=>
  int(1165940658)
  [u"astronomical_twilight_begin"]=>
  int(1165889717)
  [u"astronomical_twilight_end"]=>
  int(1165942233)
}

-- Iteration 10 --
array(9) {
  [u"sunrise"]=>
  int(1165894152)
  [u"sunset"]=>
  int(1165937798)
  [u"transit"]=>
  int(1165915975)
  [u"civil_twilight_begin"]=>
  int(1165892809)
  [u"civil_twilight_end"]=>
  int(1165939141)
  [u"nautical_twilight_begin"]=>
  int(1165891239)
  [u"nautical_twilight_end"]=>
  int(1165940710)
  [u"astronomical_twilight_begin"]=>
  int(1165889663)
  [u"astronomical_twilight_end"]=>
  int(1165942287)
}

-- Iteration 11 --
array(9) {
  [u"sunrise"]=>
  int(1165894152)
  [u"sunset"]=>
  int(1165937798)
  [u"transit"]=>
  int(1165915975)
  [u"civil_twilight_begin"]=>
  int(1165892809)
  [u"civil_twilight_end"]=>
  int(1165939141)
  [u"nautical_twilight_begin"]=>
  int(1165891239)
  [u"nautical_twilight_end"]=>
  int(1165940710)
  [u"astronomical_twilight_begin"]=>
  int(1165889663)
  [u"astronomical_twilight_end"]=>
  int(1165942287)
}

-- Iteration 12 --
array(9) {
  [u"sunrise"]=>
  int(1165894254)
  [u"sunset"]=>
  int(1165937695)
  [u"transit"]=>
  int(1165915975)
  [u"civil_twilight_begin"]=>
  int(1165892911)
  [u"civil_twilight_end"]=>
  int(1165939038)
  [u"nautical_twilight_begin"]=>
  int(1165891344)
  [u"nautical_twilight_end"]=>
  int(1165940606)
  [u"astronomical_twilight_begin"]=>
  int(1165889771)
  [u"astronomical_twilight_end"]=>
  int(1165942179)
}

-- Iteration 13 --
array(9) {
  [u"sunrise"]=>
  int(1165894152)
  [u"sunset"]=>
  int(1165937798)
  [u"transit"]=>
  int(1165915975)
  [u"civil_twilight_begin"]=>
  int(1165892809)
  [u"civil_twilight_end"]=>
  int(1165939141)
  [u"nautical_twilight_begin"]=>
  int(1165891239)
  [u"nautical_twilight_end"]=>
  int(1165940710)
  [u"astronomical_twilight_begin"]=>
  int(1165889663)
  [u"astronomical_twilight_end"]=>
  int(1165942287)
}

-- Iteration 14 --
array(9) {
  [u"sunrise"]=>
  int(1165894254)
  [u"sunset"]=>
  int(1165937695)
  [u"transit"]=>
  int(1165915975)
  [u"civil_twilight_begin"]=>
  int(1165892911)
  [u"civil_twilight_end"]=>
  int(1165939038)
  [u"nautical_twilight_begin"]=>
  int(1165891344)
  [u"nautical_twilight_end"]=>
  int(1165940606)
  [u"astronomical_twilight_begin"]=>
  int(1165889771)
  [u"astronomical_twilight_end"]=>
  int(1165942179)
}

-- Iteration 15 --
array(9) {
  [u"sunrise"]=>
  int(1165894152)
  [u"sunset"]=>
  int(1165937798)
  [u"transit"]=>
  int(1165915975)
  [u"civil_twilight_begin"]=>
  int(1165892809)
  [u"civil_twilight_end"]=>
  int(1165939141)
  [u"nautical_twilight_begin"]=>
  int(1165891239)
  [u"nautical_twilight_end"]=>
  int(1165940710)
  [u"astronomical_twilight_begin"]=>
  int(1165889663)
  [u"astronomical_twilight_end"]=>
  int(1165942287)
}

-- Iteration 16 --

Warning: date_sun_info() expects parameter 2 to be double, Unicode string given in %s on line %d
bool(false)

-- Iteration 17 --

Warning: date_sun_info() expects parameter 2 to be double, Unicode string given in %s on line %d
bool(false)

-- Iteration 18 --

Warning: date_sun_info() expects parameter 2 to be double, array given in %s on line %d
bool(false)

-- Iteration 19 --

Warning: date_sun_info() expects parameter 2 to be double, Unicode string given in %s on line %d
bool(false)

-- Iteration 20 --

Warning: date_sun_info() expects parameter 2 to be double, Unicode string given in %s on line %d
bool(false)

-- Iteration 21 --

Warning: date_sun_info() expects parameter 2 to be double, Unicode string given in %s on line %d
bool(false)

-- Iteration 22 --
array(9) {
  [u"sunrise"]=>
  int(1165894152)
  [u"sunset"]=>
  int(1165937798)
  [u"transit"]=>
  int(1165915975)
  [u"civil_twilight_begin"]=>
  int(1165892809)
  [u"civil_twilight_end"]=>
  int(1165939141)
  [u"nautical_twilight_begin"]=>
  int(1165891239)
  [u"nautical_twilight_end"]=>
  int(1165940710)
  [u"astronomical_twilight_begin"]=>
  int(1165889663)
  [u"astronomical_twilight_end"]=>
  int(1165942287)
}

-- Iteration 23 --
array(9) {
  [u"sunrise"]=>
  int(1165894152)
  [u"sunset"]=>
  int(1165937798)
  [u"transit"]=>
  int(1165915975)
  [u"civil_twilight_begin"]=>
  int(1165892809)
  [u"civil_twilight_end"]=>
  int(1165939141)
  [u"nautical_twilight_begin"]=>
  int(1165891239)
  [u"nautical_twilight_end"]=>
  int(1165940710)
  [u"astronomical_twilight_begin"]=>
  int(1165889663)
  [u"astronomical_twilight_end"]=>
  int(1165942287)
}

-- Iteration 24 --

Warning: date_sun_info() expects parameter 2 to be double, resource given in %s on line %d
bool(false)
===Done===