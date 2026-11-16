--TEST--
JIT ASSIGN_DIM: OP_DATA is freed when the array is destroyed by an undefined key warning
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=function
--EXTENSIONS--
opcache
--FILE--
<?php
class D {
    function __destruct() {
        echo "dtor\n";
    }
}

set_error_handler(function () {
    $GLOBALS["a"] = null;
    return true;
});

function test() {
    $GLOBALS["a"] = [];
    $a = &$GLOBALS["a"];
    $a[$k] = new D;
    var_dump($a);
}

test();
?>
--EXPECT--
dtor
NULL
