--TEST--
GH-21362 (ReflectionMethod::invokeArgs() for Closure::__invoke() accepts objects from different Closures)
--FILE--
<?php

$c1 = function ($foo, $bar) {
    echo "c1: foo={$foo}, bar={$bar}\n";
};

$c2 = function ($bar, $foo) {
    echo "c2: foo={$foo}, bar={$bar}\n";
};

$m = new ReflectionMethod($c1, '__invoke');

// invokeArgs with the correct Closure should work
$m->invokeArgs($c1, ['foo' => 'FOO', 'bar' => 'BAR']);

// invokeArgs with a different Closure should throw
try {
    $m->invokeArgs($c2, ['foo' => 'FOO', 'bar' => 'BAR']);
    echo "No exception thrown\n";
} catch (ReflectionException $e) {
    echo $e->getMessage() . "\n";
}

// invoke with a different Closure should also throw
try {
    $m->invoke($c2, 'FOO', 'BAR');
    echo "No exception thrown\n";
} catch (ReflectionException $e) {
    echo $e->getMessage() . "\n";
}

// Closures from the same source (e.g. loop) share the same op_array
// and should be allowed to invoke each other's ReflectionMethod
$closures = [];
for ($i = 0; $i < 3; $i++) {
    $closures[] = function () use ($i) { return $i; };
}

$m2 = new ReflectionMethod($closures[0], '__invoke');
foreach ($closures as $closure) {
    var_dump($m2->invoke($closure));
}

// Internal closures (first-class callable syntax) should also be validated
$vd = var_dump(...);
$pr = print_r(...);

$m3 = new ReflectionMethod($vd, '__invoke');
$m3->invoke($vd, 'internal closure OK');

// Same internal closure, different instance - should work
$vd2 = var_dump(...);
$m3->invoke($vd2, 'same internal closure OK');

// Different internal closure should throw
try {
    $m3->invoke($pr, 'should not print');
    echo "No exception thrown\n";
} catch (ReflectionException $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
c1: foo=FOO, bar=BAR
Given Closure is not the same as the reflected Closure
Given Closure is not the same as the reflected Closure
int(0)
int(1)
int(2)
string(19) "internal closure OK"
string(24) "same internal closure OK"
Given Closure is not the same as the reflected Closure
