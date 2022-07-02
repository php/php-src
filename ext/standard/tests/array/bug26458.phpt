--TEST--
Bug #26458 (var_dump(), var_export() & debug_zval_dump() are not binary safe for array keys)
--FILE--
<?php
$test = array("A\x00B" => "Hello world");
var_dump($test);
var_export($test);
debug_zval_dump($test);
?>
--EXPECTF--
array(1) {
  ["A%0B"]=>
  string(11) "Hello world"
}
array (
  'A' . "\0" . 'B' => 'Hello world',
)array(1) %s{
  ["A%0B"]=>
  string(11) "Hello world" %s
}
