--TEST--
Test nl_langinfo() function : error conditions  
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) == 'WIN'){  
  die('skip Not for Windows');
}
?>
--FILE--
<?php

/* Prototype  : string nl_langinfo  ( int $item  )
 * Description: Query language and locale information
 * Source code: ext/standard/string.c
*/

echo "*** Testing nl_langinfo() : error conditions ***\n";

echo "\n-- Testing nl_langinfo() function with no arguments --\n";
var_dump( nl_langinfo() );

echo "\n-- Testing nl_langinfo() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump( nl_langinfo(ABDAY_2, $extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing nl_langinfo() : error conditions ***

-- Testing nl_langinfo() function with no arguments --

Warning: nl_langinfo() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing nl_langinfo() function with more than expected no. of arguments --

Warning: nl_langinfo() expects exactly 1 parameter, 2 given in %s on line %d
NULL
===DONE===
