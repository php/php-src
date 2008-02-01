--TEST--
Test decbin() - wrong params
--FILE--
<?php
decbin();
decbin(23,2,true);
?>
--EXPECTF--

Warning: Wrong parameter count for decbin() in %s on line 2

Warning: Wrong parameter count for decbin() in %s on line 3

