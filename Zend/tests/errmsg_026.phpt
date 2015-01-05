--TEST--
errmsg: cannot redeclare class
--FILE--
<?php

class stdclass {
}

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Cannot redeclare class stdclass in %s on line %d
