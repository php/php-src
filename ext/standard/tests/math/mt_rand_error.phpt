--TEST--
Test mt_rand() - wrong params test mt_rand()
--FILE--
<?php
mt_rand(25);
mt_rand(10,100,false);
mt_rand("one", 100);
mt_rand(1, "hundered");
?>
--EXPECTF--
Warning: mt_rand() expects exactly 2 parameters, 1 given in %s on line 2

Warning: mt_rand() expects exactly 2 parameters, 3 given in %s on line 3

Warning: mt_rand() expects parameter 1 to be integer, string given in %s on line 4

Warning: mt_rand() expects parameter 2 to be integer, string given in %s on line 5
