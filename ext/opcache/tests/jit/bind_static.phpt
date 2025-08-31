--TEST--
Bind static may throw
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
function test() {
    static $N = UNDEFINED;
    throw new Exception;
}
try {
    test();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Undefined constant "UNDEFINED"
