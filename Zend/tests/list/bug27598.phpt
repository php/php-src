--TEST--
Bug #27598 (list() array key assignment causes HUGE memory leak)
--FILE--
<?php
list($out[0]) = array(1);
var_dump($out);
?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
