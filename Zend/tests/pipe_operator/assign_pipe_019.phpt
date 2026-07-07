--TEST--
Pipe assign operator with readonly properties
--FILE--
<?php

class WithReadonly {
    public function __construct(
        public readonly string $value = 'hello',
    ) {}
}

$ro = new WithReadonly();
try {
    $ro->value |>= strtoupper(...);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Error: Cannot modify readonly property WithReadonly::$value
