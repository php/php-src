--TEST--
errmsg: cannot use 'parent' as parent class name
--FILE--
<?php

class test extends parent {
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Cannot use "parent" as class name, parent is a reserved class name in %s on line %d
