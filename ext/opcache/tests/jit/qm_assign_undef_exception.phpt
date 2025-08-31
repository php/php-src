--TEST--
QM_ASSIGN of undef var may throw exception
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
set_error_handler(function($_, $m) { throw new Exception($m); });
function test() {
    $a = $b;
    X;
}
try {
    test();
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Undefined variable $b
