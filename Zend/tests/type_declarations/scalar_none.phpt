--TEST--
Scalar type missing parameters
--FILE--
<?php

$errnames = [
    E_NOTICE => 'E_NOTICE',
    E_WARNING => 'E_WARNING',
    E_RECOVERABLE_ERROR => 'E_RECOVERABLE_ERROR'
];
set_error_handler(function (int $errno, string $errmsg, string $file, int $line) use ($errnames) {
    echo "$errnames[$errno]: $errmsg on line $line\n";
    return true;
});

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
    } catch (TypeError $e) {
        echo "*** Caught " . $e->getMessage() . PHP_EOL;
    }
}
echo PHP_EOL . "Done";
--EXPECTF--
Testing int:
*** Caught Argument 1 passed to {closure}() must be of the type integer, none given, called in %s on line %d
Testing float:
*** Caught Argument 1 passed to {closure}() must be of the type float, none given, called in %s on line %d
Testing string:
*** Caught Argument 1 passed to {closure}() must be of the type string, none given, called in %s on line %d
Testing bool:
*** Caught Argument 1 passed to {closure}() must be of the type boolean, none given, called in %s on line %d
Testing int nullable:
NULL
Testing float nullable:
NULL
Testing string nullable:
NULL
Testing bool nullable:
NULL

Done
