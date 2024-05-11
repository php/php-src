--TEST--
GH-12723: JIT emits "Attempt to assign property of non-object" warning at the same time as Error is being thrown
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php

$container = new stdClass();

try {
    $container[new stdClass()] .= 'append';
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot use object of type stdClass as array
