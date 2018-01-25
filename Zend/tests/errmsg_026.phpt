--TEST--
errmsg: cannot redeclare class
--FILE--
<?php

class stdclass {
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Cannot declare class stdclass, because the name is already in use in %s on line %d
