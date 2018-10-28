--TEST--
Test strncmp() function: usage variations - case-sensitivity
--FILE--
<?php
/* Prototype  : int strncmp ( string $str1, string $str2, int $len );
 * Description: Binary safe case-sensitive string comparison of the first n characters
 * Source code: Zend/zend_builtin_functions.c
*/

/* Test strncmp() function with upper-case and lower-case alphabets as inputs for 'str1' and 'str2' */

echo "*** Test strncmp() function: with alphabets ***\n";
echo "-- Passing upper-case letters for 'str1' --\n";
for($ASCII = 65; $ASCII <= 90; $ASCII++) {
  var_dump( strncmp( chr($ASCII), chr($ASCII), 1 ) );  //comparing uppercase letters with uppercase letters; exp: int(0)
  var_dump( strncmp( chr($ASCII), chr($ASCII + 32), 1 ) );  //comparing uppercase letters with lowercase letters; exp: value < 0
}

echo "\n-- Passing lower-case letters for 'str1' --\n";
for($ASCII = 97; $ASCII <= 122; $ASCII++) {
  var_dump( strncmp( chr($ASCII), chr($ASCII), 1 ) );  //comparing lowercase letters with lowercase letters; exp: int(0)
  var_dump( strncmp( chr($ASCII), chr($ASCII - 32), 1 ) );  //comparing lowercase letters with uppercase letters; exp: value > 0
}
echo "*** Done ***";
?>
--EXPECTREGEX--
\*\*\* Test strncmp\(\) function: with alphabets \*\*\*
-- Passing upper-case letters for 'str1' --
int\(0\)
int\(-[1-9][0-9]*\)
int\(0\)
int\(-[1-9][0-9]*\)
int\(0\)
int\(-[1-9][0-9]*\)
int\(0\)
int\(-[1-9][0-9]*\)
int\(0\)
int\(-[1-9][0-9]*\)
int\(0\)
int\(-[1-9][0-9]*\)
int\(0\)
int\(-[1-9][0-9]*\)
int\(0\)
int\(-[1-9][0-9]*\)
int\(0\)
int\(-[1-9][0-9]*\)
int\(0\)
int\(-[1-9][0-9]*\)
int\(0\)
int\(-[1-9][0-9]*\)
int\(0\)
int\(-[1-9][0-9]*\)
int\(0\)
int\(-[1-9][0-9]*\)
int\(0\)
int\(-[1-9][0-9]*\)
int\(0\)
int\(-[1-9][0-9]*\)
int\(0\)
int\(-[1-9][0-9]*\)
int\(0\)
int\(-[1-9][0-9]*\)
int\(0\)
int\(-[1-9][0-9]*\)
int\(0\)
int\(-[1-9][0-9]*\)
int\(0\)
int\(-[1-9][0-9]*\)
int\(0\)
int\(-[1-9][0-9]*\)
int\(0\)
int\(-[1-9][0-9]*\)
int\(0\)
int\(-[1-9][0-9]*\)
int\(0\)
int\(-[1-9][0-9]*\)
int\(0\)
int\(-[1-9][0-9]*\)
int\(0\)
int\(-[1-9][0-9]*\)

-- Passing lower-case letters for 'str1' --
int\(0\)
int\([1-9][0-9]*\)
int\(0\)
int\([1-9][0-9]*\)
int\(0\)
int\([1-9][0-9]*\)
int\(0\)
int\([1-9][0-9]*\)
int\(0\)
int\([1-9][0-9]*\)
int\(0\)
int\([1-9][0-9]*\)
int\(0\)
int\([1-9][0-9]*\)
int\(0\)
int\([1-9][0-9]*\)
int\(0\)
int\([1-9][0-9]*\)
int\(0\)
int\([1-9][0-9]*\)
int\(0\)
int\([1-9][0-9]*\)
int\(0\)
int\([1-9][0-9]*\)
int\(0\)
int\([1-9][0-9]*\)
int\(0\)
int\([1-9][0-9]*\)
int\(0\)
int\([1-9][0-9]*\)
int\(0\)
int\([1-9][0-9]*\)
int\(0\)
int\([1-9][0-9]*\)
int\(0\)
int\([1-9][0-9]*\)
int\(0\)
int\([1-9][0-9]*\)
int\(0\)
int\([1-9][0-9]*\)
int\(0\)
int\([1-9][0-9]*\)
int\(0\)
int\([1-9][0-9]*\)
int\(0\)
int\([1-9][0-9]*\)
int\(0\)
int\([1-9][0-9]*\)
int\(0\)
int\([1-9][0-9]*\)
int\(0\)
int\([1-9][0-9]*\)
\*\*\* Done \*\*\*
