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
?>
--EXPECT--
c1: foo=FOO, bar=BAR
Given object is not an instance of the class this method was declared in
Given object is not an instance of the class this method was declared in
int(0)
int(1)
int(2)
