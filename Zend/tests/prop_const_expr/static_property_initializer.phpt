--TEST--
Property fetch in static property initializer
--FILE--
<?php

enum A: int {
    case B = 42;
}

class C {
    public static string $d = A::B->name;
    public static int $e = A::B->value;
    public static string $f = A::B?->name;
    public static int $g = A::B?->value;
}

var_dump(C::$d);
var_dump(C::$e);
var_dump(C::$f);
var_dump(C::$g);

?>
--EXPECT--
string(1) "B"
int(42)
string(1) "B"
int(42)
