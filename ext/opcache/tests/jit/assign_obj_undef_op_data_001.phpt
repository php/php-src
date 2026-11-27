--TEST--
JIT ASSIGN_OBJ: error handler releases the object while reporting undefined OP_DATA
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=function
--FILE--
<?php
class C {
    public $p = 1;
    public int $t = 1;
}

set_error_handler(function ($_, $msg) {
    echo $msg, "\n";
    if ($msg === 'Undefined variable $undef') {
        $GLOBALS['x'] = 1;
    }
});

function dynamic() {
    global $x;
    $x = new stdClass;
    $x->p = $undef;
}

function declared() {
    global $x;
    $x = new C;
    $x->p = $undef;
}

function typed() {
    global $x;
    $x = new C;
    $r = ($x->t = $undef);
}

foreach (['dynamic', 'declared', 'typed'] as $f) {
    try {
        $f();
    } catch (Error $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    var_dump($x);
}
?>
--EXPECT--
Undefined variable $undef
Error: Attempt to assign property "p" on int
int(1)
Undefined variable $undef
Error: Attempt to assign property "p" on int
int(1)
Undefined variable $undef
Error: Attempt to assign property "t" on int
int(1)
