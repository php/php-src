--TEST--
GH-19070 (setlocale($type, NULL) should not be deprecated)
--FILE--
<?php
var_dump(setlocale(LC_ALL, null));
?>
--EXPECTF--
string(%d) "%s"
