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

$r = new ReflectionGenerator($g);
var_dump($r);
?>
--EXPECTF--
object(ReflectionGenerator)#%d (0) {
}
