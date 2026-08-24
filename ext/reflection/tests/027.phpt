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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ReflectionException: Cannot fetch information from a closed Generator
