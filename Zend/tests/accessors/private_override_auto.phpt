--TEST--
Overriding private accessors (auto-generated)
--FILE--
<?php

class A {
    public string $prop1 { get; private set; }

    public function testPrivate() {
        $this->prop1 = 'A';
        var_dump($this->prop1);
        try {
            $this->prop1 = [];
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
    }
}

class B extends A {
    public string $prop1 {
        set { echo __METHOD__, "\n"; }
    }
}

$b = new B;
$b->testPrivate();
$b->prop1 = 'B';
var_dump($b->prop1);

?>
--EXPECT--
string(1) "A"
Cannot assign array to property A::$prop1 of type string
B::$prop1::set
string(1) "A"
