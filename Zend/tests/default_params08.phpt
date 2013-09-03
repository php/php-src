--TEST--
Default parameters - 8, by-refs
--FILE--
<?php

var_dump(is_callable("is_callable", default, default));
echo "Done\n";
?>
--EXPECTF--	
Fatal error: Cannot pass parameter 3 by reference in %s on line %d

