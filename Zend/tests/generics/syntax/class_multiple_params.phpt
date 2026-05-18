--TEST--
Generic syntax: class with multiple type parameters
--FILE--
<?php
class Map<K, V> {}
$r = new ReflectionClass('Map');
$ps = $r->getGenericParameters();
echo count($ps), "\n";
foreach ($ps as $p) echo $p->getName(), "\n";
?>
--EXPECT--
2
K
V
