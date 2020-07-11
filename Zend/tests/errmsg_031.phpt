--TEST--
errmsg: cannot use 'parent' as parent class name
--FILE--
<?php

class test extends parent {
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Cannot use 'parent' as class name, as it is reserved in %s on line %d
