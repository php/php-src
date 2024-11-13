--TEST--
ReflectionProperty::isFinal()
--FILE--
<?php

class C {
    public $p1;
    public final $p2;
    public $p3 { get => 42; }
    public final $p4 { get => 42; }
    public protected(set) mixed $p5;
    public protected(set) final mixed $p6;
    public private(set) mixed $p7;
    public private(set) final mixed $p8;
}

$rc = new ReflectionClass(C::class);
foreach ($rc->getProperties() as $rp) {
    echo $rp->getName(), ": ";
    var_dump($rp->isFinal());
}

?>
--EXPECT--
p1: bool(false)
p2: bool(true)
p3: bool(false)
p4: bool(true)
p5: bool(false)
p6: bool(true)
p7: bool(true)
p8: bool(true)
