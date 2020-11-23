--TEST--
Bison weirdness
--FILE--
<?php
echo "blah-$foo\n";
?>
--EXPECTF--
Warning: Undefined variable $foo in %s on line %d
blah-
