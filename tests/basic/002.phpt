--TEST--
Simple POST Method test
--POST--
a=Hello+World
--FILE--
<?php
echo $_POST['a']; ?>
--EXPECT--
Hello World
