--TEST--
Simple GET test
--POST--
--GET--
a=1
--FILE--
<?php echo $_GET['a']; ?>
--EXPECT--
1
