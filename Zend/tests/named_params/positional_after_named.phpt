--TEST--
Cannot used positional parameter after named parameter
--FILE--
<?php

test(a: 1, 2);

?>
--EXPECTF--
Fatal error: Cannot use positional argument after named argument in %s on line %d
