--TEST--
Bug #22367 (weird zval allocation problem)
--INI--
error_reporting=4095
zend.ze1_compatibility_mode=1
--FILE--
<?php

class foo
{
	public $test = array(0, 1, 2, 3, 4); 

	function a($arg) {
		var_dump(array_key_exists($arg, $this->test));
		return $this->test[$arg];
	}

	function b() {
		@$this->c();

		$zero = $this->test[0];
		$one = $this->test[1];
		$two = $this->test[2];
		$three = $this->test[3];
		$four = $this->test[4];
		return array($zero, $one, $two, $three, $four);
	}

	function c() {
		return $this->a($this->d());
	}

	function d() {}
}

class bar extends foo
{
	public $i = 0;
	public $idx;

	function bar($idx) {
		$this->idx = $idx;
	}

	function &a($arg){
		return parent::a($arg);
	}
	function d(){
		return $this->idx;
	}
}

$a = new bar(5);
var_dump($a->idx);
$a->c();
$b = $a->b();
var_dump($b);
var_dump($a->test);

$a = new bar(2);
var_dump($a->idx);
@$a->c();
$b = $a->b();
var_dump($b);
var_dump($a->test);

?>
--EXPECTF--
Strict Standards: Declaration of bar::a() should be compatible with that of foo::a() in %sbug22367.php on line %d

Strict Standards: Implicit cloning object of class 'bar' because of 'zend.ze1_compatibility_mode' in %sbug22367.php on line %d
int(5)
bool(false)

Notice: Undefined offset:  5 in %sbug22367.php on line %d

Notice: Only variable references should be returned by reference in %sbug22367.php on line %d
bool(false)
array(5) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
}
array(5) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
}

Strict Standards: Implicit cloning object of class 'bar' because of 'zend.ze1_compatibility_mode' in %sbug22367.php on line %d
int(2)
bool(true)
bool(true)
array(5) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
}
array(5) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
}
