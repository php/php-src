--TEST--
Testing list() nested with empty array and default value
--FILE--
<?php

list($a, list($b, list(list($c=100, $d)))) = array();
var_dump($a, $b, $c, $d);

?>
--EXPECTF--
Notice: Undefined offset: 0 in %s on line %d

Notice: Undefined offset: 1 in %s on line %d
NULL
NULL
int(100)
NULL
