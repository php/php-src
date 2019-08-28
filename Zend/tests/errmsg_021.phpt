--TEST--
errmsg: disabled class
--INI--
disable_classes=StdClass
--FILE--
<?php

class test extends StdClass {
}

$t = new test;

echo "Done\n";
?>
--EXPECTF--
Warning: test() has been disabled for security reasons in %s on line %d
Done
