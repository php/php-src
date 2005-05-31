--TEST--
Bug #27304
--FILE--
<?php

class Staticexample
{
	static function test()
	{
		var_dump(isset($this));
	}
}

$b = new Staticexample();
Staticexample::test();
$b->test();
        
?>
===DONE===
--EXPECT--
bool(false)
bool(false)
===DONE===
