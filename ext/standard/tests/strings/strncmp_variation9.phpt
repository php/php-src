--TEST--
Test strncmp() function: usage variations - different inputs(heredoc strings)
--FILE--
<?php
/* Prototype  : int strncmp ( string $str1, string $str2, int $len );
 * Description: Binary safe case-sensitive string comparison of the first n characters
 * Source code: Zend/zend_builtin_functions.c
*/

/* Test strncmp() function with different strings for 'str1', 'str2' and considering case sensitive */

echo "*** Test strncmp() function: with different input strings ***\n";

/* heredoc string */
$str1 = <<<EOD
Example of string
spanning multiple lines
using heredoc syntax.
EOD;

/* identifier name contains underscore */
$str2 = <<<identifier_str2
Example of heredoc
string, whose identifier
having underscore("_")
& numeric value.
identifier_str2;

/* identifier name starts with underscore */
$str3 = <<<_identifier_str3
Hello, World
hello, world
_identifier_str3;

/* string containing control characters */
$str4 = <<<identifier_str4
Hello, World\n
Hello\0World
identifier_str4;

$strings = array(
  $str1,
  $str2,
  $str3,
  $str4
);
/* loop through to compare each string with the other string */
$count = 1;
for($index1 = 0; $index1 < count($strings); $index1++) {
  var_dump( strncmp( $strings[$index1], $strings[$index1], strlen($strings[$index1]) ) );
  $count ++;
}
echo "*** Done ***\n";
?>
--EXPECT--
*** Test strncmp() function: with different input strings ***
int(0)
int(0)
int(0)
int(0)
*** Done ***
