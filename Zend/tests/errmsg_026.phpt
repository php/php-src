--TEST--
errmsg: cannot redeclare class
--FILE--
<?php

class stdclass {
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: class stdclass cannot be declared, because the name is already in use in %s on line %d
