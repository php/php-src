--TEST--
Bug #41527 (WDDX deserialize numeric string array keys)
--SKIPIF--
<?php if (!extension_loaded("wddx")) print "skip"; ?>
--FILE--
<?php
$data = array('01' => 'Zero', '+1' => 'Plus sign', ' 1' => 'Space');

var_dump(wddx_deserialize(wddx_serialize_vars('data')));
?>
--EXPECTF--
Deprecated: Function wddx_serialize_vars() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
array(1) {
  ["data"]=>
  array(3) {
    ["01"]=>
    string(4) "Zero"
    ["+1"]=>
    string(9) "Plus sign"
    [" 1"]=>
    string(5) "Space"
  }
}
