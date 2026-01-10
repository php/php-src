--TEST--
GH-18000: Lazy proxy calls set hook twice
--FILE--
<?php

class C {
    public $prop {
        set {
            echo "set\n";
            $this->prop = $value * 2;
        }
    }
}

$rc = new ReflectionClass(C::class);

$obj = $rc->newLazyProxy(function () {
    echo "init\n";
    return new C;
});

function foo(C $c) {
    $c->prop = 1;
    var_dump($c->prop);
}

foo($obj);

?>
--EXPECT--
set
init
int(2)
