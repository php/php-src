--TEST--
Bug #46781 (BC math handles minus zero incorrectly)
--SKIPIF--
<?php
if (!extension_loaded('bcmath')) die('skip bcmath extension is not available');
?>
--FILE--
<?php
var_dump(bcadd('-0.0', '-0.0', 1));
var_dump(bccomp('-0.0', '0', 1));
?>
===DONE===
--EXPECT--
string(3) "0.0"
int(0)
===DONE===
