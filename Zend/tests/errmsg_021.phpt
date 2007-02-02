--TEST--
errmsg: disabled class
--INI--
disable_classes=stdclass
--FILE--
<?php

class test extends stdclass {
}

$t = new test;

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Class 'stdclass' not found in %s on line %d
