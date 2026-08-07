--TEST--
ReflectionGenerator::getTrace()
--FILE--
<?php
function foo()
{
    yield 1;
}

$g = foo();
$r = new ReflectionGenerator($g);

$g->next();

try {
    $r->getTrace();
} catch (ReflectionException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
ReflectionException: Cannot fetch information from a closed Generator
