--TEST--
Usage of property inside hook adds backing store
--FILE--
<?php

class Test {
    public $prop1 { get => $this->prop1; }
    public $prop2 { get => fn () => $this->prop2; }
    public $prop3 { get => function () { return $this->prop3; }; }
    public $prop4 { get => $this->prop1; }
    public $prop5 { get {} }
    public $prop6 { get { var_dump($this->prop6); } }
    public $prop7 { get => new class { function test() { $this->prop7; } }; }
}

foreach ((new ReflectionClass(Test::class))->getProperties() as $prop) {
    var_dump($prop->isVirtual());
}

?>
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
