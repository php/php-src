--TEST--
Bug #53427 (stream_select does not preserve keys)
--FILE--
<?php
$read[1] = fopen(__FILE__, "r");
$read["myindex"] = reset($read);
$write = NULL;
$except = NULL;

var_dump($read);

stream_select($read, $write, $except, 0);

var_dump($read);
?>
--EXPECTF--
array(2) {
  [1]=>
  resource(%d) of type (stream)
  ["myindex"]=>
  resource(%d) of type (stream)
}
array(2) {
  [1]=>
  resource(%d) of type (stream)
  ["myindex"]=>
  resource(%d) of type (stream)
}
