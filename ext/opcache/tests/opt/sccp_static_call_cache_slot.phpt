--TEST--
SCCP propagation into the method name of a static call must allocate enough cache slots
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=0x7FFEBBFF
--EXTENSIONS--
opcache
--FILE--
<?php
class Target {
    static function m() {
        return "m";
    }
}

class O {
    public $prop = 3;
}

function g($o) {
    $name = "m";
    $r = Target::$name();
    return $r . $o->prop;
}

function h($o) {
    $c = "Target";
    $name = "m";
    $r = $c::$name();
    return $r . $o->prop;
}

$o = new O;
var_dump(g($o), g($o), g($o), h($o), h($o), h($o));
?>
--EXPECT--
string(2) "m3"
string(2) "m3"
string(2) "m3"
string(2) "m3"
string(2) "m3"
string(2) "m3"
