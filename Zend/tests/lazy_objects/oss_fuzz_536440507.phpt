--TEST--
OSS-Fuzz #536440507 (Immutable class incorrect assertion)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php

class C {
    // Something that emits a warning so it can't be folded at compile time
    public mixed $b = 340282366920938463454151235394913%435650;
}
$o = (new ReflectionClass(C::class))->newLazyGhost(function ($obj) {});
var_dump($o->b);

?>
--EXPECTF--
Warning: The float 3.4028236692093845E+32 is not representable as an int, cast occurred in %s on line %d
int(%s)
