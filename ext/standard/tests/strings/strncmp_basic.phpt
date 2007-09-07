--TEST--
Test strncmp() function : basic functionality 
--FILE--
<?php
/* Prototype  : int strncmp ( string $str1, string $str2, int $len );
 * Description: Binary safe case-sensitive string comparison of the first n characters
 * Source code: Zend/zend_builtin_functions.c
*/

/* Test strncmp() function with all three arguments */

echo "*** Test strncmp() function: basic functionality ***\n";
var_dump( strncmp("Hello", "Hello", 5) );  //expected: int(0)
var_dump( strncmp("Hello", "Hi", 5) );  //expected: int(-1)
var_dump( strncmp("Hi", "Hello", 5) );  //expected: int(1)
echo "*** Done ***";
?>
--EXPECTF--
*** Test strncmp() function: basic functionality ***
int(0)
int(-1)
int(1)
*** Done ***
--UEXPECTF--
*** Test strncmp() function: basic functionality ***
int(0)
int(-1)
int(1)
*** Done ***
