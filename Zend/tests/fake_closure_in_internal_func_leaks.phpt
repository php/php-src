--TEST--
Fake closure called from internal function leaks
--FILE--
<?php

$c = \is_array(...);
var_dump(array_filter([[]], $c));

?>
--EXPECT--
array(1) {
  [0]=>
  array(0) {
  }
}
