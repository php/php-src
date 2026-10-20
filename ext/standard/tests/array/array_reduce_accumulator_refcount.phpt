--TEST--
array_reduce(): accumulator refcount stays low during callback
--FILE--
<?php

$result = array_reduce([1, 2, 3], function ($acc, $val) {
    debug_zval_dump($acc);
    $acc[] = $val;
    return $acc;
}, []);

debug_zval_dump($result);

?>
--EXPECT--
array(0) interned {
}
array(1) packed refcount(2){
  [0]=>
  int(1)
}
array(2) packed refcount(2){
  [0]=>
  int(1)
  [1]=>
  int(2)
}
array(3) packed refcount(2){
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
