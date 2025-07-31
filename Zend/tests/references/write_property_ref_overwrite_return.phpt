--TEST--
Using return of property assignment to reference that destroys object
--FILE--
<?php

$a = new stdClass;
$a->a =& $a;
var_dump($a->a = 0);

?>
--EXPECT--
int(0)
