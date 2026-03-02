--TEST--
ReflectionClass::getMethods()
--FILE--
<?php
$l = function() {};
$o=new ReflectionObject($l);
$o->getMethods(2);
?>
DONE
--EXPECT--
DONE
