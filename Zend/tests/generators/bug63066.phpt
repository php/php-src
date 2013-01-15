--TEST--
Bug #63066 (Calling an undefined method in a generator results in a seg fault)
--FILE--
<?php
function gen($o)
{
	yield 'foo';
	$o->fatalError();
}

foreach(gen(new stdClass()) as $value)
	echo $value, "\n";
--EXPECTF--
foo

Fatal error: Call to undefined method stdClass::fatalError() in %sbug63066.php on line 5
