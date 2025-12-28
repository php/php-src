--TEST--
GH-19305 003: Operands may be released during comparison
--SKIPIF--
<?php
if (!method_exists('ReflectionClass', 'newLazyGhost')) {
    die('skip No lazy objects');
}
?>
--FILE--
<?php

class C
{
    public $s;
}
$r = new ReflectionClass(C::class);
$o = $r->newLazyProxy(function () { return new C; });

// Comparison calls initializers, which releases $o
var_dump($o >
$r->newLazyGhost(function () {
    global $o;
    $o = null;
}));

?>
--EXPECT--
bool(false)
