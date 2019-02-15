--TEST--
Using an integer as a static property name
--FILE--
<?php

class Foo {
    public static $bar = 42;
}

$n = 42;

${42} = 24;
var_dump(${42});
var_dump(${(int) 42});
var_dump(${(int) $n});

try {
    var_dump(Foo::${42});
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(Foo::${(int) 42});
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(Foo::${(int) $n});
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
int(24)
int(24)
int(24)
Access to undeclared static property: Foo::$42
Access to undeclared static property: Foo::$42
Access to undeclared static property: Foo::$42
