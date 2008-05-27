--TEST--
Test srand() - wrong params test srand()
--FILE--
<?php
var_dump(mt_srand(500, true));
var_dump(mt_srand("fivehundred"));
var_dump(mt_srand("500ABC"));
?>
--EXPECTF--
Warning: mt_srand() expects at most 1 parameter, 2 given in %s on line %d
NULL

Warning: mt_srand() expects parameter 1 to be long, Unicode string given in %s on line %d
NULL

Notice: A non well formed numeric value encountered in %s on line %d
NULL
