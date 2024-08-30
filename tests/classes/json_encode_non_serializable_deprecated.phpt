--TEST--
json_encode() on instances of classes market with ZEND_ACC_NOT_SERIALIZABLE
--FILE--
<?php
function a() { for ($i=0; $i < 10; $i++) { yield $i; }};
$fn = a(...);

echo json_encode(a());
echo json_encode($fn);
?>
--EXPECTF--
Deprecated: json_encode() of non-serializable class Generator is deprecated in %s on line %d
{}
Deprecated: json_encode() of non-serializable class Closure is deprecated in %s on line %d
{}
