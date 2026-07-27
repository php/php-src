--TEST--
GH-21478: No assertion failure when &__get forwards through initialized lazy proxy
--FILE--
<?php
class Foo {
    public $_;

    public function &__get($name) {
        global $proxy;
        printf("%s(\$%s) on %s\n", __METHOD__, $name, $this::class);
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
$a = &$real->x;
var_dump($a);
?>
--EXPECTF--
Init
Foo::__get($x) on Foo

Warning: Undefined property: Foo::$x in %s on line %d
NULL
