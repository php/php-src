--TEST--
errmsg: cannot use 'self' as interface name
--FILE--
<?php

class test implements self {
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Cannot use 'self' as interface name, as it is reserved in %s on line %d
