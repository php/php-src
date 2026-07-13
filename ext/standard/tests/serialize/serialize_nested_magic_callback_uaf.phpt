--TEST--
serialize() retains objects across nested magic callbacks
--FILE--
<?php

class A {
    public $b;
}

class B {
    public $c;
    public $pad = 123;
}

class Spray {
    public $x;
    public $y = 'CONTROLLED';
}

class C {
    public $a;

    public function __serialize(): array {
        unset($this->a->b);
        $GLOBALS['spray'] = new Spray();
        return [];
    }
}

$a = new A();
$b = new B();
$c = new C();

$a->b = $b;
$b->c = $c;
$c->a = $a;

unset($b, $c);
echo serialize($a), "\n";

?>
--EXPECT--
O:1:"A":1:{s:1:"b";O:1:"B":2:{s:1:"c";O:1:"C":0:{}s:3:"pad";i:123;}}
