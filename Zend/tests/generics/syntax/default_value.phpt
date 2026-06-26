--TEST--
Generic syntax: type parameter with default
--FILE--
<?php
class Box<T = mixed> {}
$p = (new ReflectionClass('Box'))->getGenericParameters()[0];
var_dump($p->hasDefault());
echo $p->getDefault(), "\n";
?>
--EXPECT--
bool(true)
mixed
