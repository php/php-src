--TEST--
Test decoct() - wrong params decoct()
--FILE--
<?php
decoct();
decoct(23,2,true);
?>
--EXPECTF--

Warning: decoct() expects exactly 1 parameter, 0 given in %s on line %d

Warning: decoct() expects exactly 1 parameter, 3 given in %s on line %d
