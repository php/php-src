--TEST--
Bug #76834 (single-char schemes are not recognized)
--FILE--
<?php
$path = "0://google.com/../../INSTALL";
var_dump(file_get_contents($path, false, null, 0, 10));
?>
===DONE===
--EXPECTF--
Warning: file_get_contents(): Unable to find the wrapper "0" - did you forget to enable it when you configured PHP? in %s on line %d
string(10) "For instal"
===DONE===
