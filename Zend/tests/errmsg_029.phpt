--TEST--
errmsg: cannot use 'parent' as class name
--FILE--
<?php

class parent {
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Class "parent" cannot be declared, parent is a reserved class name in %s on line %d
