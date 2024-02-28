--TEST--
Scalar type missing parameters
--FILE--
<?php

$functions = [
    'int' => function (int $i) { return $i; },
    'float' => function (float $f) { return $f; },
    'string' => function (string $s) { return $s; },
    'bool' => function (bool $b) { return $b; },
    'int nullable' => function (int $i = NULL) { return $i; },
    'float nullable' => function (float $f = NULL) { return $f; },
    'string nullable' => function (string $s = NULL) { return $s; },
    'bool nullable' => function (bool $b = NULL) { return $b; }
];

foreach ($functions as $type => $function) {
    echo "Testing $type:", PHP_EOL;
    try {
        var_dump($function());
    } catch (Throwable $e) {
        echo "*** Caught " . $e->getMessage() . PHP_EOL;
    }
}
echo PHP_EOL . "Done";
?>
--EXPECTF--
Testing int:
*** Caught Too few arguments to function {closure:%s:%d}(), 0 passed in %s on line %d and exactly 1 expected
Testing float:
*** Caught Too few arguments to function {closure:%s:%d}(), 0 passed in %s on line %d and exactly 1 expected
Testing string:
*** Caught Too few arguments to function {closure:%s:%d}(), 0 passed in %s on line %d and exactly 1 expected
Testing bool:
*** Caught Too few arguments to function {closure:%s:%d}(), 0 passed in %s on line %d and exactly 1 expected
Testing int nullable:
NULL
Testing float nullable:
NULL
Testing string nullable:
NULL
Testing bool nullable:
NULL

Done
