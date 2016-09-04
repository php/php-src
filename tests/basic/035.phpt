--TEST--
Simple DELETE Method test
--DELETE--
a=Hello+World
--FILE--
<?php
echo $_POST['a']; ?>
--EXPECT--
Hello World