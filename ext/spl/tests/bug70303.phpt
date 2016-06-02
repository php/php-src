--TEST--
Bug #70303 (Incorrect constructor reflection for ArrayObject)
--FILE--
<?php
$f = new ReflectionClass('ArrayObject');
$c = $f->getConstructor();
$params = $c->getParameters();
$param = $params[0];
var_dump($param->isOptional());
?>
--EXPECT--
bool(true)
