--TEST--
Enum case disallows dynamic properties (internal enum)
--EXTENSIONS--
zend_test
--FILE--
<?php

$bar = ZendTestUnitEnum::Bar;

try {
    $bar->baz = 'Baz';
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Cannot create dynamic property ZendTestUnitEnum::$baz
