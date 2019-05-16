--TEST--
Test range() function (variation-1)
--INI--
precision=14
--FILE--
<?php

echo "\n*** Testing range() with various low and high values ***";
$low_arr = array( "ABCD", -10.5555, TRUE, NULL, FALSE, "", array(1,2));
$high_arr = array( "ABCD", -10.5555, TRUE, NULL, FALSE, "", array(1,2));

for( $i = 0; $i < count($low_arr); $i++) {
  for( $j = 0; $j < count($high_arr); $j++) {
    echo @"\n-- creating an array with low = '$low_arr[$i]' and high = '$high_arr[$j]' --\n";
    var_dump( range( $low_arr[$i], $high_arr[$j] ) );
  }
}

echo "\n*** Possible variatins with steps ***\n";
var_dump( range( 1, 5, TRUE ) );
var_dump( range( 1, 5, array(1, 2) ) );

echo "Done\n";
?>
--EXPECT--
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
Done
