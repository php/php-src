--TEST--
list() with non-array
--FILE--
<?php

list($a) = NULL;

list($b) = 1;

list($c) = 1.;

list($d) = 'foo';

list($e) = print '';

var_dump($a, $b, $c, $d, $e);

?>
--EXPECT--
NULL
NULL
NULL
NULL
NULL
