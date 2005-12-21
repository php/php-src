--TEST--
Bug #31185 (Crash when exceptions thrown from ArrayAccess::offsetUnset())
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
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
		echo __METHOD__ . "($index, $value)\n";
		$this->array[$index] = $value;
	}

	public function offsetUnset($index) {
		throw new Exception('FAIL');
		unset($this->array[$index]);
	}

}

$i = 0; $j = 0;
$foo = new FooBar();
$foo[$j++] = $i++;
$foo[$j++] = $i++;
$foo[$j++] = $i++;
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
FooBar::offsetSet(0, 0)
FooBar::offsetSet(1, 1)
FooBar::offsetSet(2, 2)
CAUGHT: FAIL
FooBar Object
(
    [array:private] => Array
        (
            [0] => 0
            [1] => 1
            [2] => 2
        )

)
===DONE===
