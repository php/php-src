--TEST--
Argument unpacking does not work with non-integer keys
--FILE--
<?php
function foo(...$args) {
    var_dump($args);
}
function gen() {
    yield 1.23 => 123;
    yield "2.34" => 234;
}

try {
    foo(...gen());
} catch (Error $ex) {
    echo "Exception: " . $ex->getMessage() . "\n";
}

?>
--EXPECT--
Exception: Keys must be of type int|string during argument unpacking
