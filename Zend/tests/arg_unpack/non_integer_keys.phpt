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
} catch (Throwable $ex) {
    echo $ex::class, ': ', $ex->getMessage(), "\n";
}

?>
--EXPECT--
Error: Keys must be of type int|string during argument unpacking
