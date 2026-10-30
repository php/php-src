--TEST--
GH-21478: __isset on lazy proxy should not double-invoke when real instance guard is set
--FILE--
<?php

class Foo {
    public $_;

    public function __isset($name) {
        global $proxy;
        printf("__isset(\$%s) on %s\n", $name, $this::class);
        return isset($proxy->{$name});
    }
}

class Bar extends Foo {}

$rc = new ReflectionClass(Bar::class);
$proxy = $rc->newLazyProxy(function () {
    echo "Init\n";
    return new Foo();
});

$real = $rc->initializeLazyObject($proxy);
isset($real->x);

?>
--EXPECT--
Init
__isset($x) on Foo
