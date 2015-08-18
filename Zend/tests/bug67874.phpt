--TEST--
Bug #67874 Crash in array_map()
--FILE--
<?php
$a = array(1,2,3);
$data = array($a);
$data = array_map('current', $data);
var_dump($data);
?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
