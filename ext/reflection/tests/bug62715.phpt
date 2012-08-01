--TEST--
Bug #62715 (ReflectionParameter::isDefaultValueAvailable() wrong result)
--FILE--
<?php

function test(PDO $a = null, $b = 0, array $c) {}
$r = new ReflectionFunction('test');

foreach ($r->getParameters() as $p) {
    var_dump($p->isDefaultValueAvailable());
}

?>
--EXPECT--
bool(true)
bool(true)
bool(false)
