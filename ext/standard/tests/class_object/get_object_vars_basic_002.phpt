--TEST--
get_object_vars(): visibility from non static methods (target object passed as arg)
--FILE--
<?php
Class A {
    private $hiddenPriv = 'A::hiddenPriv';

    public function testA($b) {
        echo __METHOD__ . "\n";
        var_dump(get_object_vars($b));
    }
}

Class B extends A {
    private $hiddenPriv = 'B::hiddenPriv';
    private $priv = 'B::priv';
    protected $prot = 'B::prot';
    public $pub = 'B::pub';

    public function testB($b) {
        echo __METHOD__ . "\n";
        var_dump(get_object_vars($b));
    }
}


$b = new B;
echo "\n---( Declaring class: )---\n";
$b->testB($b);
echo "\n---( Superclass: )---\n";
$b->testA($b);

?>
--EXPECT--
---( Declaring class: )---
B::testB
array(4) {
  ["hiddenPriv"]=>
  string(13) "B::hiddenPriv"
  ["priv"]=>
  string(7) "B::priv"
  ["prot"]=>
  string(7) "B::prot"
  ["pub"]=>
  string(6) "B::pub"
}

---( Superclass: )---
A::testA
array(3) {
  ["hiddenPriv"]=>
  string(13) "A::hiddenPriv"
  ["prot"]=>
  string(7) "B::prot"
  ["pub"]=>
  string(6) "B::pub"
}
