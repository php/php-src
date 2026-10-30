--TEST--
DCE must not remove assignments to properties of an object escaping through __isset
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.file_update_protection=0
--EXTENSIONS--
opcache
--FILE--
<?php

class C {
    public $x = 0;
    function __isset($n) {
        global $g;
        $g = $this;
        return true;
    }
}

function f() {
    $o = new C;
    isset($o->foo);
    $o->x = 42;
}

f();
var_dump($g->x);

#[AllowDynamicProperties]
class F {
    function __isset($n) {
        $this->x = 2;
        return true;
    }
}

function i() {
    $o = new F;
    $o->x = 1;
    isset($o->foo);
    var_dump($o->x);
}
i();

?>
--EXPECT--
int(42)
int(2)
