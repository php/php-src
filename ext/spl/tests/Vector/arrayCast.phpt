--TEST--
Vector to array
--FILE--
<?php
// discards keys
$vec = new Vector([strtoupper('test')]);
var_dump((array)$vec);
$vec[0] = strtoupper('test2');
var_dump((array)$vec);
var_dump($vec);
$vec->pop();
var_dump($vec);


?>
--EXPECT--
array(1) {
  [0]=>
  string(4) "TEST"
}
array(1) {
  [0]=>
  string(5) "TEST2"
}
object(Vector)#1 (1) {
  [0]=>
  string(5) "TEST2"
}
object(Vector)#1 (0) {
}
