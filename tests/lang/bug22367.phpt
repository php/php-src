--TEST--
Bug #22367 (weird zval allocation problem)
--FILE--
<?php
class foo
{
	var $test = array(0, 1, 2, 3, 4); 

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
	var $i = 0;
	var $idx;

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

Fatal error: Only variables or references can be returned by reference in %s on line %d
