--TEST--
Simple POST Method test
--SKIPIF--
<?php if (php_sapi_name()=='cli') echo 'skip'; ?>
--POST--
a=Hello+World
--FILE--
<?php
echo $_POST['a']; ?>
--EXPECT--
Hello World
