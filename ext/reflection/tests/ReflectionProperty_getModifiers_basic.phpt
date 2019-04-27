--TEST--
Test ReflectionProperty::getModifiers() usage.
--FILE--
<?php

class C {
    public $a1;
    protected $a2;
    private $a3;
    static public $a4;
    static protected $a5;
    static private $a6;
}

class D extends C {
    public $a1;
    protected $a2;
    private $a3;
    static public $a4;
    static protected $a5;
    static private $a6;
}

for ($i = 1;$i <= 6;$i++) {
    $rp = new ReflectionProperty("C", "a$i");
    echo "C::a$i: ";
    var_dump($rp->getModifiers());
    $rp = new ReflectionProperty("D", "a$i");
    echo "D::a$i: ";
    var_dump($rp->getModifiers());
}

?>
--EXPECT--
C::a1: int(1)
D::a1: int(1)
C::a2: int(2)
D::a2: int(2)
C::a3: int(4)
D::a3: int(4)
C::a4: int(17)
D::a4: int(17)
C::a5: int(18)
D::a5: int(18)
C::a6: int(20)
D::a6: int(20)
