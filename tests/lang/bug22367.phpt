--TEST--
Bug #22367 (weird zval allocation problem)
--SKIPIF--
<?php if(version_compare(zend_version(), "2.0.0-dev", '>=')) echo "skip Overriden method does not match overriding method - incompatible with ZE2\n"; ?>
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
int(5)
bool(false)

Notice: Undefined offset:  %d in %s on line %d

Strict Standards: Only variable references should be returned by reference in %s on line %d
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
