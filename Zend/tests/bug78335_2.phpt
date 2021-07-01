--TEST--
Bug #78335: Static properties containing cycles report as leak (internal class variant)
--EXTENSIONS--
zend_test
--FILE--
<?php

$foo = [&$foo];
_ZendTestClass::$_StaticProp = $foo;

?>
===DONE===
--EXPECT--
===DONE===
