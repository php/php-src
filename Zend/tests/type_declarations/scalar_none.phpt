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
    } catch (Throwable $e) {
        echo "*** Caught " . $e->getMessage() . PHP_EOL;
    }
}
echo PHP_EOL . "Done";
--EXPECTF--
Testing int:
*** Caught Too few arguments to function {closure}(), 0 passed in %s on line %d and exactly 1 expected
Testing float:
*** Caught Too few arguments to function {closure}(), 0 passed in %s on line %d and exactly 1 expected
Testing string:
*** Caught Too few arguments to function {closure}(), 0 passed in %s on line %d and exactly 1 expected
Testing bool:
*** Caught Too few arguments to function {closure}(), 0 passed in %s on line %d and exactly 1 expected
Testing int nullable:
NULL
Testing float nullable:
NULL
Testing string nullable:
NULL
Testing bool nullable:
NULL

Done
