--TEST--
Test range() function with basic/expected string inputs
--INI--
serialize_precision=14
--FILE--
<?php

echo "-- Chars as Low and High --\n";
echo "-- An array of elements from low to high --\n";
var_dump( range("a", "z") );
echo "\n-- An array of elements from high to low --\n";
var_dump( range("z", "a") );

echo "\n-- Low and High are equal --\n";
var_dump( range("q", "q") );

echo "\n-- Testing basic string with step --\n";
var_dump( range("a", "h", 2) );

echo "Done\n";
?>
--EXPECT--
-- Chars as Low and High --
-- An array of elements from low to high --
array(26) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
  [3]=>
  string(1) "d"
  [4]=>
  string(1) "e"
  [5]=>
  string(1) "f"
  [6]=>
  string(1) "g"
  [7]=>
  string(1) "h"
  [8]=>
  string(1) "i"
  [9]=>
  string(1) "j"
  [10]=>
  string(1) "k"
  [11]=>
  string(1) "l"
  [12]=>
  string(1) "m"
  [13]=>
  string(1) "n"
  [14]=>
  string(1) "o"
  [15]=>
  string(1) "p"
  [16]=>
  string(1) "q"
  [17]=>
  string(1) "r"
  [18]=>
  string(1) "s"
  [19]=>
  string(1) "t"
  [20]=>
  string(1) "u"
  [21]=>
  string(1) "v"
  [22]=>
  string(1) "w"
  [23]=>
  string(1) "x"
  [24]=>
  string(1) "y"
  [25]=>
  string(1) "z"
}

-- An array of elements from high to low --
array(26) {
  [0]=>
  string(1) "z"
  [1]=>
  string(1) "y"
  [2]=>
  string(1) "x"
  [3]=>
  string(1) "w"
  [4]=>
  string(1) "v"
  [5]=>
  string(1) "u"
  [6]=>
  string(1) "t"
  [7]=>
  string(1) "s"
  [8]=>
  string(1) "r"
  [9]=>
  string(1) "q"
  [10]=>
  string(1) "p"
  [11]=>
  string(1) "o"
  [12]=>
  string(1) "n"
  [13]=>
  string(1) "m"
  [14]=>
  string(1) "l"
  [15]=>
  string(1) "k"
  [16]=>
  string(1) "j"
  [17]=>
  string(1) "i"
  [18]=>
  string(1) "h"
  [19]=>
  string(1) "g"
  [20]=>
  string(1) "f"
  [21]=>
  string(1) "e"
  [22]=>
  string(1) "d"
  [23]=>
  string(1) "c"
  [24]=>
  string(1) "b"
  [25]=>
  string(1) "a"
}

-- Low and High are equal --
array(1) {
  [0]=>
  string(1) "q"
}

-- Testing basic string with step --
array(4) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "c"
  [2]=>
  string(1) "e"
  [3]=>
  string(1) "g"
}
Done
