--TEST--
Bison weirdness
--FILE--
<?php
echo "blah-$foo\n";
?>
--EXPECTF--
Warning: Undefined variable $foo (This will become an error in PHP 9.0) in %s on line %d
blah-
