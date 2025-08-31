--TEST--
Property fetch in default argument
--FILE--
<?php

enum A: string {
    case B = 'C';
}

function test(
    $name = A::B->name,
    $value = A::B->value,
    $nameNullsafe = A::B?->name,
    $valueNullsafe = A::B?->value,
) {
    var_dump($name);
    var_dump($value);
    var_dump($nameNullsafe);
    var_dump($valueNullsafe);
}

test();
test('D', 'E', 'F', 'G');

?>
--EXPECT--
string(1) "B"
string(1) "C"
string(1) "B"
string(1) "C"
string(1) "D"
string(1) "E"
string(1) "F"
string(1) "G"
