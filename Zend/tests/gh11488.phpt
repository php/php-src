--TEST--
GH-11488: "Optional parameter before required" warning for union nullable type
--FILE--
<?php
function a(
    string|null $a = null,
    $b,
) {}
function b(
    Foo&Bar $c = null,
    $d,
) {}
function c(
    (Foo&Bar)|null $e = null,
    $f,
) {}
?>
--EXPECTF--
Deprecated: Optional parameter $a declared before required parameter $b is implicitly treated as a required parameter in %s on line %d

Deprecated: Optional parameter $e declared before required parameter $f is implicitly treated as a required parameter in %s on line %d
