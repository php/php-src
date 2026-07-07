--TEST--
Pipe assign operator typed property handling
--FILE--
<?php

class Foo {
    public string $foo = "foo";
}

$f = new Foo();
$f->foo |>= strlen(...);
var_dump($f->foo);

class Bar {
    public array $bar = ["bar"];
}

$b = new Bar();
try {
    $b->bar |>= count(...);
} catch (TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
string(1) "3"
Cannot assign int to property Bar::$bar of type array
