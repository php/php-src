--TEST--
JIT ASSIGN_DIM: OP_DATA is freed when the key is an illegal offset
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

function test() {
    $a[[]] = new D;
}

try {
    test();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
dtor
TypeError: Cannot access offset of type array on array
