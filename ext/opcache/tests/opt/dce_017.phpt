--TEST--
DCE must not remove assignments to properties of an object escaping through a property hook
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.file_update_protection=0
--EXTENSIONS--
opcache
--FILE--
<?php

class D {
    public $x = 0;
    public $h {
        get {
            global $g;
            $g = $this;
            return 1;
        }
    }
}

function g() {
    $o = new D;
    $o->h;
    $o->x = 42;
}

g();
var_dump($g->x);

class E {
    public $x = 0;
    public $h {
        set {
            global $g;
            $g = $this;
        }
    }
}

function h() {
    $o = new E;
    $o->h = 1;
    $o->x = 42;
}

h();
var_dump($g->x);

?>
--EXPECT--
int(42)
int(42)
