--TEST--
GH-22142: deflate_init() handles undefined variable without assertion failure
--FILE--
<?php

try {
    $cls = new LibXMLError();
    deflate_init($fusion, $cls);
} catch (\Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

?>
--EXPECTF--
Warning: Undefined variable $fusion in %s on line %d

Deprecated: deflate_init(): Passing null to parameter #1 ($encoding) of type int is deprecated in %s on line %d
TypeError: deflate_init(): Argument #2 ($options) the value for option "level" must be of type int, null given
