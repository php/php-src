--TEST--
Bug #32394 (offsetUnset() segfaults in a foreach)
--FILE--
<?php

$object = new ArrayIterator;
$object->append(1);

foreach($object as $key => $value)
{
    $object->offsetUnset($key);
}

?>
===DONE===
--EXPECT--
===DONE===
