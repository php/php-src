--TEST--
Property fetch in property initializer
--FILE--
<?php

enum A: int {
    case B = 42;
}

class C {
    public string $d = A::B->name;
    public int $e = A::B->value;
    public string $f = A::B?->name;
    public int $g = A::B?->value;
}

$c = new C();
var_dump($c->d);
var_dump($c->e);
var_dump($c->f);
var_dump($c->g);

?>
--EXPECT--
string(1) "B"
int(42)
string(1) "B"
int(42)
