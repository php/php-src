--TEST--
GH-21478 (Property access on lazy proxy may invoke magic method despite real instance guards)
--FILE--
<?php

class Foo {
    public $_;

    public function __get($name) {
        global $proxy;
        printf("__get(\$%s) on %s\n", $name, $this::class);
        return $proxy->{$name};
    }
}

class Bar extends Foo {}

$rc = new ReflectionClass(Bar::class);
$proxy = $rc->newLazyProxy(function () {
    echo "Init\n";
    return new Foo();
});

$real = $rc->initializeLazyObject($proxy);
$real->x;

?>
--EXPECTF--
Init
__get($x) on Foo

Warning: Undefined property: Foo::$x in %s on line %d
