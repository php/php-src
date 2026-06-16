--TEST--
Test Uri\QueryParams::append() - error - unbacked enum
--FILE--
<?php

enum TestEnum {
    case Bar;
}

$params = new Uri\QueryParams();

try {
    $params->append("foo", TestEnum::Bar);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: Uri\QueryParams::append(): Argument #2 ($value) must not contain an object
