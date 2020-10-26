--TEST--
errmsg: cannot use 'self' as parent class name
--FILE--
<?php

class test extends self {
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Cannot use "self" as class name, self is a reserved class name in %s on line %d
