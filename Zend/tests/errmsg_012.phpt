--TEST--
errmsg: __autoload() must take exactly 1 argument
--FILE--
<?php

function __autoload($a, $b) {}

echo "Done\n";
?>
--EXPECTF--
Fatal error: __autoload() must take exactly 1 argument in %s on line %d
