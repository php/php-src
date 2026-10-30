--TEST--
ReflectionMethod double construct call on Closure::__invoke() does not leak
--FILE--
<?php

function test(ReflectionMethod $r) {
    $r->__construct(function () {}, '__invoke');
}

$r = new ReflectionMethod(function () {}, '__invoke');
for ($i = 0; $i < 10; $i++) {
    test($r);
}

$before = memory_get_usage();
for ($i = 0; $i < 1000; $i++) {
    test($r);
}
$after = memory_get_usage();

var_dump($before === $after);
var_dump($r->name, $r->class);

?>
--EXPECT--
bool(true)
string(8) "__invoke"
string(7) "Closure"
