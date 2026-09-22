--TEST--
Custom unserialization of classes with no custom unserializer.
--FILE--
<?php
$ser = 'C:1:"C":6:{dasdas}';
$a = unserialize($ser);
eval('class C {}');
$b = unserialize($ser);

var_dump($a, $b);
echo "Done";
?>
--EXPECTF--
Warning: Class __PHP_Incomplete_Class has no unserializer in %s on line %d

Warning: unserialize(): Error at offset 11 of 18 bytes in %s on line %d

Warning: Class C has no unserializer in %s on line %d

Warning: unserialize(): Error at offset 11 of 18 bytes in %s on line %d
bool(false)
bool(false)
Done
