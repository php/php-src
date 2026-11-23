--TEST--
JIT ASSIGN_DIM: error handler releases the typed reference while reporting undefined OP_DATA
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=function
--FILE--
<?php
class C { public int $t = 1; }

set_error_handler(function ($_, $msg) {
    echo $msg, "\n";
    $GLOBALS['x'] = 1;
    $GLOBALS['o'] = null;
});

function test() {
    global $x, $o;
    $o = new C;
    $x = [0];
    $x[0] = &$o->t;
    $x[0] = $undef;
}

test();
var_dump($x, $o);
?>
--EXPECT--
Undefined variable $undef
int(1)
NULL
