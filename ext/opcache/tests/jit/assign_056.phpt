--TEST--
JIT ASSIGN: ASSING+SEND and typed reference
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
--FILE--
<?php
class Test {
    static ?Test $test;
}

$a = new Test;
Test::$test = &$a;
var_dump($a = new Test);
?>
--EXPECT--
object(Test)#2 (0) {
}
