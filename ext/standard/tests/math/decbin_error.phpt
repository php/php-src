--TEST--
Test decbin() - wrong params
--FILE--
<?php
decbin();
decbin(23,2,true);
?>
--EXPECTF--

Warning: decbin() expects exactly 1 parameter, 0 given in %s on line %d

Warning: decbin() expects exactly 1 parameter, 3 given in %s on line %d
