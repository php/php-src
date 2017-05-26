--TEST--
pcntl_signal_get_handler() errors
--SKIPIF--
<?php if (!extension_loaded("pcntl")) print "skip"; ?>
--FILE--
<?php
pcntl_signal_get_handler();
pcntl_signal_get_handler(0);
pcntl_signal_get_handler(33);
?>
--EXPECTF--

Warning: pcntl_signal_get_handler() expects exactly 1 parameter, 0 given in %s on line %d

Warning: pcntl_signal_get_handler(): Invalid signal in %s on line %d

Warning: pcntl_signal_get_handler(): Invalid signal in %s on line %d
