--TEST--
Bug #31185 (Crash when exceptions thrown from ArrayAccess::offsetUnset())
--FILE--
<?php

class FooBar implements ArrayAccess {
	private $array = array();

	public function offsetExists($index) {
		return isset($this->array[$index]);
	}

	public function offsetGet($index) {
		return $this->array[$index];
	}

	public function offsetSet($index, $value) {
		$this->array[$index] = $value;
	}

	public function offsetUnset($index) {
		throw new Exception('FAIL');
		unset($this->array[$index]);
	}

}

$i = 0;
$foo = new FooBar();
$foo[$i] = $i++;
$foo[$i] = $i++;
$foo[$i] = $i++;
try
{
	unset($foo[1]);
}
catch (Exception $e)
{
	echo "CAUGHT: " . $e->getMessage() . "\n";
}

print_R($foo);
?>
===DONE===
--EXPECT--
CAUGHT: FAIL
FooBar Object
(
    [array:private] => Array
        (
            [1] => 0
            [2] => 1
            [3] => 2
        )

)
===DONE===
