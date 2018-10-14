--TEST--
Test number_format() - wrong params test number_format()
--FILE--
<?php
/* Prototype  :  string number_format  ( float $number  [, int $decimals  ] )
 *               string number_format ( float $number , int $decimals , string $dec_point , string $thousands_sep )
 * Description: Format a number with grouped thousands
 * Source code: ext/standard/string.c
 */

echo "*** Testing number_format() : error conditions ***\n";

echo "\n-- Testing number_format() function with less than expected no. of arguments --\n";
number_format();

echo "\n-- Testing number_format() function with 3 arguments --\n";
number_format(23,2,true);

echo "\n-- Testing number_format() function with more than 4 arguments --\n";
number_format(23,2,true,false,36);

?>
===DONE===
--EXPECTF--
*** Testing number_format() : error conditions ***

-- Testing number_format() function with less than expected no. of arguments --

Warning: number_format() expects at least 1 parameter, 0 given in %s on line %d

-- Testing number_format() function with 3 arguments --

Warning: Wrong parameter count for number_format() in %s on line %d

-- Testing number_format() function with more than 4 arguments --

Warning: number_format() expects at most 4 parameters, 5 given in %s on line %d
===DONE===
