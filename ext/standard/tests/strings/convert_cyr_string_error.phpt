--TEST--
Test convert_cyr_string() function : error conditions
--FILE--
<?php

/* Prototype  : string convert_cyr_string  ( string $str  , string $from  , string $to  )
 * Description: Convert from one Cyrillic character set to another
 * Source code: ext/standard/string.c
*/

$str = "hello";
$from = "k";
$to = "d";
$extra_arg = 10;

echo "*** Testing convert_cyr_string() : error conditions ***\n";

echo "\n-- Testing convert_cyr_string() function with invalid 'from' character set --\n";
var_dump(bin2hex( convert_cyr_string($str, "?", $to) ));

echo "\n-- Testing convert_cyr_string() function with invalid 'to' character set --\n";
var_dump(bin2hex( convert_cyr_string($str, $from, "?")) );

echo "\n-- Testing convert_cyr_string() function with invalid 'from' and 'to' character set --\n";
var_dump(bin2hex( convert_cyr_string($str, ">", "?")) );

?>
===DONE===
--EXPECTF--
*** Testing convert_cyr_string() : error conditions ***

-- Testing convert_cyr_string() function with invalid 'from' character set --

Warning: convert_cyr_string(): Unknown source charset: ? in %s on line %d
string(10) "68656c6c6f"

-- Testing convert_cyr_string() function with invalid 'to' character set --

Warning: convert_cyr_string(): Unknown destination charset: ? in %s on line %d
string(10) "68656c6c6f"

-- Testing convert_cyr_string() function with invalid 'from' and 'to' character set --

Warning: convert_cyr_string(): Unknown source charset: > in %s on line %d

Warning: convert_cyr_string(): Unknown destination charset: ? in %s on line %d
string(10) "68656c6c6f"
===DONE===
