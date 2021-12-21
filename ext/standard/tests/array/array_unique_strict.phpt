--TEST--
Test array_unique() function : SORT_STRICT functionality
--FILE--
<?php
echo "*** Testing array_unique() : SORT_STRICT functionality ***\n";

var_dump( array_unique( [ "1234", 1234 ] ) );
var_dump( array_unique( [ "1234", "1234" ], SORT_STRICT ) );
var_dump( array_unique( [ "1234", 1234 ], SORT_STRICT ) );

var_dump( array_unique( [ 0, "0", 0.0, "0.0", '', null, null ] ) );
var_dump( array_unique( [ 0, "0", 0.0, "0.0", '', null, null ], SORT_STRICT ) );

// These are more values which are == but not ===
$a = (object)[];
$b = (object)[];
$a2 = [ $a ];
$b2 = [ $b ];
$a3 = (object)[ 'foo' => $a ];
$b3 = (object)[ 'foo' => $b ];
var_dump( $a == $b && $a2 == $b2 && $a3 == $b3 );
var_dump( $a === $b || $a2 === $b2 || $a3 === $b3 );

var_dump( count( array_unique( [ $a, $b, $a2, $b2, $a3, $b3 ], SORT_STRICT ) ) );

?>
--EXPECT--
*** Testing array_unique() : SORT_STRICT functionality ***
array(1) {
  [0]=>
  string(4) "1234"
}
array(1) {
  [0]=>
  string(4) "1234"
}
array(2) {
  [0]=>
  string(4) "1234"
  [1]=>
  int(1234)
}
array(3) {
  [0]=>
  int(0)
  [3]=>
  string(3) "0.0"
  [4]=>
  string(0) ""
}
array(6) {
  [0]=>
  int(0)
  [1]=>
  string(1) "0"
  [2]=>
  float(0)
  [3]=>
  string(3) "0.0"
  [4]=>
  string(0) ""
  [5]=>
  NULL
}
bool(true)
bool(false)
int(6)
