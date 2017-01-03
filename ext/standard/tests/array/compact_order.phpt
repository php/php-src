--TEST--
compact() and hashmap order
--FILE--
<?php

$foo = null;
$bar = null;

var_dump(compact('foo', 'bar'));
var_dump(compact('bar', 'foo'));

?>
--EXPECT--
array(2) {
  ["foo"]=>
  NULL
  ["bar"]=>
  NULL
}
array(2) {
  ["bar"]=>
  NULL
  ["foo"]=>
  NULL
}
