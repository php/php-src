--TEST--
JIT INIT_FCALL_BY_NAME: exception thrown by the function autoloader
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=function
opcache.jit_buffer_size=16M
--EXTENSIONS--
opcache
--FILE--
<?php
spl_autoload_register_function_loader(function (string $name) {
    throw new RuntimeException("Autoload failed for: $name");
});

function test() {
    missing_func();
}

try {
    test();
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
    var_dump($e->getTrace()[0]['line']);
}
try {
    test();
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Autoload failed for: missing_func
int(7)
Autoload failed for: missing_func
