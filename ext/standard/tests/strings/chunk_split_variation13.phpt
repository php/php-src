--TEST--
Test chunk_split() function : usage variations - default 'chunklen' with long string as 'str'argument
--FILE--
<?php
/* Prototype  : string chunk_split(string $str [, int $chunklen [, string $ending]])
 * Description: Returns split line
 * Source code: ext/standard/string.c
 * Alias to functions:
*/

/*
* passing long string as 'str' and testing default value of chunklen which is 76
*/

echo "*** Testing chunk_split() : default 'chunklen' with long string 'str' ***\n";

//Initializing variables
$values = array (
  "123456789012345678901234567890123456789012345678901234567890123456789012345678901",
  "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901"
);

//loop through each element of values for 'str' and default value of 'chunklen'
for($count = 0; $count < count($values); $count++) {
  echo "-- Iteration $count --\n";
  var_dump( chunk_split($values[$count]) );
}

echo "Done"
?>
--EXPECTF--
*** Testing chunk_split() : default 'chunklen' with long string 'str' ***
-- Iteration 0 --
string(85) "1234567890123456789012345678901234567890123456789012345678901234567890123456
78901
"
-- Iteration 1 --
string(217) "1234567890123456789012345678901234567890123456789012345678901234567890123456
7890123456789012345678901234567890123456789012345678901234567890123456789012
34567890123456789012345678901234567890123456789012345678901
"
Done
