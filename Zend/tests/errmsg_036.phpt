--TEST--
errmsg: cannot use 'parent' as interface name
--FILE--
<?php

class test implements parent {
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Cannot use "parent" as interface name, parent is a reserved class name in %s on line %d
