--TEST--
PFA variation: strict_types declared
--FILE--
<?php
declare(strict_types=1);

function foo(int $int) {
    var_dump($int);
}

$foo = foo(?);

try {
    $foo("42");
} catch (TypeError $ex) {
    printf("%s: %s\n", $ex::class, $ex->getMessage());
}
?>
--EXPECTF--
TypeError: {closure:%s:%d}(): Argument #1 ($int) must be of type int, string given, called in %s on line %d
