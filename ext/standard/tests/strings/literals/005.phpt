--TEST--
Literal Variables Single Chars
--FILE--
<?php
$chars = [
    'a',
    'b',
    'c',
    '',
    '\'',
    '\\',
    '/',
    /* and so on */
];

foreach ($chars as $char) {
    var_dump(
        $char,
        is_literal($char));
    echo PHP_EOL;
}

class A {}
class B {}
class C {}

foreach ([A::class, B::class, C::class] as $class) {
    $reflector = new ReflectionClass($class);

    var_dump(
        $class, 
        is_literal($class));
    echo PHP_EOL;
    
    var_dump(
        $reflector->getName(),
        is_literal($reflector->getName()));
    echo PHP_EOL;
}
?>
--EXPECT--
string(1) "a"
bool(true)

string(1) "b"
bool(true)

string(1) "c"
bool(true)

string(0) ""
bool(true)

string(1) "'"
bool(true)

string(1) "\"
bool(true)

string(1) "/"
bool(true)

string(1) "A"
bool(true)

string(1) "A"
bool(true)

string(1) "B"
bool(true)

string(1) "B"
bool(true)

string(1) "C"
bool(true)

string(1) "C"
bool(true)
