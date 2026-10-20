--TEST--
Testing nested list() with empty array
--FILE--
<?php

list($a, list($b, list(list($d)))) = array();

?>
--EXPECTF--
Warning: Undefined array key 0 in %s on line %d

Warning: Undefined array key 1 in %s on line %d
