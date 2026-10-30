--TEST--
GH-21478: __unset on lazy proxy should not double-invoke when real instance guard is set
--FILE--
<?php

class Foo {
    public $_;

    public function __unset($name) {
        global $proxy;
        printf("__unset(\$%s) on %s\n", $name, $this::class);
        unset($proxy->{$name});
    }
}

class Bar extends Foo {}

$rc = new ReflectionClass(Bar::class);
$proxy = $rc->newLazyProxy(function () {
    echo "Init\n";
    return new Foo();
});

$real = $rc->initializeLazyObject($proxy);
unset($real->x);

?>
--EXPECT--
Init
__unset($x) on Foo
