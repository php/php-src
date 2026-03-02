--TEST--
Bug #75318 (The parameter of UConverter::getAliases() is not optional)
--EXTENSIONS--
intl
--FILE--
<?php
$rm = new ReflectionMethod('UConverter', 'getAliases');
var_dump($rm->getNumberOfRequiredParameters());
?>
--EXPECT--
int(1)
