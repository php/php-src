--TEST--
GH-24234 (Opcache: optimizer overwrites the operator of ASSIGN_STATIC_PROP_OP)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--CREDITS--
waroad
--FILE--
<?php
class K {
    public static $s = 5;
}

function both() {
    $c = "K";
    $n = "s";
    $c::$$n += 2;
    return K::$s;
}

function class_only($n) {
    $c = "K";
    $c::$$n -= 1;
    return K::$s;
}

function prop_only($c) {
    $n = "s";
    $c::$$n *= 3;
    return K::$s;
}

function both_extra_slot() {
    K::$s;
    $c = "K";
    $n = "s";
    $c::$$n .= "x";
    return K::$s;
}

function both_nested() {
    $c = "K";
    $n = "s";
    $c::$$n <<= 1;
    strlen("x");
    return K::$s;
}

var_dump(both());
var_dump(class_only("s"));
var_dump(prop_only("K"));
K::$s = 1;
var_dump(both_nested());
var_dump(both_extra_slot());
?>
--EXPECT--
int(7)
int(6)
int(18)
int(2)
string(2) "2x"
