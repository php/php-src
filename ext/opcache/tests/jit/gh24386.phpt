--TEST--
GH-24386 (register operand must be loaded before the typed reference check)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_hot_func=2
--FILE--
<?php
class C {
    public int $prop = 0;
}

function f($n) {
    global $g, $h;
    $g += $n;
    $h += $n;
}

f(2);
var_dump($g, $h);

$c = new C;
$g =& $c->prop;
f(3);
var_dump($c->prop, $h);
?>
--EXPECT--
int(2)
int(2)
int(3)
int(5)
