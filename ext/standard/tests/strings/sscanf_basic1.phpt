--TEST--
Test sscanf() function : basic functionality - string format
--FILE--
<?php
/* Prototype  : mixed sscanf  ( string $str  , string $format  [, mixed &$...  ] )
 * Description: Parses input from a string according to a format
 * Source code: ext/standard/string.c
*/

/*
 * Testing sscanf() : basic functionality
*/

echo "*** Testing sscanf() : basic functionality - using string format ***\n";

$str = "Part: Widget Serial Number: 1234789 Stock: 25";
$format = "Part: %s Serial Number: %s Stock: %s";

echo "\n-- Try sccanf() WITHOUT optional args --\n";
// extract details using short format
list($part, $number, $stock) = sscanf($str, $format);
var_dump($part, $number, $stock);

echo "\n-- Try sccanf() WITH optional args --\n";
// extract details using long  format
$res = sscanf($str, $format, $part, $number, $stock);
var_dump($res, $part, $number, $stock);

?>
===DONE===
--EXPECT--
*** Testing sscanf() : basic functionality - using string format ***

-- Try sccanf() WITHOUT optional args --
string(6) "Widget"
string(7) "1234789"
string(2) "25"

-- Try sccanf() WITH optional args --
int(3)
string(6) "Widget"
string(7) "1234789"
string(2) "25"
===DONE===
