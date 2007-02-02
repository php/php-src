--TEST--
errmsg: only variables can be passed by reference
--FILE--
<?php

function foo (&$var) {
}

foo(1);

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Only variables can be passed by reference in %s on line %d
