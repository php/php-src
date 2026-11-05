--TEST--
JIT ASSIGN_DIM: OP_DATA is freed once when a float key conversion throws on a false container
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=function
zend.exception_ignore_args=1
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
    if (!str_starts_with($errstr, "Automatic conversion of false")) {
        throw new Exception($errstr);
    }
});

function id($x) {
    return $x;
}

function test($o) {
    $a = false;
    $a[1.5] = id($o);
}

try {
    test(new D);
} catch (Exception $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
dtor
Exception: Implicit conversion from float 1.5 to int loses precision
