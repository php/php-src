--TEST--
JIT ASSIGN_OBJ: error handler releases the object while reporting undefined OP_DATA in a trace
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=tracing
opcache.jit_hot_loop=2
--FILE--
<?php
$n = 0;
set_error_handler(function ($_, $msg) {
    if ($msg === 'Undefined variable $undef' && ++$GLOBALS['n'] === 50) {
        echo $msg, "\n";
        $GLOBALS['x'] = 1;
    }
});

function test() {
    global $x;
    for ($i = 0; $i < 100; $i++) {
        $x = new stdClass;
        $x->p = $undef;
    }
}

try {
    test();
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($x, $n);
?>
--EXPECT--
Undefined variable $undef
Error: Attempt to assign property "p" on int
int(1)
int(50)
