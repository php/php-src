--TEST--
Test hrtime() return array
--FILE--
<?php

var_dump(hrtime());

?>
--EXPECTF--
array(2) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
}
