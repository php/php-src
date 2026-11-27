--TEST--
Test Uri\QueryParams::append() - error - unbacked enum
--FILE--
<?php

enum TestEnum {
    case Bar;
}

$params = new Uri\QueryParams();

try {
    $params = $params->append("foo", TestEnum::Bar);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: Uri\QueryParams::append(): Argument #2 ($value) must be of type BackedEnum|string|int|float|null, TestEnum given
