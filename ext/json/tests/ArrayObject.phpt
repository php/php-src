--TEST--
Test json_decode() returns an ArrayObject instance
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php

$result = json_decode('{"ok": "one"}', false, 512, JSON_AS_ARRAY_OBJECT);

echo 'instanceof \stdClass:' . PHP_EOL;
var_dump($result instanceof \ArrayObject);

echo PHP_EOL;
var_dump($result->getArrayCopy());

?>
--EXPECT--
instanceof \stdClass:
bool(true)

array(1) {
  ["ok"]=>
  string(3) "one"
}
