--TEST--
Test strncasecmp() function: usage variations - case-sensitivity
--FILE--
<?php
/* Prototype  : int strncasecmp ( string $str1, string $str2, int $len );
 * Description: Binary safe case-insensitive string comparison of the first n characters
 * Source code: Zend/zend_builtin_functions.c
*/

/* Test strncasecmp() function with upper-case and lower-case alphabets as inputs for 'str1' and 'str2' */

echo "*** Test strncasecmp() function: with alphabets ***\n";
echo "-- Passing upper-case letters for 'str1' --\n";
for($ASCII = 65; $ASCII <= 90; $ASCII++) {
  var_dump( strncasecmp( chr($ASCII), chr($ASCII), 1 ) );  //comparing uppercase letter with corresponding uppercase letter; exp: int(0)
  var_dump( strncasecmp( chr($ASCII), chr($ASCII + 32), 1 ) );  //comparing uppercase letter with corresponding lowercase letter; exp: int(0)
}

echo "\n-- Passing lower-case letters for 'str1' --\n";
for($ASCII = 97; $ASCII <= 122; $ASCII++) {
  var_dump( strncasecmp( chr($ASCII), chr($ASCII), 1 ) );  //comparing lowercase letter with corresponding lowercase letter; exp: int(0)
  var_dump( strncasecmp( chr($ASCII), chr($ASCII - 32), 1 ) );  //comparing lowercase letter with corresponding uppercase letter; exp: int(0)
}
echo "*** Done ***\n";
?>
--EXPECTF--
*** Test strncasecmp() function: with alphabets ***
-- Passing upper-case letters for 'str1' --
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

-- Passing lower-case letters for 'str1' --
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
