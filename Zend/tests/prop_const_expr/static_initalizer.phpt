--TEST--
Allow fetching properties in static initializer
--FILE--
<?php

enum A: string {
    case Case = 'A::Case';
}

function foo() {
    static $name = A::Case->name;
    static $value = A::Case->value;
    static $nameNullsafe = A::Case?->name;
    static $valueNullsafe = A::Case?->value;

    var_dump($name);
    var_dump($value);
    var_dump($nameNullsafe);
    var_dump($valueNullsafe);
}

foo();

?>
--EXPECT--
string(4) "Case"
string(7) "A::Case"
string(4) "Case"
string(7) "A::Case"
