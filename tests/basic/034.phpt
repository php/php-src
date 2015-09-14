--TEST--
Simple PATCH Method test
--PATCH--
a=Hello+World
--FILE--
<?php
echo $_POST['a']; ?>
--EXPECT--
Hello World