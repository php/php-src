--TEST--
GH-16504: Segmentation fault when failing to clone
--FILE--
<?php

class C {
    public $a = FOO;
}

$reflection = new ReflectionClass(C::class);

echo "Proxy:\n";
$proxy = $reflection->newLazyProxy(function () {});
try {
    clone $proxy;
} catch ( Error $e ) {
    echo $e;
}

echo "\n\nGhost:\n";
$ghost = $reflection->newLazyGhost(function () {});
try {
    clone $ghost;
} catch ( Error $e ) {
    echo $e;
}

?>
--EXPECTF--
Proxy:
Error: Undefined constant "FOO" in %s:%d
Stack trace:
#0 {main}

Next Error: Undefined constant "FOO" in %s:%d
Stack trace:
#0 {main}

Ghost:
Error: Undefined constant "FOO" in %s:%d
Stack trace:
#0 {main}

Next Error: Undefined constant "FOO" in %s:%d
Stack trace:
#0 {main}
