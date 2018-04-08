--TEST--
Bug #75055 Out-Of-Bounds Read in timelib_meridian()
--SKIPIF--
<?php if (!extension_loaded("wddx")) print "skip"; ?>
--FILE--
<?php

$file_str = dirname(__FILE__) . "/bug75055.wddx";

$wddx_str = file_get_contents($file_str);
print strlen($wddx_str) . " bytes read.\n";

var_dump(wddx_deserialize($wddx_str));
?>
--EXPECT--
323 bytes read.
array(1) {
  ["aDateTime"]=>
  string(12) "frONt of 0 0"
}
