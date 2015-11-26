--TEST--
Test date_default_timezone_set() function : usage variations - Passing unexpected values for time_zone identifier
--FILE--
<?php
/* Prototype  : bool date_default_timezone_set ( string $timezone_identifier )
 * Description:  Sets the default timezone used by all date/time functions in a script.
 * Source code: ext/standard/data/php_date.c
 */

echo "*** Testing date_default_timezone_set() : usage variations ***\n";
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

// loop through each element of $inputs to check the behaviour of date_default_timezone_set()
$iterator = 1;
foreach($inputs as $input) {
    echo "\n-- Iteration $iterator --\n";
    var_dump(date_default_timezone_set($input));
    $iterator++;
};
?>
===Done===
--EXPECTF--
*** Testing date_default_timezone_set() : usage variations ***

-- Iteration 1 --

Notice: date_default_timezone_set(): Timezone ID '0' is invalid in %s on line %d
bool(false)

-- Iteration 2 --

Notice: date_default_timezone_set(): Timezone ID '1' is invalid in %s on line %d
bool(false)

-- Iteration 3 --

Notice: date_default_timezone_set(): Timezone ID '12345' is invalid in %s on line %d
bool(false)

-- Iteration 4 --

Notice: date_default_timezone_set(): Timezone ID '-2345' is invalid in %s on line %d
bool(false)

-- Iteration 5 --

Notice: date_default_timezone_set(): Timezone ID '10.5' is invalid in %s on line %d
bool(false)

-- Iteration 6 --

Notice: date_default_timezone_set(): Timezone ID '-10.5' is invalid in %s on line %d
bool(false)

-- Iteration 7 --

Notice: date_default_timezone_set(): Timezone ID '123456789000' is invalid in %s on line %d
bool(false)

-- Iteration 8 --

Notice: date_default_timezone_set(): Timezone ID '1.23456789E-9' is invalid in %s on line %d
bool(false)

-- Iteration 9 --

Notice: date_default_timezone_set(): Timezone ID '0.5' is invalid in %s on line %d
bool(false)

-- Iteration 10 --

Notice: date_default_timezone_set(): Timezone ID '' is invalid in %s on line %d
bool(false)

-- Iteration 11 --

Notice: date_default_timezone_set(): Timezone ID '' is invalid in %s on line %d
bool(false)

-- Iteration 12 --

Notice: date_default_timezone_set(): Timezone ID '1' is invalid in %s on line %d
bool(false)

-- Iteration 13 --

Notice: date_default_timezone_set(): Timezone ID '' is invalid in %s on line %d
bool(false)

-- Iteration 14 --

Notice: date_default_timezone_set(): Timezone ID '1' is invalid in %s on line %d
bool(false)

-- Iteration 15 --

Notice: date_default_timezone_set(): Timezone ID '' is invalid in %s on line %d
bool(false)

-- Iteration 16 --

Notice: date_default_timezone_set(): Timezone ID '' is invalid in %s on line %d
bool(false)

-- Iteration 17 --

Notice: date_default_timezone_set(): Timezone ID '' is invalid in %s on line %d
bool(false)

-- Iteration 18 --

Warning: date_default_timezone_set() expects parameter 1 to be string, array given in %s on line %d
bool(false)

-- Iteration 19 --

Notice: date_default_timezone_set(): Timezone ID 'abcxyz' is invalid in %s on line %d
bool(false)

-- Iteration 20 --

Notice: date_default_timezone_set(): Timezone ID 'abcxyz' is invalid in %s on line %d
bool(false)

-- Iteration 21 --

Notice: date_default_timezone_set(): Timezone ID 'abc
xyz' is invalid in %s on line %d
bool(false)

-- Iteration 22 --

Notice: date_default_timezone_set(): Timezone ID '' is invalid in %s on line %d
bool(false)

-- Iteration 23 --

Notice: date_default_timezone_set(): Timezone ID '' is invalid in %s on line %d
bool(false)

-- Iteration 24 --

Warning: date_default_timezone_set() expects parameter 1 to be string, resource given in %s on line %d
bool(false)
===Done===
