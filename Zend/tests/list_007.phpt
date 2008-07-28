--TEST--
Using lambda with list()
--FILE--
<?php

list($x, $y) = function() { };

var_dump($x, $y);

?>
--EXPECT--
NULL
NULL
