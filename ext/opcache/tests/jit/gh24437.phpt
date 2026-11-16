--TEST--
GH-24437 (JIT: tracing JIT gives wrong sum for `$a[0] += $i + 1` on an array parameter in a loop)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
--FILE--
<?php
class C {
    public int $prop = 10;
}

function f($a) {
    for ($i = 0; $i < 3; $i++) {
        @$a[0] += $i + 1;
    }
    return $a[0];
}

function g(array $a) {
    for ($i = 0; $i < 3; $i++) {
        $a[0] += $i + 1;
    }
    return $a[0];
}

function h($a) {
    for ($i = 0; $i < 3; $i++) {
        $a[2] %= $i + 1;
        $x = $a[0] %= 1;
    }
}

var_dump(f([1 => 2]));
$x = 10;
var_dump(g([&$x]), $x);
$c = new C;
var_dump(g([&$c->prop]), $c->prop);
@h([]);
?>
DONE
--EXPECT--
int(6)
int(16)
int(16)
int(16)
int(16)
DONE
