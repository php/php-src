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
    echo $e->getMessage();
}

?>
--EXPECT--
Cannot create dynamic property ZendTestUnitEnum::$baz
