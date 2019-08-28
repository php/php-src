--TEST--
errmsg: cannot redeclare class
--FILE--
<?php

class StdClass {
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Cannot declare class StdClass, because the name is already in use in %s on line %d
