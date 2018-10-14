--TEST--
Test nl2br() function : usage variations - double quoted strings for 'str' argument
--FILE--
<?php
/* Prototype  : string nl2br(string $str);
 * Description: Inserts HTML line breaks before all newlines in a string
 * Source code: ext/standard/string.c
*/

/* Test nl2br() function by passing double quoted strings containing various
 *   combinations of new line chars to 'str' argument
*/

echo "*** Testing nl2br() : usage variations ***\n";

$strings = array(
  //new line chars embedded in strings
  "Hello\nWorld",
  "\nHello\nWorld\n",
  "Hello\rWorld",
  "\rHello\rWorld\r",
  "Hello\r\nWorld",
  "\r\nHello\r\nWorld\r\n",

  //one blank line
  "
",

  //two blank lines
  "

",

  //inserted new line in a string
  "Hello
World"
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
--EXPECTF--
*** Testing nl2br() : usage variations ***
-- Iteration 1 --
string(17) "Hello<br />
World"
-- Iteration 2 --
string(31) "<br />
Hello<br />
World<br />
"
-- Iteration 3 --
string(17) "Hello<br />World"
-- Iteration 4 --
string(31) "<br />Hello<br />World<br />"
-- Iteration 5 --
string(18) "Hello<br />
World"
-- Iteration 6 --
string(34) "<br />
Hello<br />
World<br />
"
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
