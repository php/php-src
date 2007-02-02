--TEST--
errmsg: cannot create references to temp array
--FILE--
<?php

foreach (array(1,2,3) as $k=>&$v) {
}

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Cannot create references to elements of a temporary array expression in %s on line %d
