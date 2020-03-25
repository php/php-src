--TEST--
ReflectionGenerator::__construct()
--FILE--
<?php
function foo()
{
    yield 1;
}

$g = foo();
$g->next();

try {
    $r = new ReflectionGenerator($g);
} catch (ReflectionException $e) {
    echo "Done!\n";
}
?>
--EXPECT--
Done!
