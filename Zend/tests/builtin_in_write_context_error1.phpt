--TEST--
Cannot use built-in functions in write context (assignment)
--FILE--
<?php

strlen("foo")[0] = 1;

?>
--EXPECTF--
Fatal error: Cannot use result of built-in function in write context in %s on line %d
