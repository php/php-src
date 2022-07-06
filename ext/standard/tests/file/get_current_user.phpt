--TEST--
get_current_user() tests
--FILE--
<?php

var_dump(get_current_user());

echo "Done\n";
?>
--EXPECTF--
string(%d) %s
Done
