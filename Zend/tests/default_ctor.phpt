--TEST--
Test calling default parent class methods
--FILE--
<?php

class MyStdClass {}

class Child1 extends StdClass {
  public function __construct() { 
	  parent::__ConsTruct(); 
	  $this->me = 1;
  }
  public function __destruct() {
  	  parent::__deSTRuct();
	  echo "In dtor\n";
  }
  public function __clone() {
  	  parent::__clOnE();
	  echo "In clone\n";
	  $this->me = 42;
  }
}

class Child2 extends MyStdClass {
  public function __construct() { 
	  parent::__construct(); 
	  $this->me = 1;
  }
  public function __destruct() {
  	  parent::__destruct();
	  echo "In dtor\n";
  }
  public function __clone() {
  	  parent::__clone();
	  echo "In clone\n";
	  $this->me = 42;
  }
}

$a = new Child1();
$b = clone $a;
var_dump($b);

$a = new Child2();
$b = clone $a;
var_dump($b);

$a = $b = null;
?>
DONE
--EXPECT--
In clone
object(Child1)#2 (1) {
  ["me"]=>
  int(42)
}
In dtor
In clone
In dtor
object(Child2)#1 (1) {
  ["me"]=>
  int(42)
}
In dtor
In dtor
DONE