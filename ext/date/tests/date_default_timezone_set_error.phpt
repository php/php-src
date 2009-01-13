--TEST--
Test date_default_timezone_set() function : error variations
--FILE--
<?php
/* Prototype  : bool date_default_timezone_set ( string $timezone_identifier )
 * Description:  Sets the default timezone used by all date/time functions in a script.
 * Source code: ext/standard/data/php_date.c
 */

echo "*** Testing date_default_timezone_set() : error variations ***\n";

echo "\n-- Testing date_default_timezone_set() function with less than expected no. of arguments --\n";
var_dump( date_default_timezone_set() );

echo "\n-- Testing date_default_timezone_set() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump( date_default_timezone_set("GMT", $extra_arg) );

echo "\n-- Testing date_default_timezone_set() function with invalid timezone identifier  --\n";
var_dump( date_default_timezone_set("foo") );

?>
===Done===
--EXPECTF--
*** Testing date_default_timezone_set() : error variations ***

-- Testing date_default_timezone_set() function with less than expected no. of arguments --

Warning: date_default_timezone_set() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)

-- Testing date_default_timezone_set() function with more than expected no. of arguments --

Warning: date_default_timezone_set() expects exactly 1 parameter, 2 given in %s on line %d
bool(false)

-- Testing date_default_timezone_set() function with invalid timezone identifier  --

Notice: date_default_timezone_set(): Timezone ID 'foo' is invalid in %s on line %d
bool(false)
===Done===
