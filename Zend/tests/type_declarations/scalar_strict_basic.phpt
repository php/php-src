--TEST--
Strict scalar type basics
--FILE--
<?php

declare(strict_types=1);

$functions = [
    'int' => function (int $i) { return $i; },
    'float' => function (float $f) { return $f; },
    'string' => function (string $s) { return $s; },
    'bool' => function (bool $b) { return $b; }
];

$values = [
    1,
    1.0,
    "1",
    TRUE,
    FALSE,
    NULL,
    [],
    new StdClass,
    fopen("data:text/plain,foobar", "r")
];

function type($value) {
    if (is_float($value)) {
        return "float";
    } else if (is_bool($value)) {
        return $value ? "true" : "false";
    } else if (is_null($value)) {
        return "null";
    } else {
        return gettype($value);
    }
}

foreach ($functions as $type => $function) {
    echo PHP_EOL, "Testing '$type' type:", PHP_EOL;
    foreach ($values as $value) {
        $errored = false;
        echo PHP_EOL . "*** Trying ", type($value), " value", PHP_EOL;
        try {
            var_dump($function($value));
        } catch (TypeError $e) {
            echo "*** Caught " . $e->getMessage() . PHP_EOL;
        }
    }
}
echo PHP_EOL . "Done";
?>
--EXPECTF--
Fatal error: Uncaught Error: Class "StdClass" not found in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d