--TEST--
Bug #69196 (PharData should extends Phar class)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php

$r = new ReflectionClass('PharData');
var_dump($r->getParentClass()->getName());

?>
--EXPECT--
string(4) "Phar"