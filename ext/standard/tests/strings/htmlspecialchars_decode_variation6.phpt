--TEST--
Test htmlspecialchars_decode() function : usage variations - binary safe
--FILE--
<?php
/*
 * testing whether htmlspecialchars_decode() is binary safe or not
*/

echo "*** Testing htmlspecialchars_decode() : usage variations ***\n";

//various string inputs
$strings = array (
  "\tHello \$world ".chr(0)."\&!)The big brown fox jumped over the\t\f lazy dog\v\n",
  "\tHello \"world\"\t\v \0 This is a valid\t string",
  "This converts\t decimal to \$string".decbin(65)."Hello world",
  "This is a binary\t \v\fstring"
);

//loop through the strings array to check if htmlspecialchars_decode() is binary safe
$iterator = 1;
foreach($strings as $value) {
      echo "-- Iteration $iterator --\n";
      if ($iterator < 4) {
        var_dump( htmlspecialchars_decode($value) );
      } else {
        var_dump( bin2hex(htmlspecialchars_decode($value)));
      }

      $iterator++;
}

echo "Done";
?>
--EXPECTF--
*** Testing htmlspecialchars_decode() : usage variations ***
-- Iteration 1 --
string(65) "	Hello $world %0\&!)The big brown fox jumped over the	 lazy dog
"
-- Iteration 2 --
string(42) "	Hello "world"	 %0 This is a valid	 string"
-- Iteration 3 --
string(51) "This converts	 decimal to $string1000001Hello world"
-- Iteration 4 --
string(52) "5468697320697320612062696e61727909200b0c737472696e67"
Done
