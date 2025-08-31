--TEST--
Internal enums from/tryFrom in strict_types=1
--EXTENSIONS--
zend_test
--FILE--
<?php

declare(strict_types=1);

var_dump(ZendTestStringEnum::from("Test2"));

try {
    var_dump(ZendTestStringEnum::from(42));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(ZendTestStringEnum::tryFrom(43));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
enum(ZendTestStringEnum::Bar)
ZendTestStringEnum::from(): Argument #1 ($value) must be of type string, int given
ZendTestStringEnum::tryFrom(): Argument #1 ($value) must be of type string, int given
