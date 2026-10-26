--TEST--
GH-24436 (Tracing JIT: wrong value of assignment to typed reference passed as argument)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
--FILE--
<?php
class Box {
    public $v = 1;
    public float $f = 1;
    public string $s = "";
}

function sink($x) {
    return $x;
}

function work(&$x, $y) {
    return sink($x = $y + 2);
}

$o = new Box;
foreach (['v', 'v', 'v', 'f', 's'] as $p) {
    var_dump(work($o->$p, 3));
}
var_dump($o->f, $o->s);
?>
--EXPECT--
int(5)
int(5)
int(5)
float(5)
string(1) "5"
float(5)
string(1) "5"
