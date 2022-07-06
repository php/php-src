--TEST--
Bug #78335: Static properties containing cycles report as leak (internal class variant)
--SKIPIF--
<?php if (!extension_loaded("zend-test")) die("skip requires zend-test"); ?>
--FILE--
<?php

$foo = [&$foo];
_ZendTestClass::$_StaticProp = $foo;

?>
===DONE===
--EXPECT--
===DONE===
