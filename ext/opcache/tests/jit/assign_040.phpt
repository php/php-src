--TEST--
JIT ASSIGN: Typed reference error with return value
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--FILE--
<?php
class Test {
    public string $x;
}
function test() {
    $test = new Test;
    $test->x = "";
    $r =& $test->x;
    +($r = $y);
}
try {
    test();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Warning: Undefined variable $y in %s on line %d
Cannot assign null to reference held by property Test::$x of type string
