--TEST--
errmsg: cannot redeclare class
--FILE--
<?php

class stdclass {
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Cannot redeclare class stdClass in %s on line %d
