--TEST--
Test sscanf() function : basic functionality - exponential format
--FILE--
<?php

/* Prototype  : mixed sscanf  ( string $str  , string $format  [, mixed &$...  ] )
 * Description: Parses input from a string according to a format
 * Source code: ext/standard/string.c
*/

echo "*** Testing sscanf() : basic functionality -using exponential format ***\n";

$str = "10.12345 10.12345E3 10.12345e3 -10.12345e4" ;
$format1 = "%e %e %e %e";
$format2 = "%E %E %E %E";

echo "\n-- Try sccanf() WITHOUT optional args --\n"; 
// extract details using short format
list($arg1, $arg2, $arg3, $arg4) = sscanf($str, $format1);
var_dump($arg1, $arg2, $arg3, $arg4);
list($arg1, $arg2, $arg3, $arg4) = sscanf($str, $format2);
var_dump($arg1, $arg2, $arg3, $arg4);

echo "\n-- Try sccanf() WITH optional args --\n"; 
// extract details using long  format
$res = sscanf($str, $format1, $arg1, $arg2, $arg3, $arg4);
var_dump($res, $arg1, $arg2, $arg3, $arg4); 
$res = sscanf($str, $format2,$arg1, $arg2, $arg3, $arg4);
var_dump($res, $arg1, $arg2, $arg3, $arg4); 


?>
===DONE===
--EXPECT--
*** Testing sscanf() : basic functionality -using exponential format ***

-- Try sccanf() WITHOUT optional args --
float(10.12345)
float(10123.45)
float(10123.45)
float(-101234.5)
float(10.12345)
float(10123.45)
float(10123.45)
float(-101234.5)

-- Try sccanf() WITH optional args --
int(4)
float(10.12345)
float(10123.45)
float(10123.45)
float(-101234.5)
int(4)
float(10.12345)
float(10123.45)
float(10123.45)
float(-101234.5)
===DONE===
