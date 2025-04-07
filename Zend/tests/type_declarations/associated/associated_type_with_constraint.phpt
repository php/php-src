--TEST--
Associated type with a constraint
--FILE--
<?php

interface I {
    type T : int|string;
    public function foo(T $param): T;
}

class CS implements I {
    public function foo(string $param): string {
        return $param . '!';
    }
}

class CI implements I {
    public function foo(int $param): int {
        return $param + 42;
    }
}

$cs = new CS();
var_dump($cs->foo("Hello"));

$ci = new CI();
var_dump($ci->foo(5));

?>
--EXPECT--
string(6) "Hello!"
int(47)
