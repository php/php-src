--TEST--
Check deprecation to bool type in weak mode
--FILE--
<?php

function foo(bool $b) {
    var_dump($b);
}

function bar($v): bool {
    var_dump($v);
    return $v;
}

class C {
    public bool $b;
}

$o = new C();

echo "int:\n";
foo(42);
bar(42);
$o->b = 42;
var_dump($o->b);

echo "float:\n";
foo(2.5);
bar(12.5);
$o->b = 12.5;
var_dump($o->b);

echo "string:\n";
foo("hello");
bar("hello");
$o->b = "hello";
var_dump($o->b);

?>
--EXPECTF--
int:

Deprecated: foo(): Passing int to parameter #1 ($b) of type bool is deprecated in %s on line %d
bool(true)
int(42)

Deprecated: bar(): Returning type int which is implicitly converted to type bool is deprecated in %s on line %d

Deprecated: Assigning int to type property which is implicitly converted to type bool is deprecated in %s on line %d
bool(true)
float:

Deprecated: foo(): Passing float to parameter #1 ($b) of type bool is deprecated in %s on line %d
bool(true)
float(12.5)

Deprecated: bar(): Returning type float which is implicitly converted to type bool is deprecated in %s on line %d

Deprecated: Assigning float to type property which is implicitly converted to type bool is deprecated in %s on line %d
bool(true)
string:

Deprecated: foo(): Passing string to parameter #1 ($b) of type bool is deprecated in %s on line %d
bool(true)
string(5) "hello"

Deprecated: bar(): Returning type string which is implicitly converted to type bool is deprecated in %s on line %d

Deprecated: Assigning string to type property which is implicitly converted to type bool is deprecated in %s on line %d
bool(true)
