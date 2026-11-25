--TEST--
PFA: arrow functions capture variables used in the bound arguments of a partial application
--FILE--
<?php

function f($a, $b) {
    return "$a $b";
}

class C {
    public function m($a, $b) {
        return "$a $b";
    }

    public static function s($a, $b) {
        return "$a $b";
    }
}

$a = 'a';
$o = new C;

var_dump((fn() => f($a, ?))()('b'));
var_dump((fn() => array_map(f($a, ?), ['b'])[0])());
var_dump((fn() => $o->m($a, ?))()('b'));
var_dump((fn() => C::s($a, ?))()('b'));
var_dump((fn() => f(a: $a, b: ?))()('b'));
var_dump((fn() => f($a, ...))()('b'));
var_dump((fn() => f(f($a, ?)('b'), ?))()('c'));
var_dump((fn() => f((fn() => $a)(), ?))()('b'));

/* Captured by value when the arrow function is created, like any other variable. */
$fn = fn() => f($a, ?);
$a = 'changed';
var_dump($fn()('b'));

?>
--EXPECT--
string(3) "a b"
string(3) "a b"
string(3) "a b"
string(3) "a b"
string(3) "a b"
string(3) "a b"
string(5) "a b c"
string(3) "a b"
string(3) "a b"
