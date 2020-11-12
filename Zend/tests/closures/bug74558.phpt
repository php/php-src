--TEST--
Bug #74558 (Can't rebind closure returned by Closure::fromCallable())
--FILE--
<?php

class A {
    private $name;
    public function __construct($name) {
        $this->name = $name;
    }
}

function getName() {
    return $this->name;
}

$bob = new A("Bob");

$cl1 = Closure::fromCallable("getName");
$cl1->bindTo($bob, A::class);
?>
--EXPECTF--
Warning: Cannot rebind scope of closure in %s on line %d
