--TEST--
get_current_user() tests
--FILE--
<?php

var_dump(get_current_user("blah"));
var_dump(get_current_user());

echo "Done\n";
?>
--EXPECTF--
Warning: get_current_user() expects exactly 0 parameters, 1 given in %s on line %d
NULL
string(%d) %s
Done
