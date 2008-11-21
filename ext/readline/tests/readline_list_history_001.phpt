--TEST--
readline_list_history(): Basic test
--FILE--
<?php

var_dump(readline_list_history());
var_dump(readline_list_history(1));

?>
--EXPECTF--
array(0) {
}

Warning: Wrong parameter count for readline_list_history() in %s on line %d
NULL
