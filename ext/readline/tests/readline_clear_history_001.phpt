--TEST--
readline_clear_history(): Basic test
--FILE--
<?php

var_dump(readline_clear_history());
var_dump(readline_clear_history(1));

?>
--EXPECTF--
bool(true)

Warning: Wrong parameter count for readline_clear_history() in %s on line %d
NULL
