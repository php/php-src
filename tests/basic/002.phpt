--TEST--
Simple POST Method test
--POST--
a=Hello+World
--GET--
--FILE--
<?php error_reporting(0);
echo $_POST['a']; ?>
--EXPECT--
Hello World
