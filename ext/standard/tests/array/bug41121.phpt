--TEST--
Bug #41121 (range() overflow handling for large numbers on 32bit machines)
--INI--
precision=14
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only"); ?>
--FILE--
<?php

// posotive steps
var_dump(range(2147483400, 2147483600, 100));
var_dump( range(2147483646, 2147483648, 1 ) );
var_dump( range(2147483646, 2147483657, 1 ) );
var_dump( range(2147483630, 2147483646, 5 ) );
 
// negative steps  
var_dump( range(-2147483645, -2147483648, 1 ) );
var_dump( range(-2147483645, -2147483649, 1 ) );
var_dump( range(-2147483630, -2147483646, 5 ) );

// low > high
var_dump(range(2147483647, 2147483645, 1 ));
var_dump(range(2147483648, 2147483645, 1 ));

?>
--EXPECT--	
array(3) {
  [0]=>
  int(2147483400)
  [1]=>
  int(2147483500)
  [2]=>
  int(2147483600)
}
array(3) {
  [0]=>
  float(2147483646)
  [1]=>
  float(2147483647)
  [2]=>
  float(2147483648)
}
array(12) {
  [0]=>
  float(2147483646)
  [1]=>
  float(2147483647)
  [2]=>
  float(2147483648)
  [3]=>
  float(2147483649)
  [4]=>
  float(2147483650)
  [5]=>
  float(2147483651)
  [6]=>
  float(2147483652)
  [7]=>
  float(2147483653)
  [8]=>
  float(2147483654)
  [9]=>
  float(2147483655)
  [10]=>
  float(2147483656)
  [11]=>
  float(2147483657)
}
array(4) {
  [0]=>
  int(2147483630)
  [1]=>
  int(2147483635)
  [2]=>
  int(2147483640)
  [3]=>
  int(2147483645)
}
array(4) {
  [0]=>
  float(-2147483645)
  [1]=>
  float(-2147483646)
  [2]=>
  float(-2147483647)
  [3]=>
  float(-2147483648)
}
array(5) {
  [0]=>
  float(-2147483645)
  [1]=>
  float(-2147483646)
  [2]=>
  float(-2147483647)
  [3]=>
  float(-2147483648)
  [4]=>
  float(-2147483649)
}
array(4) {
  [0]=>
  int(-2147483630)
  [1]=>
  int(-2147483635)
  [2]=>
  int(-2147483640)
  [3]=>
  int(-2147483645)
}
array(3) {
  [0]=>
  int(2147483647)
  [1]=>
  int(2147483646)
  [2]=>
  int(2147483645)
}
array(4) {
  [0]=>
  float(2147483648)
  [1]=>
  float(2147483647)
  [2]=>
  float(2147483646)
  [3]=>
  float(2147483645)
}
