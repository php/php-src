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
--EXPECTF--
Warning: Variable of type null does not accept array offsets in %s on line %d

Warning: Variable of type integer does not accept array offsets in %s on line %d

Warning: Variable of type float does not accept array offsets in %s on line %d

Warning: Variable of type string does not accept array offsets in %s on line %d

Warning: Variable of type integer does not accept array offsets in %s on line %d
NULL
NULL
NULL
NULL
NULL
