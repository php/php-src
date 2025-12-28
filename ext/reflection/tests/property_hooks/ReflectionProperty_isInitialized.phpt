--TEST--
ReflectionProperty::isInitialized() on hooked properties
--FILE--
<?php

class Test {
    // Plain
    public $p1;
    public string $p2;
    // Virtual
    public $v1 { get => throw new Exception(); }
    public $v2 { set { throw new Exception(); } }
    // Backed
    public $b1 { get => throw new Exception($this->b1); }
    public string $b2 { get => throw new Exception($this->b2); }
    public $b3 { set => throw new Exception(); }
    public string $b4 { set => throw new Exception(); }
}

$test = new Test();
$rc = new ReflectionClass(Test::class);
foreach ($rc->getProperties() as $rp) {
    echo $rp->getName(), "\n";
    var_dump($rp->isInitialized($test));
    try {
        $rp->setRawValue($test, 42);
    } catch (Error $e) {}
    var_dump($rp->isInitialized($test));
}

?>
--EXPECT--
p1
bool(true)
bool(true)
p2
bool(false)
bool(true)
v1
bool(true)
bool(true)
v2
bool(true)
bool(true)
b1
bool(true)
bool(true)
b2
bool(false)
bool(true)
b3
bool(true)
bool(true)
b4
bool(false)
bool(true)
