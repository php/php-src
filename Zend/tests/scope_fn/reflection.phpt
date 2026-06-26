--TEST--
Reflection on scope function closures
--FILE--
<?php
function test() {
    $fn = fn(int $a, string $b = "hi"): bool { return true; };

    $r = new ReflectionFunction($fn);
    var_dump($r->isClosure());
    var_dump($r->isScopeFunction());
    var_dump($r->getNumberOfParameters());
    var_dump($r->getNumberOfRequiredParameters());
    var_dump($r->getReturnType()->getName());

    // Regular closure should return false for isScopeFunction
    $regular = function() {};
    $r2 = new ReflectionFunction($regular);
    var_dump($r2->isScopeFunction());

    // Arrow function should return false too
    $arrow = fn() => 1;
    $r3 = new ReflectionFunction($arrow);
    var_dump($r3->isScopeFunction());
}
test();
?>
--EXPECT--
bool(true)
bool(true)
int(2)
int(1)
string(4) "bool"
bool(false)
bool(false)
