--TEST--
Bug #72543.3 (different references behavior comparing to PHP 5)
--FILE--
<?php
$x = new stdClass;
$x->a = 1;
$ref =& $x->a;
unset($ref);
var_dump($x->a + ($x->a = 2));
?>
--EXPECT--
int(3)
