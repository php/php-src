--TEST--
Destructuring with list() a value of type null
--FILE--
<?php

list($a) = null;

var_dump($a);

?>
--EXPECT--
NULL
