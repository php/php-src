--TEST--
errmsg: cannot use 'self' as interface name
--FILE--
<?php

class test implements self {
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Cannot use "self" as interface name, self is a reserved class name in %s on line %d
