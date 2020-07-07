--TEST--
Testing nested list() with empty array
--FILE--
<?php

list($a, list($b, list(list($d)))) = array();

?>
--EXPECTF--
Notice: Undefined array offset 0 in %s on line %d

Notice: Undefined array offset 1 in %s on line %d
