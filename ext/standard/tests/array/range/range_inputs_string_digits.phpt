--TEST--
Test range() function with string digits
--FILE--
<?php
echo "Only digits\n";
var_dump( range("1", "9") );
var_dump( range("9", "1") );

echo "Only digits and char\n";
var_dump( range("9", "A") );
var_dump( range("A", "9") );
echo "Done\n";
?>
--EXPECT--
Only digits
array(9) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
  [2]=>
  string(1) "3"
  [3]=>
  string(1) "4"
  [4]=>
  string(1) "5"
  [5]=>
  string(1) "6"
  [6]=>
  string(1) "7"
  [7]=>
  string(1) "8"
  [8]=>
  string(1) "9"
}
array(9) {
  [0]=>
  string(1) "9"
  [1]=>
  string(1) "8"
  [2]=>
  string(1) "7"
  [3]=>
  string(1) "6"
  [4]=>
  string(1) "5"
  [5]=>
  string(1) "4"
  [6]=>
  string(1) "3"
  [7]=>
  string(1) "2"
  [8]=>
  string(1) "1"
}
Only digits and char
array(9) {
  [0]=>
  string(1) "9"
  [1]=>
  string(1) ":"
  [2]=>
  string(1) ";"
  [3]=>
  string(1) "<"
  [4]=>
  string(1) "="
  [5]=>
  string(1) ">"
  [6]=>
  string(1) "?"
  [7]=>
  string(1) "@"
  [8]=>
  string(1) "A"
}
array(9) {
  [0]=>
  string(1) "A"
  [1]=>
  string(1) "@"
  [2]=>
  string(1) "?"
  [3]=>
  string(1) ">"
  [4]=>
  string(1) "="
  [5]=>
  string(1) "<"
  [6]=>
  string(1) ";"
  [7]=>
  string(1) ":"
  [8]=>
  string(1) "9"
}
Done
