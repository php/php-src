--TEST--
Scalar type nullability
--FILE--
<?php

$functions = [
    'int' => function (int $i) { return $i; },
    'float' => function (float $f) { return $f; },
    'string' => function (string $s) { return $s; },
    'bool' => function (bool $b) { return $b; },
    'int nullable' => function (?int $i) { return $i; },
    'float nullable' => function (?float $f) { return $f; },
    'string nullable' => function (?string $s) { return $s; },
    'bool nullable' => function (?bool $b) { return $b; }
];

foreach ($functions as $type => $function) {
    echo "Testing $type:", PHP_EOL;
    try {
        var_dump($function(null));
    } catch (TypeError $e) {
        echo "*** Caught " . $e->getMessage() . PHP_EOL;
    }
}

echo PHP_EOL . "Done";
?>
--EXPECTF--
Testing int:
*** Caught {closure:%s:%d}(): Argument #1 ($i) must be of type int, null given, called in %s on line %d
Testing float:
*** Caught {closure:%s:%d}(): Argument #1 ($f) must be of type float, null given, called in %s on line %d
Testing string:
*** Caught {closure:%s:%d}(): Argument #1 ($s) must be of type string, null given, called in %s on line %d
Testing bool:
*** Caught {closure:%s:%d}(): Argument #1 ($b) must be of type bool, null given, called in %s on line %d
Testing int nullable:
NULL
Testing float nullable:
NULL
Testing string nullable:
NULL
Testing bool nullable:
NULL

Done
