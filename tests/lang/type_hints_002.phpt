--TEST--
ZE2 type hinting
--FILE--
<?php
class P { }
class T {
    function f(P $p = NULL) {
        var_dump($p);
        echo "-\n";
    }
}

$o=new T();
$o->f(new P);
$o->f();
$o->f(NULL);
?>
--EXPECTF--
Deprecated: T::f(): Implicitly marking parameter $p as nullable is deprecated, the explicit nullable type must be used instead in %s on line %d
object(P)#2 (0) {
}
-
NULL
-
NULL
-
