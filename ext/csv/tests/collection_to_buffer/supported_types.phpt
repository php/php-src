--TEST--
Test Csv\collection_to_buffer() and Csv\collection_to_file() field type conversion and errors
--EXTENSIONS--
csv
--INI--
serialize_precision=-1
--FILE--
<?php

enum Suit: string { case Hearts = 'H'; }
enum Pure { case Foo; }

class ThrowingStringable {
    public function __toString(): string { throw new RuntimeException('__toString() must not be called'); }
}

$collection = [
    ['a', 1, 1.5, true, false, null, Suit::Hearts],
    ['b', 2, 0.1 + 0.2, false, true, 'x', Suit::Hearts],
];
var_dump(Csv\collection_to_buffer($collection, eolSequence: "\n"));

$file = __DIR__ . '/supported_types.csv';
Csv\collection_to_file($file, $collection, eolSequence: "\n");
var_dump(file_get_contents($file));

$invalid = [
    [['ok'], [Pure::Foo]],
    [['ok'], ['name' => fopen('php://memory', 'r')]],
    [['ok'], [new ThrowingStringable()]],
];

foreach ($invalid as $rows) {
    try {
        Csv\collection_to_buffer($rows);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    try {
        Csv\collection_to_file($file, $rows);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/supported_types.csv');
?>
--EXPECT--
string(47) "a,1,1.5,1,0,,H
b,2,0.30000000000000004,0,1,x,H
"
string(47) "a,1,1.5,1,0,,H
b,2,0.30000000000000004,0,1,x,H
"
TypeError: Field 0 of element 1 of the collection must be of type null|bool|int|float|string|BackedEnum, Pure given
TypeError: Field 0 of element 1 of the collection must be of type null|bool|int|float|string|BackedEnum, Pure given
TypeError: Field "name" of element 1 of the collection must be of type null|bool|int|float|string|BackedEnum, resource given
TypeError: Field "name" of element 1 of the collection must be of type null|bool|int|float|string|BackedEnum, resource given
TypeError: Field 0 of element 1 of the collection must be of type null|bool|int|float|string|BackedEnum, ThrowingStringable given
TypeError: Field 0 of element 1 of the collection must be of type null|bool|int|float|string|BackedEnum, ThrowingStringable given
