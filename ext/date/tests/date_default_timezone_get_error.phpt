--TEST--
Test date_default_timezone_get() function : error conditions
--FILE--
<?php
/* Prototype  : string date_default_timezone_get ( void )
 * Description:  Gets the default timezone used by all date/time functions in a script.
 * Source code: ext/standard/data/php_date.c
 */

date_default_timezone_set("UTC");

echo "*** Testing date_default_timezone_get() : error conditions ***\n";

echo "\n-- Testing date_create() function with more than expected no. of arguments --\n";
$extra_arg = 99;
var_dump( date_default_timezone_get($extra_arg));

?>

===Done===
--EXPECTF--
*** Testing date_default_timezone_get() : error conditions ***

-- Testing date_create() function with more than expected no. of arguments --
string(3) "UTC"

===Done===
