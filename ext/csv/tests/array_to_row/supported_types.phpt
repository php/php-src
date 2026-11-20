--TEST--
Test Csv\array_to_row() conversion of each supported and unsupported field type
--EXTENSIONS--
csv
--INI--
precision=14
serialize_precision=-1
--FILE--
<?php

enum Suit: string { case Hearts = 'H'; case Spades = 'S, or "spades"'; }
enum Level: int { case High = 3; }
enum Pure { case Foo; }

class Name implements Stringable {
    public function __toString(): string { return 'Name, as string'; }
}
class ImplicitStringable {
    public function __toString(): string { return 'implicit'; }
}

$string = 'ref';
$supported = [
    'string' => 'foo',
    'null' => null,
    'true' => true,
    'false' => false,
    'int' => -42,
    'float' => 0.1 + 0.2,
    'float integral' => 1.0,
    'float exponent' => 1e100,
    'float -0' => -0.0,
    'INF' => INF,
    '-INF' => -INF,
    'NAN' => NAN,
    'string backed enum' => Suit::Hearts,
    'string backed enum needing enclosure' => Suit::Spades,
    'int backed enum' => Level::High,
    'reference' => &$string,
];

foreach ($supported as $label => $value) {
    echo str_pad($label, 40), var_export(Csv\array_to_row([$value], eolSequence: "\n"), true), "\n";
}

echo "\n";

$unsupported = [
    'array' => [1],
    'resource' => fopen('php://memory', 'r'),
    'pure enum' => Pure::Foo,
    'object' => new stdClass(),
    'Stringable' => new Name(),
    'implicit Stringable' => new ImplicitStringable(),
    'closure' => fn () => 1,
];

foreach ($unsupported as $label => $value) {
    try {
        Csv\array_to_row(['ok', 'key' => $value]);
    } catch (TypeError $e) {
        echo str_pad($label, 40), $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
string                                  'foo
'
null                                    '
'
true                                    '1
'
false                                   '0
'
int                                     '-42
'
float                                   '0.30000000000000004
'
float integral                          '1
'
float exponent                          '1.0E+100
'
float -0                                '-0
'
INF                                     'INF
'
-INF                                    '-INF
'
NAN                                     'NAN
'
string backed enum                      'H
'
string backed enum needing enclosure    '"S, or ""spades"""
'
int backed enum                         '3
'
reference                               'ref
'

array                                   Field "key" must be of type null|bool|int|float|string|BackedEnum, array given
resource                                Field "key" must be of type null|bool|int|float|string|BackedEnum, resource given
pure enum                               Field "key" must be of type null|bool|int|float|string|BackedEnum, Pure given
object                                  Field "key" must be of type null|bool|int|float|string|BackedEnum, stdClass given
Stringable                              Field "key" must be of type null|bool|int|float|string|BackedEnum, Name given
implicit Stringable                     Field "key" must be of type null|bool|int|float|string|BackedEnum, ImplicitStringable given
closure                                 Field "key" must be of type null|bool|int|float|string|BackedEnum, Closure given
