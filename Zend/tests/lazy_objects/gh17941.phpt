--TEST--
GH-17941 (Stack-use-after-return with lazy objects and hooks)
--FILE--
<?php

class SubClass {
    public $prop {get => $this->prop; set($x) => $this->prop = $x;}
}

$rc = new ReflectionClass(SubClass::class);
$obj = $rc->newLazyProxy(function ($object) {
    echo "init\n";
    return new SubClass;
});

function foo(SubClass $x) {
    $x->prop = 1;
    var_dump($x->prop);
}

foo($obj);

?>
--EXPECT--
init
int(1)
