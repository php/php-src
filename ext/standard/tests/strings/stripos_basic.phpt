--TEST--
Test stripos() function : basic functionality 
--FILE--
<?php
/* Prototype  : int stripos ( string $haystack, string $needle [, int $offset] );
 * Description: Find position of first occurrence of a case-insensitive string
 * Source code: ext/standard/string.c
*/

/* Test stripos() function by providing all the parameters */

echo "*** Test stripos() : basic functionality ***\n";
/* Initialize all required variables */
$haystack = 'string';
$needle = 'ring';
$offset = 3;
/* Calling stripos() with default arguments */
var_dump( stripos($haystack, $needle) );
/* Calling stripos() with all arguments */
var_dump( stripos($haystack, $needle, $offset) );
echo "*** Done ***\n";
?>
--EXPECTF--
*** Test stripos() : basic functionality ***
int(2)
bool(false)
*** Done ***
