--TEST--
get_current_user() tests
--FILE--
<?php

var_dump(get_current_user("blah"));
var_dump(get_current_user());

echo "Done\n";
?>
--EXPECTF--	
Warning: Wrong parameter count for get_current_user() in %s on line %d
NULL
string(%d) %s
Done
