--TEST--
Bison weirdness
--FILE--
<?php
error_reporting(E_ALL & ~E_NOTICE);
echo "blah-$foo\n";
?>
--EXPECT--
blah-
