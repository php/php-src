--TEST--
IS_IDENTICAL_EMPTY_ARRAY specialization must not be used for an IS_CONST op1
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
--FILE--
<?php
if (A::class->p === []) {
    echo "identical\n";
}
if (A::class->p !== []) {
    echo "not identical\n";
}
echo "OK\n";
?>
--EXPECTF--
Warning: Attempt to read property "p" on string in %s on line %d

Warning: Attempt to read property "p" on string in %s on line %d
not identical
OK
