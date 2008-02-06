--TEST--
Test octdec() - wrong params  test octdec()
--FILE--
<?php
octdec();
octdec('0123567',true);
?>
--EXPECTF--

Warning: octdec() expects exactly 1 parameter, 0 given in %s on line %d

Warning: octdec() expects exactly 1 parameter, 2 given in %s on line %d
