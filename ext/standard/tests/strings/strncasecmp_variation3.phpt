--TEST--
Test strncasecmp() function: usage variations - various lengths
--FILE--
<?php
/* Prototype  : int strncasecmp ( string $str1, string $str2, int $len );
 * Description: Binary safe case-insensitive string comparison of the first n characters
 * Source code: Zend/zend_builtin_functions.c
*/

/* Test strncasecmp() with various lengths */

echo "*** Test strncasecmp() function: with different lengths ***\n";
/* definitions of required variables */
$str1 = "Hello, World\n";
$str2 = "Hello, world\n";

/* loop through to compare the strings, for various length values */
for($len = strlen($str1); $len >= 0; $len--) {
  var_dump( strncasecmp($str1, $str2, $len) );
}
echo "*** Done ***\n";
?>
--EXPECT--
*** Test strncasecmp() function: with different lengths ***
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
*** Done ***
