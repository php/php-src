--TEST--
Test strncasecmp() function: usage variations - heredoc strings
--FILE--
<?php
/* Prototype  : int strncasecmp ( string $str1, string $str2, int $len );
 * Description: Binary safe case-insensitive string comparison of the first n characters
 * Source code: Zend/zend_builtin_functions.c
*/

/* Test strncasecmp() function with here-doc strings for 'str1', 'str2' */

echo "*** Test strncasecmp() function: with here-doc strings ***\n";

/* multi line heredoc string */
$multi_line_str = <<<EOD
Example of string
spanning multiple lines
using heredoc syntax.
EOD;

/* identifier name contains underscore */
$identifier_str1 = <<<identifier_str1
Example of heredoc
string, whose identifier
having underscore("_") 
& numeric value.
identifier_str1;

/* identifier name starts with underscore */
$identifier_str2 = <<<_identifier_str2
Hello, World
hello, world
_identifier_str2;

/* string containing control character */
$control_char_str = <<<EOD
Hello, World\n
Hello\0World
EOD;

/* heredoc string with quote chars & slash */
$quote_char_string = <<<EOD
it's bright,but i cann't see it.
"things in double quote"
'things in single quote'
this\line is /with\slashs
EOD;

/* heredoc string with blank line */
$blank_line = <<<EOD

EOD;

/* empty heredoc string */
$empty_string = <<<EOD
EOD;

$strings = array(
  $multi_line_str,
  $identifier_str1,
  $identifier_str2,
  $control_char_str,
  $quote_char_string,
  $blank_line,
  $empty_string
);
/* loop through to compare the strings */
$index2 = count($strings);
for($index1 = 0; $index1 < count($strings); $index1++) {
  $index2--;
  var_dump( strncasecmp( $strings[$index1], $strings[$index1], strlen($strings[$index1]) ) );
  var_dump( strncasecmp( $strings[$index1], $strings[$index2], strlen($strings[$index1]) ) );
}
echo "*** Done ***\n";
?>
--EXPECTF--
*** Test strncasecmp() function: with here-doc strings ***
int(0)
int(63)
int(0)
int(84)
int(0)
int(-1)
int(0)
int(0)
int(0)
int(1)
int(0)
int(0)
int(0)
int(0)
*** Done ***
