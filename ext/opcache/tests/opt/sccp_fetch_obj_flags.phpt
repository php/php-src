--TEST--
SCCP must preserve object fetch flags
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
class O {
    public $p = 1;
    public ?int $t = null;
    public int $i = 0;
}

function f() {
    return 5;
}

function returns_function($o) {
    $n = "p";
    $o->$n = &f();
}

function dim_write($o) {
    $n = "t";
    $o->$n[] = 1;
}

function fetch_ref($o) {
    $n = "i";
    $r = &$o->$n;
    $r = "x";
}

function fetch_ref_arg($o) {
    $n = "i";
    settype($o->$n, "array");
}

foreach (['returns_function', 'dim_write', 'fetch_ref', 'fetch_ref_arg'] as $func) {
    $o = new O;
    try {
        $func($o);
    } catch (Throwable $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
    }
    var_dump($o->p, $o->t, $o->i);
}
?>
--EXPECTF--
Notice: Only variables should be assigned by reference in %s on line %d
int(5)
NULL
int(0)
TypeError: Cannot auto-initialize an array inside property O::$t of type ?int
int(1)
NULL
int(0)
TypeError: Cannot assign string to reference held by property O::$i of type int
int(1)
NULL
int(0)
TypeError: Cannot assign array to reference held by property O::$i of type int
int(1)
NULL
int(0)
