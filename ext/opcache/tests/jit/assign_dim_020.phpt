--TEST--
JIT ASSIGN_DIM: OP_DATA is freed when a resource key conversion throws
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

set_error_handler(function ($errno, $errstr) {
    throw new Exception($errstr);
});

function test() {
    $a = [];
    $a[STDIN] = new D;
}

try {
    test();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECTF--
dtor
Exception: Resource ID#%d used as offset, casting to integer (%d)
