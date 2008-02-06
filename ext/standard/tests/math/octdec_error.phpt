--TEST--
Test octdec() - wrong params  test octdec()
--FILE--
<?php
octdec();
octdec('0123567',true);
?>
--EXPECTF--

Warning: Wrong parameter count for octdec() in %s on line 2

Warning: Wrong parameter count for octdec() in %s on line 3
