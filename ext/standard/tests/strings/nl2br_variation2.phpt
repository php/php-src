--TEST--
Test nl2br() function : usage variations - single quoted strings for 'str' argument
--FILE--
<?php
/* Prototype  : string nl2br(string $str);
 * Description: Inserts HTML line breaks before all newlines in a string
 * Source code: ext/standard/string.c
*/

/* Test nl2br() function by passing single quoted strings containing various
 *   combinations of new line chars to 'str' argument
*/

echo "*** Testing nl2br() : usage variations ***\n";
$strings = array(
  '\n',
  '\r',
  '\r\n',
  'Hello\nWorld',
  'Hello\rWorld',
  'Hello\r\nWorld',

  //one blank line
  '
',

  //two blank lines
  '

',

  //inserted new line
  'Hello
World'
);

//loop through $strings array to test nl2br() function with each element
$count = 1;
foreach( $strings as $str ){
  echo "-- Iteration $count --\n";
  var_dump(nl2br($str) );
  $count ++ ;
}
echo "Done";
?>
--EXPECT--
*** Testing nl2br() : usage variations ***
-- Iteration 1 --
string(2) "\n"
-- Iteration 2 --
string(2) "\r"
-- Iteration 3 --
string(4) "\r\n"
-- Iteration 4 --
string(12) "Hello\nWorld"
-- Iteration 5 --
string(12) "Hello\rWorld"
-- Iteration 6 --
string(14) "Hello\r\nWorld"
-- Iteration 7 --
string(7) "<br />
"
-- Iteration 8 --
string(14) "<br />
<br />
"
-- Iteration 9 --
string(17) "Hello<br />
World"
Done
