--TEST--
Bug #78531 (Crash when using undefined variable as object)
--FILE--
<?php
try {
    $u1->a += 5;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $x = ++$u2->a;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $x = $u3->a++;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $u4->a->a += 5;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Warning: Undefined variable $u1 in %s on line %d
Attempt to assign property "a" on null

Warning: Undefined variable $u2 in %s on line %d
Attempt to increment/decrement property "a" on null

Warning: Undefined variable $u3 in %s on line %d
Attempt to increment/decrement property "a" on null

Warning: Undefined variable $u4 in %s on line %d

Warning: Attempt to read property "a" on null in %s on line %d
Attempt to assign property "a" on null
