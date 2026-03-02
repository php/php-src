--TEST--
Random: Engine: User: Returning by reference works
--FILE--
<?php

use Random\Engine;
use Random\Randomizer;

final class ReferenceEngine implements Engine
{
	private $field = 'abcdef';

	public function &generate(): string
	{
		return $this->field;
	}
}

$randomizer = new Randomizer(new ReferenceEngine());

var_dump($randomizer->getBytes(64));

?>
--EXPECT--
string(64) "abcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcdefabcd"
