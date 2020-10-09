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
    echo $e->getMessage();
}
?>
--EXPECT--
Cannot fetch information from a terminated Generator
