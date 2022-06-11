--TEST--
Closure type return type covariance
--FILE--
<?php

interface Foo {}
interface Bar {}

function test($type, $value) {
    echo ($type ?: 'none') . ' > ' . ($value ?: 'none') . ': ';
    $typeColon = $type ? ': ' : '';
    $valueColon = $value ? ': ' : '';
    try {
        eval("(function (\\Closure(){$typeColon}{$type} \$c) {})(function (){$valueColon}{$value} {});");
        echo 'true';
    } catch (Error $e) {
        echo 'false';
    }
    echo "\n";
}

$values = [
    '',
    'void',
    'string',
    'int',
    'bool',
    'string|int',
    'string|int|bool',
    '?string',
    'object',
    'Foo',
    'Bar',
    'Foo&Bar',
    '\\Closure(): void',
    '\\Closure(): string',
    '\\Closure(): string|int',
    'callable',
];

foreach ($values as $type) {
    foreach ($values as $value) {
        test($type, $value);
    }
}

?>
--EXPECT--
none > none: true
none > void: true
none > string: true
none > int: true
none > bool: true
none > string|int: true
none > string|int|bool: true
none > ?string: true
none > object: true
none > Foo: true
none > Bar: true
none > Foo&Bar: true
none > \Closure(): void: true
none > \Closure(): string: true
none > \Closure(): string|int: true
none > callable: true
void > none: false
void > void: true
void > string: false
void > int: false
void > bool: false
void > string|int: false
void > string|int|bool: false
void > ?string: false
void > object: false
void > Foo: false
void > Bar: false
void > Foo&Bar: false
void > \Closure(): void: false
void > \Closure(): string: false
void > \Closure(): string|int: false
void > callable: false
string > none: false
string > void: false
string > string: true
string > int: false
string > bool: false
string > string|int: false
string > string|int|bool: false
string > ?string: false
string > object: false
string > Foo: false
string > Bar: false
string > Foo&Bar: false
string > \Closure(): void: false
string > \Closure(): string: false
string > \Closure(): string|int: false
string > callable: false
int > none: false
int > void: false
int > string: false
int > int: true
int > bool: false
int > string|int: false
int > string|int|bool: false
int > ?string: false
int > object: false
int > Foo: false
int > Bar: false
int > Foo&Bar: false
int > \Closure(): void: false
int > \Closure(): string: false
int > \Closure(): string|int: false
int > callable: false
bool > none: false
bool > void: false
bool > string: false
bool > int: false
bool > bool: true
bool > string|int: false
bool > string|int|bool: false
bool > ?string: false
bool > object: false
bool > Foo: false
bool > Bar: false
bool > Foo&Bar: false
bool > \Closure(): void: false
bool > \Closure(): string: false
bool > \Closure(): string|int: false
bool > callable: false
string|int > none: false
string|int > void: false
string|int > string: true
string|int > int: true
string|int > bool: false
string|int > string|int: true
string|int > string|int|bool: false
string|int > ?string: false
string|int > object: false
string|int > Foo: false
string|int > Bar: false
string|int > Foo&Bar: false
string|int > \Closure(): void: false
string|int > \Closure(): string: false
string|int > \Closure(): string|int: false
string|int > callable: false
string|int|bool > none: false
string|int|bool > void: false
string|int|bool > string: true
string|int|bool > int: true
string|int|bool > bool: true
string|int|bool > string|int: true
string|int|bool > string|int|bool: true
string|int|bool > ?string: false
string|int|bool > object: false
string|int|bool > Foo: false
string|int|bool > Bar: false
string|int|bool > Foo&Bar: false
string|int|bool > \Closure(): void: false
string|int|bool > \Closure(): string: false
string|int|bool > \Closure(): string|int: false
string|int|bool > callable: false
?string > none: false
?string > void: false
?string > string: true
?string > int: false
?string > bool: false
?string > string|int: false
?string > string|int|bool: false
?string > ?string: true
?string > object: false
?string > Foo: false
?string > Bar: false
?string > Foo&Bar: false
?string > \Closure(): void: false
?string > \Closure(): string: false
?string > \Closure(): string|int: false
?string > callable: false
object > none: false
object > void: false
object > string: false
object > int: false
object > bool: false
object > string|int: false
object > string|int|bool: false
object > ?string: false
object > object: true
object > Foo: true
object > Bar: true
object > Foo&Bar: true
object > \Closure(): void: true
object > \Closure(): string: true
object > \Closure(): string|int: true
object > callable: false
Foo > none: false
Foo > void: false
Foo > string: false
Foo > int: false
Foo > bool: false
Foo > string|int: false
Foo > string|int|bool: false
Foo > ?string: false
Foo > object: false
Foo > Foo: true
Foo > Bar: false
Foo > Foo&Bar: true
Foo > \Closure(): void: false
Foo > \Closure(): string: false
Foo > \Closure(): string|int: false
Foo > callable: false
Bar > none: false
Bar > void: false
Bar > string: false
Bar > int: false
Bar > bool: false
Bar > string|int: false
Bar > string|int|bool: false
Bar > ?string: false
Bar > object: false
Bar > Foo: false
Bar > Bar: true
Bar > Foo&Bar: true
Bar > \Closure(): void: false
Bar > \Closure(): string: false
Bar > \Closure(): string|int: false
Bar > callable: false
Foo&Bar > none: false
Foo&Bar > void: false
Foo&Bar > string: false
Foo&Bar > int: false
Foo&Bar > bool: false
Foo&Bar > string|int: false
Foo&Bar > string|int|bool: false
Foo&Bar > ?string: false
Foo&Bar > object: false
Foo&Bar > Foo: false
Foo&Bar > Bar: false
Foo&Bar > Foo&Bar: true
Foo&Bar > \Closure(): void: false
Foo&Bar > \Closure(): string: false
Foo&Bar > \Closure(): string|int: false
Foo&Bar > callable: false
\Closure(): void > none: false
\Closure(): void > void: false
\Closure(): void > string: false
\Closure(): void > int: false
\Closure(): void > bool: false
\Closure(): void > string|int: false
\Closure(): void > string|int|bool: false
\Closure(): void > ?string: false
\Closure(): void > object: false
\Closure(): void > Foo: false
\Closure(): void > Bar: false
\Closure(): void > Foo&Bar: false
\Closure(): void > \Closure(): void: true
\Closure(): void > \Closure(): string: false
\Closure(): void > \Closure(): string|int: false
\Closure(): void > callable: false
\Closure(): string > none: false
\Closure(): string > void: false
\Closure(): string > string: false
\Closure(): string > int: false
\Closure(): string > bool: false
\Closure(): string > string|int: false
\Closure(): string > string|int|bool: false
\Closure(): string > ?string: false
\Closure(): string > object: false
\Closure(): string > Foo: false
\Closure(): string > Bar: false
\Closure(): string > Foo&Bar: false
\Closure(): string > \Closure(): void: false
\Closure(): string > \Closure(): string: true
\Closure(): string > \Closure(): string|int: false
\Closure(): string > callable: false
\Closure(): string|int > none: false
\Closure(): string|int > void: false
\Closure(): string|int > string: false
\Closure(): string|int > int: false
\Closure(): string|int > bool: false
\Closure(): string|int > string|int: false
\Closure(): string|int > string|int|bool: false
\Closure(): string|int > ?string: false
\Closure(): string|int > object: false
\Closure(): string|int > Foo: false
\Closure(): string|int > Bar: false
\Closure(): string|int > Foo&Bar: false
\Closure(): string|int > \Closure(): void: false
\Closure(): string|int > \Closure(): string: true
\Closure(): string|int > \Closure(): string|int: true
\Closure(): string|int > callable: false
callable > none: false
callable > void: false
callable > string: false
callable > int: false
callable > bool: false
callable > string|int: false
callable > string|int|bool: false
callable > ?string: false
callable > object: false
callable > Foo: false
callable > Bar: false
callable > Foo&Bar: false
callable > \Closure(): void: true
callable > \Closure(): string: true
callable > \Closure(): string|int: true
callable > callable: true
