--TEST--
DCE must not remove ISSET_ISEMPTY_DIM_OBJ with a resource key
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.file_update_protection=0
--EXTENSIONS--
opcache
--FILE--
<?php
set_error_handler(function ($errno, $errstr) {
    throw new Exception($errstr);
});

function test() {
    $k = fopen("php://memory", "r");
    $a = [];
    isset($a[$k]);
}

try {
    test();
} catch (Exception $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECTF--
Exception: Resource ID#%d used as offset, casting to integer (%d)
