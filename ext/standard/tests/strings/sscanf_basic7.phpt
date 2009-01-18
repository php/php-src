--TEST--
Test sscanf() function : basic functionality - octal format
--FILE--
<?php

/* Prototype  : mixed sscanf  ( string $str  , string $format  [, mixed &$...  ] )
 * Description: Parses input from a string according to a format
 * Source code: ext/standard/string.c
*/

echo "*** Testing sscanf() : basic functionality - using octal format ***\n";

$str = "0123 -0123 +0123 0129 -0129 +0129";
$format = "%o %o %o %o %o %o";

echo "\n-- Try sccanf() WITHOUT optional args --\n"; 
// extract details using short format
list($arg1, $arg2, $arg3, $arg4, $arg5, $arg6) = sscanf($str, $format);
var_dump($arg1, $arg2, $arg3, $arg4, $arg5, $arg6);

echo "\n-- Try sccanf() WITH optional args --\n"; 
// extract details using long  format
$res = sscanf($str, $format, $arg1, $arg2, $arg3, $arg4, $arg5, $arg6);
var_dump($res, $arg1, $arg2, $arg3, $arg4, $arg5, $arg6); 

?>
===DONE===
--EXPECT--
*** Testing sscanf() : basic functionality - using octal format ***

-- Try sccanf() WITHOUT optional args --
int(83)
int(-83)
int(83)
int(10)
NULL
NULL

-- Try sccanf() WITH optional args --
int(4)
int(83)
int(-83)
int(83)
int(10)
NULL
NULL
===DONE===
