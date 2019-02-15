--TEST--
Test mt_srand() - wrong params test mt_srand()
--FILE--
<?php
var_dump(mt_srand(500, 0, true));
var_dump(mt_srand("fivehundred"));
var_dump(mt_srand("500ABC"));
?>
--EXPECTF--
Warning: mt_srand() expects at most 2 parameters, 3 given in %s on line 2
NULL

Warning: mt_srand() expects parameter 1 to be int, string given in %s on line 3
NULL

Notice: A non well formed numeric value encountered in %s on line 4
NULL
