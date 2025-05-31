--TEST--
User engine reference return value
--FILE--
<?php

class MyEngine implements Random\Engine {
	private $field = 'abcdef';

	public function &generate(): string
	{
		return $this->field;
	}
}

$randomizer = new Random\Randomizer(new MyEngine);
var_dump($randomizer->getBytes(64));

?>
--EXPECT--
string(64) "abcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcd"
