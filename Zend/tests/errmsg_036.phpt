--TEST--
errmsg: cannot use 'parent' as interface name
--FILE--
<?php

class test implements parent {
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Cannot use 'parent' as interface name as it is reserved in %s on line %d
