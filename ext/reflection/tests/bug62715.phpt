--TEST--
Bug #62715 (ReflectionParameter::isDefaultValueAvailable() wrong result)
--FILE--
<?php

function test(PDO $a = null, $b = 0, array $c) {}
$r = new ReflectionFunction('test');

foreach ($r->getParameters() as $p) {
    var_dump($p->isDefaultValueAvailable());
}

foreach ($r->getParameters() as $p) {
    if ($p->isDefaultValueAvailable()) {
        var_dump($p->getDefaultValue());
    }
}
?>
--EXPECTF--
Deprecated: Optional parameter $b declared before required parameter $c is implicitly treated as a required parameter in %s on line %d
bool(false)
bool(false)
bool(false)
