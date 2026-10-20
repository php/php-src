--TEST--
GH-10072 (PHP crashes when execute_ex is overridden and a trampoline is used from internal code during shutdown)
--EXTENSIONS--
zend_test
--INI--
zend_test.replace_zend_execute_ex=1
opcache.jit=disable
--FILE--
<?php

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        echo 'Trampoline for ', $name, PHP_EOL;
    }
}

register_shutdown_function([new TrampolineTest(), 'shutdown']);
?>
--EXPECT--
Trampoline for shutdown
