--TEST--
new class(...)  in assert
--FILE--
<?php

assert(new class(...) {});

?>
--EXPECTF--
Fatal error: Cannot create Closure for new expression in %s on line %d
