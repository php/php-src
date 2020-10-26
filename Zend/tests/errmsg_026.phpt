--TEST--
errmsg: cannot redeclare class
--FILE--
<?php

class stdclass {
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: class stdclass has already been declared in %s on line %d
