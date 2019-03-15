--TEST--
Bug #75055 Out-Of-Bounds Read in timelib_meridian()
--SKIPIF--
<?php if (!extension_loaded("wddx")) print "skip"; ?>
--FILE--
<?php

$file_str = __DIR__ . "/bug75055.wddx";

$wddx_str = file_get_contents($file_str);
print strlen($wddx_str) . " bytes read.\n";

var_dump(wddx_deserialize($wddx_str));
?>
--EXPECTF--
323 bytes read.

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
array(1) {
  ["aDateTime"]=>
  string(12) "frONt of 0 0"
}
