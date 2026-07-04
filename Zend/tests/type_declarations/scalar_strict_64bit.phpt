--TEST--
Scalar type strict mode
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php
declare(strict_types=1);

$functions = [
    'int' => function (int $i) { return $i; },
    'float' => function (float $f) { return $f; },
    'string' => function (string $s) { return $s; },
    'bool' => function (bool $b) { return $b; }
];

class StringCapable {
    public function __toString() {
        return "foobar";
    }
}

$values = [
    1,
    "1",
    1.0,
    1.5,
    "1a",
    "a",
    "",
    PHP_INT_MAX,
    NAN,
    TRUE,
    FALSE,
    NULL,
    [],
    new StdClass,
    new StringCapable,
    STDERR,
];

foreach ($functions as $type => $function) {
    echo PHP_EOL, "Testing '$type' type:", PHP_EOL;
    foreach ($values as $value) {
        echo PHP_EOL . "*** Trying ";
        var_dump($value);
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