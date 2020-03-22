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
Deprecated: Required parameter $c follows optional parameter $b in %s on line %d
bool(true)
bool(true)
bool(false)
NULL
int(0)
