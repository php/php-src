--TEST--
Bug #75318 (The parameter of UConverter::getAliases() is not optional)
--SKIPIF--
<?php
if (!extension_loaded('intl')) die('skip intl extension is not available');
?>
--FILE--
<?php
$rm = new ReflectionMethod('UConverter', 'getAliases');
var_dump($rm->getNumberOfRequiredParameters());
?>
--EXPECT--
int(1)
