--TEST--
User engine leak on error
--FILE--
<?php

class MyEngine implements Random\Engine {
    private $field = '';

	public function &generate(): string
	{
		return $this->field;
	}
}

$randomizer = new Random\Randomizer(new MyEngine);
try {
    $randomizer->getBytes(64);
} catch (Random\BrokenRandomEngineError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
A random engine must return a non-empty string
