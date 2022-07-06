--TEST--
Bug #79919 (Stack use-after-scope in define())
--SKIPIF--
<?php
if (!extension_loaded('simplexml')) die('skip simplexml extension not available');
?>
--FILE--
<?php
$b = error_log(0);
$b = simplexml_load_string('<xml/>', null, $b);
define(0, $b);
?>
--EXPECT--
0
