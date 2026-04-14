--TEST--
GH-21478: __set on lazy proxy should not double-invoke when real instance guard is set
--FILE--
<?php

#[AllowDynamicProperties]
class Foo {
    public $_;

    public function __set($name, $value) {
        global $proxy;
        printf("__set(\$%s) on %s\n", $name, $this::class);
        $proxy->{$name} = $value;
    }
}

#[AllowDynamicProperties]
class Bar extends Foo {}

$rc = new ReflectionClass(Bar::class);
$proxy = $rc->newLazyProxy(function () {
    echo "Init\n";
    return new Foo();
});

$real = $rc->initializeLazyObject($proxy);
$real->x = 1;

?>
--EXPECT--
Init
__set($x) on Foo
