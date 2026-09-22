--TEST--
GH-17409 (Assertion failure Zend/zend_hash.c)
--EXTENSIONS--
simplexml
--CREDITS--
YuanchengJiang
--FILE--
<?php
$root = simplexml_load_string('<?xml version="1.0"?>
<root xmlns:reserved="reserved-ns">
<child reserved:attribute="Sample" />
</root>
');
// Need to use $GLOBALS such that simplexml object is destroyed
var_dump(array_merge_recursive($GLOBALS, $GLOBALS)["root"]);
?>
--EXPECT--
array(1) {
  ["child"]=>
  array(0) {
  }
}
