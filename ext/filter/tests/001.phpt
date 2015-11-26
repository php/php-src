--TEST--
Simple GET test
--GET--
a=1
--FILE--
<?php echo $_GET['a']; ?>
--EXPECT--
1
