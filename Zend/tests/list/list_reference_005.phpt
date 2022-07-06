--TEST--
"Reference Unpacking - Class Property and Methods" list()
--FILE--
<?php
class A {
    public $a = [['hello']];
    public $b = ['world'];

    public function getVar() {
        return $this->a;
    }

    public function &getVarRef() {
        return $this->a;
    }
}

class B {
    static $a = [['world']];
}

$a = new A();
[&$var] = $a->a;
[&$var_too] = $a->b;
var_dump($a->a);
var_dump($a->b);

$a = new A();
[&$var] = $a->getVar();
var_dump($a->a);

$a = new A();
[&$var] = $a->getVarRef();
var_dump($a->a);

[&$var] = B::$a;
var_dump(B::$a);
?>
--EXPECTF--
array(1) {
  [0]=>
  &array(1) {
    [0]=>
    string(5) "hello"
  }
}
array(1) {
  [0]=>
  &string(5) "world"
}

Notice: Attempting to set reference to non referenceable value in %s on line %d
array(1) {
  [0]=>
  array(1) {
    [0]=>
    string(5) "hello"
  }
}
array(1) {
  [0]=>
  &array(1) {
    [0]=>
    string(5) "hello"
  }
}
array(1) {
  [0]=>
  &array(1) {
    [0]=>
    string(5) "world"
  }
}
