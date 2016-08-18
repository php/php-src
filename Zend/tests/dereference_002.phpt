--TEST--
Testing array dereference on method calls
--FILE--
<?php

error_reporting(E_ALL);

class  foo {
	public function bar() {
		$x = array();
		$x[] = 3;
		$x[] = array(1, 5);
		$x[] = new foo;
		return $x;
	}
}

$foo = new foo;

var_dump($x = $foo->bar()[1]);
var_dump($foo->bar()[1][1]);
var_dump($x[0]);
var_dump($x = $foo->bar()[2]);
var_dump($x->bar());
var_dump($x->bar()[0]);

$x = array();
$x[] = new foo;
var_dump($x[0]->bar()[2]);
var_dump($foo->bar()[2]->bar()[1]);
var_dump($foo->bar()[2]->bar()[2]->bar()[1][0]);
var_dump($foo->bar()[2]->bar()[2]->bar()[1][0][1]);
var_dump($foo->bar()[2]->bar()[2]->bar()[4]);
var_dump($foo->bar()[3]->bar());

?>
--EXPECTF--
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(5)
}
int(5)
int(1)
object(foo)#2 (0) {
}
array(3) {
  [0]=>
  int(3)
  [1]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(5)
  }
  [2]=>
  object(foo)#3 (0) {
  }
}
int(3)
object(foo)#3 (0) {
}
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(5)
}
int(1)
NULL

Notice: Undefined offset: 4 in %s on line %d
NULL

Notice: Undefined offset: 3 in %s on line %d

Fatal error: Uncaught Error: Call to a member function bar() on null in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
