--TEST--
Enum case disallows dynamic properties (internal enum)
--EXTENSIONS--
zend_test
--FILE--
<?php

$bar = ZendTestUnitEnum::Bar;

try {
    $bar->baz = 'Baz';
} catch (\Error $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Error: Cannot create dynamic property ZendTestUnitEnum::$baz
