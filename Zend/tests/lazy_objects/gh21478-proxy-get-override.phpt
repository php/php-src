--TEST--
GH-21478: Proxy's own __get runs when accessed directly (not from real instance)
--FILE--
<?php

class Foo {
    private $_;

    public function __get($name) {
        echo __CLASS__, " ", $name, "\n";
    }
}

class Bar extends Foo {
    public function __get($name) {
        echo __CLASS__, " ", $name, "\n";
    }
}

$rc = new ReflectionClass(Bar::class);
$proxy = $rc->newLazyProxy(function () {
    return new Foo();
});
$rc->initializeLazyObject($proxy);

$proxy->x;

?>
--EXPECT--
Bar x
