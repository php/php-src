--TEST--
Test typed properties overflowing
--PLATFORM--
bits: 64
--FILE--
<?php

class Foo {
    public static int $bar = PHP_INT_MAX;
};

try {
    Foo::$bar++;
} catch(TypeError $t) {
    var_dump($t->getMessage());
}

var_dump(Foo::$bar);

try {
    Foo::$bar += 1;
} catch(TypeError $t) {
    var_dump($t->getMessage());
}

var_dump(Foo::$bar);

try {
    ++Foo::$bar;
} catch(TypeError $t) {
    var_dump($t->getMessage());
}

var_dump(Foo::$bar);

try {
    Foo::$bar = Foo::$bar + 1;
} catch(TypeError $t) {
    var_dump($t->getMessage());
}

var_dump(Foo::$bar);

?>
--EXPECT--
string(70) "Cannot increment property Foo::$bar of type int past its maximal value"
int(9223372036854775807)
string(53) "Cannot assign float to property Foo::$bar of type int"
int(9223372036854775807)
string(70) "Cannot increment property Foo::$bar of type int past its maximal value"
int(9223372036854775807)
string(53) "Cannot assign float to property Foo::$bar of type int"
int(9223372036854775807)
