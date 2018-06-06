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
C::a1: int(256)
D::a1: int(256)
C::a2: int(512)
D::a2: int(512)
C::a3: int(1024)
D::a3: int(1024)
C::a4: int(257)
D::a4: int(257)
C::a5: int(513)
D::a5: int(513)
C::a6: int(1025)
D::a6: int(1025)
