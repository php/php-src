--TEST--
ReflectionClass::isIterateable() variations
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
--FILE--
<?php

class BasicClass {}

function dump_iterateable($obj)
{
	$reflection = new ReflectionClass($obj);
	var_dump($reflection->isIterateable());
}

$basicClass = new BasicClass();
$StdClass = new StdClass();

dump_iterateable($basicClass);
dump_iterateable($StdClass);

?>
--EXPECT--
bool(false)
bool(false)
