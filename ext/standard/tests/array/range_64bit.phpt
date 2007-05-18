--TEST--
Test range() function
--INI--
precision=14
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
/* Prototype: array range ( mixed $low, mixed $high [, number $step] );
   Description: Create an array containing a range of elements
*/

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

echo "\n*** Testing range() with various low and high values ***";
$low_arr = array( "ABCD", -10.5555, TRUE, NULL, FALSE, "", array(1,2));
$high_arr = array( "ABCD", -10.5555, TRUE, NULL, FALSE, "", array(1,2));

for( $i = 0; $i < count($low_arr); $i++) {
  for( $j = 0; $j < count($high_arr); $j++) {
    echo "\n-- creating an array with low = '$low_arr[$i]' and high = '$high_arr[$j]' --\n";
    var_dump( range( $low_arr[$i], $high_arr[$j] ) );
  }
}


echo "\n*** Possible variatins with steps ***\n";
var_dump( range( 1, 5, TRUE ) );
var_dump( range( 1, 5, array(1, 2) ) );

echo "\n*** Testing max/outof range values ***\n";
/*var_dump( range("a", "z", 255) );
var_dump( range("z", "a", 255) ); */
var_dump( range(2147483645, 2147483646) );
var_dump( range(2147483646, 2147483648) );
var_dump( range(-2147483647, -2147483646) );
var_dump( range(-2147483648, -2147483647) );
var_dump( range(-2147483649, -2147483647) );

echo "\n*** Testing error conditions ***\n";

echo "\n-- Testing ( (low < high) && (step = 0) ) --"; 
var_dump( range(1, 2, 0) );
var_dump( range("a", "b", 0) );

echo "\n\n-- Testing ( (low > high) && (step = 0) ) --";
var_dump( range(2, 1, 0) );
var_dump( range("b", "a", 0) );

echo "\n\n-- Testing ( (low < high) && (high-low < step) ) --";
var_dump( range(1.0, 7.0, 6.5) );

echo "\n\n-- Testing ( (low > high) && (low-high < step) ) --";
var_dump( range(7.0, 1.0, 6.5) );

echo "\n-- Testing Invalid number of arguments --";        
var_dump( range() );  // No.of args = 0
var_dump( range(1) );  // No.of args < expected
var_dump( range(1,2,3,4) );  // No.of args > expected
var_dump( range(-1, -2, 2) );  
var_dump( range("a", "j", "z") );

echo "\n-- Testing Invalid steps --";
$step_arr = array( "string", NULL, FALSE, "", "\0" );

foreach( $step_arr as $step ) {
  var_dump( range( 1, 5, $step ) );
}

echo "\nDone";
?>
--EXPECTF--
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

*** Testing range() with various low and high values ***
-- creating an array with low = 'ABCD' and high = 'ABCD' --
array(1) {
  [0]=>
  string(1) "A"
}

-- creating an array with low = 'ABCD' and high = '-10.5555' --
array(11) {
  [0]=>
  float(0)
  [1]=>
  float(-1)
  [2]=>
  float(-2)
  [3]=>
  float(-3)
  [4]=>
  float(-4)
  [5]=>
  float(-5)
  [6]=>
  float(-6)
  [7]=>
  float(-7)
  [8]=>
  float(-8)
  [9]=>
  float(-9)
  [10]=>
  float(-10)
}

-- creating an array with low = 'ABCD' and high = '1' --
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}

-- creating an array with low = 'ABCD' and high = '' --
array(1) {
  [0]=>
  int(0)
}

-- creating an array with low = 'ABCD' and high = '' --
array(1) {
  [0]=>
  int(0)
}

-- creating an array with low = 'ABCD' and high = '' --
array(1) {
  [0]=>
  int(0)
}

-- creating an array with low = 'ABCD' and high = 'Array' --
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}

-- creating an array with low = '-10.5555' and high = 'ABCD' --
array(11) {
  [0]=>
  float(-10.5555)
  [1]=>
  float(-9.5555)
  [2]=>
  float(-8.5555)
  [3]=>
  float(-7.5555)
  [4]=>
  float(-6.5555)
  [5]=>
  float(-5.5555)
  [6]=>
  float(-4.5555)
  [7]=>
  float(-3.5555)
  [8]=>
  float(-2.5555)
  [9]=>
  float(-1.5555)
  [10]=>
  float(-0.5555)
}

-- creating an array with low = '-10.5555' and high = '-10.5555' --
array(1) {
  [0]=>
  float(-10.5555)
}

-- creating an array with low = '-10.5555' and high = '1' --
array(12) {
  [0]=>
  float(-10.5555)
  [1]=>
  float(-9.5555)
  [2]=>
  float(-8.5555)
  [3]=>
  float(-7.5555)
  [4]=>
  float(-6.5555)
  [5]=>
  float(-5.5555)
  [6]=>
  float(-4.5555)
  [7]=>
  float(-3.5555)
  [8]=>
  float(-2.5555)
  [9]=>
  float(-1.5555)
  [10]=>
  float(-0.5555)
  [11]=>
  float(0.4445)
}

-- creating an array with low = '-10.5555' and high = '' --
array(11) {
  [0]=>
  float(-10.5555)
  [1]=>
  float(-9.5555)
  [2]=>
  float(-8.5555)
  [3]=>
  float(-7.5555)
  [4]=>
  float(-6.5555)
  [5]=>
  float(-5.5555)
  [6]=>
  float(-4.5555)
  [7]=>
  float(-3.5555)
  [8]=>
  float(-2.5555)
  [9]=>
  float(-1.5555)
  [10]=>
  float(-0.5555)
}

-- creating an array with low = '-10.5555' and high = '' --
array(11) {
  [0]=>
  float(-10.5555)
  [1]=>
  float(-9.5555)
  [2]=>
  float(-8.5555)
  [3]=>
  float(-7.5555)
  [4]=>
  float(-6.5555)
  [5]=>
  float(-5.5555)
  [6]=>
  float(-4.5555)
  [7]=>
  float(-3.5555)
  [8]=>
  float(-2.5555)
  [9]=>
  float(-1.5555)
  [10]=>
  float(-0.5555)
}

-- creating an array with low = '-10.5555' and high = '' --
array(11) {
  [0]=>
  float(-10.5555)
  [1]=>
  float(-9.5555)
  [2]=>
  float(-8.5555)
  [3]=>
  float(-7.5555)
  [4]=>
  float(-6.5555)
  [5]=>
  float(-5.5555)
  [6]=>
  float(-4.5555)
  [7]=>
  float(-3.5555)
  [8]=>
  float(-2.5555)
  [9]=>
  float(-1.5555)
  [10]=>
  float(-0.5555)
}

-- creating an array with low = '-10.5555' and high = 'Array' --
array(12) {
  [0]=>
  float(-10.5555)
  [1]=>
  float(-9.5555)
  [2]=>
  float(-8.5555)
  [3]=>
  float(-7.5555)
  [4]=>
  float(-6.5555)
  [5]=>
  float(-5.5555)
  [6]=>
  float(-4.5555)
  [7]=>
  float(-3.5555)
  [8]=>
  float(-2.5555)
  [9]=>
  float(-1.5555)
  [10]=>
  float(-0.5555)
  [11]=>
  float(0.4445)
}

-- creating an array with low = '1' and high = 'ABCD' --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(0)
}

-- creating an array with low = '1' and high = '-10.5555' --
array(12) {
  [0]=>
  float(1)
  [1]=>
  float(0)
  [2]=>
  float(-1)
  [3]=>
  float(-2)
  [4]=>
  float(-3)
  [5]=>
  float(-4)
  [6]=>
  float(-5)
  [7]=>
  float(-6)
  [8]=>
  float(-7)
  [9]=>
  float(-8)
  [10]=>
  float(-9)
  [11]=>
  float(-10)
}

-- creating an array with low = '1' and high = '1' --
array(1) {
  [0]=>
  int(1)
}

-- creating an array with low = '1' and high = '' --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(0)
}

-- creating an array with low = '1' and high = '' --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(0)
}

-- creating an array with low = '1' and high = '' --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(0)
}

-- creating an array with low = '1' and high = 'Array' --
array(1) {
  [0]=>
  int(1)
}

-- creating an array with low = '' and high = 'ABCD' --
array(1) {
  [0]=>
  int(0)
}

-- creating an array with low = '' and high = '-10.5555' --
array(11) {
  [0]=>
  float(0)
  [1]=>
  float(-1)
  [2]=>
  float(-2)
  [3]=>
  float(-3)
  [4]=>
  float(-4)
  [5]=>
  float(-5)
  [6]=>
  float(-6)
  [7]=>
  float(-7)
  [8]=>
  float(-8)
  [9]=>
  float(-9)
  [10]=>
  float(-10)
}

-- creating an array with low = '' and high = '1' --
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}

-- creating an array with low = '' and high = '' --
array(1) {
  [0]=>
  int(0)
}

-- creating an array with low = '' and high = '' --
array(1) {
  [0]=>
  int(0)
}

-- creating an array with low = '' and high = '' --
array(1) {
  [0]=>
  int(0)
}

-- creating an array with low = '' and high = 'Array' --
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}

-- creating an array with low = '' and high = 'ABCD' --
array(1) {
  [0]=>
  int(0)
}

-- creating an array with low = '' and high = '-10.5555' --
array(11) {
  [0]=>
  float(0)
  [1]=>
  float(-1)
  [2]=>
  float(-2)
  [3]=>
  float(-3)
  [4]=>
  float(-4)
  [5]=>
  float(-5)
  [6]=>
  float(-6)
  [7]=>
  float(-7)
  [8]=>
  float(-8)
  [9]=>
  float(-9)
  [10]=>
  float(-10)
}

-- creating an array with low = '' and high = '1' --
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}

-- creating an array with low = '' and high = '' --
array(1) {
  [0]=>
  int(0)
}

-- creating an array with low = '' and high = '' --
array(1) {
  [0]=>
  int(0)
}

-- creating an array with low = '' and high = '' --
array(1) {
  [0]=>
  int(0)
}

-- creating an array with low = '' and high = 'Array' --
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}

-- creating an array with low = '' and high = 'ABCD' --
array(1) {
  [0]=>
  int(0)
}

-- creating an array with low = '' and high = '-10.5555' --
array(11) {
  [0]=>
  float(0)
  [1]=>
  float(-1)
  [2]=>
  float(-2)
  [3]=>
  float(-3)
  [4]=>
  float(-4)
  [5]=>
  float(-5)
  [6]=>
  float(-6)
  [7]=>
  float(-7)
  [8]=>
  float(-8)
  [9]=>
  float(-9)
  [10]=>
  float(-10)
}

-- creating an array with low = '' and high = '1' --
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}

-- creating an array with low = '' and high = '' --
array(1) {
  [0]=>
  int(0)
}

-- creating an array with low = '' and high = '' --
array(1) {
  [0]=>
  int(0)
}

-- creating an array with low = '' and high = '' --
array(1) {
  [0]=>
  int(0)
}

-- creating an array with low = '' and high = 'Array' --
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}

-- creating an array with low = 'Array' and high = 'ABCD' --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(0)
}

-- creating an array with low = 'Array' and high = '-10.5555' --
array(12) {
  [0]=>
  float(1)
  [1]=>
  float(0)
  [2]=>
  float(-1)
  [3]=>
  float(-2)
  [4]=>
  float(-3)
  [5]=>
  float(-4)
  [6]=>
  float(-5)
  [7]=>
  float(-6)
  [8]=>
  float(-7)
  [9]=>
  float(-8)
  [10]=>
  float(-9)
  [11]=>
  float(-10)
}

-- creating an array with low = 'Array' and high = '1' --
array(1) {
  [0]=>
  int(1)
}

-- creating an array with low = 'Array' and high = '' --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(0)
}

-- creating an array with low = 'Array' and high = '' --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(0)
}

-- creating an array with low = 'Array' and high = '' --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(0)
}

-- creating an array with low = 'Array' and high = 'Array' --
array(1) {
  [0]=>
  int(1)
}

*** Possible variatins with steps ***
array(5) {
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
}
array(5) {
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
}

*** Testing max/outof range values ***
array(2) {
  [0]=>
  int(2147483645)
  [1]=>
  int(2147483646)
}
array(3) {
  [0]=>
  int(2147483646)
  [1]=>
  int(2147483647)
  [2]=>
  int(2147483648)
}
array(2) {
  [0]=>
  int(-2147483647)
  [1]=>
  int(-2147483646)
}
array(2) {
  [0]=>
  int(-2147483648)
  [1]=>
  int(-2147483647)
}
array(3) {
  [0]=>
  int(-2147483649)
  [1]=>
  int(-2147483648)
  [2]=>
  int(-2147483647)
}

*** Testing error conditions ***

-- Testing ( (low < high) && (step = 0) ) --
Warning: range(): step exceeds the specified range in %s on line %d
bool(false)

Warning: range(): step exceeds the specified range in %s on line %d
bool(false)


-- Testing ( (low > high) && (step = 0) ) --
Warning: range(): step exceeds the specified range in %s on line %d
bool(false)

Warning: range(): step exceeds the specified range in %s on line %d
bool(false)


-- Testing ( (low < high) && (high-low < step) ) --
Warning: range(): step exceeds the specified range in %s on line %d
bool(false)


-- Testing ( (low > high) && (low-high < step) ) --
Warning: range(): step exceeds the specified range in %s on line %d
bool(false)

-- Testing Invalid number of arguments --
Warning: range() expects at least 2 parameters, 0 given in %s on line %d
bool(false)

Warning: range() expects at least 2 parameters, 1 given in %s on line %d
bool(false)

Warning: range() expects at most 3 parameters, 4 given in %s on line %d
bool(false)

Warning: range(): step exceeds the specified range in %s on line %d
bool(false)

Warning: range(): step exceeds the specified range in %s on line %d
bool(false)

-- Testing Invalid steps --
Warning: range(): step exceeds the specified range in %s on line %d
bool(false)

Warning: range(): step exceeds the specified range in %s on line %d
bool(false)

Warning: range(): step exceeds the specified range in %s on line %d
bool(false)

Warning: range(): step exceeds the specified range in %s on line %d
bool(false)

Warning: range(): step exceeds the specified range in %s on line %d
bool(false)

Done
