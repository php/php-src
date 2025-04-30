--TEST--
First class callables and strict types
--FILE--
<?php
declare(strict_types=1);

function test(int $i) {
    var_dump($i);
}

require __DIR__ . '/first_class_callable_015_weak.inc';
require __DIR__ . '/first_class_callable_015_strict.inc';
$fn = test(...);
do_weak_call($fn);
try {
    do_strict_call($fn);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
int(42)
test(): Argument #1 ($i) must be of type int, string given, called in %s on line %d
