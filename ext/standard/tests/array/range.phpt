--TEST--
Test range() function
--INI--
serialize_precision=14
--FILE--
<?php

echo "*** Testing range() function on basic operations ***\n";

echo "\n-- Integers as Low and High --\n";
echo "-- An array of elements from low to high --\n";
var_dump( range(1, 10) );
echo "\n-- An array of elements from high to low --\n";
var_dump( range(10, 1) );

echo "\n-- Numeric Strings as Low and High --\n";
echo "-- An array of elements from low to high --\n";
var_dump( range("1", "10") );
echo "\n-- An array of elements from high to low --\n";
var_dump( range("10", "1") );

echo "\n-- Chars as Low and High --\n";
echo "-- An array of elements from low to high --\n";
var_dump( range("a", "z") );
echo "\n-- An array of elements from high to low --\n";
var_dump( range("z", "a") );

echo "\n-- Low and High are equal --\n";
var_dump( range(5, 5) );
var_dump( range("q", "q") );

echo "\n-- floats as Low and High --\n";
var_dump( range(5.1, 10.1) );
var_dump( range(10.1, 5.1) );

var_dump( range("5.1", "10.1") );
var_dump( range("10.1", "5.1") );

echo "\n-- Passing step with Low and High --\n";
var_dump( range(1, 2, 0.1) );
var_dump( range(2, 1, 0.1) );

var_dump( range(1, 2, "0.1") );
var_dump( range("1", "2", 0.1) );

echo "\n-- Testing basic string with step --\n";
var_dump( range("abcd", "mnop", 2) );

echo "Done\n";
?>
--EXPECT--
*** Testing range() function on basic operations ***

-- Integers as Low and High --
-- An array of elements from low to high --
array(10) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
  [5]=>
  int(6)
  [6]=>
  int(7)
  [7]=>
  int(8)
  [8]=>
  int(9)
  [9]=>
  int(10)
}

-- An array of elements from high to low --
array(10) {
  [0]=>
  int(10)
  [1]=>
  int(9)
  [2]=>
  int(8)
  [3]=>
  int(7)
  [4]=>
  int(6)
  [5]=>
  int(5)
  [6]=>
  int(4)
  [7]=>
  int(3)
  [8]=>
  int(2)
  [9]=>
  int(1)
}

-- Numeric Strings as Low and High --
-- An array of elements from low to high --
array(10) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
  [5]=>
  int(6)
  [6]=>
  int(7)
  [7]=>
  int(8)
  [8]=>
  int(9)
  [9]=>
  int(10)
}

-- An array of elements from high to low --
array(10) {
  [0]=>
  int(10)
  [1]=>
  int(9)
  [2]=>
  int(8)
  [3]=>
  int(7)
  [4]=>
  int(6)
  [5]=>
  int(5)
  [6]=>
  int(4)
  [7]=>
  int(3)
  [8]=>
  int(2)
  [9]=>
  int(1)
}

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
  int(5)
}
array(1) {
  [0]=>
  string(1) "q"
}

-- floats as Low and High --
array(6) {
  [0]=>
  float(5.1)
  [1]=>
  float(6.1)
  [2]=>
  float(7.1)
  [3]=>
  float(8.1)
  [4]=>
  float(9.1)
  [5]=>
  float(10.1)
}
array(6) {
  [0]=>
  float(10.1)
  [1]=>
  float(9.1)
  [2]=>
  float(8.1)
  [3]=>
  float(7.1)
  [4]=>
  float(6.1)
  [5]=>
  float(5.1)
}
array(6) {
  [0]=>
  float(5.1)
  [1]=>
  float(6.1)
  [2]=>
  float(7.1)
  [3]=>
  float(8.1)
  [4]=>
  float(9.1)
  [5]=>
  float(10.1)
}
array(6) {
  [0]=>
  float(10.1)
  [1]=>
  float(9.1)
  [2]=>
  float(8.1)
  [3]=>
  float(7.1)
  [4]=>
  float(6.1)
  [5]=>
  float(5.1)
}

-- Passing step with Low and High --
array(11) {
  [0]=>
  float(1)
  [1]=>
  float(1.1)
  [2]=>
  float(1.2)
  [3]=>
  float(1.3)
  [4]=>
  float(1.4)
  [5]=>
  float(1.5)
  [6]=>
  float(1.6)
  [7]=>
  float(1.7)
  [8]=>
  float(1.8)
  [9]=>
  float(1.9)
  [10]=>
  float(2)
}
array(11) {
  [0]=>
  float(2)
  [1]=>
  float(1.9)
  [2]=>
  float(1.8)
  [3]=>
  float(1.7)
  [4]=>
  float(1.6)
  [5]=>
  float(1.5)
  [6]=>
  float(1.4)
  [7]=>
  float(1.3)
  [8]=>
  float(1.2)
  [9]=>
  float(1.1)
  [10]=>
  float(1)
}
array(11) {
  [0]=>
  float(1)
  [1]=>
  float(1.1)
  [2]=>
  float(1.2)
  [3]=>
  float(1.3)
  [4]=>
  float(1.4)
  [5]=>
  float(1.5)
  [6]=>
  float(1.6)
  [7]=>
  float(1.7)
  [8]=>
  float(1.8)
  [9]=>
  float(1.9)
  [10]=>
  float(2)
}
array(11) {
  [0]=>
  float(1)
  [1]=>
  float(1.1)
  [2]=>
  float(1.2)
  [3]=>
  float(1.3)
  [4]=>
  float(1.4)
  [5]=>
  float(1.5)
  [6]=>
  float(1.6)
  [7]=>
  float(1.7)
  [8]=>
  float(1.8)
  [9]=>
  float(1.9)
  [10]=>
  float(2)
}

-- Testing basic string with step --
array(7) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "c"
  [2]=>
  string(1) "e"
  [3]=>
  string(1) "g"
  [4]=>
  string(1) "i"
  [5]=>
  string(1) "k"
  [6]=>
  string(1) "m"
}
Done
