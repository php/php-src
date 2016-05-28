--TEST--
Test array_change_keys() function with duplicate keys
--FILE--
<?php

var_dump(array_change_keys([1, 2, 3], function(){ return 'foo'; }));

var_dump(array_change_keys([1, 2, 3], function(){ return 99; }));

var_dump(array_change_keys(range(1, 10), function($k, $v){ return $k % 2 ? 'foo' : 99; }));

?>
--EXPECTF--
array(1) {
  ["foo"]=>
  int(3)
}
array(1) {
  [99]=>
  int(3)
}
array(2) {
  [99]=>
  int(9)
  ["foo"]=>
  int(10)
}