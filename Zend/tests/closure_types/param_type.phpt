--TEST--
Closure type return type covariance
--FILE--
<?php

interface Foo {}
interface Bar {}

function test($type, $value) {
    echo ($type ?: 'none') . ' > ' . ($value ?: 'none') . ': ';
    try {
        eval("(function (fn($type \$p) \$c) {})(function ($value \$p) {});");
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
    'fn(): void',
    'fn(): string',
    'fn(): string|int',
    'callable',
];

foreach ($values as $type) {
    foreach ($values as $value) {
        test($type, $value);
    }
}

?>
--EXPECT--

none > none: false
none > void: false
none > string: false
none > int: false
none > bool: false
none > string|int: false
none > string|int|bool: false
none > ?string: false
none > object: false
none > Foo: false
none > Bar: false
none > Foo&Bar: false
none > fn(): void: false
none > fn(): string: false
none > fn(): string|int: false
none > callable: false
void > none: false
void > void: false
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
void > fn(): void: false
void > fn(): string: false
void > fn(): string|int: false
void > callable: false
string > none: false
string > void: false
string > string: false
string > int: false
string > bool: false
string > string|int: false
string > string|int|bool: false
string > ?string: false
string > object: false
string > Foo: false
string > Bar: false
string > Foo&Bar: false
string > fn(): void: false
string > fn(): string: false
string > fn(): string|int: false
string > callable: false
int > none: false
int > void: false
int > string: false
int > int: false
int > bool: false
int > string|int: false
int > string|int|bool: false
int > ?string: false
int > object: false
int > Foo: false
int > Bar: false
int > Foo&Bar: false
int > fn(): void: false
int > fn(): string: false
int > fn(): string|int: false
int > callable: false
bool > none: false
bool > void: false
bool > string: false
bool > int: false
bool > bool: false
bool > string|int: false
bool > string|int|bool: false
bool > ?string: false
bool > object: false
bool > Foo: false
bool > Bar: false
bool > Foo&Bar: false
bool > fn(): void: false
bool > fn(): string: false
bool > fn(): string|int: false
bool > callable: false
string|int > none: false
string|int > void: false
string|int > string: false
string|int > int: false
string|int > bool: false
string|int > string|int: false
string|int > string|int|bool: false
string|int > ?string: false
string|int > object: false
string|int > Foo: false
string|int > Bar: false
string|int > Foo&Bar: false
string|int > fn(): void: false
string|int > fn(): string: false
string|int > fn(): string|int: false
string|int > callable: false
string|int|bool > none: false
string|int|bool > void: false
string|int|bool > string: false
string|int|bool > int: false
string|int|bool > bool: false
string|int|bool > string|int: false
string|int|bool > string|int|bool: false
string|int|bool > ?string: false
string|int|bool > object: false
string|int|bool > Foo: false
string|int|bool > Bar: false
string|int|bool > Foo&Bar: false
string|int|bool > fn(): void: false
string|int|bool > fn(): string: false
string|int|bool > fn(): string|int: false
string|int|bool > callable: false
?string > none: false
?string > void: false
?string > string: false
?string > int: false
?string > bool: false
?string > string|int: false
?string > string|int|bool: false
?string > ?string: false
?string > object: false
?string > Foo: false
?string > Bar: false
?string > Foo&Bar: false
?string > fn(): void: false
?string > fn(): string: false
?string > fn(): string|int: false
?string > callable: false
object > none: false
object > void: false
object > string: false
object > int: false
object > bool: false
object > string|int: false
object > string|int|bool: false
object > ?string: false
object > object: false
object > Foo: false
object > Bar: false
object > Foo&Bar: false
object > fn(): void: false
object > fn(): string: false
object > fn(): string|int: false
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
Foo > Foo: false
Foo > Bar: false
Foo > Foo&Bar: false
Foo > fn(): void: false
Foo > fn(): string: false
Foo > fn(): string|int: false
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
Bar > Bar: false
Bar > Foo&Bar: false
Bar > fn(): void: false
Bar > fn(): string: false
Bar > fn(): string|int: false
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
Foo&Bar > Foo&Bar: false
Foo&Bar > fn(): void: false
Foo&Bar > fn(): string: false
Foo&Bar > fn(): string|int: false
Foo&Bar > callable: false
fn(): void > none: false
fn(): void > void: false
fn(): void > string: false
fn(): void > int: false
fn(): void > bool: false
fn(): void > string|int: false
fn(): void > string|int|bool: false
fn(): void > ?string: false
fn(): void > object: false
fn(): void > Foo: false
fn(): void > Bar: false
fn(): void > Foo&Bar: false
fn(): void > fn(): void: false
fn(): void > fn(): string: false
fn(): void > fn(): string|int: false
fn(): void > callable: false
fn(): string > none: false
fn(): string > void: false
fn(): string > string: false
fn(): string > int: false
fn(): string > bool: false
fn(): string > string|int: false
fn(): string > string|int|bool: false
fn(): string > ?string: false
fn(): string > object: false
fn(): string > Foo: false
fn(): string > Bar: false
fn(): string > Foo&Bar: false
fn(): string > fn(): void: false
fn(): string > fn(): string: false
fn(): string > fn(): string|int: false
fn(): string > callable: false
fn(): string|int > none: false
fn(): string|int > void: false
fn(): string|int > string: false
fn(): string|int > int: false
fn(): string|int > bool: false
fn(): string|int > string|int: false
fn(): string|int > string|int|bool: false
fn(): string|int > ?string: false
fn(): string|int > object: false
fn(): string|int > Foo: false
fn(): string|int > Bar: false
fn(): string|int > Foo&Bar: false
fn(): string|int > fn(): void: false
fn(): string|int > fn(): string: false
fn(): string|int > fn(): string|int: false
fn(): string|int > callable: false
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
callable > fn(): void: false
callable > fn(): string: false
callable > fn(): string|int: false
callable > callable: false
