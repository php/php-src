--TEST--
JIT ASSIGN_DIM: OP_DATA is freed when appending to a full array fails
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
    $a = [PHP_INT_MAX => 1];
    $a[] = new D;
}

try {
    test();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
dtor
Error: Cannot add element to the array as the next element is already occupied
