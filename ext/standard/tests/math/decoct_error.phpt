--TEST--
Test decoct() - wrong params decoct()
--FILE--
<?php
decoct();
decoct(23,2,true);
?>
--EXPECTF--

Warning: Wrong parameter count for decoct() in %s on line 2

Warning: Wrong parameter count for decoct() in %s on line 3

