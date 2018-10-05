--TEST--
list() with non-array
--FILE--
<?php

list($a) = NULL;
var_dump($a);

list($b) = 1;
var_dump($b);

list($c) = 1.;
var_dump($c);

list($d) = 'foo';
var_dump($d);

list($e) = print '';
var_dump($e);

?>
--EXPECT--
NULL
NULL
NULL
NULL
NULL
