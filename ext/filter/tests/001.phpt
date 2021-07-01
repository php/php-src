--TEST--
Simple GET test
--EXTENSIONS--
filter
--GET--
a=1
--FILE--
<?php echo $_GET['a']; ?>
--EXPECT--
1
