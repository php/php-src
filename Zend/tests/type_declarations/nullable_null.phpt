--TEST--
nullable class
--FILE--
<?php
function test(Foo $a = null) {
    echo "ok\n";
}
test(null);
?>
--EXPECTF--
Deprecated: test(): Implicitly marking parameter $a as nullable is deprecated, the explicit nullable type must be used instead in %s on line %d
ok
