--TEST--
GH-12723: Function JIT emits "Uninitialized string offset" warning at the same time as invalid offset Error
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php

$container = '';
$dimension = [];

try {
    var_dump($container[$dimension]);
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot access offset of type array on string
