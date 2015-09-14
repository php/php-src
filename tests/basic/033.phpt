--TEST--
Simple PUT Method test
--PUT--
a=Hello+World
--FILE--
<?php
echo $_POST['a']; ?>
--EXPECT--
Hello World