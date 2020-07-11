--TEST--
Test to ensure semi reserved words allow deference
--FILE--
<?php

class Foo {
    const use = 'yay';

    public static function new() {
        echo __METHOD__, PHP_EOL;
        return new static();
    }

    public function self() {
        echo __METHOD__, PHP_EOL;
        return $this;
    }
}

Foo::new()::new()::new();

var_dump(
    (new Foo)->self()::new()->self()->self()::use
);

Foo::{'new'}();

var_dump(Foo::use);

echo "\nDone\n";
--EXPECT--
Foo::new
Foo::new
Foo::new
Foo::self
Foo::new
Foo::self
Foo::self
string(3) "yay"
Foo::new
string(3) "yay"

Done
