--TEST--
Test typed properties overflowing
--SKIPIF--
<?php if (PHP_INT_SIZE == 4) die("SKIP: 64 bit test"); ?>
--FILE--
<?php

class Foo {
    public static int $bar = PHP_INT_MAX;
};

try {
    Foo::$bar++;
} catch(Throwable $t) {
    echo $t::class, ': ', $t->getMessage(), "\n";
}

var_dump(Foo::$bar);

try {
    Foo::$bar += 1;
} catch(Throwable $t) {
    echo $t::class, ': ', $t->getMessage(), "\n";
}

var_dump(Foo::$bar);

try {
    ++Foo::$bar;
} catch(Throwable $t) {
    echo $t::class, ': ', $t->getMessage(), "\n";
}

var_dump(Foo::$bar);

try {
    Foo::$bar = Foo::$bar + 1;
} catch(Throwable $t) {
    echo $t::class, ': ', $t->getMessage(), "\n";
}

var_dump(Foo::$bar);

?>
--EXPECT--
TypeError: Cannot increment property Foo::$bar of type int past its maximal value
int(9223372036854775807)
TypeError: Cannot assign float to property Foo::$bar of type int
int(9223372036854775807)
TypeError: Cannot increment property Foo::$bar of type int past its maximal value
int(9223372036854775807)
TypeError: Cannot assign float to property Foo::$bar of type int
int(9223372036854775807)
