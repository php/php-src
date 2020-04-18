--TEST--
Test strip_tags() function : usage variations - binary safe checking
--FILE--
<?php
/* Prototype  : string strip_tags(string $str [, string $allowable_tags])
 * Description: Strips HTML and PHP tags from a string
 * Source code: ext/standard/string.c
*/

/*
 * testing whether strip_tags() is binary safe or not
*/

echo "*** Testing strip_tags() : usage variations ***\n";

//various string inputs
$strings = array (
  "<html> I am html string </html>".chr(0)."<?php I am php string ?>",
  "<html> I am html string\0 </html><?php I am php string ?>",
  "<a>I am html string</a>",
  "<html>I am html string</html>".decbin(65)."<?php I am php string?>"
);

//loop through the strings array to check if strip_tags() is binary safe
$iterator = 1;
foreach($strings as $value)
{
      echo "-- Iteration $iterator --\n";
      var_dump( strip_tags($value) );
      $iterator++;
}

echo "Done";
?>
--EXPECT--
*** Testing strip_tags() : usage variations ***
-- Iteration 1 --
string(18) " I am html string "
-- Iteration 2 --
string(18) " I am html string "
-- Iteration 3 --
string(16) "I am html string"
-- Iteration 4 --
string(23) "I am html string1000001"
Done
