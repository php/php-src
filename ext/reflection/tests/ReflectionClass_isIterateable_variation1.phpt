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
$stdClass = new StdClass();

dump_iterateable($basicClass);
dump_iterateable($stdClass);

?>
--EXPECT--
bool(false)
bool(false)
