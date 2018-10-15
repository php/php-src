--TEST--
Test  rand() - wrong params test rand()
--FILE--
<?php
rand(25);
rand(10,100,false);
rand("one", 100);
rand(1, "hundered");
?>
--EXPECTF--
Warning: rand() expects exactly 2 parameters, 1 given in %s on line 2

Warning: rand() expects exactly 2 parameters, 3 given in %s on line 3

Warning: rand() expects parameter 1 to be integer, string given in %s on line 4

Warning: rand() expects parameter 2 to be integer, string given in %s on line 5
