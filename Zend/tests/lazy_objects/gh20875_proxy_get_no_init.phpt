--TEST--
GH-20875 (Lazy proxy should not initialize when __get handles dynamic property access)
--FILE--
<?php

class Foo {
    public $_;

    public function &__get($name) {
        echo "__get\n";
        return $name;
    }
}

$proxy = (new ReflectionClass(Foo::class))->newLazyProxy(function () {
    echo "init\n";
    return new Foo();
});
$x = &$proxy->x;

?>
--EXPECT--
__get
