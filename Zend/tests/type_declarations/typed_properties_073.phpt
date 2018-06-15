--TEST--
Typed property must cast when used with &__get()
--FILE--
<?php
  
class Test {
	public $prop = "42";
	public int $val;

	public function &__get($name) {
		return $this->prop;
	}
}

$test = new Test;
var_dump($test);
var_dump($intval = &$test->val);
var_dump($test);

try {
	$test->prop = "x";
} catch (TypeError $e) { print $e->getMessage()."\n"; }
var_dump($intval);

?>
--EXPECT--
object(Test)#1 (1) {
  ["prop"]=>
  string(2) "42"
  ["val"]=>
  uninitialized(int)
}
int(42)
object(Test)#1 (1) {
  ["prop"]=>
  &int(42)
  ["val"]=>
  uninitialized(int)
}
Cannot assign string to reference of type int
int(42)
