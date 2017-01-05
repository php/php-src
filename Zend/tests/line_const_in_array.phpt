--TEST--
Use of __LINE__ in arrays
--FILE--
<?php

var_dump([
    __LINE__,
    __LINE__,
    __LINE__,
]);

?>
--EXPECT--
array(3) {
  [0]=>
  int(4)
  [1]=>
  int(5)
  [2]=>
  int(6)
}
