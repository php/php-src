--TEST--
JIT ASSIGN_DIM: undefined OP_DATA is not reported when the false to array deprecation throws
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=function
--FILE--
<?php
set_error_handler(function ($_, $msg) {
    throw new Exception($msg);
});
function test() {
    $x = false;
    $x[0] = $undef;
}
try {
    test();
} catch (Exception $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
Exception: Automatic conversion of false to array is deprecated
