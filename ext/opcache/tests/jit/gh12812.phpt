--TEST--
GH-12812: JIT: Integer string in variable used as offset produces wrong undefined array key warning
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php

$container = [];
$dimension = '7';

try {
    var_dump($container['7']);
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($container[$dimension]);
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Warning: Undefined array key 7 in %s on line %d
NULL

Warning: Undefined array key 7 in %s on line %d
NULL
