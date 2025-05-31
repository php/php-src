--TEST--
Random: Engine: User: Empty strings do not leak
--FILE--
<?php

use Random\Engine;
use Random\Randomizer;
use Random\BrokenRandomEngineError;

class MyEngine implements Engine {
    private $field = '';

	public function &generate(): string
	{
		return $this->field;
	}
}

$randomizer = new Randomizer(new MyEngine());
try {
    $randomizer->getBytes(64);
} catch (BrokenRandomEngineError $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
A random engine must return a non-empty string
