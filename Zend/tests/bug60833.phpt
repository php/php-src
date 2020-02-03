--TEST--
Bug #60833 (self, parent, static behave inconsistently case-sensitive)
--FILE--
<?php

class A
{
    static $x = "A";
    function testit()
    {
        $this->v1 = new self();
        $this->v2 = new self();
    }
}

class B extends A
{
    static $x = "B";
    function testit()
    {
        parent::testit();
        $this->v3 = new self();
        $this->v4 = new PARENT();
        $this->v4 = static::$x;
    }
}
$t = new B();
$t->testit();
var_dump($t);

?>
--EXPECTF--
object(B)#%d (4) {
  ["v1"]=>
  object(A)#%d (0) {
  }
  ["v2"]=>
  object(A)#%d (0) {
  }
  ["v3"]=>
  object(B)#%d (0) {
  }
  ["v4"]=>
  string(1) "B"
}
